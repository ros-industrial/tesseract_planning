/**
 * @file instruction.cpp
 * @brief
 *
 * @author Levi Armstrong
 * @date June 15, 2020
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
#include <iostream>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/null_instruction.h>

namespace tesseract_planning
{
const std::string& NullInstruction::getDescription() const { return description_; }

void NullInstruction::setDescription(const std::string& description) { description_ = description; }

void NullInstruction::print(const std::string& prefix) const  // NOLINT
{
  std::cout << prefix + "Null Instruction, Description: " << getDescription() << std::endl;
}
bool NullInstruction::operator==(const NullInstruction& /*rhs*/) const { return true; }
bool NullInstruction::operator!=(const NullInstruction& /*rhs*/) const { return false; }

template <class Archive>
void NullInstruction::serialize(Archive& /*ar*/, const unsigned int /*version*/)
{
}
}  // namespace tesseract_planning

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::NullInstruction)
TESSERACT_INSTRUCTION_EXPORT_IMPLEMENT(tesseract_planning::NullInstruction);
