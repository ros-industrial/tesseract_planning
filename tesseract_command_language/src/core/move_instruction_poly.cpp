#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <boost/serialization/nvp.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/core/move_instruction_poly.h>

template <class Archive>
void tesseract_planning::detail_move_instruction::MoveInstructionInterface::serialize(
    Archive& ar,
    const unsigned int /*version*/)  // NOLINT
{
  ar& boost::serialization::make_nvp("base",
                                     boost::serialization::base_object<tesseract_common::TypeErasureInterface>(*this));
}

void tesseract_planning::MoveInstructionPoly::assignCartesianWaypoint(CartesianWaypointPoly waypoint)
{
  getInterface().assignCartesianWaypoint(std::move(waypoint));
}
void tesseract_planning::MoveInstructionPoly::assignJointWaypoint(JointWaypointPoly waypoint)
{
  getInterface().assignJointWaypoint(std::move(waypoint));
}
void tesseract_planning::MoveInstructionPoly::assignStateWaypoint(StateWaypointPoly waypoint)
{
  getInterface().assignStateWaypoint(std::move(waypoint));
}
tesseract_planning::Waypoint& tesseract_planning::MoveInstructionPoly::getWaypoint()
{
  return getInterface().getWaypoint();
}
const tesseract_planning::Waypoint& tesseract_planning::MoveInstructionPoly::getWaypoint() const
{
  return getInterface().getWaypoint();
}

void tesseract_planning::MoveInstructionPoly::setManipulatorInfo(tesseract_common::ManipulatorInfo info)
{
  getInterface().setManipulatorInfo(std::move(info));
}
const tesseract_common::ManipulatorInfo& tesseract_planning::MoveInstructionPoly::getManipulatorInfo() const
{
  return getInterface().getManipulatorInfo();
}
tesseract_common::ManipulatorInfo& tesseract_planning::MoveInstructionPoly::getManipulatorInfo()
{
  return getInterface().getManipulatorInfo();
}

void tesseract_planning::MoveInstructionPoly::setProfile(const std::string& profile)
{
  getInterface().setProfile(profile);
}
const std::string& tesseract_planning::MoveInstructionPoly::getProfile() const { return getInterface().getProfile(); }

void tesseract_planning::MoveInstructionPoly::setPathProfile(const std::string& profile)
{
  getInterface().setPathProfile(profile);
}
const std::string& tesseract_planning::MoveInstructionPoly::getPathProfile() const
{
  return getInterface().getPathProfile();
}

void tesseract_planning::MoveInstructionPoly::setMoveType(tesseract_planning::MoveInstructionType move_type)
{
  getInterface().setMoveType(move_type);
}
tesseract_planning::MoveInstructionType tesseract_planning::MoveInstructionPoly::getMoveType() const
{
  return getInterface().getMoveType();
}

const std::string& tesseract_planning::MoveInstructionPoly::getDescription() const
{
  return getInterface().getDescription();
}

void tesseract_planning::MoveInstructionPoly::setDescription(const std::string& description)
{
  getInterface().setDescription(description);
}

void tesseract_planning::MoveInstructionPoly::print(const std::string& prefix) const { getInterface().print(prefix); }

tesseract_planning::CartesianWaypointPoly tesseract_planning::MoveInstructionPoly::createCartesianWaypoint() const
{
  return getInterface().createCartesianWaypoint();
}
tesseract_planning::JointWaypointPoly tesseract_planning::MoveInstructionPoly::createJointWaypoint() const
{
  return getInterface().createJointWaypoint();
}
tesseract_planning::StateWaypointPoly tesseract_planning::MoveInstructionPoly::createStateWaypoint() const
{
  return getInterface().createStateWaypoint();
}

bool tesseract_planning::MoveInstructionPoly::isLinear() const
{
  return (getInterface().getMoveType() == MoveInstructionType::LINEAR);
}

bool tesseract_planning::MoveInstructionPoly::isFreespace() const
{
  return (getInterface().getMoveType() == MoveInstructionType::FREESPACE);
}

bool tesseract_planning::MoveInstructionPoly::isCircular() const
{
  return (getInterface().getMoveType() == MoveInstructionType::CIRCULAR);
}

bool tesseract_planning::MoveInstructionPoly::isStart() const
{
  return (getInterface().getMoveType() == MoveInstructionType::START);
}

template <class Archive>
void tesseract_planning::MoveInstructionPoly::serialize(Archive& ar, const unsigned int /*version*/)  // NOLINT
{
  ar& boost::serialization::make_nvp("base", boost::serialization::base_object<MoveInstructionPolyBase>(*this));
}

#include <tesseract_common/serialization.h>
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::detail_move_instruction::MoveInstructionInterface)
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::MoveInstructionPolyBase)
TESSERACT_SERIALIZE_ARCHIVES_INSTANTIATE(tesseract_planning::MoveInstructionPoly)

BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::detail_move_instruction::MoveInstructionInterface)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::MoveInstructionPolyBase)
BOOST_CLASS_EXPORT_IMPLEMENT(tesseract_planning::MoveInstructionPoly)

TESSERACT_INSTRUCTION_EXPORT_IMPLEMENT(tesseract_planning::MoveInstructionPoly);
