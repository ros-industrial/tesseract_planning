/**
 * @file check_input_task.cpp
 * @brief Task for checking input data structure
 *
 * @author Levi Armstrong
 * @date November 2. 2021
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

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <console_bridge/console.h>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <tesseract_common/serialization.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_task_composer/planning/nodes/check_input_task.h>
#include <tesseract_task_composer/planning/profiles/check_input_profile.h>
#include <tesseract_task_composer/planning/planning_task_composer_problem.h>

#include <tesseract_task_composer/core/task_composer_context.h>
#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/core/task_composer_data_storage.h>

#include <tesseract_motion_planners/planner_utils.h>

namespace tesseract_planning
{
CheckInputTask::CheckInputTask() : TaskComposerTask("CheckInputTask", true) {}

CheckInputTask::CheckInputTask(std::string name, std::vector<std::string> input_keys, bool is_conditional)
  : TaskComposerTask(std::move(name), is_conditional)
{
  input_keys_ = std::move(input_keys);
}

CheckInputTask::CheckInputTask(std::string name, std::string input_key, bool is_conditional)
  : CheckInputTask(std::move(name), std::vector<std::string>({ std::move(input_key) }), is_conditional)
{
}

CheckInputTask::CheckInputTask(std::string name,
                               const YAML::Node& config,
                               const TaskComposerPluginFactory& /*plugin_factory*/)
  : TaskComposerTask(std::move(name), config)
{
  if (input_keys_.empty())
    throw std::runtime_error("CheckInputTask, config missing 'inputs' entry");
}

std::unique_ptr<TaskComposerNodeInfo> CheckInputTask::runImpl(TaskComposerContext& context,
                                                              OptionalTaskComposerExecutor /*executor*/) const
{
  auto info = std::make_unique<TaskComposerNodeInfo>(*this);
  info->return_value = 0;
  info->status_code = 0;

  // Get the problem
  auto& problem = dynamic_cast<PlanningTaskComposerProblem&>(*context.problem);

  // Get Composite Profile
  for (const auto& key : input_keys_)
  {
    auto input_data_poly = context.data_storage->getData(key);
    if (input_data_poly.isNull() || input_data_poly.getType() != std::type_index(typeid(CompositeInstruction)))
    {
      info->status_message = "Input key '" + key + "' is missing";
      CONSOLE_BRIDGE_logError("%s", info->status_message.c_str());
      return info;
    }

    const auto& ci = input_data_poly.as<CompositeInstruction>();
    std::string profile = ci.getProfile();
    profile = getProfileString(ns_, profile, problem.composite_profile_remapping);
    auto cur_composite_profile =
        getProfile<CheckInputProfile>(ns_, profile, *problem.profiles, std::make_shared<CheckInputProfile>());
    cur_composite_profile = applyProfileOverrides(ns_, profile, cur_composite_profile, ci.getProfileOverrides());

    if (!cur_composite_profile->isValid(context))
    {
      info->status_message = "Validator failed";
      return info;
    }
  }

  info->color = "green";
  info->status_code = 1;
  info->status_message = "Successful";
  info->return_value = 1;
  return info;
}

bool CheckInputTask::operator==(const CheckInputTask& rhs) const { return (TaskComposerTask::operator==(rhs)); }
bool CheckInputTask::operator!=(const CheckInputTask& rhs) const { return !operator==(rhs); }

template <class Archive>
void CheckInputTask::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerTask);
}

}  // namespace tesseract_planning

TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::CheckInputTask)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::CheckInputTask)
