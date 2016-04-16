#ifndef SNAKEBITE_GAME_CLIENT_H_
#define SNAKEBITE_GAME_CLIENT_H_

#include <acoross/snakebite/win/targetver.h>
#include <boost/asio.hpp>

#include <acoross/snakebite/protos/snakebite_message.rpc.h>

#include <acoross/snakebite/game_session/game_session_system.h>
#include <acoross/snakebite/game_client_base.h>

using boost::asio::ip::tcp;
enum { max_length = 1024 };

namespace acoross {
namespace snakebite {

class GameClient final
	: public GameClientBase
	, public std::enable_shared_from_this<GameClient>
{
public:
	GameClient(boost::asio::io_service& io_service, tcp::socket&& socket)
		: stub_(new messages::SnakebiteService::Stub(io_service, std::move(socket)))
	{}

	void start()
	{
		stub_->start();
	}

	virtual void InitPlayer() override
	{
		messages::InitPlayerSnakeRequest rq;
		rq.set_name("remote player");
		
		stub_->InitPlayer(rq, 
			[client = shared_from_this()](acoross::rpc::ErrCode ec, messages::InitPlayerSnakeReply& rp)
		{
			if (ec == acoross::rpc::ErrCode::NoError)
			{
				client->set_player_handle(rp.handle());
			}
			else
			{
				assert(false);
			}
		});
	}

	virtual void SetKeyDown(PlayerKey pk) override
	{
		messages::TurnKeyDownRequest rq;
		rq.set_key((google::protobuf::int32)pk);

		stub_->SetKeyDown(rq,
			[client = shared_from_this()](acoross::rpc::ErrCode ec, messages::VoidReply&)
		{});
	}

	virtual void SetKeyUp(PlayerKey pk) override
	{
		messages::TurnKeyUpRequest rq;
		rq.set_key((google::protobuf::int32)pk);

		stub_->SetKeyUp(rq,
			[client = shared_from_this()](acoross::rpc::ErrCode ec, messages::VoidReply&)
		{});
	}

	virtual void Draw(Win::WDC& wdc, RECT& client_rect) override
	{
		if (clone_list_changed_.load() == false)
		{
			return;
		}

		// snake �� apple �� ������ ����Ʈ�� �޾ƿ� �� ȭ�鿡 �׸���.
		// ���� ª�� ������ �ɱ� ������ ȿ�����̶�� �����Ѵ�.
		std::list<std::pair<Handle<Snake>::Type, GameObjectClone>> snake_pairs;
		std::list<GameObjectClone> apples;
		RetrieveObjectList(snake_pairs, apples);
		//

		acoross::Win::WDC memdc(::CreateCompatibleDC(wdc.Get()));
		static HBITMAP hbitmap = ::CreateCompatibleBitmap(memdc.Get(), client_rect.right, client_rect.bottom);
		HBITMAP oldbit = (HBITMAP)::SelectObject(memdc.Get(), hbitmap);

		double ratio = 1.0;

		// �׵θ� �׸���
		memdc.Rectangle(0, 0,
			screen_width, screen_height);

		// TODO
		// ȭ��� game_session ũ�⸦ ����� ratio �� ���� ��,
		// ratio �� ���� ũ�⸦ �����ؼ� �׸���.

		{
			MeanProcessTimeChecker mean_draw(mean_draw_time_ms_);
			//auto player = player_.lock();
			for (auto& snake_pair : snake_pairs)
			{
				if (snake_pair.first == player_handle.handle)
				{
					HBRUSH oldbrush = (HBRUSH)::SelectObject(memdc.Get(), ::GetStockObject(BLACK_BRUSH));
					DrawSnake(memdc, snake_pair.second);
					(HBRUSH)::SelectObject(memdc.Get(), oldbrush);
				}
				else
				{
					DrawSnake(memdc, snake_pair.second);
				}
			}

			for (auto& apple : apples)
			{
				DrawMovingObject(memdc, apple.head_);
			}
		}

		::BitBlt(wdc.Get(), 0, 0, client_rect.right, client_rect.bottom, memdc.Get(), 0, 0, SRCCOPY);

		::SelectObject(memdc.Get(), oldbit);
		::DeleteObject(memdc.Get());
	}
	
	void set_player_handle(uintptr_t handle)
	{
		player_handle = handle;
	}

	bool UpdateGameObjectPositions(messages::UpdateGameObjectsEvent& got_msg);

private:
	std::shared_ptr<messages::SnakebiteService::Stub> stub_;

	int screen_width{ 500 };	// init value. set this by server value
	int screen_height{ 500 };	// init value. set this by server value
	Handle<Snake> player_handle{ nullptr };
};

}
}
#endif //SNAKEBITE_GAME_CLIENT_H_