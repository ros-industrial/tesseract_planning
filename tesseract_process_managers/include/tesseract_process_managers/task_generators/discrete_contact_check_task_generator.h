/**
 * @file discrete_contact_check_task_generator.h
 * @brief Discrete Collision check trajectory
 *
 * @author Levi Armstrong
 * @date August 10. 2020
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
#ifndef TESSERACT_PROCESS_MANAGERS_DISCRETE_CONTACT_CHECK_TASK_GENERATOR_H
#define TESSERACT_PROCESS_MANAGERS_DISCRETE_CONTACT_CHECK_TASK_GENERATOR_H
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <vector>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_process_managers/core/task_generator.h>
#include <tesseract_process_managers/core/default_task_namespaces.h>

namespace tesseract_planning
{
class DiscreteContactCheckTaskGenerator : public TaskGenerator
{
public:
  using UPtr = std::unique_ptr<DiscreteContactCheckTaskGenerator>;

  DiscreteContactCheckTaskGenerator(std::string name = profile_ns::DISCRETE_CONTACT_CHECK_DEFAULT_NAMESPACE);

  ~DiscreteContactCheckTaskGenerator() override = default;
  DiscreteContactCheckTaskGenerator(const DiscreteContactCheckTaskGenerator&) = delete;
  DiscreteContactCheckTaskGenerator& operator=(const DiscreteContactCheckTaskGenerator&) = delete;
  DiscreteContactCheckTaskGenerator(DiscreteContactCheckTaskGenerator&&) = delete;
  DiscreteContactCheckTaskGenerator& operator=(DiscreteContactCheckTaskGenerator&&) = delete;

  int conditionalProcess(TaskInput input, std::size_t unique_id) const override final;

  void process(TaskInput input, std::size_t unique_id) const override final;
};

class DiscreteContactCheckTaskInfo : public TaskInfo
{
public:
  using Ptr = std::shared_ptr<DiscreteContactCheckTaskInfo>;
  using ConstPtr = std::shared_ptr<const DiscreteContactCheckTaskInfo>;

  DiscreteContactCheckTaskInfo(std::size_t unique_id,
                               std::string name = profile_ns::DISCRETE_CONTACT_CHECK_DEFAULT_NAMESPACE);

  std::vector<tesseract_collision::ContactResultMap> contact_results;

  TaskInfo::UPtr clone() const override;
};

}  // namespace tesseract_planning

#endif  // TESSERACT_PROCESS_MANAGERS_DISCRETE_CONTACT_CHECK_TASK_GENERATOR_H
