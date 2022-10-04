/**
 * @file done_task.h
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
#include <tesseract_common/timer.h>

#include <tesseract_task_composer/nodes/done_task.h>

namespace tesseract_planning
{
DoneTask::DoneTask(bool is_conditional, std::string name) : TaskComposerTask(is_conditional, std::move(name)) {}

TaskComposerNodeInfo::UPtr DoneTask::runImpl(TaskComposerInput& input, OptionalTaskComposerExecutor /*executor*/) const
{
  auto info = std::make_unique<TaskComposerNodeInfo>(*this);
  info->return_value = 0;
  info->env = input.problem.env;

  if (input.isAborted())
  {
    info->message = "Aborted";
    return info;
  }

  info->return_value = 1;
  info->message = "Successful";
  CONSOLE_BRIDGE_logDebug("%s", info->message.c_str());
  return info;
}

bool DoneTask::operator==(const DoneTask& rhs) const
{
  bool equal = true;
  equal &= TaskComposerTask::operator==(rhs);
  return equal;
}
bool DoneTask::operator!=(const DoneTask& rhs) const { return !operator==(rhs); }

template <class Archive>
void DoneTask::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerTask);
}

}  // namespace tesseract_planning

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::DoneTask)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::DoneTask)
