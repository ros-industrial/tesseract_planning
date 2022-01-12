/**
 * @file move_instruction_unit.cpp
 * @brief Contains unit tests for MoveInstruction
 *
 * @author Levi Armstrong
 * @date February 15, 2021
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
#include <gtest/gtest.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <fstream>
TESSERACT_COMMON_IGNORE_WARNINGS_POP
#include <tesseract_command_language/core/instruction.h>
#include <tesseract_command_language/core/serialization.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/state_waypoint.h>
#include <tesseract_command_language/null_instruction.h>

using namespace tesseract_planning;

TEST(TesseractCommandLanguageMoveInstructionUnit, constructor)  // NOLINT
{
  Eigen::VectorXd jv = Eigen::VectorXd::Ones(6);
  std::vector<std::string> jn = { "j1", "j2", "j3", "j4", "j5", "j6" };
  StateWaypoint swp(jn, jv);

  // Minimum arguments
  {
    MoveInstruction instr(swp, MoveInstructionType::START);
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::START);
    EXPECT_EQ(instr.getProfile(), DEFAULT_PROFILE_KEY);
    EXPECT_TRUE(instr.getPathProfile().empty());
    EXPECT_FALSE(instr.getDescription().empty());
  }

  {
    MoveInstruction instr(swp, MoveInstructionType::FREESPACE);
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::FREESPACE);
    EXPECT_EQ(instr.getProfile(), DEFAULT_PROFILE_KEY);
    EXPECT_TRUE(instr.getPathProfile().empty());
    EXPECT_FALSE(instr.getDescription().empty());
  }

  {
    MoveInstruction instr(swp, MoveInstructionType::LINEAR);
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::LINEAR);
    EXPECT_EQ(instr.getProfile(), DEFAULT_PROFILE_KEY);
    EXPECT_EQ(instr.getPathProfile(), DEFAULT_PROFILE_KEY);
    EXPECT_FALSE(instr.getDescription().empty());
  }

  // With plan profile
  {
    MoveInstruction instr(swp, MoveInstructionType::START, "TEST_PROFILE");
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::START);
    EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");
    EXPECT_TRUE(instr.getPathProfile().empty());
    EXPECT_FALSE(instr.getDescription().empty());
  }

  {
    MoveInstruction instr(swp, MoveInstructionType::FREESPACE, "TEST_PROFILE");
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::FREESPACE);
    EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");
    EXPECT_TRUE(instr.getPathProfile().empty());
    EXPECT_FALSE(instr.getDescription().empty());
  }

  {
    MoveInstruction instr(swp, MoveInstructionType::LINEAR, "TEST_PROFILE");
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::LINEAR);
    EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");
    EXPECT_EQ(instr.getPathProfile(), "TEST_PROFILE");
    EXPECT_FALSE(instr.getDescription().empty());
  }

  // With plan and path profile
  {
    MoveInstruction instr(swp, MoveInstructionType::START, "TEST_PROFILE", "TEST_PATH_PROFILE");
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::START);
    EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");
    EXPECT_EQ(instr.getPathProfile(), "TEST_PATH_PROFILE");
    EXPECT_FALSE(instr.getDescription().empty());
  }

  {
    MoveInstruction instr(swp, MoveInstructionType::FREESPACE, "TEST_PROFILE", "TEST_PATH_PROFILE");
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::FREESPACE);
    EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");
    EXPECT_EQ(instr.getPathProfile(), "TEST_PATH_PROFILE");
    EXPECT_FALSE(instr.getDescription().empty());
  }

  {
    MoveInstruction instr(swp, MoveInstructionType::LINEAR, "TEST_PROFILE", "TEST_PATH_PROFILE");
    EXPECT_EQ(instr.getWaypoint(), swp);
    EXPECT_EQ(instr.getMoveType(), MoveInstructionType::LINEAR);
    EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");
    EXPECT_EQ(instr.getPathProfile(), "TEST_PATH_PROFILE");
    EXPECT_FALSE(instr.getDescription().empty());
  }
}

TEST(TesseractCommandLanguageMoveInstructionUnit, setters)  // NOLINT
{
  Eigen::VectorXd jv = Eigen::VectorXd::Ones(6);
  std::vector<std::string> jn = { "j1", "j2", "j3", "j4", "j5", "j6" };
  StateWaypoint swp(jn, jv);

  MoveInstruction instr(swp, MoveInstructionType::START);
  EXPECT_EQ(instr.getWaypoint(), swp);
  EXPECT_EQ(instr.getMoveType(), MoveInstructionType::START);
  EXPECT_EQ(instr.getProfile(), DEFAULT_PROFILE_KEY);
  EXPECT_TRUE(instr.getPathProfile().empty());
  EXPECT_FALSE(instr.getDescription().empty());

  StateWaypoint test_swp(jn, 5 * jv);
  instr.setWaypoint(test_swp);
  EXPECT_EQ(instr.getWaypoint(), test_swp);

  instr.setMoveType(MoveInstructionType::LINEAR);
  EXPECT_EQ(instr.getMoveType(), MoveInstructionType::LINEAR);

  instr.setProfile("TEST_PROFILE");
  EXPECT_EQ(instr.getProfile(), "TEST_PROFILE");

  instr.setPathProfile("TEST_PATH_PROFILE");
  EXPECT_EQ(instr.getPathProfile(), "TEST_PATH_PROFILE");

  instr.setDescription("This is a test.");
  EXPECT_EQ(instr.getDescription(), "This is a test.");
}

TEST(TesseractCommandLanguageMoveInstructionUnit, boostSerialization)  // NOLINT
{
  Eigen::VectorXd jv = Eigen::VectorXd::Ones(6);
  std::vector<std::string> jn = { "j1", "j2", "j3", "j4", "j5", "j6" };
  StateWaypoint swp(jn, jv);

  MoveInstruction instr(swp, MoveInstructionType::START);
  instr.setMoveType(MoveInstructionType::LINEAR);
  instr.setProfile("TEST_PROFILE");
  instr.setPathProfile("TEST_PATH_PROFILE");
  instr.setDescription("This is a test.");

  Serialization::toArchiveFileXML<Instruction>(instr, "/tmp/move_instruction_boost.xml");

  auto ninstr = Serialization::fromArchiveFileXML<Instruction>("/tmp/move_instruction_boost.xml").as<MoveInstruction>();

  EXPECT_TRUE(instr == ninstr);
  EXPECT_EQ(ninstr.getWaypoint(), swp);
  EXPECT_EQ(ninstr.getMoveType(), MoveInstructionType::LINEAR);
  EXPECT_EQ(ninstr.getProfile(), "TEST_PROFILE");
  EXPECT_EQ(ninstr.getPathProfile(), "TEST_PATH_PROFILE");
  EXPECT_EQ(ninstr.getDescription(), "This is a test.");
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
