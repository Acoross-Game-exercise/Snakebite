#ifndef SNAKEBITE_LOCAL_GAME_CLIENT_H_
#define SNAKEBITE_LOCAL_GAME_CLIENT_H_

#include <acoross/snakebite/win/WinWrapper.h>

#include <acoross/snakebite/game_session.h>
#include <acoross/snakebite/game_client_base.h>
#include "game_server.h"

namespace acoross {
namespace snakebite {

class LocalGameClient final
	: public GameClientBase
{
public:
	LocalGameClient(GameServer& game_server)
		: GameClientBase()
		, game_server_(game_server)
	{}
	virtual ~LocalGameClient(){}
	
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
			game_server_.Width, game_server_.Height);
		
		// TODO
		// ȭ��� game_session ũ�⸦ ����� ratio �� ���� ��,
		// ratio �� ���� ũ�⸦ �����ؼ� �׸���.

		{
			MeanProcessTimeChecker mean_draw(mean_draw_time_ms_);
			auto player = player_.lock();
			for (auto& snake_pair : snake_pairs)
			{
				if (snake_pair.first == Handle<Snake>(player.get()).handle)
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

	//@lock
	virtual void InitPlayer() override
	{
		game_server_.RequestToSession(
			[_this = this](GameSession& session)
		{
			if (auto player = _this->player_.lock())
			{
				session.RemoveSnake(Handle<Snake>(player.get()).handle);
			}
			_this->player_ = session.AddSnake(Snake::EventHandler(), "local player");
		});
	}
	//

	//@atomic for Snake
	virtual void SetKeyDown(PlayerKey player_key) override
	{
		if (auto player = player_.lock())
		{
			player->SetKeyDown(player_key);
		}
	}

	virtual void SetKeyUp(PlayerKey player_key) override
	{
		if (auto player = player_.lock())
		{
			player->SetKeyUp(player_key);
		}
	}
	//
	
private:
	//FIXME: !!! player_ �� exchange �� atomic �ؾ���
	// lock �� �߰��ϴ��� ���� �ڵ� ���� �ʿ�.
	//std::atomic<std::weak_ptr<Snake>> player_;
	std::weak_ptr<Snake> player_;
	GameServer& game_server_;
};

}
}
#endif //SNAKEBITE_LOCAL_GAME_CLIENT_H_