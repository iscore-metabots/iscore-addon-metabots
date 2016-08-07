#pragma once
#include <OSSIA/Protocols/OSSIADevice.hpp>
#include <Device/Protocol/ProtocolFactoryInterface.hpp>
#include <Device/Protocol/ProtocolSettingsWidget.hpp>
#include <QString>
#include <QVariant>
#include <QLineEdit>
#include <QComboBox>
#include <QSerialPortInfo>
namespace Metabots
{
class MetabotDevice final : public Ossia::Protocols::OwningOSSIADevice
{
    public:
        MetabotDevice(const Device::DeviceSettings& settings);

        bool reconnect() override;

        void disconnect() override;

        Device::Node refresh() override;
};

struct MetabotSpecificSettings
{
        QSerialPortInfo port;
};

class ProtocolFactory final :
        public Device::ProtocolFactory
{
        ISCORE_CONCRETE_FACTORY("51a1115e-4c46-4f13-bc8e-28316aad335f")

        // Implement with OSSIA::Device
        QString prettyName() const override;

        Device::DeviceInterface* makeDevice(
                const Device::DeviceSettings& settings,
                const iscore::DocumentContext& ctx) override;

        const Device::DeviceSettings& defaultSettings() const override;

        Device::ProtocolSettingsWidget* makeSettingsWidget() override;

        QVariant makeProtocolSpecificSettings(
                const VisitorVariant& visitor) const override;

        void serializeProtocolSpecificSettings(
                const QVariant& data,
                const VisitorVariant& visitor) const override;

        bool checkCompatibility(
                const Device::DeviceSettings& a,
                const Device::DeviceSettings& b) const override;
};

class ProtocolSettingsWidget :
        public Device::ProtocolSettingsWidget
{
        Q_OBJECT

    public:
        ProtocolSettingsWidget(QWidget* parent = nullptr);


    private:
        Device::DeviceSettings getSettings() const override;

        void setSettings(const Device::DeviceSettings& settings) override;


        void buildGUI();

        QLineEdit* m_name;
        QComboBox* m_port;
};
}
Q_DECLARE_METATYPE(Metabots::MetabotSpecificSettings)
