#include <llarp/messages/dht.hpp>
#include <llarp/messages/path_confirm.hpp>
#include <llarp/messages/path_latency.hpp>
#include <llarp/messages/path_transfer.hpp>
#include <llarp/routing/message.hpp>

namespace llarp
{
  namespace routing
  {
    InboundMessageParser::InboundMessageParser()
    {
      reader.user   = this;
      reader.on_key = &OnKey;
      firstKey      = false;
    }

    bool
    InboundMessageParser::OnKey(dict_reader* r, llarp_buffer_t* key)
    {
      InboundMessageParser* self =
          static_cast< InboundMessageParser* >(r->user);

      if(key == nullptr && self->firstKey)
      {
        // empty dict
        return false;
      }
      if(!key)
        return true;
      if(self->firstKey)
      {
        llarp_buffer_t strbuf;
        if(!llarp_buffer_eq(*key, "A"))
          return false;
        if(!bencode_read_string(r->buffer, &strbuf))
          return false;
        if(strbuf.sz != 1)
          return false;
        switch(*strbuf.cur)
        {
          case 'L':
            self->msg = new PathLatencyMessage;
            break;
          case 'M':
            self->msg = new DHTMessage;
            break;
          case 'P':
            self->msg = new PathConfirmMessage;
            break;
          case 'T':
            self->msg = new PathTransferMessage;
            break;
          default:
            llarp::LogError("invalid routing message id: ", *strbuf.cur);
        }
        self->firstKey = false;
        return self->msg != nullptr;
      }
      else
      {
        return self->msg->DecodeKey(*key, r->buffer);
      }
    }

    bool
    InboundMessageParser::ParseMessageBuffer(llarp_buffer_t buf,
                                             IMessageHandler* h,
                                             llarp_router* r)
    {
      bool result = false;
      msg         = nullptr;
      firstKey    = true;
      if(bencode_read_dict(&buf, &reader))
      {
        result = msg->HandleMessage(h, r);
        delete msg;
      }
      else
        llarp::LogError("read dict failed");
      return result;
    }
  }  // namespace routing
}  // namespace llarp