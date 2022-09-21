/**
 * @file simple_planner_utils.cpp
 * @brief Provides interpolation utils structs
 *
 * @author Levi Armstrong
 * @date January 12, 2021
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2021, Southwest Research Institute
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

#include <tesseract_motion_planners/simple/profile/simple_planner_utils.h>
#include <tesseract_command_language/utils.h>
#include <tesseract_kinematics/core/utils.h>

namespace tesseract_planning
{
JointGroupInstructionInfo::JointGroupInstructionInfo(const MoveInstructionPoly& plan_instruction,
                                                     const PlannerRequest& request,
                                                     const tesseract_common::ManipulatorInfo& manip_info)
  : instruction(plan_instruction)
{
  assert(!(manip_info.empty() && plan_instruction.getManipulatorInfo().empty()));
  tesseract_common::ManipulatorInfo mi = manip_info.getCombined(plan_instruction.getManipulatorInfo());

  // Check required manipulator information
  if (mi.manipulator.empty())
    throw std::runtime_error("InstructionInfo, manipulator is empty!");

  if (mi.tcp_frame.empty())
    throw std::runtime_error("InstructionInfo, TCP frame is empty!");

  if (mi.working_frame.empty())
    throw std::runtime_error("InstructionInfo, working frame is empty!");

  // Get Previous Instruction Kinematics
  manip = request.env->getJointGroup(mi.manipulator);

  // Get Previous Instruction TCP and Working Frame
  working_frame = mi.working_frame;
  working_frame_transform = request.env_state.link_transforms.at(working_frame);
  tcp_frame = mi.tcp_frame;
  tcp_offset = request.env->findTCPOffset(mi);

  // Get Previous Instruction Waypoint Info
  if (plan_instruction.getWaypoint().isStateWaypoint() || plan_instruction.getWaypoint().isJointWaypoint())
    has_cartesian_waypoint = false;
  else if (plan_instruction.getWaypoint().isCartesianWaypoint())
    has_cartesian_waypoint = true;
  else
    throw std::runtime_error("Simple planner currently only supports State, Joint and Cartesian Waypoint types!");
}

Eigen::Isometry3d JointGroupInstructionInfo::calcCartesianPose(const Eigen::VectorXd& jp, bool in_world) const
{
  tesseract_common::TransformMap transforms = manip->calcFwdKin(jp);

  if (in_world)
    return transforms[tcp_frame] * tcp_offset;

  return working_frame_transform.inverse() * (transforms[tcp_frame] * tcp_offset);
}

Eigen::Isometry3d JointGroupInstructionInfo::extractCartesianPose(bool in_world) const
{
  if (!instruction.getWaypoint().isCartesianWaypoint())
    throw std::runtime_error("Instruction waypoint type is not a CartesianWaypoint, unable to extract cartesian pose!");

  if (in_world)
    return working_frame_transform * instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform();

  return instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform();
}

const Eigen::VectorXd& JointGroupInstructionInfo::extractJointPosition() const
{
  return getJointPosition(instruction.getWaypoint());
}

KinematicGroupInstructionInfo::KinematicGroupInstructionInfo(const MoveInstructionPoly& plan_instruction,
                                                             const PlannerRequest& request,
                                                             const tesseract_common::ManipulatorInfo& manip_info)
  : instruction(plan_instruction)
{
  assert(!(manip_info.empty() && plan_instruction.getManipulatorInfo().empty()));
  tesseract_common::ManipulatorInfo mi = manip_info.getCombined(plan_instruction.getManipulatorInfo());

  // Check required manipulator information
  if (mi.manipulator.empty())
    throw std::runtime_error("InstructionInfo, manipulator is empty!");

  if (mi.tcp_frame.empty())
    throw std::runtime_error("InstructionInfo, TCP frame is empty!");

  if (mi.working_frame.empty())
    throw std::runtime_error("InstructionInfo, working frame is empty!");

  // Get Previous Instruction Kinematics
  manip = request.env->getKinematicGroup(mi.manipulator);

  // Get Previous Instruction TCP and Working Frame
  working_frame = mi.working_frame;
  working_frame_transform = request.env_state.link_transforms.at(working_frame);
  tcp_frame = mi.tcp_frame;
  tcp_offset = request.env->findTCPOffset(mi);

  // Get Previous Instruction Waypoint Info
  if (plan_instruction.getWaypoint().isStateWaypoint() || plan_instruction.getWaypoint().isJointWaypoint())
    has_cartesian_waypoint = false;
  else if (plan_instruction.getWaypoint().isCartesianWaypoint())
    has_cartesian_waypoint = true;
  else
    throw std::runtime_error("Simple planner currently only supports State, Joint and Cartesian Waypoint types!");
}

Eigen::Isometry3d KinematicGroupInstructionInfo::calcCartesianPose(const Eigen::VectorXd& jp, bool in_world) const
{
  tesseract_common::TransformMap transforms = manip->calcFwdKin(jp);

  if (in_world)
    return transforms[tcp_frame] * tcp_offset;

  return working_frame_transform.inverse() * (transforms[tcp_frame] * tcp_offset);
}

Eigen::Isometry3d KinematicGroupInstructionInfo::extractCartesianPose(bool in_world) const
{
  if (!instruction.getWaypoint().isCartesianWaypoint())
    throw std::runtime_error("Instruction waypoint type is not a CartesianWaypoint, unable to extract cartesian pose!");

  if (in_world)
    return working_frame_transform * instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform();

  return instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform();
}

const Eigen::VectorXd& KinematicGroupInstructionInfo::extractJointPosition() const
{
  return getJointPosition(instruction.getWaypoint());
}

CompositeInstruction getInterpolatedComposite(const std::vector<std::string>& joint_names,
                                              const Eigen::MatrixXd& states,
                                              const MoveInstructionPoly& base_instruction)
{
  CompositeInstruction composite;
  composite.setManipulatorInfo(base_instruction.getManipulatorInfo());
  composite.setDescription(base_instruction.getDescription());
  composite.setProfile(base_instruction.getProfile());
  //  composite.profile_overrides = base_instruction.profile_overrides;

  // Convert to MoveInstructions
  for (long i = 1; i < states.cols() - 1; ++i)
  {
    MoveInstructionPoly move_instruction = base_instruction.createChild();
    JointWaypointPoly jwp = move_instruction.createJointWaypoint();
    jwp.setNames(joint_names);
    jwp.setPosition(states.col(i));
    jwp.setIsConstrained(false);
    move_instruction.assignJointWaypoint(jwp);
    if (!base_instruction.getPathProfile().empty())
    {
      move_instruction.setProfile(base_instruction.getPathProfile());
      move_instruction.setPathProfile(base_instruction.getPathProfile());
    }
    composite.appendMoveInstruction(move_instruction);
  }

  MoveInstructionPoly move_instruction{ base_instruction };
  if (base_instruction.getWaypoint().isCartesianWaypoint())
    move_instruction.getWaypoint().as<CartesianWaypointPoly>().setSeed(
        tesseract_common::JointState(joint_names, states.col(states.cols() - 1)));

  composite.appendMoveInstruction(move_instruction);
  return composite;
}

CompositeInstruction getInterpolatedComposite(const tesseract_common::VectorIsometry3d& poses,
                                              const std::vector<std::string>& joint_names,
                                              const Eigen::MatrixXd& states,
                                              const MoveInstructionPoly& base_instruction)
{
  CompositeInstruction composite;
  composite.setManipulatorInfo(base_instruction.getManipulatorInfo());
  composite.setDescription(base_instruction.getDescription());
  composite.setProfile(base_instruction.getProfile());
  //  composite.profile_overrides = base_instruction.profile_overrides;

  // Convert to MoveInstructions
  if (base_instruction.getWaypoint().isCartesianWaypoint())
  {
    for (long i = 1; i < states.cols() - 1; ++i)
    {
      MoveInstructionPoly move_instruction = base_instruction.createChild();
      move_instruction.getWaypoint().as<CartesianWaypointPoly>().setTransform(poses[static_cast<std::size_t>(i)]);
      move_instruction.getWaypoint().as<CartesianWaypointPoly>().setSeed(
          tesseract_common::JointState(joint_names, states.col(i)));
      if (!base_instruction.getPathProfile().empty())
      {
        move_instruction.setProfile(base_instruction.getPathProfile());
        move_instruction.setPathProfile(base_instruction.getPathProfile());
      }
      composite.appendMoveInstruction(move_instruction);
    }

    MoveInstructionPoly move_instruction = base_instruction;
    move_instruction.getWaypoint().as<CartesianWaypointPoly>().setSeed(
        tesseract_common::JointState(joint_names, states.col(states.cols() - 1)));
    composite.appendMoveInstruction(move_instruction);
  }
  else
  {
    for (long i = 1; i < states.cols() - 1; ++i)
    {
      MoveInstructionPoly move_instruction = base_instruction.createChild();
      CartesianWaypointPoly cwp = move_instruction.createCartesianWaypoint();
      cwp.setTransform(poses[static_cast<std::size_t>(i)]);
      cwp.setSeed(tesseract_common::JointState(joint_names, states.col(i)));
      move_instruction.assignCartesianWaypoint(cwp);
      if (!base_instruction.getPathProfile().empty())
      {
        move_instruction.setProfile(base_instruction.getPathProfile());
        move_instruction.setPathProfile(base_instruction.getPathProfile());
      }
      composite.appendMoveInstruction(move_instruction);
    }

    composite.appendMoveInstruction(base_instruction);
  }

  return composite;
}

Eigen::VectorXd getClosestJointSolution(const KinematicGroupInstructionInfo& info, const Eigen::VectorXd& seed)
{
  auto limits = info.manip->getLimits();
  auto redundancy_indices = info.manip->getRedundancyCapableJointIndices();

  if (!info.has_cartesian_waypoint)
    throw std::runtime_error("Instruction waypoint type is not a CartesianWaypoint, unable to extract cartesian pose!");

  Eigen::Isometry3d cwp =
      info.instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform() * info.tcp_offset.inverse();

  Eigen::VectorXd jp_final;
  tesseract_kinematics::IKSolutions jp;
  tesseract_kinematics::KinGroupIKInput ik_input(cwp, info.working_frame, info.tcp_frame);
  tesseract_kinematics::IKSolutions solutions = info.manip->calcInvKin({ ik_input }, seed);
  for (const auto& sol : solutions)
  {
    jp.push_back(sol);
    auto redundant_solutions =
        tesseract_kinematics::getRedundantSolutions<double>(sol, limits.joint_limits, redundancy_indices);
    jp.insert(jp.end(), redundant_solutions.begin(), redundant_solutions.end());
  }

  if (!jp.empty())
  {
    // Find closest solution to the start state
    double dist = std::numeric_limits<double>::max();
    for (const auto& solution : jp)
    {
      if (tesseract_common::satisfiesPositionLimits<double>(solution, limits.joint_limits))
      {
        if (jp_final.rows() == 0)
        {
          jp_final = solution;
          dist = (solution - seed).norm();
          continue;
        }

        /// @todo: May be nice to add contact checking to find best solution, but may not be necessary because this is
        /// used to generate the seed
        double d = (solution - seed).norm();
        if (d < dist)
        {
          jp_final = solution;
          dist = d;
        }
      }
    }
  }
  return jp_final;
}

std::array<Eigen::VectorXd, 2> getClosestJointSolution(const KinematicGroupInstructionInfo& info1,
                                                       const KinematicGroupInstructionInfo& info2,
                                                       const Eigen::VectorXd& seed)
{
  auto manip1_limits = info1.manip->getLimits();
  auto manip1_redundancy_indices = info1.manip->getRedundancyCapableJointIndices();

  auto manip2_limits = info2.manip->getLimits();
  auto manip2_redundancy_indices = info2.manip->getRedundancyCapableJointIndices();

  if (!info1.has_cartesian_waypoint || !info2.has_cartesian_waypoint)
    throw std::runtime_error("Instruction waypoint type is not a CartesianWaypoint, unable to extract cartesian pose!");

  Eigen::Isometry3d cwp1 =
      info1.instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform() * info1.tcp_offset.inverse();
  Eigen::Isometry3d cwp2 =
      info2.instruction.getWaypoint().as<CartesianWaypointPoly>().getTransform() * info2.tcp_offset.inverse();

  std::array<Eigen::VectorXd, 2> results;

  // Calculate IK for start and end
  Eigen::VectorXd j1_final;
  tesseract_kinematics::IKSolutions j1;
  tesseract_kinematics::KinGroupIKInput ik_input1(cwp1, info1.working_frame, info1.tcp_frame);
  tesseract_kinematics::IKSolutions j1_solutions = info1.manip->calcInvKin({ ik_input1 }, seed);
  j1_solutions.erase(std::remove_if(j1_solutions.begin(),
                                    j1_solutions.end(),
                                    [&manip1_limits](const Eigen::VectorXd& solution) {
                                      return !tesseract_common::satisfiesPositionLimits<double>(
                                          solution, manip1_limits.joint_limits);
                                    }),
                     j1_solutions.end());

  // Get redundant solutions
  for (const auto& sol : j1_solutions)
  {
    j1.push_back(sol);
    auto redundant_solutions =
        tesseract_kinematics::getRedundantSolutions<double>(sol, manip1_limits.joint_limits, manip1_redundancy_indices);
    j1.insert(j1.end(), redundant_solutions.begin(), redundant_solutions.end());
  }

  Eigen::VectorXd j2_final;
  tesseract_kinematics::IKSolutions j2;
  tesseract_kinematics::KinGroupIKInput ik_input2(cwp2, info2.working_frame, info2.tcp_frame);
  tesseract_kinematics::IKSolutions j2_solutions = info2.manip->calcInvKin({ ik_input2 }, seed);
  j2_solutions.erase(std::remove_if(j2_solutions.begin(),
                                    j2_solutions.end(),
                                    [&manip2_limits](const Eigen::VectorXd& solution) {
                                      // NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
                                      return !tesseract_common::satisfiesPositionLimits<double>(
                                          solution, manip2_limits.joint_limits);
                                    }),
                     j2_solutions.end());

  // Get redundant solutions
  for (const auto& sol : j2_solutions)
  {
    j2.push_back(sol);
    auto redundant_solutions =
        tesseract_kinematics::getRedundantSolutions<double>(sol, manip2_limits.joint_limits, manip2_redundancy_indices);
    j2.insert(j2.end(), redundant_solutions.begin(), redundant_solutions.end());
  }

  if (!j1.empty() && !j2.empty())
  {
    // Find closest solution to the end state
    double dist = std::numeric_limits<double>::max();
    j1_final = j1[0];
    j2_final = j2[0];
    for (const auto& j1_solution : j1)
    {
      for (const auto& j2_solution : j2)
      {
        /// @todo: May be nice to add contact checking to find best solution, but may not be necessary because this is
        /// used to generate the seed.
        double d = (j2_solution - j1_solution).norm();
        if (d < dist)
        {
          j1_final = j1_solution;
          j2_final = j2_solution;
          dist = d;
        }
      }
    }
    results[0] = j1_final;
    results[1] = j2_final;
  }
  else if (!j1.empty())
  {
    double dist = std::numeric_limits<double>::max();
    j1_final = j1[0];
    for (const auto& j1_solution : j1)
    {
      double d = (seed - j1_solution).norm();
      if (d < dist)
      {
        j1_final = j1_solution;
        dist = d;
      }
    }

    results[0] = j1_final;
  }
  else if (!j2.empty())
  {
    double dist = std::numeric_limits<double>::max();
    j2_final = j2[0];
    for (const auto& j2_solution : j2)
    {
      double d = (seed - j2_solution).norm();
      if (d < dist)
      {
        j2_final = j2_solution;
        dist = d;
      }
    }

    results[1] = j1_final;
  }

  return results;
}

}  // namespace tesseract_planning
