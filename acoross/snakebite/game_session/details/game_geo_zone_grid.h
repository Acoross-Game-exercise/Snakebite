#ifndef ACOROSS_GAME_GEO_ZONE_GRID_H_
#define ACOROSS_GAME_GEO_ZONE_GRID_H_

#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <mutex>
#include <vector>
#include <queue>

#include <acoross/snakebite/moving_object_system/moving_object_system.h>
#include "game_geo_zone_define.h"

namespace acoross {
namespace snakebite {

///////////////////////////////////////////////////////////////////
class GameGeoZone;

// GeoZone �� ���� const container
// @thread-safe: containing-geo zone �� ���� ������ constant �ϹǷ� thread-safe
//	���빰�� geo zone �� ����Լ� ȣ���� geo zone ������ ���� �ٸ�.
class GameGeoZoneGrid final
{
public:
	GameGeoZoneGrid(::boost::asio::io_service& io_service, 
		int zone_width, int zone_height, int n_x, int n_y);
	~GameGeoZoneGrid() {};

	bool ProcessAllZone(std::function<bool(GameGeoZone&)> func, bool stop_condition = false);

	// x, y: ��ǥ
	// return: �ش� ��ġ�� zone
	std::shared_ptr<GameGeoZone> get_zone(double x, double y);
	// idx_x, idx_y: grid �ε���
	std::shared_ptr<GameGeoZone> get_zone_by_idx(int idx_x, int idx_y);

	MovingObjectContainer& GetBoundaryContainer()
	{
		return game_boundary_;
	}
	
	const int N_X;
	const int N_Y;

	const int ZoneWidth;
	const int ZoneHeight;

private:
	std::vector<std::vector<std::shared_ptr<GameGeoZone>>> zone_grid_;
	MovingObjectContainer game_boundary_;
};

}
}
#endif //ACOROSS_GAME_GEO_ZONE_GRID_H_