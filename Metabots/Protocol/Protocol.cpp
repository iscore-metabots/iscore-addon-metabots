#include "Protocol.hpp"
#include <sstream>
#include <iomanip>

bool Metabots::Metabot::pushAddressValue(const OSSIA::Address& addr) const
{
    auto& ad = dynamic_cast<const MetabotAddress&>(addr);
    std::stringstream s;
    s << addr.getNode()->getName();

    switch(addr.getValueType())
    {
        case OSSIA::Type::FLOAT:
            s << " " << std::setprecision(4) << std::to_string(ad.getValue().get<OSSIA::Float>().value);
            break;
        case OSSIA::Type::INT:
            s << " " << std::to_string(ad.getValue().get<OSSIA::Int>().value);
            break;
        case OSSIA::Type::IMPULSE:
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


