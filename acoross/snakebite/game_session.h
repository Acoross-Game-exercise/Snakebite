#ifndef SNAKEBITE_GAME_SESSION_H_
#define SNAKEBITE_GAME_SESSION_H_

#include <list>
#include <memory>
#include <utility>

#include <acoross/snakebite/moving_object_system/moving_object_system.h>
#include "snake_piece.h"

enum PlayerKey
{
	PK_NONE,
	PK_RIGHT,
	PK_LEFT
};

namespace acoross {
namespace snakebite {

// ��, MovingObject �� �����Ǵ� �ϳ��� ���� ����.
class GameSession final
{
public:
	typedef MovingObjectContainer::ListMovingObject ListMovingObject;
	typedef std::list<std::weak_ptr<SnakePiece>> ListSnakePiece;

	GameSession() 
	{
		Initialize();
	}
	~GameSession() 
	{
		CleanUp();
	}

	void Initialize();
	void CleanUp();

	// moving_objects_ �� ��ġ�� �����Ѵ�.
	void UpdateMove(int64_t diff_in_ms);
	
	void ProcessCollisions()
	{
		//container_.CheckCollisions();
	}

	//�ӽ÷� �����ִ� API
	ListMovingObject& GetMovingObjects()
	{ 
		return container_.GetMovingObjects();
	}

	void SetPlayerKey(PlayerKey player_key)
	{
		last_pk_ = player_key;
	}

	void SetKeyUp(PlayerKey player_key)
	{
		if (last_pk_ == player_key)
		{
			last_pk_ = PK_NONE;
		}
	}

	PlayerKey GetPlayerKey() const
	{
		return last_pk_;
	}

	PlayerKey RetrievePlayerKey()
	{
		auto ret = last_pk_;
		last_pk_ = PK_NONE;
		return ret;
	}

	// �ӽ�
	const MovingObjectContainer& GetContainer() const { return container_; }

private:
	ListSnakePiece snakes_;
	MovingObjectContainer container_;
	PlayerKey last_pk_{ PK_NONE };
};

typedef std::shared_ptr<GameSession> GameSessionSP;
typedef std::weak_ptr<GameSession> GameSessionWP;

}
}
#endif //SNAKEBITE_GAME_SESSION_H_