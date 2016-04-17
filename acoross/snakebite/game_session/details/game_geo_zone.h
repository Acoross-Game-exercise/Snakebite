#ifndef SNAKEBITE_GAME_GEO_ZONE_H_
#define SNAKEBITE_GAME_GEO_ZONE_H_

#include <mutex>
#include <vector>
#include <queue>

#include "game_geo_zone_define.h"

namespace acoross {
namespace snakebite {

//////////////////////////////////////////////////
// ���� ������ �������� zone ���� �ɰ��� ����.
class GameGeoZoneGrid;

// �ϳ��� GameZone �� serializer �̿��� �̱۽������ �����ϰ� �غ���.
class GameGeoZone
{
public:
	explicit GameGeoZone(GameGeoZoneGrid& owner_zone_grid, int idx_zone_x, int idx_zone_y, 
		MovingObjectContainer& game_boundary, int left, int top, int width, int height);
	~GameGeoZone();

#pragma region use_snakes_mutex_
	// update every snake position
	void UpdateMove(int64_t diff_in_ms);
	// check collision and handle collsion event for objects in near zone.
	void ProcessCollisions(
		std::map<std::pair<int, int>, std::pair<MapSnake, ListApple>>& zone_grid_objects);

	bool AddApple(std::shared_ptr<Apple> apple);
	bool RemoveApple(Apple* apple);
	bool AddSnake(std::shared_ptr<Snake> snake);
	bool RemoveSnake(Handle<Snake>::Type snake);

	std::list<std::pair<Handle<Snake>::Type, GameObjectClone>>
		CloneSnakeList();
	std::list<GameObjectClone> CloneAppleList();
	size_t CalculateSnakeCount();
	size_t CalculateAppleCount();
	auto& GetSnakes()
	{
		std::lock_guard<std::recursive_mutex> lock(snakes_mutex_);
		return snakes_;
	}
	auto& GetApples()
	{
		std::lock_guard<std::recursive_mutex> lock(snakes_mutex_);
		return apples_;
	}
#pragma endregion use_snakes_mutex_

	/*void CheckAndRequestChangeZone();

	void RequestEnterZone(std::shared_ptr<Snake> snake, int org_idx_zone_x, int org_idx_zone_y);
	void ProcessAllEnterZoneRequests();

	void RequestLeaveZone(std::shared_ptr<Snake> snake, int org_idx_zone_x, int org_idx_zone_y);
	void ProcessAllLeaaveRequests();*/

public:
	const int IDX_ZONE_X;
	const int IDX_ZONE_Y;

private:
	// �ٸ� ���ϰ��� �浹üũ
	void ProcessCollisionToOtherZone(GameGeoZone& other_zone);

	// ���� �浹�ߴ��� üũ.
	// ���� �浹�ϸ� ƨ�ܳ���.
	void ProcessCollisionToWall(SnakeSP actor);

private:
	GameGeoZoneGrid& zone_grid_;
	MovingObjectContainer zone_boundary_;
	MovingObjectContainer& game_boundary_;

#pragma region snakes - use_snakes_mutex_
	MapSnake snakes_;
	ListApple apples_;
	std::recursive_mutex snakes_mutex_;
#pragma endregion snakes - use_snakes_mutex_

	CollisionSet wall_collision_set_;

	/*struct EnteringInfo
	{
		std::shared_ptr<Snake> snake;
		int org_idx_zone_x;
		int org_idx_zone_y;
	};
	std::recursive_mutex enter_leave_mutex_;
	std::queue<EnteringInfo> leaving_snakes_;
	std::queue<EnteringInfo> entering_snakes_;*/
};
//


}
}
#endif //SNAKEBITE_GAME_GEO_ZONE_H_