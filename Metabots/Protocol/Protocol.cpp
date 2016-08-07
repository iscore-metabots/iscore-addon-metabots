#include "Protocol.hpp"
#include <sstream>
#include <iomanip>

bool Metabots::Metabot::push(const ossia::net::address_base& addr)
{
    auto& ad = dynamic_cast<const MetabotAddress&>(addr);
    std::stringstream s;
    s << addr.getNode().getName();

    switch(addr.getValueType())
    {
        case ossia::val_type::FLOAT:
            s << " " << std::setprecision(4) << std::to_string(ad.getValue().get<ossia::Float>().value);
            break;
        case ossia::val_type::INT:
            s << " " << std::to_string(ad.getValue().get<ossia::Int>().value);
            break;
        case ossia::val_type::IMPULSE:
            break;
        default:
            throw;
    }

    s << '\n';

    auto str = s.str();
    qDebug() << s.str();
    m_serialPort.write(QByteArray(str.c_str()));
    return false;
}

Metabots::Metabot::~Metabot()
{

}



Metabots::MetabotNode::MetabotNode(
        std::string name,
        Metabots::MetabotDeviceImpl& aDevice,
        Metabots::MetabotNode* aParent):
    mName{std::move(name)},
    mDevice{aDevice},
    mParent{aParent}
{
}

std::string Metabots::MetabotNode::getName() const
{ return mName; }

ossia::net::device_base&Metabots::MetabotNode::getDevice() const
{
    return mDevice;
}

ossia::net::node_base*Metabots::MetabotNode::getParent() const
{
    return mParent;
}

ossia::net::node_base&Metabots::MetabotNode::setName(std::__cxx11::string)
{
    return *this;
}

ossia::net::address_base*Metabots::MetabotNode::getAddress() const
{
    return mAddress.get();
}

ossia::net::address_base*Metabots::MetabotNode::createAddress(ossia::val_type)
{
    return getAddress();
}

bool Metabots::MetabotNode::removeAddress()
{
    return false;
}
