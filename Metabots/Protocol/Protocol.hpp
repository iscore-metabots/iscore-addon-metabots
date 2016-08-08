#pragma once

#include <vector>
#include <string>
#include <ossia/ossia.hpp>
#include <QtSerialPort>
#include <iostream>
namespace Metabots
{
class SerialPortWrapper:
        public QObject
{
        Q_OBJECT

        QSerialPort m_serialPort;
    public:
        SerialPortWrapper(const QSerialPortInfo& port):
            m_serialPort{port}
        {
            m_serialPort.open(QIODevice::WriteOnly);
            connect(this, &SerialPortWrapper::write,
                    this, &SerialPortWrapper::on_write,
                    Qt::QueuedConnection);
        }

    signals:
        void write(QByteArray);

    public slots:
        void on_write(QByteArray b)
        {
            qDebug() << b.data();
            m_serialPort.write(b);
        }
};

class Metabot final :
        public QObject,
        public ossia::net::protocol_base
{
        mutable SerialPortWrapper m_serialPort;
    public:
        // Param : the name of the serial port
        Metabot(const QSerialPortInfo& bot):
            m_serialPort{bot}
        {
            /*
            m_serialPort.write("start\n");
            connect(&m_serialPort, &QSerialPort::readyRead,
                    this, [=] () {
                if(m_serialPort.canReadLine())
                    qDebug() << "yay" << m_serialPort.readAll();
            });
            */
        }

        bool pull(ossia::net::address_base&) override
        { return false; }
        bool push(const ossia::net::address_base&) override;

        bool observe(ossia::net::address_base&, bool) override
        { return false; }

        bool update(ossia::net::node_base& node_base) override
        { return false; }

        virtual ~Metabot();
};

class MetabotDeviceImpl;
class MetabotAddress;
class MetabotNode :
        public ossia::net::node_base
{
    protected:
        std::string mName;
        MetabotDeviceImpl& mDevice;
        MetabotNode* mParent{};
        std::unique_ptr<MetabotAddress> mAddress;

    public:
        MetabotNode(
                std::string name,
                MetabotDeviceImpl& aDevice,
                MetabotNode* aParent);

        template<typename... Args>
        void init(Args&&... args);

        std::string getName() const override;

        ossia::net::device_base& getDevice() const final override;

        ossia::net::node_base* getParent() const final override;

        ossia::net::node_base& setName(std::string) override;

        ossia::net::address_base* getAddress() const final override;

        ossia::net::address_base* createAddress(ossia::val_type) final override;

        bool removeAddress() final override;

        std::unique_ptr<node_base> makeChild(const std::string& name) override
        { return nullptr; }
        void removingChild(ossia::net::node_base& node_base) override
        { }
};


struct MetabotData
{
        struct FloatProperties {
                using ossia_type = ossia::Float;
                static const constexpr ossia::val_type type = ossia::val_type::FLOAT;
                std::string name;
                ossia::Float min;
                ossia::Float max;
        };
        struct IntProperties {
                using ossia_type = ossia::Int;
                static const constexpr ossia::val_type type = ossia::val_type::INT;
                std::string name;
                ossia::Int min;
                ossia::Int max;
        };
        struct ImpulseProperties {
                using ossia_type = ossia::Impulse;
                static const constexpr ossia::val_type type = ossia::val_type::IMPULSE;
                std::string name;
                ossia::Impulse min{};
                ossia::Impulse max{};
        };

        const std::array<FloatProperties, 7> floats{ {
                { "dx", -300, 300 },
                { "dy", -300, 300 },
                { "h", -120, -30 },
                { "turn", -300, 300 },
                { "crab", 0, 10 },
                { "backleg", 0, 10 },
                { "freq", 0, 3 }
                                                     } };
        const std::array<IntProperties, 1> ints{
            { { "alt", 0, 10 } }
        };
        const std::array<ImpulseProperties, 4> impulses{
            { { "start" }, { "stop" }, { "back" }, { "gait" } }
        };
};

class MetabotAddress final :
        public ossia::net::address_base,
        public std::enable_shared_from_this<MetabotAddress>
{
        MetabotNode& mParent;
        Metabot& mProtocol;
        ossia::net::domain mDomain;

        ossia::val_type mType;
        ossia::value mValue;
        std::string mTextAddress;
    public:
        template<typename Prop_T>
        MetabotAddress(Prop_T p, MetabotNode& parent):
            mParent{parent},
            mProtocol{static_cast<Metabot&>(parent.getDevice().getProtocol())},
            mDomain{ossia::net::makeDomain(p.min, p.max)},
            mType{p.type},
            mValue{typename Prop_T::ossia_type{}}
        {
            mTextAddress = "/" + mParent.getName();
        }


        MetabotNode& getNode() const override
        {
            return mParent;
        }

        void pullValue() override
        {
        }

        ossia::net::address_base& pushValue(const ossia::value& val) override
        {
            // TODO sanitize type
            mValue = val;
            mProtocol.push(*this);
            return *this;
        }

        ossia::net::address_base& pushValue() override
        {
            mProtocol.push(*this);
            return *this;
        }


        const ossia::value& getValue() const
        {
            return mValue;
        }

        ossia::value cloneValue(ossia::destination_index) const override
        {
            // TODO use the vec parameter
            return mValue;
        }

        ossia::net::address_base& setValue(const ossia::value& v) override
        {
            mValue = v;
            return *this;
        }


        ossia::val_type getValueType() const override
        {
            return mType;
        }

        ossia::net::address_base& setValueType(ossia::val_type) override
        {
            return *this;
        }


        ossia::access_mode getAccessMode() const override
        {
            return ossia::access_mode::BI;
        }

        ossia::net::address_base& setAccessMode(ossia::access_mode) override
        {
            return *this;
        }


        const ossia::net::domain& getDomain() const override
        {
            return mDomain;
        }

        ossia::net::address_base& setDomain(const ossia::net::domain&) override
        {
            return *this;
        }


        ossia::bounding_mode getBoundingMode() const override
        {
            return ossia::bounding_mode::CLIP;
        }

        ossia::net::address_base&setBoundingMode(ossia::bounding_mode) override
        {
            return *this;
        }

        ossia::repetition_filter getRepetitionFilter() const override
        {
            return ossia::repetition_filter::OFF;
        }

        ossia::net::address_base&setRepetitionFilter(ossia::repetition_filter) override
        {
            return *this;
        }


        void valueCallback(const ossia::value& val)
        {
            this->setValue(val);
            send(mValue);
        }

        const std::string& getTextualAddress() const override
        {
            return mTextAddress;
        }
};

template<typename... Args>
void MetabotNode::init(Args&&... args)
{
    mAddress = std::make_unique<MetabotAddress>(
                   std::forward<Args>(args)...,
                   *this);
}

class MetabotDeviceImpl :
        public ossia::net::device_base,
        public MetabotNode
{
    public:
        MetabotDeviceImpl(const QSerialPortInfo& bot):
            ossia::net::device_base{std::make_unique<Metabot>(bot)},
            MetabotNode{"", *this, nullptr}
        {
        }

        std::string getName() const override
        { return mName; }
        node_base& setName(std::string n) override
        { mName = n; return *this; }

        const ossia::net::node_base& getRootNode() const override
        {
          return *this;
        }
        ossia::net::node_base& getRootNode() override
        {
          return *this;
        }


        bool updateNamespace()
        {
            mChildren.clear();

            try {

                const MetabotData m{};
                for(auto prop : m.impulses)
                {
                    auto node = std::make_unique<MetabotNode>(prop.name, *this, this);
                    node->init(prop);
                    mChildren.push_back(std::move(node));
                }

                for(auto prop : m.floats)
                {
                    auto node = std::make_unique<MetabotNode>(prop.name, *this, this);
                    node->init(prop);
                    mChildren.push_back(std::move(node));
                }

                for(auto prop : m.ints)
                {
                    auto node = std::make_unique<MetabotNode>(prop.name, *this, this);
                    node->init(prop);
                    mChildren.push_back(std::move(node));
                }
            }
            catch(...)
            {
                std::cerr << "refresh failed\n";
                return false;
            }
            return true;
        }
};
}

