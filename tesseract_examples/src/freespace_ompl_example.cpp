/**
 * @file freespace_ompl_example.cpp
 * @brief An example of a feespace motion planning with OMPL.
 *
 * @author Levi Armstrong
 * @date March 16, 2020
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2020, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <console_bridge/console.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_examples/freespace_ompl_example.h>
#include <tesseract_environment/utils.h>
#include <tesseract_environment/commands.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/state_waypoint.h>
#include <tesseract_command_language/cartesian_waypoint.h>
#include <tesseract_command_language/joint_waypoint.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/utils.h>
#include <tesseract_motion_planners/default_planner_namespaces.h>
#include <tesseract_motion_planners/ompl/profile/ompl_default_plan_profile.h>
#include <tesseract_motion_planners/core/utils.h>
#include <tesseract_task_composer/task_composer_problem.h>
#include <tesseract_task_composer/task_composer_input.h>
#include <tesseract_task_composer/task_composer_node_names.h>
#include <tesseract_task_composer/nodes/freespace_motion_pipeline_task.h>
#include <tesseract_task_composer/taskflow/taskflow_task_composer_executor.h>
#include <tesseract_visualization/markers/toolpath_marker.h>

using namespace tesseract_environment;
using namespace tesseract_scene_graph;
using namespace tesseract_collision;
using namespace tesseract_visualization;
using namespace tesseract_planning;
using tesseract_common::ManipulatorInfo;

namespace tesseract_examples
{
FreespaceOMPLExample::FreespaceOMPLExample(tesseract_environment::Environment::Ptr env,
                                           tesseract_visualization::Visualization::Ptr plotter,
                                           double range,
                                           double planning_time)
  : Example(std::move(env), std::move(plotter)), range_(range), planning_time_(planning_time)
{
}

Command::Ptr FreespaceOMPLExample::addSphere()
{
  // Add sphere to environment
  Link link_sphere("sphere_attached");

  Visual::Ptr visual = std::make_shared<Visual>();
  visual->origin = Eigen::Isometry3d::Identity();
  visual->origin.translation() = Eigen::Vector3d(0.5, 0, 0.55);
  visual->geometry = std::make_shared<tesseract_geometry::Sphere>(0.15);
  link_sphere.visual.push_back(visual);

  Collision::Ptr collision = std::make_shared<Collision>();
  collision->origin = visual->origin;
  collision->geometry = visual->geometry;
  link_sphere.collision.push_back(collision);

  Joint joint_sphere("joint_sphere_attached");
  joint_sphere.parent_link_name = "base_link";
  joint_sphere.child_link_name = link_sphere.getName();
  joint_sphere.type = JointType::FIXED;

  return std::make_shared<tesseract_environment::AddLinkCommand>(link_sphere, joint_sphere);
}

bool FreespaceOMPLExample::run()
{
  // Add sphere to environment
  Command::Ptr cmd = addSphere();
  if (!env_->applyCommand(cmd))
    return false;

  if (plotter_ != nullptr)
    plotter_->waitForConnection();

  // Set the robot initial state
  std::vector<std::string> joint_names;
  joint_names.emplace_back("joint_a1");
  joint_names.emplace_back("joint_a2");
  joint_names.emplace_back("joint_a3");
  joint_names.emplace_back("joint_a4");
  joint_names.emplace_back("joint_a5");
  joint_names.emplace_back("joint_a6");
  joint_names.emplace_back("joint_a7");

  Eigen::VectorXd joint_start_pos(7);
  joint_start_pos(0) = -0.4;
  joint_start_pos(1) = 0.2762;
  joint_start_pos(2) = 0.0;
  joint_start_pos(3) = -1.3348;
  joint_start_pos(4) = 0.0;
  joint_start_pos(5) = 1.4959;
  joint_start_pos(6) = 0.0;

  Eigen::VectorXd joint_end_pos(7);
  joint_end_pos(0) = 0.4;
  joint_end_pos(1) = 0.2762;
  joint_end_pos(2) = 0.0;
  joint_end_pos(3) = -1.3348;
  joint_end_pos(4) = 0.0;
  joint_end_pos(5) = 1.4959;
  joint_end_pos(6) = 0.0;

  env_->setState(joint_names, joint_start_pos);

  // Create Program
  CompositeInstruction program(
      "FREESPACE", CompositeInstructionOrder::ORDERED, ManipulatorInfo("manipulator", "base_link", "tool0"));

  // Start and End Joint Position for the program
  StateWaypointPoly wp0{ StateWaypoint(joint_names, joint_start_pos) };
  StateWaypointPoly wp1{ StateWaypoint(joint_names, joint_end_pos) };

  MoveInstruction start_instruction(wp0, MoveInstructionType::START);
  program.setStartInstruction(start_instruction);

  // Plan freespace from start
  MoveInstruction plan_f0(wp1, MoveInstructionType::FREESPACE, "FREESPACE");
  plan_f0.setDescription("freespace_plan");

  // Add Instructions to program
  program.appendMoveInstruction(plan_f0);

  // Print Diagnostics
  program.print("Program: ");

  CONSOLE_BRIDGE_logInform("freespace OMPL plan example");

  // Create Executor
  auto executor = std::make_unique<TaskflowTaskComposerExecutor>(5);

  // Create OMPL Profile
  auto ompl_profile = std::make_shared<OMPLDefaultPlanProfile>();
  auto ompl_planner_config = std::make_shared<RRTConnectConfigurator>();
  ompl_planner_config->range = range_;
  ompl_profile->planning_time = planning_time_;
  ompl_profile->planners = { ompl_planner_config, ompl_planner_config };

  // Create profile dictionary
  auto profiles = std::make_shared<ProfileDictionary>();
  profiles->addProfile<OMPLPlanProfile>(profile_ns::OMPL_DEFAULT_NAMESPACE, "FREESPACE", ompl_profile);

  // Create Task Input Data
  TaskComposerDataStorage input_data;
  input_data.setData("input_program", program);

  // Create Task Composer Problem
  TaskComposerProblem problem(env_, input_data);

  // Solve task
  TaskComposerInput input(problem, profiles);
  FreespaceMotionPipelineTask task("input_program", "output_program");
  TaskComposerFuture::UPtr future = executor->run(task, input);
  future->wait();

  // Plot Process Trajectory
  if (plotter_ != nullptr && plotter_->isConnected())
  {
    plotter_->waitForInput();
    auto ci = input.data_storage.getData("output_program").as<CompositeInstruction>();
    tesseract_common::Toolpath toolpath = toToolpath(ci, *env_);
    tesseract_common::JointTrajectory trajectory = toJointTrajectory(ci);
    auto state_solver = env_->getStateSolver();
    plotter_->plotMarker(ToolpathMarker(toolpath));
    plotter_->plotTrajectory(trajectory, *state_solver);
  }

  CONSOLE_BRIDGE_logInform("Final trajectory is collision free");
  return input.isSuccessful();
}
}  // namespace tesseract_examples
