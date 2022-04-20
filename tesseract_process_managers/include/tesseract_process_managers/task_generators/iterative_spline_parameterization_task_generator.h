/**
 * @file iterative_spline_parameterization_task_generator.h
 * @brief Perform iterative spline time parameterization
 *
 * @author Levi Armstrong
 * @date August 11. 2020
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
#ifndef TESSERACT_PROCESS_MANAGERS_ITERATIVE_SPLINE_PARAMETERIZATION_TASK_GENERATOR_H
#define TESSERACT_PROCESS_MANAGERS_ITERATIVE_SPLINE_PARAMETERIZATION_TASK_GENERATOR_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <boost/serialization/access.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_process_managers/core/task_generator.h>
#include <tesseract_time_parameterization/iterative_spline_parameterization.h>
#include <tesseract_process_managers/core/default_task_namespaces.h>

#ifdef SWIG
%shared_ptr(tesseract_planning::IterativeSplineParameterizationProfile)
%ignore IterativeSplineParameterizationTaskGenerator;
%ignore IterativeSplineParameterizationTaskInfo;
#endif  // SWIG

namespace tesseract_planning
{
class IterativeSplineParameterizationTaskGenerator : public TaskGenerator
{
public:
  using UPtr = std::unique_ptr<IterativeSplineParameterizationTaskGenerator>;

  IterativeSplineParameterizationTaskGenerator(
      bool add_points = true,
      std::string name = profile_ns::ITERATIVE_SPLINE_PARAMETERIZATION_DEFAULT_NAMESPACE);

  ~IterativeSplineParameterizationTaskGenerator() override = default;
  IterativeSplineParameterizationTaskGenerator(const IterativeSplineParameterizationTaskGenerator&) = delete;
  IterativeSplineParameterizationTaskGenerator& operator=(const IterativeSplineParameterizationTaskGenerator&) = delete;
  IterativeSplineParameterizationTaskGenerator(IterativeSplineParameterizationTaskGenerator&&) = delete;
  IterativeSplineParameterizationTaskGenerator& operator=(IterativeSplineParameterizationTaskGenerator&&) = delete;

  int conditionalProcess(TaskInput input, std::size_t unique_id) const override final;

  void process(TaskInput input, std::size_t unique_id) const override final;

private:
  IterativeSplineParameterization solver_;
};

class IterativeSplineParameterizationTaskInfo : public TaskInfo
{
public:
  using Ptr = std::shared_ptr<IterativeSplineParameterizationTaskInfo>;
  using ConstPtr = std::shared_ptr<const IterativeSplineParameterizationTaskInfo>;

  IterativeSplineParameterizationTaskInfo() = default;
  IterativeSplineParameterizationTaskInfo(
      std::size_t unique_id,
      std::string name = profile_ns::ITERATIVE_SPLINE_PARAMETERIZATION_DEFAULT_NAMESPACE);

  TaskInfo::UPtr clone() const override;

  bool operator==(const IterativeSplineParameterizationTaskInfo& rhs) const;
  bool operator!=(const IterativeSplineParameterizationTaskInfo& rhs) const;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};
}  // namespace tesseract_planning

#include <boost/serialization/export.hpp>
#include <boost/serialization/tracking.hpp>
BOOST_CLASS_EXPORT_KEY2(tesseract_planning::IterativeSplineParameterizationTaskInfo,
                        "IterativeSplineParameterizationTaskInfo")
#endif  // TESSERACT_PROCESS_MANAGERS_ITERATIVE_SPLINE_PARAMETERIZATION_TASK_GENERATOR_H
