/**
 * @file puzzle_piece_auxillary_axes_example.h
 * @brief An example of a robot with a two axis position leveraging trajopt
 * and tesseract to leverage all DOF to create an optimal motion trajectory
 * for a complex cartesian path.
 *
 * @author Levi Armstrong
 * @date July 22, 2018
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2017, Southwest Research Institute
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
#ifndef TESSERACT_EXAMPLES_PUZZLE_PIECE_AUXILLARY_AXES_EXAMPLE_H
#define TESSERACT_EXAMPLES_PUZZLE_PIECE_AUXILLARY_AXES_EXAMPLE_H

#include <tesseract_examples/example.h>

namespace tesseract_examples
{
/**
 * @brief An example of a robot with a two axis position leveraging trajopt
 * and tesseract to leverage all DOF to create an optimal motion trajectory
 * for a complex cartesian path.
 */
class PuzzlePieceAuxillaryAxesExample : public Example
{
public:
  PuzzlePieceAuxillaryAxesExample(std::shared_ptr<tesseract_environment::Environment> env,
                                  std::shared_ptr<tesseract_visualization::Visualization> plotter = nullptr,
                                  bool ifopt = false,
                                  bool debug = false);
  ~PuzzlePieceAuxillaryAxesExample() override = default;
  PuzzlePieceAuxillaryAxesExample(const PuzzlePieceAuxillaryAxesExample&) = default;
  PuzzlePieceAuxillaryAxesExample& operator=(const PuzzlePieceAuxillaryAxesExample&) = default;
  PuzzlePieceAuxillaryAxesExample(PuzzlePieceAuxillaryAxesExample&&) = default;
  PuzzlePieceAuxillaryAxesExample& operator=(PuzzlePieceAuxillaryAxesExample&&) = default;

  bool run() override final;

private:
  bool ifopt_;
  bool debug_;
};

}  // namespace tesseract_examples

#endif  // TESSERACT_EXAMPLES_PUZZLE_PIECE_AUXILLARY_AXES_EXAMPLE_H
