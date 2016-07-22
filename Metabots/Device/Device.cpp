#include <Network/Device.h>
#include <Metabots/Protocol/Protocol.hpp>
#include <QString>
#include <memory>

#include <Device/Protocol/DeviceSettings.hpp>
#include "Device.hpp"
#include <OSSIA/OSSIA2iscore.hpp>
#include <QFormLayout>
#include <QtSerialPort>
#include <QHBoxLayout>

namespace Metabots
{
MetabotDevice::MetabotDevice(const Device::DeviceSettings &settings):
    OSSIADevice{settings}
{
    using namespace OSSIA;
    m_capas.canRefreshTree = true;
    m_capas.canSerialize = false;

    reconnect();
}

bool MetabotDevice::reconnect()
{
    disconnect();
    m_dev.reset();

    MetabotSpecificSettings set = settings().deviceSpecificSettings.value<MetabotSpecificSettings>();
    try {
        auto proto = std::make_shared<Metabot>(set.port);
        m_dev = std::make_shared<MetabotDeviceImpl>(proto);
        m_dev->setName(settings().name.toStdString());
        m_dev->updateNamespace();
    }
    catch(std::exception& e)
    {
        qDebug() << e.what();
    }

    return connected();
}

void MetabotDevice::disconnect()
{
    if(connected())
    {
        removeListening_impl(*m_dev.get(), State::Address{m_settings.name, {}});
    }

    m_callbacks.clear();
    m_dev.reset();
}

Device::Node MetabotDevice::refresh()
{
    Device::Node device_node{settings(), nullptr};

    if(!connected())
    {
        return device_node;
    }
    else
    {
        auto& children = m_dev->children();
        device_node.reserve(children.size());
        for(const auto& node : children)
        {
            device_node.push_back(Ossia::convert::ToDeviceExplorer(*node.get()));
        }
    }

    device_node.get<Device::DeviceSettings>().name = settings().name;
    return device_node;
}



QString ProtocolFactory::prettyName() const
{
    return QObject::tr("Metabot");
}

Device::DeviceInterface* ProtocolFactory::makeDevice(
        const Device::DeviceSettings& settings,
        const iscore::DocumentContext& ctx)
{
    return new MetabotDevice{settings};
}

const Device::DeviceSettings& ProtocolFactory::defaultSettings() const
{
    static const Device::DeviceSettings settings = [&] () {
        Device::DeviceSettings s;
        s.protocol = concreteFactoryKey();
        s.name = "Metabot";
        MetabotSpecificSettings specif;
        s.deviceSpecificSettings = QVariant::fromValue(specif);
        return s;
    }();
    return settings;
}

Device::ProtocolSettingsWidget* ProtocolFactory::makeSettingsWidget()
{
    return new ProtocolSettingsWidget;
}

QVariant ProtocolFactory::makeProtocolSpecificSettings(const VisitorVariant& visitor) const
{
    return makeProtocolSpecificSettings_T<MetabotSpecificSettings>(visitor);
}

void ProtocolFactory::serializeProtocolSpecificSettings(const QVariant& data, const VisitorVariant& visitor) const
{
    serializeProtocolSpecificSettings_T<MetabotSpecificSettings>(data, visitor);
}

bool ProtocolFactory::checkCompatibility(const Device::DeviceSettings& a, const Device::DeviceSettings& b) const
{
    return a.name != b.name;
}



ProtocolSettingsWidget::ProtocolSettingsWidget(QWidget* parent)
    : Device::ProtocolSettingsWidget(parent)
{
    buildGUI();
}

void
ProtocolSettingsWidget::buildGUI()
{
    m_name = new QLineEdit;
    m_port = new QComboBox;

    for(auto port : QSerialPortInfo::availablePorts())
    {
        m_port->addItem(port.portName());
    }


    auto lay = new QFormLayout;
    lay->addRow(tr("Name"), m_name);
    lay->addRow(tr("Port"), m_port);
    setLayout(lay);

}

Device::DeviceSettings ProtocolSettingsWidget::getSettings() const
{

    // TODO *** Initialize with ProtocolFactory.defaultSettings().
    Device::DeviceSettings s;
    MetabotSpecificSettings metabot;
    s.name = m_name->text();


    for(auto port : QSerialPortInfo::availablePorts())
    {
        if(port.portName() == m_port->currentText())
        {
            metabot.port = port;
        }
    }

    s.deviceSpecificSettings = QVariant::fromValue(metabot);

    return s;
}

void
ProtocolSettingsWidget::setSettings(const Device::DeviceSettings &settings)
{

    if (settings.deviceSpecificSettings.canConvert<MetabotSpecificSettings>())
    {
        MetabotSpecificSettings metabot = settings.deviceSpecificSettings.value<MetabotSpecificSettings>();

        m_port->setCurrentText(metabot.port.portName());
    }
}
}



template<>
void Visitor<Reader<DataStream>>::readFrom_impl(const Metabots::MetabotSpecificSettings& n)
{
    ISCORE_TODO;
    //m_stream << n.port;
    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(Metabots::MetabotSpecificSettings& n)
{
    ISCORE_TODO;
    //m_stream >> n.port;
    checkDelimiter();
}

template<>
void Visitor<Reader<JSONObject>>::readFrom_impl(const Metabots::MetabotSpecificSettings& n)
{
    ISCORE_TODO;
   // m_obj["Port"] = n.port;
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(Metabots::MetabotSpecificSettings& n)
{
    ISCORE_TODO;
   // n.port = m_obj["Port"].toString();
}

