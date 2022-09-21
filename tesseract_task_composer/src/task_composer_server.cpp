/**
 * @file task_composer_server.h
 * @brief A task server
 *
 * @author Levi Armstrong
 * @date August 27, 2022
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

#include <tesseract_task_composer/task_composer_server.h>

namespace tesseract_planning
{
void TaskComposerServer::addExecutor(const TaskComposerExecutor::Ptr& executor)
{
  executors_[executor->getName()] = executor;
}

TaskComposerExecutor::Ptr TaskComposerServer::getExecutor(const std::string& name)
{
  auto it = executors_.find(name);
  if (it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  return it->second;
}

bool TaskComposerServer::hasExecutor(const std::string& name) const
{
  return (executors_.find(name) != executors_.end());
}

std::vector<std::string> TaskComposerServer::getAvailableExecutors() const
{
  std::vector<std::string> executors;
  executors.reserve(executors_.size());
  for (const auto& executor : executors_)
    executors.push_back(executor.first);

  return executors;
}

void TaskComposerServer::addTask(TaskComposerNode::UPtr task)
{
  if (tasks_.find(task->getName()) != tasks_.end())
    CONSOLE_BRIDGE_logDebug("Task %s already exist so replacing with new task.", task->getName().c_str());

  tasks_[task->getName()] = std::move(task);
}

bool TaskComposerServer::hasTask(const std::string& name) const { return (tasks_.find(name) != tasks_.end()); }

std::vector<std::string> TaskComposerServer::getAvailableTasks() const
{
  std::vector<std::string> tasks;
  tasks.reserve(tasks_.size());
  for (const auto& task : tasks_)
    tasks.push_back(task.first);

  return tasks;
}

TaskComposerFuture::UPtr TaskComposerServer::run(TaskComposerInput& task_input, const std::string& name)
{
  auto e_it = executors_.find(name);
  if (e_it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  auto t_it = tasks_.find(task_input.problem.name);
  if (t_it == tasks_.end())
    throw std::runtime_error("Task with name '" + task_input.problem.name + "' does not exist!");

  return e_it->second->run(*t_it->second, task_input);
}

TaskComposerFuture::UPtr TaskComposerServer::run(const TaskComposerNode& node,
                                                 TaskComposerInput& task_input,
                                                 const std::string& name)
{
  auto it = executors_.find(name);
  if (it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  return it->second->run(node, task_input);
}

TaskComposerFuture::UPtr TaskComposerServer::run(const TaskComposerGraph& task_graph,
                                                 TaskComposerInput& task_input,
                                                 const std::string& name)
{
  auto it = executors_.find(name);
  if (it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  return it->second->run(task_graph, task_input);
}

TaskComposerFuture::UPtr TaskComposerServer::run(const TaskComposerTask& task,
                                                 TaskComposerInput& task_input,
                                                 const std::string& name)
{
  auto it = executors_.find(name);
  if (it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  return it->second->run(task, task_input);
}

long TaskComposerServer::getWorkerCount(const std::string& name) const
{
  auto it = executors_.find(name);
  if (it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  return it->second->getWorkerCount();
}

long TaskComposerServer::getTaskCount(const std::string& name) const
{
  auto it = executors_.find(name);
  if (it == executors_.end())
    throw std::runtime_error("Executor with name '" + name + "' does not exist!");

  return it->second->getTaskCount();
}
}  // namespace tesseract_planning
