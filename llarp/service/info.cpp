#include <buffer.hpp>
#include <service/Info.hpp>
#include <service/address.hpp>

#include <cassert>

#include <sodium/crypto_generichash.h>

namespace llarp
{
  namespace service
  {
    bool
    ServiceInfo::DecodeKey(llarp_buffer_t key, llarp_buffer_t* val)
    {
      bool read = false;
      if(!BEncodeMaybeReadDictEntry("e", enckey, read, key, val))
        return false;
      if(!BEncodeMaybeReadDictEntry("s", signkey, read, key, val))
        return false;
      if(!BEncodeMaybeReadDictInt("v", version, read, key, val))
        return false;
      if(!BEncodeMaybeReadDictEntry("x", vanity, read, key, val))
        return false;
      return read;
    }

    bool
    ServiceInfo::BEncode(llarp_buffer_t* buf) const
    {
      if(!bencode_start_dict(buf))
        return false;
      if(!BEncodeWriteDictEntry("e", enckey, buf))
        return false;
      if(!BEncodeWriteDictEntry("s", signkey, buf))
        return false;
      if(!BEncodeWriteDictInt("v", LLARP_PROTO_VERSION, buf))
        return false;
      if(!vanity.IsZero())
      {
        if(!BEncodeWriteDictEntry("x", vanity, buf))
          return false;
      }
      return bencode_end(buf);
    }

    std::string
    ServiceInfo::Name() const
    {
      if(m_CachedAddr.IsZero())
      {
        Address addr;
        CalculateAddress(addr.data());
        return addr.ToString();
      }
      return m_CachedAddr.ToString();
    }

    bool
    ServiceInfo::CalculateAddress(byte_t* addr) const
    {
      byte_t tmp[256] = {0};
      auto buf        = llarp::StackBuffer< decltype(tmp) >(tmp);
      if(!BEncode(&buf))
        return false;
      return crypto_generichash_blake2b(addr, 32, buf.base, buf.cur - buf.base,
                                        nullptr, 0)
          != -1;
    }

    bool
    ServiceInfo::UpdateAddr()
    {
      return CalculateAddress(m_CachedAddr.data());
    }

  }  // namespace service
}  // namespace llarp
