// Generated by the acoross-rpc protobuf plugin.
// If you make any local change, they will be lost.
// source: snakebite_message.proto
#ifndef ACOROSS_RPC_snakebite_5fmessage_2eproto__INCLUDED
#define ACOROSS_RPC_snakebite_5fmessage_2eproto__INCLUDED

#include "snakebite_message.pb.h"

#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <acoross/rpc/rpc_service.h>
#include <acoross/rpc/rpc_stub.h>
#include <acoross/rpc/rpc_macros.h>

namespace acoross {
namespace snakebite {
namespace messages {

class SC_PushService final {
 public:
  enum Protocol
  {
    UpdateGameObjects,
  };

  class Service : public ::acoross::rpc::RpcService 
  {
  public:
    Service(::boost::asio::io_service& io_service, ::boost::asio::ip::tcp::socket&& socket);
    virtual ~Service() {}

  private:
    DEF_SERVICE(UpdateGameObjects, ::acoross::snakebite::messages::UpdateGameObjectsEvent, ::acoross::snakebite::messages::VoidReply)

  };

  class Stub : public ::acoross::rpc::RpcStub
  {
  public:
    Stub(::boost::asio::io_service& io_service, ::boost::asio::ip::tcp::socket&& socket);
    virtual ~Stub() {}
    DEF_STUB(UpdateGameObjects, ::acoross::snakebite::messages::UpdateGameObjectsEvent, ::acoross::snakebite::messages::VoidReply)
  };

};


class SnakebiteService final {
 public:
  enum Protocol
  {
    InitPlayer,
    SetKeyDown,
    SetKeyUp,
  };

  class Service : public ::acoross::rpc::RpcService 
  {
  public:
    Service(::boost::asio::io_service& io_service, ::boost::asio::ip::tcp::socket&& socket);
    virtual ~Service() {}

  private:
    DEF_SERVICE(InitPlayer, ::acoross::snakebite::messages::InitPlayerSnakeRequest, ::acoross::snakebite::messages::InitPlayerSnakeReply)
    DEF_SERVICE(SetKeyDown, ::acoross::snakebite::messages::TurnKeyDownRequest, ::acoross::snakebite::messages::VoidReply)
    DEF_SERVICE(SetKeyUp, ::acoross::snakebite::messages::TurnKeyUpRequest, ::acoross::snakebite::messages::VoidReply)

  };

  class Stub : public ::acoross::rpc::RpcStub
  {
  public:
    Stub(::boost::asio::io_service& io_service, ::boost::asio::ip::tcp::socket&& socket);
    virtual ~Stub() {}
    DEF_STUB(InitPlayer, ::acoross::snakebite::messages::InitPlayerSnakeRequest, ::acoross::snakebite::messages::InitPlayerSnakeReply)
    DEF_STUB(SetKeyDown, ::acoross::snakebite::messages::TurnKeyDownRequest, ::acoross::snakebite::messages::VoidReply)
    DEF_STUB(SetKeyUp, ::acoross::snakebite::messages::TurnKeyUpRequest, ::acoross::snakebite::messages::VoidReply)
  };

};


}  // namespace messages
}  // namespace snakebite
}  // namespace acoross


#endif  // ACOROSS_RPC_snakebite_5fmessage_2eproto__INCLUDED