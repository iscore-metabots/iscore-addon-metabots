#include "Protocol.hpp"
#include <sstream>

bool Metabots::Metabot::pushAddressValue(const OSSIA::Address& addr) const
{
    auto& ad = dynamic_cast<const MetabotAddress&>(addr);
    std::stringstream s;

    s << addr.getNode()->getName() << " " << std::to_string(ad.getValue().get<OSSIA::Float>().value) << '\n';

    auto str = s.str();
    m_serialPort.write(str.c_str());
    return false;
}

Metabots::Metabot::~Metabot()
{

}


