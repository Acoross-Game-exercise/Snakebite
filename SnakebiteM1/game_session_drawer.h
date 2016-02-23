#ifndef SNAKEBITE_GAME_SESSION_DRAWER_H_
#define SNAKEBITE_GAME_SESSION_DRAWER_H_

#include <memory>
#include <utility>

#include "WinWrapper.h"
#include "game_session.h"

namespace acoross {
namespace snakebite {

class GameSessionDrawer final
{
public:
	GameSessionDrawer(GameSessionSP& gs)
		: game_session_(gs)
	{}
	virtual ~GameSessionDrawer(){}

	void Draw(Win::WDC& wdc)
	{
		double ratio = 1.0;

		auto gs = game_session_.lock();
		if (gs)
		{
			// �׵θ� �׸���
			wdc.Rectangle(gs->GetContainer().Left, gs->GetContainer().Top, 
				gs->GetContainer().Right, gs->GetContainer().Bottom);
			
			// TODO
			// ȭ��� game_session ũ�⸦ ����� ratio �� ���� ��,
			// ratio �� ���� ũ�⸦ �����ؼ� �׸���.
			
			auto& mo_list = gs->GetMovingObjects();
			for (auto& mo : mo_list)
			{
				const int radius = (int)mo->GetRadius();

				auto pos = mo->GetPosition();
				//auto ang = mo->GetAngle();

				int center_x = (int)pos.x;
				int center_y = (int)pos.y;

				auto collider_handle = mo->GetColliderHandle();
				if (collider_handle && collider_handle->GetCollider().Collided)
				{
					HBRUSH oldbrush = (HBRUSH)::SelectObject(wdc.Get(), ::GetStockObject(BLACK_BRUSH));
					
					wdc.Ellipse(center_x - radius, center_y - radius,
						center_x + radius, center_y + radius);

					(HBRUSH)::SelectObject(wdc.Get(), oldbrush);
				}
				else
				{
					wdc.Ellipse(center_x - radius, center_y - radius,
						center_x + radius, center_y + radius);
				}
			}
		}
	}

private:
	std::weak_ptr<GameSession> game_session_;
};

}
}
#endif