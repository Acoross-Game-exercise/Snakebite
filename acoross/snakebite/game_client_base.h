#ifndef SNAKEBITE_GAME_CLIENT_BASE_H_
#define SNAKEBITE_GAME_CLIENT_BASE_H_

#include <acoross/snakebite/win/WinWrapper.h>

#include <list>
#include <atomic>
#include <mutex>
#include <memory>

#include <acoross/snakebite/util.h>
#include <acoross/snakebite/moving_object_system/moving_object_system.h>
#include <acoross/snakebite/game_session/game_session_system.h>

namespace acoross {
namespace snakebite {

class GameClientBase
{
public:
	virtual ~GameClientBase() {}

	void SetObjectList(
		int idx_x, int idx_y, 
		std::list<std::pair<Handle<Snake>::Type, GameObjectClone>>&& snake_clone_list,
		std::list<GameObjectClone>&& apple_clone_list)
	{
		std::lock_guard<std::mutex> lock(clone_list_mutex_);
		zone_snake_clone_list_[std::make_pair(idx_x, idx_y)] = std::move(snake_clone_list);
		zone_apple_clone_list_[std::make_pair(idx_x, idx_y)] = std::move(apple_clone_list);
		zone_clone_list_changed_[std::make_pair(idx_x, idx_y)].store(true);
	}

	void RetrieveObjectList(
		int idx_x, int idx_y, 
		std::list<std::pair<Handle<Snake>::Type, GameObjectClone>>& snake_clone_list,
		std::list<GameObjectClone>& apple_clone_list)
	{
		std::lock_guard<std::mutex> lock(clone_list_mutex_);
		
		auto it_zone_snakes = zone_snake_clone_list_.find(std::make_pair(idx_x, idx_y));
		if (it_zone_snakes != zone_snake_clone_list_.end())
		{
			snake_clone_list = it_zone_snakes->second;
		}

		auto it_zone_apples = zone_apple_clone_list_.find(std::make_pair(idx_x, idx_y));
		if (it_zone_apples != zone_apple_clone_list_.end())
		{
			apple_clone_list = it_zone_apples->second;
		}
	}

	virtual void Draw(Win::WDC& wdc, RECT& client_rect) = 0;

	virtual void InitPlayer() = 0;

	virtual void SetKeyDown(PlayerKey player_key) = 0;

	virtual void SetKeyUp(PlayerKey player_key) = 0;

	std::atomic<size_t> snake_count_{ 0 };
	std::atomic<size_t> apple_count_{ 0 };
	std::atomic<double> mean_draw_time_ms_{ 0 };

protected:
	//@need GameSession::snakes_mutex_ locked
	static void DrawSnake(Win::WDC& wdc, GameObjectClone& snake)
	{
		DrawSnakeName(wdc, snake);

		DrawMovingObject(wdc, snake.head_);
		for (auto& body : snake.body_list_)
		{
			DrawMovingObject(wdc, body);
		}
	}
	static void DrawMovingObject(Win::WDC& wdc, MovingObject& mo)
	{
		const int radius = (int)mo.GetRadius();
		const auto pos = mo.GetPosition();
		const int center_x = (int)pos.x;
		const int center_y = (int)pos.y;

		wdc.Ellipse(center_x - radius, center_y - radius,
			center_x + radius, center_y + radius);
	}
	static void DrawSnakeName(Win::WDC& wdc, GameObjectClone& snake)
	{
		auto pos = snake.head_.GetPosition();
		RECT rect{
			(long)pos.x - 50, (long)pos.y - 20,
			(long)pos.x + 50, (long)pos.y - 5
		};
		wdc.DrawTextA(snake.Name, rect, DT_CENTER | DT_VCENTER);
	}
	//

protected:
	using CloneSnakeList = std::list<std::pair<Handle<Snake>::Type, GameObjectClone>>;
	using CloneAppleList = std::list<GameObjectClone>;
	std::mutex clone_list_mutex_;
	std::map<std::pair<int, int>, CloneSnakeList> zone_snake_clone_list_;
	std::map<std::pair<int, int>, CloneAppleList> zone_apple_clone_list_;
	std::map<std::pair<int, int>, std::atomic<bool>> zone_clone_list_changed_;

	int limit_idx_x_{ 1 };
	int limit_idx_y_{ 1 };
	int zone_width_{ 1 };
	int zone_height_{ 1 };
};

}
}
#endif //SNAKEBITE_GAME_CLIENT_BASE_H_