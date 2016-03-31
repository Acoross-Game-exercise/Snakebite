#include <iostream>
#include <functional>

#include <acoross/snakebite/win/targetver.h>
#include <boost/asio.hpp>
#include <acoross/rpc/asio_protobuf_rpc_stub.h>
#include <acoross/snakebite/protos/snakebite_message.pb.h>

using namespace acoross::snakebite;
using namespace acoross;
using ::boost::asio::ip::tcp;

namespace acoross {
namespace snakebite {

class MyRpc : public rpc::RpcStub
{
public:
	MyRpc(::boost::asio::io_service& io_service, tcp::socket socket)
		: rpc::RpcStub(io_service, std::move(socket))
	{}
	virtual ~MyRpc() {}

	#define DEF_END
	#define PROCEDURE_LIST \
	DEF_PROCEDURE(Hello, messages::HelloRequest, messages::HelloReply)	\
	DEF_END

	// enum
	enum class MyRpcType
	{
	#define DEF_PROCEDURE(name, requestT, replyT) name##_type,
		PROCEDURE_LIST
	#undef DEF_PROCEDURE
	};

	// member functions
	#define DEF_PROCEDURE(name, requestT, replyT) void name(const requestT& rq, std::function<void(rpc::ErrCode, replyT&)> cb){	RpcCaller<replyT>(0, rq, std::move(cb));	}
	PROCEDURE_LIST
	#undef DEF_PROCEDURE
};
}
}

int main()
{
	try
	{
		::boost::asio::io_service io_service;
		
		tcp::socket socket(io_service);
		{
			tcp::resolver resolver(io_service);
			boost::asio::connect(socket, resolver.resolve({ "localhost", "22001" }));
		}

		MyRpc rpc_stub(io_service, std::move(socket));
		rpc_stub.start();

		messages::HelloRequest rq;
		rq.set_name("shin");
		rpc_stub.Hello(rq,
			[](rpc::ErrCode err_code, messages::HelloReply& rp)
		{
			std::cout << rp.message() << std::endl;
		});

		io_service.run();
	}
	catch (std::exception ex)
	{
		std::cout << "exception: " << ex.what() << std::endl;
	}

	return 0;
}