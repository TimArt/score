// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "score_git_info.hpp"

#include <QAction>
#include <QApplication>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <algorithm>
#include <core/view/QRecentFilesMenu.h>
#include <core/document/Document.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/presenter/Presenter.hpp>
#include <core/settings/Settings.hpp>
#include <core/settings/SettingsView.hpp>
#include <core/view/Window.hpp>
#include <cstdint>
#include <functional>
#include <qnamespace.h>
#include <score/actions/Menu.hpp>
#include <score/application/ApplicationComponents.hpp>
#include <score/model/Identifier.hpp>
#include <score/plugins/application/GUIApplicationPlugin.hpp>
#include <score/plugins/customfactory/StringFactoryKey.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <score/tools/std/Optional.hpp>
#include <sys/types.h>
#include <utility>
#include <vector>

#include <wobjectimpl.h>
W_OBJECT_IMPL(score::Presenter)
namespace score
{

  static auto get_menubar(View* view)
  {
#ifdef __APPLE__
    return view ? new QMenuBar : (QMenuBar*)nullptr;
#else
    return view ? view->menuBar() : (QMenuBar*)nullptr;
#endif
  }
Presenter::Presenter(
    const score::ApplicationSettings& app,
    const score::Settings& set,
    score::ProjectSettings& pset,
    View* view,
    QObject* arg_parent)
    : QObject{arg_parent}
    , m_view{view}
    , m_settings{set}
    , m_projectSettings{pset}
    , m_docManager{view, this}
    , m_components{}
    , m_components_readonly{m_components}
    , m_menubar{get_menubar(view)}
    , m_context{
        app,       m_components_readonly, m_docManager, m_menus, m_toolbars,
        m_actions, m_settings.settings(), m_view}
{
  m_docManager.init(m_context); // It is necessary to break
  // this dependency cycle.

  connect(
      &m_context.docManager, &DocumentManager::documentChanged, &m_actions,
      &ActionManager::reset);

  if (m_view)
    m_view->setPresenter(this);
}

bool Presenter::exit()
{
  return m_docManager.closeAllDocuments(m_context);
}

View* Presenter::view() const
{
  return m_view;
}

void Presenter::setupGUI()
{
  // 1. Show the menus
  // If the menu has no parent menu, we add it to the main menu bar.
  {
    std::vector<Menu> menus;
    menus.reserve(m_menus.get().size());
    for (auto& elt : m_menus.get())
    {
      if (elt.second.toplevel())
        menus.push_back(elt.second);
    }
    std::sort(menus.begin(), menus.end(), [](auto& lhs, auto& rhs) {
      return lhs.column() < rhs.column();
    });

    if (view())
    {
      for (Menu& menu : menus)
      {
        view()->menuBar()->addMenu(menu.menu());
      }
    }
  }

  // 2. Show the toolbars
  // Put them in a matrix corresponding to their organization
  {
    std::unordered_map<Qt::ToolBarArea, std::vector<Toolbar>> toolbars;

    for (auto& tb : m_toolbars.get())
    {
      toolbars[(Qt::ToolBarArea)tb.second.row()].push_back(tb.second);
    }

    if (!view())
      return;

    for(auto& tb : toolbars)
    {
      auto area = tb.first;
      auto& tb_row = tb.second;
      int i = 0;
      int n = toolbars.size();
      ossia::sort(tb_row, [](auto& lhs, auto& rhs) {
        return lhs.column() < rhs.column();
      });

      for (const Toolbar& tb : tb_row)
      {
        view()->addToolBar(area, tb.toolbar());
        tb.toolbar()->setFloatable(false);
        tb.toolbar()->setMovable(false);
      }

      i++;
      if (i < n - 1)
        view()->addToolBarBreak(area);
    }
  }
}

void Presenter::optimize()
{
  score::optimize_hash_map(m_components.commands);
  auto& com = m_components.commands;
  auto com_end = com.end();
  for (auto it = com.begin(); it != com_end; ++it)
  {
    score::optimize_hash_map(it.value());
  }

  score::optimize_hash_map(m_components.factories);
  for (auto& fact : m_components.factories)
  {
    fact.second->optimize();
  }

  score::optimize_hash_map(m_menus.get());
  score::optimize_hash_map(m_actions.get());
  score::optimize_hash_map(m_toolbars.get());
}
}
