#pragma once

#include <vector>
#include <string>
#include <Network/Protocol.h>
#include <Network/Device.h>
#include <Network/Address.h>
#include <Editor/Value/SafeValue.h>
#include <QtSerialPort>
#include <iostream>
namespace Metabots
{
class Metabot final :
        public QObject,
        public OSSIA::Protocol
{
       mutable QSerialPort m_serialPort;
    public:
        // Param : the name of the serial port
        Metabot(const std::string& bot):
            m_serialPort{QString::fromStdString(bot)}
        {
            connect(&m_serialPort, &QSerialPort::readyRead,
                    this, [=] () {
                qDebug() << "yay" << m_serialPort.readAll();
            });
        }



        void setLogger(std::shared_ptr<OSSIA::NetworkLogger>) override { } // TODO
        std::shared_ptr<OSSIA::NetworkLogger> getLogger() const override { return {}; } // TODO

        bool pullAddressValue(OSSIA::Address&) const override
        {
            return false;
        }

        bool pushAddressValue(const OSSIA::Address& addr) const override;

        bool observeAddressValue(std::shared_ptr<OSSIA::Address>, bool) const override
        {
            return false;
        }

        bool updateChildren(OSSIA::Node& node) const override
        {
            return false;
        }

        virtual ~Metabot();
};


class MetabotNode :
    public virtual OSSIA::Node,
    public std::enable_shared_from_this<MetabotNode>
{
protected:
  std::string mName;
  std::weak_ptr<OSSIA::Device> mDevice;
  std::weak_ptr<OSSIA::Node> mParent;
  std::shared_ptr<OSSIA::Address> mAddress;

public:
  using iterator = OSSIA::Container<OSSIA::Node>::iterator;
  using const_iterator = OSSIA::Container<OSSIA::Node>::const_iterator;
  MetabotNode() = default;
  MetabotNode(
      std::string name,
      std::weak_ptr<OSSIA::Device> aDevice,
      std::weak_ptr<OSSIA::Node> aParent):
    mName{std::move(name)},
    mDevice{std::move(aDevice)},
    mParent{std::move(aParent)}
  {

  }

  std::string getName() const override
  { return mName; }

  std::shared_ptr<OSSIA::Device> getDevice() const final override
  {
    return mDevice.lock();
  }

  std::shared_ptr<Node> getParent() const final override
  {
    return mParent.lock();
  }

  std::shared_ptr<Node> getThis() final override
  {
    return shared_from_this();
  }

  Node& setName(std::string) override
  {
    return *this;
  }

  std::shared_ptr<OSSIA::Address> getAddress() const final override
  {
    return mAddress;
  }

  std::shared_ptr<OSSIA::Address> createAddress(OSSIA::Type) final override
  {
    return mAddress;
  }

  bool removeAddress() final override
  {
    return false;
  }

  iterator emplace(const_iterator, std::string) final override
  {
    return {};
  }

  iterator emplace(const_iterator,
                   const std::string&,
                   OSSIA::Type,
                   OSSIA::AccessMode,
                   const std::shared_ptr<OSSIA::Domain>&,
                   OSSIA::BoundingMode,
                   bool repetitionFilter) final override
  {
    return {};
  }

  iterator insert(const_iterator, std::shared_ptr<Node>, std::string) final override
  {
    return {};
  }

  iterator erase(const_iterator) final override
  {
    return {};
  }
};

class MetabotAddress final :
    public OSSIA::Address,
    public std::enable_shared_from_this<MetabotAddress>
{
  std::weak_ptr<OSSIA::Node> mParent;
  std::weak_ptr<Metabot> mProtocol;
  std::shared_ptr<OSSIA::Domain> mDomain;

  OSSIA::Type mType;
  OSSIA::SafeValue mValue;
public:
  MetabotAddress(std::shared_ptr<OSSIA::Node> parent):
    mParent{parent},
    mProtocol{std::dynamic_pointer_cast<Metabot>(parent->getDevice()->getProtocol())},
    mDomain{OSSIA::Domain::create(OSSIA::Float{-100.}, OSSIA::Float{100.})},
    mType{OSSIA::Type::FLOAT},
    mValue{OSSIA::Float{0.}}
  {
    assert(mProtocol.lock());
  }


  const std::shared_ptr<OSSIA::Node> getNode() const override
  {
    return mParent.lock();
  }

  void pullValue() override
  {
    mProtocol.lock()->pullAddressValue(*this);
  }

  Address& pushValue(const OSSIA::SafeValue& val) override
  {
    mValue = val;
    mProtocol.lock()->pushAddressValue(*this);
    return *this;
  }

  Address& pushValue() override
  {
    mProtocol.lock()->pushAddressValue(*this);
    return *this;
  }


  const OSSIA::SafeValue& getValue() const
  {
    return mValue;
  }

  OSSIA::SafeValue cloneValue(std::vector<char>) const override
  {
    // TODO use the vec parameter
    return mValue;
  }

  Address& setValue(const OSSIA::SafeValue& v) override
  {
    mValue = v;
    return *this;
  }


  OSSIA::Type getValueType() const override
  {
    return mType;
  }

  Address& setValueType(OSSIA::Type) override
  {
    return *this;
  }


  OSSIA::AccessMode getAccessMode() const override
  {
    return OSSIA::AccessMode::BI;
  }

  Address& setAccessMode(OSSIA::AccessMode) override
  {
    return *this;
  }


  const std::shared_ptr<OSSIA::Domain>& getDomain() const override
  {
    return mDomain;
  }

  Address& setDomain(std::shared_ptr<OSSIA::Domain>) override
  {
    return *this;
  }


  OSSIA::BoundingMode getBoundingMode() const override
  {
    return OSSIA::BoundingMode::CLIP;
  }

  Address&setBoundingMode(OSSIA::BoundingMode) override
  {
    return *this;
  }

  bool getRepetitionFilter() const override
  {
    return false;
  }

  Address&setRepetitionFilter(bool) override
  {
    return *this;
  }


  Address::iterator addCallback(OSSIA::ValueCallback callback) override
  {
    auto it = CallbackContainer::addCallback(std::move(callback));

    if (callbacks().size() == 1)
    {
      mProtocol.lock()->observeAddressValue(shared_from_this(), true);
    }

    return it;
  }

  void removeCallback(Address::iterator callback) override
  {
    CallbackContainer::removeCallback(callback);

    if (callbacks().size() == 0)
    {
      mProtocol.lock()->observeAddressValue(shared_from_this(), false);
    }
  }

  void valueCallback(const OSSIA::SafeValue& val)
  {
    this->setValue(val);
    send(mValue);
  }
};

class MetabotDeviceImpl :
    public OSSIA::Device,
    public MetabotNode
{
  std::shared_ptr<OSSIA::Protocol> mProtocol;
public:
  MetabotDeviceImpl(std::shared_ptr<OSSIA::Protocol> prot):
    mProtocol{prot}
  {
  }


  Node & setName(std::string n) override
  {
    mName = n;
    return *this;
  }


  std::shared_ptr<OSSIA::Protocol> getProtocol() const override
  {
    return mProtocol;
  }

  bool updateNamespace() override
  {
    m_children.clear();

    try {
      auto ptr = shared_from_this();
      auto dev_ptr = std::dynamic_pointer_cast<OSSIA::Device>(ptr);

      for(auto str : {"dx", "dy", "h", "crab"})
      {
          m_children.push_back(std::make_shared<MetabotNode>(str, dev_ptr, dev_ptr));
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

