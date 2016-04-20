#ifndef SNAKEBITE_LOCAL_GAME_CLIENT_H_
#define SNAKEBITE_LOCAL_GAME_CLIENT_H_

#include <acoross/snakebite/win/WinWrapper.h>

#include <acoross/snakebite/game_session/game_session_system.h>
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
	{
		RequestZoneInfo();
	}
	virtual ~LocalGameClient(){}
	
	virtual void Draw(Win::WDC& wdc, RECT& client_rect) override
	{
		acoross::Win::WDC memdc(::CreateCompatibleDC(wdc.Get()));
		static HBITMAP hbitmap = ::CreateCompatibleBitmap(wdc.Get(), client_rect.right, client_rect.bottom);
		HBITMAP oldbit = (HBITMAP)::SelectObject(memdc.Get(), hbitmap);

		double ratio = 1.0;

		for (int idx_x = 0; idx_x < limit_idx_x_; ++idx_x)
		{
			for (int idx_y = 0; idx_y < limit_idx_y_; ++idx_y)
			{
				DrawGrid(memdc, idx_x, idx_y);
			}
		}

		for (int idx_x = 0; idx_x < limit_idx_x_; ++idx_x)
		{
			for (int idx_y = 0; idx_y < limit_idx_y_; ++idx_y)
			{
				DrawZone(memdc, idx_x, idx_y);
			}
		}

		::BitBlt(wdc.Get(), 0, 0, client_rect.right, client_rect.bottom, memdc.Get(), 0, 0, SRCCOPY);

		::SelectObject(memdc.Get(), oldbit);
		::DeleteObject(memdc.Get());
	}

	void DrawGrid(Win::WDC& memdc, int idx_x, int idx_y)
	{
		auto it = zone_clone_list_changed_.find(std::make_pair(idx_x, idx_y));
		if (it == zone_clone_list_changed_.end() || it->second == false)
		{
			return;
		}

		// �׵θ� �׸���
		if (idx_zone_player_x == idx_x && idx_zone_player_y == idx_y)
		{
			HBRUSH oldbrush = (HBRUSH)::SelectObject(memdc.Get(), ::GetStockObject(GRAY_BRUSH));
			memdc.Rectangle(zone_width_ * idx_x, zone_height_ * idx_y,
				zone_width_ * (idx_x + 1), zone_height_ * (idx_y + 1));
			(HBRUSH)::SelectObject(memdc.Get(), oldbrush);
		}
		else
		{
			memdc.Rectangle(zone_width_ * idx_x, zone_height_ * idx_y,
				zone_width_ * (idx_x + 1), zone_height_ * (idx_y + 1));
		}
	}

	void DrawZone(Win::WDC& memdc, int idx_x, int idx_y)
	{
		auto it = zone_clone_list_changed_.find(std::make_pair(idx_x, idx_y));
		if (it == zone_clone_list_changed_.end() || it->second == false)
		{
			return;
		}

		// snake �� apple �� ������ ����Ʈ�� �޾ƿ� �� ȭ�鿡 �׸���.
		// ���� ª�� ������ �ɱ� ������ ȿ�����̶�� �����Ѵ�.
		std::list<std::pair<Handle<Snake>::Type, GameObjectClone>> snake_pairs;
		std::list<GameObjectClone> apples;
		RetrieveObjectList(idx_x, idx_y, snake_pairs, apples);
		//

		// TODO
		// ȭ��� game_session ũ�⸦ ����� ratio �� ���� ��,
		// ratio �� ���� ũ�⸦ �����ؼ� �׸���.

		{
			MeanProcessTimeChecker mean_draw(mean_draw_time_ms_);
			for (auto& snake_pair : snake_pairs)
			{
				if (snake_pair.first == player_handle_)
				{
					auto& pos = snake_pair.second.body_list_.begin()->GetPosition();
					idx_zone_player_x = int(pos.x / game_server_.ZoneWidth);
					idx_zone_player_y = int(pos.y / game_server_.ZoneHeight);

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
	}

	void RequestZoneInfo()
	{
		limit_idx_x_ = game_server_.COUNT_ZONE_X;
		limit_idx_y_ = game_server_.COUNT_ZONE_Y;
		zone_width_ = game_server_.ZoneWidth;
		zone_height_ = game_server_.ZoneHeight;
	}

	//@lock
	virtual void InitPlayer() override
	{
		game_server_.RequestToSession(
			[_this = this, handle = player_handle_](GameSession& session)
		{
			session.RemoveSnake(_this->player_handle_);
			_this->player_handle_ = session.MakeNewSnake("local player",
				[_this](Snake& snake)
			{
				_this->SetPlayerHandleZero();
			});
		});
	}

	void SetPlayerHandleZero()
	{
		player_handle_ = 0;
	}

	//

	//@atomic for Snake
	virtual void SetKeyDown(PlayerKey player_key) override
	{
		if (player_key == player_key_)
		{
			return;
		}

		player_key_ = player_key;

		game_server_.RequestToSession(
			[player_key, handle = player_handle_](GameSession& session)
		{
			session.RequestToSnake(handle, 
				[player_key](Snake& snake)
			{
				snake.SetKeyDown(player_key);
			});
		});
	}

	virtual void SetKeyUp(PlayerKey player_key) override
	{
		if (player_key != player_key_)
		{
			return;
		}

		player_key_ = PlayerKey::PK_NONE;

		game_server_.RequestToSession(
			[player_key, handle = player_handle_](GameSession& session)
		{
			session.RequestToSnake(handle,
				[player_key](Snake& snake)
			{
				snake.SetKeyUp(player_key);
			});
		});
	}
	//
	
private:
	int idx_zone_player_x{ -1 };
	int idx_zone_player_y{ -1 };

	//FIXME: !!! player_ �� exchange �� atomic �ؾ���
	// lock �� �߰��ϴ��� ���� �ڵ� ���� �ʿ�.
	Handle<Snake>::Type player_handle_{ 0 };
	PlayerKey player_key_{ PlayerKey::PK_NONE };
	GameServer& game_server_;
};

}
}
#endif //SNAKEBITE_LOCAL_GAME_CLIENT_H_