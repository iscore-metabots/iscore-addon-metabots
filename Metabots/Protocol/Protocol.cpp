#include "Protocol.hpp"

bool Metabots::Metabot::pushAddressValue(const OSSIA::Address& addr) const
{
    auto& ad = dynamic_cast<const MetabotAddress&>(addr);
    QByteArray b;
    {
        QDataStream s{&b, QIODevice::WriteOnly};
        s << addr.getNode()->getName().c_str() << " " << ad.getValue().get<OSSIA::Float>().value << '\n';
    }
    m_serialPort.write(b);
    return false;
}
