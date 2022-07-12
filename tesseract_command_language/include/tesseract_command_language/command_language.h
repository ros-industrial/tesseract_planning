/**
 * @file command_language.h
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
#ifndef TESSERACT_COMMAND_LANGUAGE_COMMAND_LANGUAGE_H
#define TESSERACT_COMMAND_LANGUAGE_COMMAND_LANGUAGE_H

#include <tesseract_command_language/types.h>
#include <tesseract_command_language/null_instruction.h>
#include <tesseract_command_language/null_waypoint.h>
#include <tesseract_command_language/cartesian_waypoint.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/constants.h>
#include <tesseract_command_language/instruction_type.h>
#include <tesseract_command_language/joint_waypoint.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/state_waypoint.h>
#include <tesseract_command_language/waypoint_type.h>
#include <tesseract_command_language/timer_instruction.h>
#include <tesseract_command_language/wait_instruction.h>
#include <tesseract_command_language/set_tool_instruction.h>
#include <tesseract_command_language/set_analog_instruction.h>

#ifdef SWIG
%include "tesseract_command_language/instruction_type.h"
%include "tesseract_command_language/null_instruction.h"
%include "tesseract_command_language/null_waypoint.h"
%include "tesseract_command_language/cartesian_waypoint.h"
%include "tesseract_command_language/composite_instruction.h"
%include "tesseract_command_language/joint_waypoint.h"
%include "tesseract_command_language/move_instruction.h"
%include "tesseract_command_language/state_waypoint.h"
%include "tesseract_command_language/waypoint_type.h"
%include "tesseract_command_language/timer_instruction.h"
%include "tesseract_command_language/wait_instruction.h"
%include "tesseract_command_language/set_tool_instruction.h"
%include "tesseract_command_language/set_analog_instruction.h"
#endif  // SWIG

#endif  // TESSERACT_COMMAND_LANGUAGE_WAYPOINT_TYPE_H
