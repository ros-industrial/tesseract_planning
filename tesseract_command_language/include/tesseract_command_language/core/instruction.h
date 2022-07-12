/**
 * @file instruction.h
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
#ifndef TESSERACT_COMMAND_LANGUAGE_INSTRUCTION_H
#define TESSERACT_COMMAND_LANGUAGE_INSTRUCTION_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <string>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/concept_check.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/core/waypoint.h>
#include <tesseract_common/serialization.h>
#include <tesseract_common/type_erasure.h>

#ifdef SWIG
%ignore std::vector<tesseract_planning::Instruction>::vector(size_type);
%ignore std::vector<tesseract_planning::Instruction>::resize(size_type);
%ignore tesseract_planning::Instruction::getType;
%pythondynamic tesseract_planning::Instruction;
#endif  // SWIG

/** @brief If shared library, this must go in the header after the class definition */
#define TESSERACT_INSTRUCTION_EXPORT_KEY(N, C)                                                                         \
  namespace N                                                                                                          \
  {                                                                                                                    \
  using C##InstanceBase =                                                                                              \
      tesseract_common::TypeErasureInstance<C, tesseract_planning::detail_instruction::InstructionInterface>;          \
  using C##Instance = tesseract_planning::detail_instruction::InstructionInstance<C>;                                  \
  using C##InstanceWrapper = tesseract_common::TypeErasureInstanceWrapper<C##Instance>;                                \
  }                                                                                                                    \
  BOOST_CLASS_EXPORT_KEY(N::C##InstanceBase)                                                                           \
  BOOST_CLASS_EXPORT_KEY(N::C##Instance)                                                                               \
  BOOST_CLASS_EXPORT_KEY(N::C##InstanceWrapper)                                                                        \
  BOOST_CLASS_TRACKING(N::C##InstanceBase, boost::serialization::track_never)                                          \
  BOOST_CLASS_TRACKING(N::C##Instance, boost::serialization::track_never)                                              \
  BOOST_CLASS_TRACKING(N::C##InstanceWrapper, boost::serialization::track_never)

/** @brief If shared library, this must go in the cpp after the implicit instantiation of the serialize function */
#define TESSERACT_INSTRUCTION_EXPORT_IMPLEMENT(inst)                                                                   \
  BOOST_CLASS_EXPORT_IMPLEMENT(inst##InstanceBase)                                                                     \
  BOOST_CLASS_EXPORT_IMPLEMENT(inst##Instance)                                                                         \
  BOOST_CLASS_EXPORT_IMPLEMENT(inst##InstanceWrapper)

/**
 * @brief This should not be used within shared libraries use the two above.
 * If not in a shared library it can go in header or cpp
 */
#define TESSERACT_INSTRUCTION_EXPORT(N, C)                                                                             \
  TESSERACT_INSTRUCTION_EXPORT_KEY(N, C)                                                                               \
  TESSERACT_INSTRUCTION_EXPORT_IMPLEMENT(N::C)

#ifndef SWIG
namespace tesseract_planning::detail_instruction
{
template <typename T>
struct InstructionConcept  // NOLINT
  : boost::Assignable<T>,
    boost::CopyConstructible<T>,
    boost::EqualityComparable<T>
{
  BOOST_CONCEPT_USAGE(InstructionConcept)
  {
    T cp(c);
    T assign = c;
    bool eq = (c == cp);
    bool neq = (c != cp);
    UNUSED(assign);
    UNUSED(eq);
    UNUSED(neq);

    const std::string& desc = c.getDescription();
    UNUSED(desc);

    c.setDescription("test");
    c.print();
    c.print("prefix_");
  }

private:
  T c;
};

struct InstructionInterface : tesseract_common::TypeErasureInterface
{
  virtual const std::string& getDescription() const = 0;

  virtual void setDescription(const std::string& description) = 0;

  virtual void print(const std::string& prefix) const = 0;

private:
  friend class boost::serialization::access;
  friend struct tesseract_common::Serialization;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};

template <typename T>
struct InstructionInstance : tesseract_common::TypeErasureInstance<T, InstructionInterface>  // NOLINT
{
  using BaseType = tesseract_common::TypeErasureInstance<T, InstructionInterface>;
  InstructionInstance() = default;
  InstructionInstance(const T& x) : BaseType(x) {}
  InstructionInstance(InstructionInstance&& x) noexcept : BaseType(std::move(x)) {}

  BOOST_CONCEPT_ASSERT((InstructionConcept<T>));

  const std::string& getDescription() const final { return this->get().getDescription(); }

  void setDescription(const std::string& description) final { this->get().setDescription(description); }

  void print(const std::string& prefix) const final { this->get().print(prefix); }

private:
  friend class boost::serialization::access;
  friend struct tesseract_common::Serialization;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/)  // NOLINT
  {
    ar& boost::serialization::make_nvp("base", boost::serialization::base_object<BaseType>(*this));
  }
};
}  // namespace tesseract_planning::detail_instruction
#endif  // SWIG

namespace tesseract_planning
{
using InstructionBase = tesseract_common::TypeErasureBase<detail_instruction::InstructionInterface,
                                                          detail_instruction::InstructionInstance>;
struct Instruction : InstructionBase
{
  using InstructionBase::InstructionBase;

  const std::string& getDescription() const;

  void setDescription(const std::string& description);

  void print(const std::string& prefix = "") const;

private:
  friend class boost::serialization::access;
  friend struct tesseract_common::Serialization;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/);  // NOLINT
};

}  // namespace tesseract_planning

#ifdef SWIG
%template(Instructions) std::vector<tesseract_planning::Instruction>;
#else
BOOST_SERIALIZATION_ASSUME_ABSTRACT(tesseract_planning::detail_instruction::InstructionInterface)
BOOST_CLASS_EXPORT_KEY(tesseract_planning::detail_instruction::InstructionInterface)
BOOST_CLASS_TRACKING(tesseract_planning::detail_instruction::InstructionInterface, boost::serialization::track_never)

BOOST_CLASS_EXPORT_KEY(tesseract_planning::InstructionBase)
BOOST_CLASS_TRACKING(tesseract_planning::InstructionBase, boost::serialization::track_never)

BOOST_CLASS_EXPORT_KEY(tesseract_planning::Instruction)
BOOST_CLASS_TRACKING(tesseract_planning::Instruction, boost::serialization::track_never);
#endif  // SWIG

#endif  // TESSERACT_COMMAND_LANGUAGE_INSTRUCTION_H
