#ifndef LLARP_SERVICE_LOOKUP_HPP
#define LLARP_SERVICE_LOOKUP_HPP

#include <llarp/router.h>
#include <llarp/routing/message.hpp>
#include <llarp/service/IntroSet.hpp>
#include <set>

namespace llarp
{
  // forward declare
  namespace path
  {
    struct Path;
  }

  namespace service
  {
    struct IServiceLookup
    {
      IServiceLookup() : m_created(llarp_time_now_ms())
      {
      }

      virtual ~IServiceLookup(){};

      /// handle lookup result
      virtual bool
      HandleResponse(const std::set< IntroSet >& results) = 0;

      /// determine if this request has timed out
      bool
      IsTimedOut(llarp_time_t now, llarp_time_t timeout = 5000) const
      {
        if(now <= m_created)
          return false;
        return now - m_created > timeout;
      }

      /// build request message for service lookup
      virtual llarp::routing::IMessage*
      BuildRequestMessage() = 0;

      /// build a new requset message and send it via a path
      bool
      SendRequestViaPath(llarp::path::Path* p, llarp_router* r);

     private:
      llarp_time_t m_created;
    };

  }  // namespace service
}  // namespace llarp

#endif