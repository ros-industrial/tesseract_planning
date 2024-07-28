/**
 * @file motion_planner_task_info.h
 * @brief Task Composer motion planner task info
 *
 * @author Levi Armstrong
 * @date June 8. 2023
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2023, Levi Armstrong
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
#include <boost/serialization/shared_ptr.hpp>

#include <tesseract_common/serialization.h>
#include <tesseract_common/utils.h>

#include <tesseract_environment/environment.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_task_composer/planning/nodes/motion_planner_task_info.h>

#include <tesseract_task_composer/core/task_composer_context.h>
#include <tesseract_task_composer/core/task_composer_task.h>
#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/core/task_composer_data_storage.h>

namespace tesseract_planning
{
MotionPlannerTaskInfo::MotionPlannerTaskInfo(const TaskComposerTask& task) : TaskComposerNodeInfo(task) {}

std::unique_ptr<TaskComposerNodeInfo> MotionPlannerTaskInfo::clone() const
{
  return std::make_unique<MotionPlannerTaskInfo>(*this);
}

bool MotionPlannerTaskInfo::operator==(const MotionPlannerTaskInfo& rhs) const
{
  bool equal = true;
  equal &= TaskComposerNodeInfo::operator==(rhs);
  equal &= tesseract_common::pointersEqual(env, rhs.env);
  return equal;
}
bool MotionPlannerTaskInfo::operator!=(const MotionPlannerTaskInfo& rhs) const { return !operator==(rhs); }

template <class Archive>
void MotionPlannerTaskInfo::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerNodeInfo);
  ar& BOOST_SERIALIZATION_NVP(env);
}
}  // namespace tesseract_planning

BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::MotionPlannerTaskInfo)
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::MotionPlannerTaskInfo)
