/**
 * @file update_start_state_task.h
 *
 * @author Levi Armstrong
 * @date August 5, 2022
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

#include <tesseract_task_composer/planning/nodes/update_start_state_task.h>

#include <tesseract_task_composer/core/task_composer_context.h>
#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/core/task_composer_data_storage.h>

#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/poly/move_instruction_poly.h>
#include <tesseract_command_language/poly/cartesian_waypoint_poly.h>
#include <tesseract_command_language/poly/state_waypoint_poly.h>
#include <tesseract_command_language/poly/joint_waypoint_poly.h>

namespace tesseract_planning
{
UpdateStartStateTask::UpdateStartStateTask(std::string name,
                                           std::string input_prev_key,
                                           std::string output_key,
                                           bool conditional)
  : TaskComposerTask(std::move(name), conditional)
{
  input_keys_.push_back(uuid_str_);
  input_keys_.push_back(std::move(input_prev_key));
  output_keys_.push_back(std::move(output_key));
}

UpdateStartStateTask::UpdateStartStateTask(std::string name,
                                           std::string input_key,
                                           std::string input_prev_key,
                                           std::string output_key,
                                           bool conditional)
  : TaskComposerTask(std::move(name), conditional)
{
  input_keys_.push_back(std::move(input_key));
  input_keys_.push_back(std::move(input_prev_key));
  output_keys_.push_back(std::move(output_key));
}

std::unique_ptr<TaskComposerNodeInfo> UpdateStartStateTask::runImpl(TaskComposerContext& context,
                                                                    OptionalTaskComposerExecutor /*executor*/) const
{
  auto info = std::make_unique<TaskComposerNodeInfo>(*this);
  info->return_value = 0;

  auto input_data_poly = context.data_storage->getData(input_keys_[0]);
  auto input_prev_data_poly = context.data_storage->getData(input_keys_[1]);

  // --------------------
  // Check that inputs are valid
  // --------------------
  if (input_data_poly.isNull() || input_data_poly.getType() != std::type_index(typeid(CompositeInstruction)))
  {
    info->message = "UpdateStartStateTask: Input data for key '" + input_keys_[0] + "' must be a composite instruction";
    CONSOLE_BRIDGE_logError("%s", info->message.c_str());
    return info;
  }

  if (input_prev_data_poly.isNull() || input_prev_data_poly.getType() != std::type_index(typeid(CompositeInstruction)))
  {
    info->message = "UpdateStartStateTask: Input data for key '" + input_keys_[1] + "' must be a composite instruction";
    CONSOLE_BRIDGE_logError("%s", info->message.c_str());
    return info;
  }

  // Make a non-const copy of the input instructions to update the start/end
  auto& instructions = input_data_poly.as<CompositeInstruction>();
  auto* first_move_instruction = instructions.getFirstMoveInstruction();
  /** @todo Should the waypoint profile be updated to the path profile if it exists? **/

  // Update start instruction
  const auto* prev_last_move = input_prev_data_poly.as<CompositeInstruction>().getLastMoveInstruction();
  if (prev_last_move->getWaypoint().isCartesianWaypoint())
    first_move_instruction->assignCartesianWaypoint(prev_last_move->getWaypoint().as<CartesianWaypointPoly>());
  else if (prev_last_move->getWaypoint().isJointWaypoint())
    first_move_instruction->assignJointWaypoint(prev_last_move->getWaypoint().as<JointWaypointPoly>());
  else if (prev_last_move->getWaypoint().isStateWaypoint())
    first_move_instruction->assignStateWaypoint(prev_last_move->getWaypoint().as<StateWaypointPoly>());
  else
    throw std::runtime_error("Invalid waypoint type");

  // Store results
  context.data_storage->setData(output_keys_[0], input_data_poly);

  info->color = "green";
  info->message = "Successful";
  info->return_value = 1;
  return info;
}

bool UpdateStartStateTask::operator==(const UpdateStartStateTask& rhs) const
{
  return (TaskComposerTask::operator==(rhs));
}
bool UpdateStartStateTask::operator!=(const UpdateStartStateTask& rhs) const { return !operator==(rhs); }

template <class Archive>
void UpdateStartStateTask::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerTask);
}

}  // namespace tesseract_planning

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::UpdateStartStateTask)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::UpdateStartStateTask)
