#ifndef SNAKEBITE_GAME_SESSION_H_
#define SNAKEBITE_GAME_SESSION_H_

#include <list>
#include <memory>
#include <utility>

#include "moving_object.h"

namespace acoross {
namespace snakebite {

// ��, MovingObject �� �����Ǵ� �ϳ��� ���� ����.
class GameSession final
{
public:
	typedef std::list<std::unique_ptr<MovingObject>> ListMovingObject;
	GameSession() {}
	~GameSession() {}

	void Initialize();
	void CleanUp();

	// moving_objects_ �� ��ġ�� �����Ѵ�.
	void UpdateMove(int64_t diff_in_ms);

	// game field APIs
	const int Left{ 0 };
	const int Right{ 1000 };
	const int Top{ 0 };
	const int Bottom{ 1000 };

	int Width() const { return Right - Left; }
	int Height() const { return Top - Bottom; }

	//�ӽ÷� �����ִ� API
	ListMovingObject& GetMovingObjects() { return moving_objects_; }

private:
	// unique_ptr �̶� �ڵ� ������.
	ListMovingObject moving_objects_;
};

typedef std::shared_ptr<GameSession> GameSessionSP;
typedef std::weak_ptr<GameSession> GameSessionWP;

}
}
#endif //SNAKEBITE_GAME_SESSION_H_