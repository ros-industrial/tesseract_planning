/**
 * @file fix_state_bounds_task_generator.h
 * @brief Process generator for process that pushes plan instructions back within joint limits
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
#ifndef TESSERACT_PROCESS_MANAGERS_FIX_STATE_BOUNDS_TASK_GENERATOR_H
#define TESSERACT_PROCESS_MANAGERS_FIX_STATE_BOUNDS_TASK_GENERATOR_H

#include <tesseract_process_managers/core/task_generator.h>
#include <tesseract_time_parameterization/iterative_spline_parameterization.h>

namespace tesseract_planning
{
struct FixStateBoundsProfile
{
  using Ptr = std::shared_ptr<FixStateBoundsProfile>;
  using ConstPtr = std::shared_ptr<const FixStateBoundsProfile>;

  enum class Settings
  {
    START_ONLY,
    END_ONLY,
    ALL,
    DISABLED
  };

  FixStateBoundsProfile(Settings mode = Settings::ALL) : mode(mode) {}

  /** @brief Sets which terms will be corrected  */
  Settings mode;

  /** @brief Maximum amount the process is allowed to correct. If deviation is further than this, it will fail */
  double max_deviation_global = std::numeric_limits<double>::max();

  /** @brief Amount to reduce the upper bounds before clamping limits. Should be > 1 */
  double upper_bounds_reduction{ std::numeric_limits<float>::epsilon() };

  /** @brief Amount to increase the lower bounds before clamping limits. Should be > 1 */
  double lower_bounds_reduction{ std::numeric_limits<float>::epsilon() };
};
using FixStateBoundsProfileMap = std::unordered_map<std::string, FixStateBoundsProfile::ConstPtr>;

/**
 * @brief This generator modifies the const input instructions in order to push waypoints that are outside of their
 * limits back within them.
 */
class FixStateBoundsTaskGenerator : public TaskGenerator
{
public:
  using UPtr = std::unique_ptr<FixStateBoundsTaskGenerator>;

  FixStateBoundsTaskGenerator(std::string name = "Fix State Bounds");

  ~FixStateBoundsTaskGenerator() override = default;
  FixStateBoundsTaskGenerator(const FixStateBoundsTaskGenerator&) = delete;
  FixStateBoundsTaskGenerator& operator=(const FixStateBoundsTaskGenerator&) = delete;
  FixStateBoundsTaskGenerator(FixStateBoundsTaskGenerator&&) = delete;
  FixStateBoundsTaskGenerator& operator=(FixStateBoundsTaskGenerator&&) = delete;

  int conditionalProcess(TaskInput input, std::size_t unique_id) const override;

  void process(TaskInput input, std::size_t unique_id) const override;
};

class FixStateBoundsTaskInfo : public TaskInfo
{
public:
  using Ptr = std::shared_ptr<FixStateBoundsTaskInfo>;
  using ConstPtr = std::shared_ptr<const FixStateBoundsTaskInfo>;

  FixStateBoundsTaskInfo(std::size_t unique_id, std::string name = "Fix State Bounds");

  std::vector<tesseract_collision::ContactResultMap> contact_results;
};
}  // namespace tesseract_planning
#endif  // TESSERACT_PROCESS_MANAGERS_FIX_STATE_BOUNDS_TASK_GENERATOR_H
