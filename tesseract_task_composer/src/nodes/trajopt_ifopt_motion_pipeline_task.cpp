/**
 * @file trajopt_ifopt_motion_planner_task.cpp
 * @brief TrajOpt Ifopt motion planning pipeline
 *
 * @author Levi Armstrong
 * @date July 29. 2022
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2022, Levi Armstrong
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
#include <boost/serialization/string.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP
#include <tesseract_common/timer.h>

#include <tesseract_task_composer/nodes/trajopt_ifopt_motion_pipeline_task.h>

#include <tesseract_task_composer/nodes/motion_planner_task.h>
#include <tesseract_task_composer/nodes/min_length_task.h>
#include <tesseract_task_composer/nodes/discrete_contact_check_task.h>
#include <tesseract_task_composer/nodes/iterative_spline_parameterization_task.h>
#include <tesseract_task_composer/nodes/done_task.h>
#include <tesseract_task_composer/nodes/error_task.h>

#include <tesseract_motion_planners/trajopt_ifopt/trajopt_ifopt_motion_planner.h>

namespace tesseract_planning
{
TrajOptIfoptMotionPipelineTask::TrajOptIfoptMotionPipelineTask(std::string name) : TaskComposerGraph(std::move(name))
{
  ctor(uuid_str_, uuid_str_);
}

TrajOptIfoptMotionPipelineTask::TrajOptIfoptMotionPipelineTask(std::string input_key,
                                                               std::string output_key,
                                                               std::string name)
  : TaskComposerGraph(std::move(name))
{
  ctor(std::move(input_key), std::move(output_key));
}

void TrajOptIfoptMotionPipelineTask::ctor(std::string input_key, std::string output_key)
{
  input_keys_.push_back(std::move(input_key));
  output_keys_.push_back(std::move(output_key));

  boost::uuids::uuid done_task = addNode(std::make_unique<DoneTask>());
  boost::uuids::uuid error_task = addNode(std::make_unique<ErrorTask>());

  // Setup Seed Min Length Process Generator
  // This is required because trajopt requires a minimum length trajectory.
  // This is used to correct the input if it is to short.
  boost::uuids::uuid min_length_task = addNode(std::make_unique<MinLengthTask>(input_keys_[0], output_keys_[0]));

  // Setup TrajOpt
  auto motion_planner = std::make_shared<TrajOptIfoptMotionPlanner>();
  boost::uuids::uuid motion_planner_task =
      addNode(std::make_unique<MotionPlannerTask>(motion_planner, output_keys_[0], output_keys_[0], false));

  // Setup post collision check
  boost::uuids::uuid contact_check_task = addNode(std::make_unique<DiscreteContactCheckTask>(output_keys_[0]));

  // Setup time parameterization
  boost::uuids::uuid time_parameterization_task =
      addNode(std::make_unique<IterativeSplineParameterizationTask>(output_keys_[0], output_keys_[0]));

  addEdges(min_length_task, { motion_planner_task });
  addEdges(motion_planner_task, { error_task, contact_check_task });
  addEdges(contact_check_task, { error_task, time_parameterization_task });
  addEdges(time_parameterization_task, { error_task, done_task });
}

bool TrajOptIfoptMotionPipelineTask::operator==(const TrajOptIfoptMotionPipelineTask& rhs) const
{
  bool equal = true;
  equal &= TaskComposerGraph::operator==(rhs);
  return equal;
}
bool TrajOptIfoptMotionPipelineTask::operator!=(const TrajOptIfoptMotionPipelineTask& rhs) const
{
  return !operator==(rhs);
}

template <class Archive>
void TrajOptIfoptMotionPipelineTask::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerGraph);
}

}  // namespace tesseract_planning

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::TrajOptIfoptMotionPipelineTask)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::TrajOptIfoptMotionPipelineTask)
