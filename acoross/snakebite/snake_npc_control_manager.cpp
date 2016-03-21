#include "snake_npc_control_manager.h"

#include <chrono>
#include <algorithm>

#include <acoross/snakebite/game_session.h>

namespace acoross {
namespace snakebite {

// �ӽ�:
// �����ϰ� ������ ����.
// UpdatteMove �� �Ҹ� Ƚ���� �������,
// �ð��� ������ȯ Ƚ���� �����ϵ��� ������ ����.
static bool checkChangeDirection(int64_t diff_in_ms)
{
	static int64_t delay_sum = 0;

	delay_sum += diff_in_ms;
	if (delay_sum > 100 /*1s*/)
	{
		delay_sum = 0;
		return true;
	}

	return false;
}

static void changeDirection(std::default_random_engine& re, SnakeNpcControlManager::MapSnakeWP& snakes, int64_t diff_in_ms)
{
	std::uniform_int_distribution<int> unin(0, 100);

	for (auto& snake_wp : snakes)
	{
		if (auto snake = snake_wp.second.lock())
		{
			int p = unin(re);
			if (p < 15) // 5 percent
			{
				snake->SetKeyDown(PK_LEFT);
			}
			else if (p < 30) // another 5 percent
			{
				snake->SetKeyDown(PK_RIGHT);
			}
			else if (p < 45)
			{
				snake->SetKeyUp(PK_LEFT);
				snake->SetKeyUp(PK_RIGHT);
			}
		}
	}
}

SnakeNpcControlManager::SnakeNpcControlManager(GameSession& game_session)
	: game_session_(game_session)
{
	auto clock = std::chrono::high_resolution_clock();
	auto t = clock.now();
	random_engine_.seed((unsigned int)t.time_since_epoch().count());
}

void SnakeNpcControlManager::ChangeNpcDirection(int64_t diff_in_ms)
{
	std::lock_guard<std::recursive_mutex> lock(snake_npcs_mutex_);

	// �ӽ�:
	// �����ϰ� ������ ����.
	// UpdatteMove �� �Ҹ� Ƚ���� �������,
	// �ð��� ������ȯ Ƚ���� �����ϵ��� ������ ����.
	if (checkChangeDirection(diff_in_ms))
	{
		changeDirection(random_engine_, snake_npcs_, diff_in_ms);
	}
}

std::weak_ptr<Snake> SnakeNpcControlManager::AddSnakeNpc()
{
	auto snake = game_session_.AddSnake(
		/* onDie */
		[this_ = this](Snake& snake)
		{
			this_->RemoveSnakeNpc(&snake);
			this_->AddSnakeNpc();	// ������ ������ ����
		}
	);

	if (auto snakesp = snake.lock())
	{
		std::lock_guard<std::recursive_mutex> lock(snake_npcs_mutex_);
		snake_npcs_.emplace(snakesp.get(), snake);
	}
	return snake;
}

bool SnakeNpcControlManager::RemoveSnakeNpc(Snake* snake)
{
	{
		std::lock_guard<std::recursive_mutex> lock(snake_npcs_mutex_);
		snake_npcs_.erase(snake);
	}
	
	return game_session_.RemoveSnake(snake);
}

}//namespace snakebite
}//namespace acoross