// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Focus/FocusDispatcher.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>

#include <ossia/detail/math.hpp>

#include <Spline/SplineModel.hpp>
#include <Spline/SplinePresenter.hpp>
#include <Spline/SplineView.hpp>
#include <wobjectimpl.h>
namespace Spline
{
Presenter::Presenter(
    const Spline::ProcessModel& layer,
    View* view,
    const Process::Context& ctx,
    QObject* parent)
    : LayerPresenter{layer, view, ctx, parent}, m_view{view}
{
  putToFront();
  connect(&layer, &ProcessModel::splineChanged, this, [&] { m_view->setSpline(layer.spline()); });

  m_view->setSpline(layer.spline());
  connect(m_view, &View::changed, this, [&] {
    context().context.dispatcher.submit<ChangeSpline>(layer, m_view->spline());
  });
  connect(m_view, &View::released, this, [&] {
    context().context.dispatcher.commit();
  });

  connect(m_view, &View::pressed, this, [&] { m_context.context.focusDispatcher.focus(this); });
  connect(m_view, &View::askContextMenu, this, &Presenter::contextMenuRequested);
}

void Presenter::setWidth(qreal val, qreal defaultWidth)
{
  m_view->setWidth(val);
  m_view->recenter();
}

void Presenter::setHeight(qreal val)
{
  m_view->setHeight(val);
  m_view->recenter();
}

void Presenter::putToFront()
{
  m_view->setEnabled(true);
}

void Presenter::putBehind()
{
  m_view->setEnabled(false);
}

void Presenter::on_zoomRatioChanged(ZoomRatio r)
{
  parentGeometryChanged();
}

void Presenter::parentGeometryChanged() { }

}
