/**
 * @file raster_only_motion_task.h
 * @brief Plans raster paths only
 *
 * @author Matthew Powelson
 * @date July 15, 2020
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
#ifndef TESSERACT_TASK_COMPOSER_RASTER_ONLY_MOTION_TASK_HPP
#define TESSERACT_TASK_COMPOSER_RASTER_ONLY_MOTION_TASK_HPP

#include <tesseract_task_composer/task_composer_task.h>
#include <tesseract_common/any_poly.h>

#include <tesseract_task_composer/task_composer_future.h>
#include <tesseract_task_composer/task_composer_executor.h>
#include <tesseract_task_composer/nodes/start_task.h>
#include <tesseract_task_composer/nodes/update_start_and_end_state_task.h>
#include <tesseract_task_composer/nodes/update_end_state_task.h>
#include <tesseract_task_composer/nodes/update_start_state_task.h>
#include <tesseract_command_language/composite_instruction.h>

namespace tesseract_planning
{
/**
 * @brief The RasterCtOnlyMotionTask class
 * @details The required format is below.
 *
 * Composite
 * {
 *   Composite - Raster segment
 *   Composite - Transitions
 *   Composite - Raster segment
 *   Composite - Transitions
 *   Composite - Raster segment
 * }
 */
template <typename RasterTaskType, typename TransitionTaskType>
class RasterOnlyMotionTask : public TaskComposerTask
{
public:
  using class_type = RasterOnlyMotionTask<RasterTaskType, TransitionTaskType>;

  RasterOnlyMotionTask() = default;  // Required for serialization
  // NOLINTNEXTLINE(performance-unnecessary-value-param)
  RasterOnlyMotionTask(std::string input_key, std::string output_key, bool is_conditional, std::string name)
    : TaskComposerTask(is_conditional, std::move(name))
  {
    input_keys_.push_back(std::move(input_key));
    output_keys_.push_back(std::move(output_key));
  }
  ~RasterOnlyMotionTask() override = default;
  RasterOnlyMotionTask(const class_type&) = delete;
  RasterOnlyMotionTask& operator=(const class_type&) = delete;
  RasterOnlyMotionTask(class_type&&) = delete;
  RasterOnlyMotionTask& operator=(class_type&&) = delete;

  bool operator==(const class_type& rhs) const
  {
    bool equal = true;
    equal &= TaskComposerTask::operator==(rhs);
    return equal;
  }
  bool operator!=(const class_type& rhs) const { return !operator==(rhs); }

protected:
  friend class tesseract_common::Serialization;
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/)  // NOLINT
  {
    ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(TaskComposerTask);
  }

  TaskComposerNodeInfo::UPtr runImpl(TaskComposerInput& input,
                                     OptionalTaskComposerExecutor executor) const override final
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
    try
    {
      checkTaskInput(input_data_poly);
    }
    catch (const std::exception& e)
    {
      info->message = e.what();
      info->elapsed_time = timer.elapsedSeconds();
      CONSOLE_BRIDGE_logError("%s", info->message.c_str());
      return info;
    }

    auto& program = input_data_poly.template as<CompositeInstruction>();
    TaskComposerGraph task_graph;

    tesseract_common::ManipulatorInfo program_manip_info =
        program.getManipulatorInfo().getCombined(input.problem.manip_info);

    auto start_task = std::make_unique<StartTask>();
    auto start_uuid = task_graph.addNode(std::move(start_task));

    std::vector<std::pair<boost::uuids::uuid, std::string>> raster_tasks;
    raster_tasks.reserve(program.size());

    // Generate all of the raster tasks. They don't depend on anything
    std::size_t raster_idx = 0;
    for (std::size_t idx = 0; idx < program.size(); idx += 2)
    {
      // Get Raster program
      auto raster_input = program[idx].template as<CompositeInstruction>();

      // Set the manipulator info
      raster_input.setManipulatorInfo(raster_input.getManipulatorInfo().getCombined(program_manip_info));

      // Insert Start Plan Instruction
      if (idx > 0)
      {
        const InstructionPoly& pre_input_instruction = program[idx - 1];
        assert(pre_input_instruction.isCompositeInstruction());
        const auto& tci = pre_input_instruction.as<CompositeInstruction>();
        const auto* li = tci.getLastMoveInstruction();
        assert(li != nullptr);
        raster_input.insertMoveInstruction(raster_input.begin(), *li);
      }

      auto raster_pipeline_task = std::make_unique<RasterTaskType>("Raster #" + std::to_string(raster_idx + 1) + ": " +
                                                                   raster_input.getDescription());
      std::string raster_pipeline_key = raster_pipeline_task->getUUIDString();
      auto raster_pipeline_uuid = task_graph.addNode(std::move(raster_pipeline_task));
      raster_tasks.emplace_back(raster_pipeline_uuid, raster_pipeline_key);
      input.data_storage.setData(raster_pipeline_key, raster_input);

      task_graph.addEdges(start_uuid, { raster_pipeline_uuid });

      raster_idx++;
    }

    // Loop over all transitions
    std::vector<std::string> transition_keys;
    transition_keys.reserve(program.size());
    std::size_t transition_idx = 0;
    for (std::size_t idx = 1; idx < program.size() - 1; idx += 2)
    {
      // Get transition program
      auto transition_input = program[idx].template as<CompositeInstruction>();

      // Set the manipulator info
      transition_input.setManipulatorInfo(transition_input.getManipulatorInfo().getCombined(program_manip_info));

      const InstructionPoly& pre_input_instruction = program[idx - 1];
      assert(pre_input_instruction.isCompositeInstruction());
      const auto& tci = pre_input_instruction.as<CompositeInstruction>();
      const auto* li = tci.getLastMoveInstruction();
      assert(li != nullptr);
      transition_input.insertMoveInstruction(transition_input.begin(), *li);

      auto transition_pipeline_task = std::make_unique<TransitionTaskType>(
          "Transition #" + std::to_string(transition_idx + 1) + ": " + transition_input.getDescription());
      std::string transition_pipeline_key = transition_pipeline_task->getUUIDString();
      auto transition_pipeline_uuid = task_graph.addNode(std::move(transition_pipeline_task));
      transition_keys.push_back(transition_pipeline_key);

      const auto& prev = raster_tasks[transition_idx];
      const auto& next = raster_tasks[transition_idx + 1];
      auto transition_mux_task =
          std::make_unique<UpdateStartAndEndStateTask>(prev.second, next.second, transition_pipeline_key, false);
      std::string transition_mux_key = transition_mux_task->getUUIDString();
      auto transition_mux_uuid = task_graph.addNode(std::move(transition_mux_task));

      input.data_storage.setData(transition_mux_key, transition_input);

      task_graph.addEdges(transition_mux_uuid, { transition_pipeline_uuid });
      task_graph.addEdges(prev.first, { transition_mux_uuid });
      task_graph.addEdges(next.first, { transition_mux_uuid });

      transition_idx++;
    }

    // Debug remove
    std::ofstream tc_out_data;
    tc_out_data.open(tesseract_common::getTempPath() + "task_composer_raster_subgraph_example.dot");
    task_graph.dump(tc_out_data);  // dump the graph including dynamic tasks
    tc_out_data.close();

    TaskComposerFuture::UPtr future = executor.value().get().run(task_graph, input);
    future->wait();

    if (input.isAborted())
    {
      info->message = "Raster only subgraph failed";
      info->elapsed_time = timer.elapsedSeconds();
      CONSOLE_BRIDGE_logError("%s", info->message.c_str());
      return info;
    }

    program.clear();
    for (std::size_t i = 0; i < raster_tasks.size(); ++i)
    {
      CompositeInstruction segment = input.data_storage.getData(raster_tasks[i].second).as<CompositeInstruction>();
      if (i != 0)
        segment.erase(segment.begin());

      program.emplace_back(segment);

      if (i < raster_tasks.size() - 1)
      {
        CompositeInstruction transition = input.data_storage.getData(transition_keys[i]).as<CompositeInstruction>();
        transition.erase(transition.begin());
        program.emplace_back(transition);
      }
    }

    input.data_storage.setData(output_keys_[0], program);

    info->message = "Successful";
    info->return_value = 1;
    info->elapsed_time = timer.elapsedSeconds();
    return info;
  }

  static void checkTaskInput(const tesseract_common::AnyPoly& input)
  {
    // -------------
    // Check Input
    // -------------
    if (input.isNull())
      throw std::runtime_error("RasterOnlyMotionTask, input is null");

    if (input.getType() != std::type_index(typeid(CompositeInstruction)))
      throw std::runtime_error("RasterOnlyMotionTask, input is not a composite instruction");

    const auto& composite = input.as<CompositeInstruction>();

    // Check rasters and transitions
    for (const auto& i : composite)
    {
      // Both rasters and transitions should be a composite
      if (!i.isCompositeInstruction())
        throw std::runtime_error("RasterOnlyMotionTask, Both rasters and transitions should be a composite");
    }
  }
};

}  // namespace tesseract_planning

#endif  // TESSERACT_TASK_COMPOSER_RASTER_ONLY_MOTION_TASK_HPP
