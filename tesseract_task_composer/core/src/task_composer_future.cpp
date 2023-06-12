/**
 * @file task_composer_future.cpp
 * @brief A task composer future
 *
 * @author Levi Armstrong
 * @date August 18, 2020
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

#include <tesseract_task_composer/core/task_composer_future.h>

namespace tesseract_planning
{
bool TaskComposerFuture::operator==(const tesseract_planning::TaskComposerFuture& /*rhs*/) const { return true; }

bool TaskComposerFuture::operator!=(const tesseract_planning::TaskComposerFuture& rhs) const
{
  return !operator==(rhs);
}

template <class Archive>
void TaskComposerFuture::serialize(Archive& /*ar*/, const unsigned int /*version*/)
{
}
}  // namespace tesseract_planning

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::TaskComposerFuture)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::TaskComposerFuture)
