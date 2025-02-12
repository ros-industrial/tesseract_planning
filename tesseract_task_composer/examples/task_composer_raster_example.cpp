
#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <fstream>
#include <iostream>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_task_composer/core/task_composer_context.h>
#include <tesseract_task_composer/core/task_composer_future.h>
#include <tesseract_task_composer/core/task_composer_node_info.h>
#include <tesseract_task_composer/core/task_composer_data_storage.h>
#include <tesseract_task_composer/core/task_composer_graph.h>
#include <tesseract_task_composer/core/task_composer_executor.h>
#include <tesseract_task_composer/core/task_composer_plugin_factory.h>
#include <tesseract_task_composer/core/test_suite/test_programs.hpp>

#include <tesseract_common/types.h>
#include <tesseract_common/utils.h>
#include <tesseract_state_solver/state_solver.h>
#include <tesseract_environment/environment.h>
#include <tesseract_command_language/profile_dictionary.h>
#include <tesseract_command_language/utils.h>
#include <tesseract_visualization/visualization.h>
#include <tesseract_visualization/visualization_loader.h>
#include <tesseract_common/resource_locator.h>

using namespace tesseract_planning;

int main()
{
  // --------------------
  // Perform setup
  // --------------------
  auto locator = std::make_shared<tesseract_common::GeneralResourceLocator>();
  auto env = std::make_shared<tesseract_environment::Environment>();
  tesseract_common::fs::path urdf_path(
      locator->locateResource("package://tesseract_support/urdf/abb_irb2400.urdf")->getFilePath());
  tesseract_common::fs::path srdf_path(
      locator->locateResource("package://tesseract_support/urdf/abb_irb2400.srdf")->getFilePath());
  env->init(urdf_path, srdf_path, locator);

  // Dynamically load ignition visualizer if exist
  tesseract_visualization::VisualizationLoader loader;
  auto plotter = loader.get();

  if (plotter != nullptr)
  {
    plotter->waitForConnection(3);
    if (plotter->isConnected())
      plotter->plotEnvironment(*env);
  }
  // Get plugin factory
  auto resource = locator->locateResource("package://tesseract_task_composer/config/task_composer_plugins.yaml");
  tesseract_common::fs::path config_path(resource->getFilePath());
  TaskComposerPluginFactory factory(config_path, *resource);

  // Create raster task
  TaskComposerNode::UPtr task = factory.createTaskComposerNode("RasterFtPipeline");
  const std::string input_key = task->getInputKeys().get("program");
  const std::string output_key = task->getOutputKeys().get("program");

  // Define profiles
  auto profiles = std::make_shared<ProfileDictionary>();
  /** @todo Add default profiles */

  // Define the program
  CompositeInstruction program = test_suite::rasterExampleProgram();
  program.print();

  // Create data storage
  auto task_data = std::make_unique<TaskComposerDataStorage>();
  task_data->setData(input_key, program);
  task_data->setData("environment", std::shared_ptr<const tesseract_environment::Environment>(env));
  task_data->setData("profiles", profiles);

  // Solve raster plan
  auto task_executor = factory.createTaskComposerExecutor("TaskflowExecutor");
  TaskComposerFuture::UPtr future = task_executor->run(*task, std::move(task_data), true);
  future->wait();

  // Save dot graph
  std::ofstream tc_out_data;
  tc_out_data.open(tesseract_common::getTempPath() + "task_composer_raster_example.dot");
  task->dump(tc_out_data, nullptr, future->context->task_infos.getInfoMap());
  tc_out_data.close();

  // Plot Process Trajectory
  auto output_program = future->context->data_storage->getData(output_key).as<CompositeInstruction>();
  if (plotter != nullptr && plotter->isConnected())
  {
    plotter->waitForInput();
    plotter->plotTrajectory(toJointTrajectory(output_program), *env->getStateSolver());
  }

  std::cout << "Execution Complete\n";

  //  // Print summary statistics
  //  std::map<std::size_t, TaskInfo::UPtr> info_map = response.interface->getTaskInfoMap();
  //  TaskInfoProfiler profiler;
  //  profiler.load(info_map);
  //  profiler.print();

  return 0;
}
