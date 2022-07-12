/**
 * @file type_erasure_benchmark.cpp
 * @brief
 *
 * @author Levi Armstrong
 * @date August 20, 2020
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
#include <gtest/gtest.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <fstream>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <benchmark/benchmark.h>
#include <tesseract_common/serialization.h>
#include <tesseract_common/any.h>
#include <tesseract_command_language/command_language.h>
#include <tesseract_command_language/utils/utils.h>
#include <tesseract_common/utils.h>

using namespace tesseract_planning;

CompositeInstruction getProgram()
{
  CompositeInstruction program(
      "raster_program", CompositeInstructionOrder::ORDERED, ManipulatorInfo("manipulator", "world", "tool0"));

  // Start Joint Position for the program
  std::vector<std::string> joint_names = { "joint_1", "joint_2", "joint_3", "joint_4", "joint_5", "joint_6" };
  Waypoint wp0 = StateWaypoint(joint_names, Eigen::VectorXd::Zero(6));
  MoveInstruction start_instruction(wp0, MoveInstructionType::START);
  program.setStartInstruction(start_instruction);

  // Define raster poses
  Waypoint wp1 = CartesianWaypoint(Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.8, -0.3, 0.8) *
                                   Eigen::Quaterniond(0, 0, -1.0, 0));
  Waypoint wp2 = CartesianWaypoint(Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.8, -0.2, 0.8) *
                                   Eigen::Quaterniond(0, 0, -1.0, 0));
  Waypoint wp3 = CartesianWaypoint(Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.8, -0.1, 0.8) *
                                   Eigen::Quaterniond(0, 0, -1.0, 0));
  Waypoint wp4 = CartesianWaypoint(Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.8, 0.0, 0.8) *
                                   Eigen::Quaterniond(0, 0, -1.0, 0));
  Waypoint wp5 = CartesianWaypoint(Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.8, 0.1, 0.8) *
                                   Eigen::Quaterniond(0, 0, -1.0, 0));
  Waypoint wp6 = CartesianWaypoint(Eigen::Isometry3d::Identity() * Eigen::Translation3d(0.8, 0.2, 0.8) *
                                   Eigen::Quaterniond(0, 0, -1.0, 0));
  Waypoint wp7 = JointWaypoint(joint_names, Eigen::VectorXd::Ones(6));

  // Define raster move instruction
  MoveInstruction plan_c0(wp2, MoveInstructionType::LINEAR, "RASTER");
  MoveInstruction plan_c1(wp3, MoveInstructionType::LINEAR, "RASTER", "RASTER");
  MoveInstruction plan_c2(wp4, MoveInstructionType::LINEAR, "RASTER");
  MoveInstruction plan_c3(wp5, MoveInstructionType::LINEAR, "RASTER", "RASTER");
  MoveInstruction plan_c4(wp6, MoveInstructionType::LINEAR, "RASTER");
  MoveInstruction plan_c5(wp7, MoveInstructionType::LINEAR, "RASTER", "RASTER");

  MoveInstruction plan_f0(wp1, MoveInstructionType::FREESPACE, "freespace_profile");
  plan_f0.setDescription("from_start_plan");
  CompositeInstruction from_start;
  from_start.setDescription("from_start");
  from_start.push_back(plan_f0);
  program.push_back(from_start);

  {
    CompositeInstruction raster_segment;
    raster_segment.setDescription("raster_segment");
    raster_segment.push_back(plan_c0);
    raster_segment.push_back(plan_c1);
    raster_segment.push_back(plan_c2);
    raster_segment.push_back(plan_c3);
    raster_segment.push_back(plan_c4);
    raster_segment.push_back(plan_c5);
    program.push_back(raster_segment);
  }

  {
    MoveInstruction plan_f1(wp1, MoveInstructionType::FREESPACE, "freespace_profile");
    plan_f1.setDescription("transition_from_end_plan");
    CompositeInstruction transition_from_end;
    transition_from_end.setDescription("transition_from_end");
    transition_from_end.push_back(plan_f1);
    CompositeInstruction transition_from_start;
    transition_from_start.setDescription("transition_from_start");
    transition_from_start.push_back(plan_f1);

    CompositeInstruction transitions(DEFAULT_PROFILE_KEY, CompositeInstructionOrder::UNORDERED);
    transitions.setDescription("transitions");
    transitions.push_back(transition_from_start);
    transitions.push_back(transition_from_end);
    program.push_back(transitions);
  }

  {
    CompositeInstruction raster_segment;
    raster_segment.setDescription("raster_segment");
    raster_segment.push_back(plan_c0);
    raster_segment.push_back(plan_c1);
    raster_segment.push_back(plan_c2);
    raster_segment.push_back(plan_c3);
    raster_segment.push_back(plan_c4);
    raster_segment.push_back(plan_c5);
    program.push_back(raster_segment);
  }

  {
    MoveInstruction plan_f1(wp1, MoveInstructionType::FREESPACE, "freespace_profile");
    plan_f1.setDescription("transition_from_end_plan");
    CompositeInstruction transition_from_end;
    transition_from_end.setDescription("transition_from_end");
    transition_from_end.push_back(plan_f1);
    CompositeInstruction transition_from_start;
    transition_from_start.setDescription("transition_from_start");
    transition_from_start.push_back(plan_f1);

    CompositeInstruction transitions(DEFAULT_PROFILE_KEY, CompositeInstructionOrder::UNORDERED);
    transitions.setDescription("transitions");
    transitions.push_back(transition_from_start);
    transitions.push_back(transition_from_end);
    program.push_back(transitions);
  }

  {
    CompositeInstruction raster_segment;
    raster_segment.setDescription("raster_segment");
    raster_segment.push_back(plan_c0);
    raster_segment.push_back(plan_c1);
    raster_segment.push_back(plan_c2);
    raster_segment.push_back(plan_c3);
    raster_segment.push_back(plan_c4);
    raster_segment.push_back(plan_c5);
    program.push_back(raster_segment);
  }

  MoveInstruction plan_f2(wp1, MoveInstructionType::FREESPACE, "freespace_profile");
  plan_f2.setDescription("to_end_plan");
  CompositeInstruction to_end;
  to_end.setDescription("to_end");
  to_end.push_back(plan_f2);
  program.push_back(to_end);

  // Add a wait instruction
  WaitInstruction wait_instruction_time(1.5);
  program.push_back(wait_instruction_time);

  WaitInstruction wait_instruction_io(WaitInstructionType::DIGITAL_INPUT_LOW, 10);
  program.push_back(wait_instruction_io);

  // Add a timer instruction
  TimerInstruction timer_instruction(TimerInstructionType::DIGITAL_OUTPUT_LOW, 3.1, 5);
  program.push_back(timer_instruction);

  // Add a set tool instruction
  SetToolInstruction set_tool_instruction(5);
  program.push_back(set_tool_instruction);

  // Add a set tool instruction
  SetAnalogInstruction set_analog_instruction("R", 0, 1.5);
  program.push_back(set_analog_instruction);

  return program;
}

std::vector<Waypoint> createVectorStateWaypointPoly()
{
  std::vector<Waypoint> results;
  results.reserve(1000);
  for (std::size_t i = 0; i < 1000; ++i)
  {
    Waypoint wp1 = JointWaypoint({ "j1", "j2", "j3", "j4", "j5", "j6" }, Eigen::VectorXd::Ones(6));

    results.push_back(std::move(wp1));
  }
  return results;
}

std::vector<std::unique_ptr<StateWaypoint>> createVectorStateWaypointUPtr()
{
  std::vector<std::unique_ptr<StateWaypoint>> results;
  results.reserve(1000);
  for (std::size_t i = 0; i < 1000; ++i)
  {
    std::vector<std::string> joint_names = { "j1", "j2", "j3", "j4", "j5", "j6" };
    auto wp1 = std::make_unique<StateWaypoint>(joint_names, Eigen::VectorXd::Ones(6));

    results.push_back(std::move(wp1));
  }
  return results;
}

static void BM_InstructionPolyCreation(benchmark::State& state)
{
  for (auto _ : state)
    Instruction i{ NullInstruction() };
}

BENCHMARK(BM_InstructionPolyCreation);

static void BM_WaypointPolyCreation(benchmark::State& state)
{
  for (auto _ : state)
    Waypoint w{ NullWaypoint() };
}

BENCHMARK(BM_WaypointPolyCreation);

static void BM_CompositeInstructionCreation(benchmark::State& state)
{
  for (auto _ : state)
    CompositeInstruction ci;
}

BENCHMARK(BM_CompositeInstructionCreation);

static void BM_ProgramCreation(benchmark::State& state)
{
  for (auto _ : state)
    CompositeInstruction ci = getProgram();
}

BENCHMARK(BM_ProgramCreation);

static void BM_InstructionPolyCopy(benchmark::State& state)
{
  Instruction i{ MoveInstruction() };
  for (auto _ : state)
    Instruction copy(i);
}

BENCHMARK(BM_InstructionPolyCopy);

static void BM_WaypointPolyCopy(benchmark::State& state)
{
  Waypoint w{ StateWaypoint() };
  for (auto _ : state)
    Waypoint copy(w);
}

BENCHMARK(BM_WaypointPolyCopy);

static void BM_CompositeInstructionCopy(benchmark::State& state)
{
  CompositeInstruction ci = getProgram();
  for (auto _ : state)
    CompositeInstruction copy(ci);
}

BENCHMARK(BM_CompositeInstructionCopy);

static void BM_InstructionPolyAssign(benchmark::State& state)
{
  Instruction i{ MoveInstruction() };
  for (auto _ : state)
    Instruction copy = i;
}

BENCHMARK(BM_InstructionPolyAssign);

static void BM_WaypointPolyAssign(benchmark::State& state)
{
  Waypoint w{ StateWaypoint() };
  for (auto _ : state)
    Waypoint copy = w;
}

BENCHMARK(BM_WaypointPolyAssign);

static void BM_CompositeInstructionAssign(benchmark::State& state)
{
  CompositeInstruction ci = getProgram();
  for (auto _ : state)
    CompositeInstruction copy = ci;
}

BENCHMARK(BM_CompositeInstructionAssign);

static void BM_InstructionPolyCast(benchmark::State& state)
{
  Instruction i{ MoveInstruction() };
  for (auto _ : state)
    auto& mi = i.as<MoveInstruction>();  // NOLINT
}

BENCHMARK(BM_InstructionPolyCast);

static void BM_WaypointPolyCast(benchmark::State& state)
{
  Waypoint w{ StateWaypoint() };
  for (auto _ : state)
    auto& sw = w.as<StateWaypoint>();  // NOLINT
}

BENCHMARK(BM_WaypointPolyCast);

static void BM_InstructionPolyAccess(benchmark::State& state)
{
  Instruction i{ MoveInstruction() };
  for (auto _ : state)
    const std::string& description = i.getDescription();  // NOLINT
}

BENCHMARK(BM_InstructionPolyAccess);

static void BM_WaypointPolyAccess(benchmark::State& state)
{
  Waypoint w{ StateWaypoint() };
  for (auto _ : state)
    std::type_index type = w.getType();
}

BENCHMARK(BM_WaypointPolyAccess);

static void BM_VectorStateWaypointPolyCreation(benchmark::State& state)
{
  for (auto _ : state)
    std::vector<Waypoint> w = createVectorStateWaypointPoly();
}

BENCHMARK(BM_VectorStateWaypointPolyCreation);

static void BM_VectorStateWaypointUPtrCreation(benchmark::State& state)
{
  for (auto _ : state)
    std::vector<std::unique_ptr<StateWaypoint>> w = createVectorStateWaypointUPtr();
}

BENCHMARK(BM_VectorStateWaypointUPtrCreation);

static void BM_VectorStateWaypointPolyCopy(benchmark::State& state)
{
  std::vector<Waypoint> w = createVectorStateWaypointPoly();
  for (auto _ : state)
    std::vector<Waypoint> copy(w);
}

BENCHMARK(BM_VectorStateWaypointPolyCopy);

static void BM_VectorStateWaypointUPtrCopy(benchmark::State& state)
{
  std::vector<std::unique_ptr<StateWaypoint>> w = createVectorStateWaypointUPtr();
  for (auto _ : state)
  {
    std::vector<std::unique_ptr<StateWaypoint>> copy;
    copy.reserve(w.size());
    for (const auto& i : w)
      copy.emplace_back(std::make_unique<StateWaypoint>(*i));
  }
}

BENCHMARK(BM_VectorStateWaypointUPtrCopy);

BENCHMARK_MAIN();
