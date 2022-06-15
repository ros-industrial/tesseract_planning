/**
 * @file state_waypoint.h
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
#ifndef TESSERACT_COMMAND_LANGUAGE_STATE_WAYPOINT_H
#define TESSERACT_COMMAND_LANGUAGE_STATE_WAYPOINT_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <Eigen/Core>
#include <memory>
#include <vector>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/core/waypoint.h>
#include <tesseract_common/joint_state.h>
#include <tesseract_common/utils.h>
#include <tesseract_common/types.h>
#include <tesseract_common/serialization.h>
#include <tesseract_common/joint_state.h>

namespace tesseract_planning
{
class StateWaypoint : public tesseract_common::JointState
{
public:
  StateWaypoint() = default;
  StateWaypoint(std::vector<std::string> joint_names, const Eigen::Ref<const Eigen::VectorXd>& position);

  void print(const std::string& prefix = "") const;

  bool operator==(const StateWaypoint& rhs) const;
  bool operator!=(const StateWaypoint& rhs) const;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};
}  // namespace tesseract_planning

#ifdef SWIG
%tesseract_command_language_add_waypoint_type(StateWaypoint)
#else
TESSERACT_WAYPOINT_EXPORT_KEY(tesseract_planning, StateWaypoint);
#endif  // SWIG

#endif  // TESSERACT_COMMAND_LANGUAGE_JOINT_WAYPOINT_H
