/**
 * @file task_composer_context.h
 * @brief The context data structure to the pipeline
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
#ifndef TESSERACT_TASK_COMPOSER_TASK_COMPOSER_CONTEXT_H
#define TESSERACT_TASK_COMPOSER_TASK_COMPOSER_CONTEXT_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <memory>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/uuid/uuid.hpp>
#include <tesseract_common/fwd.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_task_composer/core/task_composer_node_info.h>

namespace tesseract_planning
{
struct TaskComposerProblem;
class TaskComposerDataStorage;
class TaskComposerNode;

/**
 * @brief This struct is passed as an input to each process in the decision tree
 *
 * Container for use internally by tasks and task executors to facilitate the execution of tasks
 *
 * This object contains:
 * - `TaskComposerProblem`: represents the mutable input data needed to initiate the task and additional meta-data.
 * - `TaskComposerDataStorage`: container in which tasks are expected to find their required input data and where tasks
 * should save their results at run-time. This class member is originally set equal to the `TaskComposerDataStorage`
 * member of the `TaskComposerProblem`, which represents the immutable input data for the task
 * - `TaskComposerNodeInfoContainer`: container for meta-data generated by task(s) during execution.
 */
struct TaskComposerContext
{
  using Ptr = std::shared_ptr<TaskComposerContext>;
  using ConstPtr = std::shared_ptr<const TaskComposerContext>;
  using UPtr = std::unique_ptr<TaskComposerContext>;
  using ConstUPtr = std::unique_ptr<const TaskComposerContext>;

  TaskComposerContext() = default;  // Required for serialization
  TaskComposerContext(std::shared_ptr<TaskComposerProblem> problem);
  TaskComposerContext(std::shared_ptr<TaskComposerProblem> problem,
                      std::shared_ptr<TaskComposerDataStorage> data_storage);
  TaskComposerContext(const TaskComposerContext&) = delete;
  TaskComposerContext(TaskComposerContext&&) noexcept = delete;
  TaskComposerContext& operator=(const TaskComposerContext&) = delete;
  TaskComposerContext& operator=(TaskComposerContext&&) = delete;
  virtual ~TaskComposerContext() = default;

  /** @brief The problem */
  std::shared_ptr<TaskComposerProblem> problem;

  /**
   * @brief The location data is stored and retrieved during execution
   * @details The problem input data is copied into this structure when constructed
   */
  std::shared_ptr<TaskComposerDataStorage> data_storage;

  /** @brief Container for meta-data generated by task(s) during execution */
  TaskComposerNodeInfoContainer task_infos;

  /**
   * @brief Check if process has been aborted
   * @details This accesses the internal process interface class
   * @return True if aborted otherwise false;
   */
  bool isAborted() const;

  /**
   * @brief If it was not aborted then it was successful
   * @return True if successful, otherwise false
   */
  bool isSuccessful() const;

  /**
   * @brief Abort the process input
   * @note If calling within a node you must provide the uuid
   * @details This accesses the internal process interface class to abort the process
   */
  void abort(const boost::uuids::uuid& calling_node = boost::uuids::uuid());

  /**
   * @brief Abort the process input
   * @note This method should be used if calling abort from within an node
   * @param caller The node calling abort
   */
  void abort(const TaskComposerNode& caller);

  bool operator==(const TaskComposerContext& rhs) const;
  bool operator!=(const TaskComposerContext& rhs) const;

private:
  friend struct tesseract_common::Serialization;
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT

  mutable std::atomic<bool> aborted_{ false };
};
}  // namespace tesseract_planning

BOOST_CLASS_EXPORT_KEY2(tesseract_planning::TaskComposerContext, "TaskComposerContext")

#endif  // TESSERACT_TASK_COMPOSER_TASK_COMPOSER_CONTEXT_H
