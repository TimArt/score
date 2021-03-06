#include "score_plugin_gfx.hpp"

#include <score/plugins/FactorySetup.hpp>

#include <Gfx/CommandFactory.hpp>
#include <Gfx/Filter/Executor.hpp>
#include <Gfx/Filter/Layer.hpp>
#include <Gfx/Filter/Process.hpp>
#include <Gfx/GfxApplicationPlugin.hpp>
#include <Gfx/GfxDevice.hpp>
#include <Gfx/CameraDevice.hpp>
#include <Gfx/Images/Executor.hpp>
#include <Gfx/Images/Inspector.hpp>
#include <Gfx/Images/Layer.hpp>
#include <Gfx/Images/Process.hpp>
#include <Gfx/Mesh/Executor.hpp>
#include <Gfx/Mesh/Layer.hpp>
#include <Gfx/Mesh/Process.hpp>
#include <Gfx/TexturePort.hpp>
#include <Gfx/WindowDevice.hpp>
#include <Gfx/Video/Executor.hpp>
#include <Gfx/Video/Inspector.hpp>
#include <Gfx/Video/Layer.hpp>
#include <Gfx/Video/Process.hpp>
#if defined(_WIN32)
#include <Gfx/SpoutDevice.hpp>
#endif
#include <score_plugin_gfx_commands_files.hpp>
#include <score_plugin_engine.hpp>

score_plugin_gfx::score_plugin_gfx() { }

score_plugin_gfx::~score_plugin_gfx() { }

std::vector<std::unique_ptr<score::InterfaceBase>> score_plugin_gfx::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<Device::ProtocolFactory
      , Gfx::WindowProtocolFactory
      , Gfx::CameraProtocolFactory
#if defined(_WIN32)
      , Gfx::SpoutProtocolFactory
#endif
      >,
      FW<Process::ProcessModelFactory,
         Gfx::Filter::ProcessFactory,
         Gfx::Video::ProcessFactory,
         Gfx::Mesh::ProcessFactory,
         Gfx::Images::ProcessFactory>,
      FW<Process::LayerFactory,
         Gfx::Filter::LayerFactory,
         Gfx::Video::LayerFactory,
         Gfx::Mesh::LayerFactory,
         Gfx::Images::LayerFactory>,
      FW<Execution::ProcessComponentFactory,
         Gfx::Filter::ProcessExecutorComponentFactory,
         Gfx::Video::ProcessExecutorComponentFactory,
         Gfx::Mesh::ProcessExecutorComponentFactory,
         Gfx::Images::ProcessExecutorComponentFactory>,
      FW<Inspector::InspectorWidgetFactory,
         Gfx::Video::InspectorFactory,
         Gfx::Images::InspectorFactory>,
      FW<Process::PortFactory, Gfx::TextureInletFactory, Gfx::TextureOutletFactory>,
      FW<Process::ProcessDropHandler,
         Gfx::Filter::DropHandler,
         Gfx::Video::DropHandler,
         Gfx::Mesh::DropHandler,
         Gfx::Images::DropHandler>,
      FW<Library::LibraryInterface,
         Gfx::Filter::LibraryHandler,
         Gfx::Video::LibraryHandler,
         Gfx::Mesh::LibraryHandler,
         Gfx::Images::LibraryHandler>>(ctx, key);
}

score::GUIApplicationPlugin*
score_plugin_gfx::make_guiApplicationPlugin(const score::GUIApplicationContext& app)
{
  return new Gfx::ApplicationPlugin{app};
}

std::pair<const CommandGroupKey, CommandGeneratorMap> score_plugin_gfx::make_commands()
{
  using namespace Gfx;
  std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
      CommandFactoryName(), CommandGeneratorMap{}};

  ossia::for_each_type<
#include <score_plugin_gfx_commands.hpp>
      >(score::commands::FactoryInserter{cmds.second});

  return cmds;
}
auto score_plugin_gfx::required() const -> std::vector<score::PluginKey>
{
  return {score_plugin_engine::static_key()};
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_plugin_gfx)
