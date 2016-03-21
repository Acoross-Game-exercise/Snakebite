#ifndef SNAKEBITE_GAME_SESSION_H_
#define SNAKEBITE_GAME_SESSION_H_

#include <list>
#include <map>
#include <memory>
#include <utility>
#include <random>
#include <mutex>

#include <acoross/snakebite/moving_object_system/moving_object_system.h>
#include "snake.h"
#include "Apple.h"
#include "snake_npc_control_manager.h"

namespace acoross {
namespace snakebite {

// ��, MovingObject �� �����Ǵ� �ϳ��� ���� ����.
class GameSession final
{
public:
	using ListMovingObject = MovingObjectContainer::ListMovingObject;
	
	explicit GameSession(unsigned int init_apple_count = 20, int width = 500, int height = 500);
	~GameSession();

	// use lock
	void UpdateMove(int64_t diff_in_ms);
	void ProcessCollisions();
	
	SnakeWP AddSnakeNpc()
	{
		return npc_controll_manager_.AddSnakeNpc();
	}

	SnakeWP AddSnake(Snake::EventHandler onDieHandler = Snake::EventHandler());
	void AddApple();
	bool RemoveSnake(Handle<Snake>::Type snake);
	bool RemoveApple(Apple* apple);

	auto CloneSnakeList()
	{
		std::lock_guard<std::recursive_mutex> lock(snakes_mutex_);
		std::list<std::pair<Handle<Snake>::Type, GameObjectClone>> snakes;
		
		for (auto pair : snakes_)
		{
			snakes.push_back(std::make_pair(pair.first, pair.second->Clone()));
		}

		return snakes;
	}
	std::list<GameObjectClone> CloneAppleList()
	{
		std::lock_guard<std::recursive_mutex> lock(snakes_mutex_);
		std::list<GameObjectClone> apples;

		for (auto apple : apples_)
		{
			apples.push_back(apple->Clone());
		}

		return apples;
	}
	//

	size_t CalculateSnakeCount() 
	{
		std::lock_guard<std::recursive_mutex> lock(snakes_mutex_);
		return snakes_.size();
	}

	size_t CalculateAppleCount()
	{
		std::lock_guard<std::recursive_mutex> lock(snakes_mutex_);
		return apples_.size();
	}

	std::recursive_mutex& LockSnakes()
	{
		return snakes_mutex_;
	}

	//�ӽ÷� �����ִ� API
	MovingObjectContainer& GetContainer() { return container_; }
	//

private:
	using MapSnake = std::map<Handle<Snake>::Type, SnakeSP>;
	using ListApple = std::list<AppleSP>;
	using CollisionMap = std::map<Handle<Snake>::Type, GameObjectWP>;
	using CollisionSet = std::set<Handle<Snake>::Type>;

	void ProcessCollisionToWall(SnakeSP actor);

	CollisionSet wall_collision_set_;
	CollisionMap collision_map_;
	MovingObjectContainer container_;
	SnakeNpcControlManager npc_controll_manager_;

	std::default_random_engine random_engine_;

#pragma region snakes
	MapSnake snakes_;
	ListApple apples_;

	// ���Ű����� drawer �� ����ȭ �ϱ� ���� �߰��� lock �̴�...
	// ���� �ʿ�.
	std::recursive_mutex snakes_mutex_;
#pragma endregion snakes

	const double radius{ 5. };		// UNIT
	const double velocity{ 0.06 };	// UNIT/ms
	const Position2D player_pos{ 100, 100 };

	//�ӽ�
	friend class GameClient;
};

typedef std::shared_ptr<GameSession> GameSessionSP;
typedef std::weak_ptr<GameSession> GameSessionWP;

}
}
#endif //SNAKEBITE_GAME_SESSION_H_