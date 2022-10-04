/**
 * @file fix_state_bounds_task.cpp
 * @brief Task that changes the plan instructions to make push them back within joint limits
 *
 * @author Matthew Powelson
 * @date August 31. 2020
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
#include <boost/serialization/string.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP
#include <tesseract_common/timer.h>

//#include <tesseract_process_managers/core/utils.h>
#include <tesseract_task_composer/nodes/fix_state_bounds_task.h>
#include <tesseract_task_composer/profiles/fix_state_bounds_profile.h>
#include <tesseract_command_language/utils.h>
#include <tesseract_motion_planners/planner_utils.h>

namespace tesseract_planning
{
FixStateBoundsTask::FixStateBoundsTask(std::string input_key,
                                       std::string output_key,
                                       bool is_conditional,
                                       std::string name)
  : TaskComposerTask(is_conditional, std::move(name))
{
  input_keys_.push_back(std::move(input_key));
  output_keys_.push_back(std::move(output_key));
}

TaskComposerNodeInfo::UPtr FixStateBoundsTask::runImpl(TaskComposerInput& input,
                                                       OptionalTaskComposerExecutor /*executor*/) const
{
  auto info = std::make_unique<TaskComposerNodeInfo>(*this);
  info->return_value = 0;
  info->env = input.problem.env;

  if (input.isAborted())
  {
    info->message = "Aborted";
    return info;
  }

  tesseract_common::Timer timer;
  timer.start();

  // --------------------
  // Check that inputs are valid
  // --------------------
  auto input_data_poly = input.data_storage.getData(input_keys_[0]);
  if (input_data_poly.isNull() || input_data_poly.getType() != std::type_index(typeid(CompositeInstruction)))
  {
    info->message = "Input instruction to FixStateBounds must be a composite instruction";
    info->elapsed_time = timer.elapsedSeconds();
    CONSOLE_BRIDGE_logError("%s", info->message.c_str());
    return info;
  }

  auto& ci = input_data_poly.as<CompositeInstruction>();
  ci.setManipulatorInfo(ci.getManipulatorInfo().getCombined(input.problem.manip_info));
  const tesseract_common::ManipulatorInfo& manip_info = ci.getManipulatorInfo();
  auto joint_group = input.problem.env->getJointGroup(manip_info.manipulator);
  auto limits = joint_group->getLimits();

  // Get Composite Profile
  std::string profile = ci.getProfile();
  profile = getProfileString(name_, profile, input.problem.composite_profile_remapping);
  auto cur_composite_profile =
      getProfile<FixStateBoundsProfile>(name_, profile, *input.profiles, std::make_shared<FixStateBoundsProfile>());
  cur_composite_profile = applyProfileOverrides(name_, profile, cur_composite_profile, ci.getProfileOverrides());

  if (cur_composite_profile->mode == FixStateBoundsProfile::Settings::DISABLED)
  {
    input.data_storage.setData(output_keys_[0], input_data_poly);
    info->message = "Successful, DISABLED";
    info->return_value = 1;
    info->elapsed_time = timer.elapsedSeconds();
    return info;
  }

  limits.joint_limits.col(0) = limits.joint_limits.col(0).array() + cur_composite_profile->lower_bounds_reduction;
  limits.joint_limits.col(1) = limits.joint_limits.col(1).array() - cur_composite_profile->upper_bounds_reduction;
  switch (cur_composite_profile->mode)
  {
    case FixStateBoundsProfile::Settings::START_ONLY:
    {
      MoveInstructionPoly* first_mi = ci.getFirstMoveInstruction();
      if (first_mi != nullptr)
      {
        if (!isWithinJointLimits(first_mi->getWaypoint(), limits.joint_limits))
        {
          CONSOLE_BRIDGE_logInform("FixStateBoundsTask is modifying the input instructions");
          if (!clampToJointLimits(
                  first_mi->getWaypoint(), limits.joint_limits, cur_composite_profile->max_deviation_global))
          {
            info->message = "Failed to clamp to joint limits";
            info->elapsed_time = timer.elapsedSeconds();
            return info;
          }
        }
      }
    }
    break;
    case FixStateBoundsProfile::Settings::END_ONLY:
    {
      MoveInstructionPoly* last_mi = ci.getLastMoveInstruction();
      if (last_mi != nullptr)
      {
        if (!isWithinJointLimits(last_mi->getWaypoint(), limits.joint_limits))
        {
          CONSOLE_BRIDGE_logInform("FixStateBoundsTask is modifying the input instructions");
          if (!clampToJointLimits(
                  last_mi->getWaypoint(), limits.joint_limits, cur_composite_profile->max_deviation_global))
          {
            info->message = "Failed to clamp to joint limits";
            info->elapsed_time = timer.elapsedSeconds();
            return info;
          }
        }
      }
    }
    break;
    case FixStateBoundsProfile::Settings::ALL:
    {
      auto flattened = ci.flatten(moveFilter);
      if (flattened.empty())
      {
        input.data_storage.setData(output_keys_[0], input_data_poly);
        info->message = "FixStateBoundsTask found no MoveInstructions to process";
        info->return_value = 1;
        info->elapsed_time = timer.elapsedSeconds();
        CONSOLE_BRIDGE_logWarn("%s", info->message.c_str());
        return info;
      }

      bool inside_limits = true;
      for (const auto& instruction : flattened)
      {
        inside_limits &=
            isWithinJointLimits(instruction.get().as<MoveInstructionPoly>().getWaypoint(), limits.joint_limits);
      }
      if (inside_limits)
        break;

      CONSOLE_BRIDGE_logInform("FixStateBoundsTask is modifying the input instructions");
      for (auto& instruction : flattened)
      {
        auto& plan = instruction.get().as<MoveInstructionPoly>();
        if (!clampToJointLimits(plan.getWaypoint(), limits.joint_limits, cur_composite_profile->max_deviation_global))
        {
          info->message = "Failed to clamp to joint limits";
          info->elapsed_time = timer.elapsedSeconds();
          return info;
        }
      }
    }
    break;
    case FixStateBoundsProfile::Settings::DISABLED:
      input.data_storage.setData(output_keys_[0], input_data_poly);
      info->message = "Successful, DISABLED";
      info->return_value = 1;
      info->elapsed_time = timer.elapsedSeconds();
      return info;
  }

  input.data_storage.setData(output_keys_[0], input_data_poly);
  info->message = "Successful";
  info->return_value = 1;
  info->elapsed_time = timer.elapsedSeconds();
  CONSOLE_BRIDGE_logDebug("FixStateBoundsTask succeeded");
  return info;
}

bool FixStateBoundsTask::operator==(const FixStateBoundsTask& rhs) const
{
  bool equal = true;
  equal &= TaskComposerTask::operator==(rhs);
  return equal;
}
bool FixStateBoundsTask::operator!=(const FixStateBoundsTask& rhs) const { return !operator==(rhs); }

template <class Archive>
void FixStateBoundsTask::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerTask);
}

}  // namespace tesseract_planning

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::FixStateBoundsTask)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::FixStateBoundsTask)
