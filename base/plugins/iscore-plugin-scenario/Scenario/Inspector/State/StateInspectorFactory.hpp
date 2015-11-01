#pragma once
#include <QObject>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>


class StateInspectorFactory final : public InspectorWidgetFactory
{
    public:
        StateInspectorFactory() :
            InspectorWidgetFactory {}
        {

        }

        virtual InspectorWidgetBase* makeWidget(
                const QObject& sourceElement,
                iscore::Document& doc,
                QWidget* parent) override;

        virtual QList<QString> correspondingObjectsNames() const override;
};
