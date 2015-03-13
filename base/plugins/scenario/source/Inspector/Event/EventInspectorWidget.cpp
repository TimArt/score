#include "EventInspectorWidget.hpp"

#include "Document/Event/EventModel.hpp"
#include "Document/Event/State/State.hpp"
#include "Commands/Event/AddStateToEvent.hpp"
#include "Commands/Event/SetCondition.hpp"
#include "Commands/Event/RemoveStateFromEvent.hpp"

#include "Document/TimeNode/TimeNodeModel.hpp"

#include <InspectorInterface/InspectorSectionWidget.hpp>
#include "Inspector/MetadataWidget.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QFormLayout>
#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QCompleter>
#include <QToolButton>

#include "Process/ScenarioModel.hpp"
#include "iscore/document/DocumentInterface.hpp"
#include "iscore/selection/SelectionDispatcher.hpp"
#include <DeviceInterface/DeviceCompleter.hpp>
#include <DeviceInterface/DeviceExplorerInterface.hpp>
#include "Document/Constraint/ConstraintModel.hpp"

// TODO : pour cohérence avec les autres inspectors : Scenario ou Senario::Commands ?
EventInspectorWidget::EventInspectorWidget(EventModel* object, QWidget* parent) :
    InspectorWidgetBase {object, parent},
    m_model {object}
{
    setObjectName("EventInspectorWidget");
    setInspectedObject(m_model);
    setParent(parent);

    connect(m_model, &EventModel::messagesChanged,
            this, &EventInspectorWidget::updateInspector);

    // date
    auto dateWid = new QWidget{this};
    auto dateLay = new QHBoxLayout{dateWid};
    auto dateTitle = new QLabel{tr("default date : ")};
    m_date = new QLabel{QString::number(object->date().msec()) };
    dateLay->addWidget(dateTitle);
    dateLay->addWidget(m_date);

    m_properties.push_back(dateWid);

    // timeNode

    QWidget* tnWid = new QWidget {this};
    QHBoxLayout* tnLay = new QHBoxLayout {tnWid};
    QPushButton* tnBtn = new QPushButton {tr("None"), tnWid};
    tnBtn->setFlat(true);
    tnLay->addWidget(new QLabel{tr("TimeNode :"), tnWid});
    tnLay->addWidget(tnBtn);

    auto timeNode = m_model->timeNode();
    if(timeNode)
    {
        tnBtn->setText(QString::number(*timeNode.val()));
        auto scenar = m_model->parentScenario();
        if (scenar)
            connect(tnBtn,  &QPushButton::clicked,
                    [=] () { selectionDispatcher()->send(Selection{scenar->timeNode(timeNode)}); });
    }

    m_properties.push_back(tnWid);

    // Condition
    m_conditionWidget = new QLineEdit{this};
    connect(m_conditionWidget, SIGNAL(editingFinished()),
            this,			 SLOT(on_conditionChanged()));

    // TODO : attention, ordre de m_properties utilisé (dans addAddress() !! faudrait changer ...
    m_properties.push_back(new QLabel{tr("Condition")});
    m_properties.push_back(m_conditionWidget);

    // State
    m_addressesWidget = new QWidget{this};
    auto dispLayout = new QVBoxLayout{m_addressesWidget};
    m_addressesWidget->setLayout(dispLayout);

    QWidget* addAddressWidget = new QWidget{this};
    auto addLayout = new QHBoxLayout{addAddressWidget};

    m_addressLineEdit = new QLineEdit{addAddressWidget};

    auto deviceexplorer = DeviceExplorer::getModel(m_model);

    if(deviceexplorer)
    {
        auto completer = new DeviceCompleter {deviceexplorer, this};
        m_addressLineEdit->setCompleter(completer);
    }


    auto ok_button = new QPushButton{"Add", addAddressWidget};
    connect(ok_button, &QPushButton::clicked,
            this,	   &EventInspectorWidget::on_addAddressClicked);
    addLayout->addWidget(m_addressLineEdit);
    addLayout->addWidget(ok_button);

    m_properties.push_back(new QLabel{"States"});
    m_properties.push_back(m_addressesWidget);
    m_properties.push_back(addAddressWidget);

    // Constraint list
    m_prevConstraints = new InspectorSectionWidget{tr("Previous Constraints") };
    m_nextConstraints = new InspectorSectionWidget{tr("Next Constraints") };
    m_properties.push_back(m_prevConstraints);
    m_properties.push_back(m_nextConstraints);

    updateSectionsView(areaLayout(), m_properties);
    areaLayout()->addStretch();

    // metadata
    m_metadata = new MetadataWidget{&object->metadata, commandDispatcher(), this};
    m_metadata->setType(EventModel::prettyName()); // TODO le faire automatiquement avec T::className

    m_metadata->setupConnections(m_model);

    addHeader(m_metadata);

    // display data
    updateDisplayedValues(object);

    connect (object,   &EventModel::dateChanged,
             this,      &EventInspectorWidget::modelDateChanged);
}

void EventInspectorWidget::addAddress(const QString& addr)
{
    auto address = new QWidget {this};
    auto lay = new QHBoxLayout {address};
    auto lbl = new QLabel {addr, this};
    lay->addWidget(lbl);

    QToolButton* rmBtn = new QToolButton {};
    rmBtn->setText("X");
    lay->addWidget(rmBtn);

    connect(rmBtn, &QToolButton::clicked,
            [ = ]()
    {
        removeState(lbl->text());
    });

    m_addresses.push_back(address);
    m_addressesWidget->layout()->addWidget(address);
}

void EventInspectorWidget::updateDisplayedValues(EventModel* event)
{
    // Cleanup
    for(auto& elt : m_addresses)
    {
        delete elt;
    }

    m_addresses.clear();

    m_prevConstraints->removeAll();
    m_nextConstraints->removeAll();

    m_date->clear();

    // DEMO
    if(event)
    {
        m_date->setText(QString::number(m_model->date().msec()));

        auto scenar = event->parentScenario();

        for(State* state : event->states())
        {
            for(auto& msg : state->messages())
            {
                addAddress(msg);
            }
        }

        for(auto cstr : event->previousConstraints())
        {
            auto cstrBtn = new QPushButton {};
            cstrBtn->setText(QString::number(*cstr.val()));
            cstrBtn->setFlat(true);
            m_prevConstraints->addContent(cstrBtn);

            connect(cstrBtn, &QPushButton::clicked,
                    [ = ]()
            {
                selectionDispatcher()->send(Selection{scenar->constraint(cstr)});
            });
        }

        for(auto cstr : event->nextConstraints())
        {
            auto cstrBtn = new QPushButton {};
            cstrBtn->setText(QString::number(*cstr.val()));
            cstrBtn->setFlat(true);
            m_nextConstraints->addContent(cstrBtn);

            connect(cstrBtn, &QPushButton::clicked,
                    [ = ]()
            {
                selectionDispatcher()->send(Selection{scenar->constraint(cstr)});
            });
        }


        m_conditionWidget->setText(event->condition());
    }
}


using namespace iscore::IDocument;
using namespace Scenario;
void EventInspectorWidget::on_addAddressClicked()
{
    auto txt = m_addressLineEdit->text();
    auto cmd = new Command::AddStateToEvent{path(m_model), txt};

    emit commandDispatcher()->submitCommand(cmd);
    m_addressLineEdit->clear();
}

void EventInspectorWidget::on_conditionChanged()
{
    auto txt = m_conditionWidget->text();

    if(txt == m_model->condition())
    {
        return;
    }

    auto cmd = new Command::SetCondition{path(m_model), txt};
    emit commandDispatcher()->submitCommand(cmd);
}

void EventInspectorWidget::removeState(QString state)
{
    auto cmd = new Command::RemoveStateFromEvent{path(m_model), state};
    emit commandDispatcher()->submitCommand(cmd);
}

void EventInspectorWidget::updateInspector()
{
    updateDisplayedValues(m_model);
}

void EventInspectorWidget::modelDateChanged()
{
    m_date->setText(QString::number(m_model->date().msec()));
}
