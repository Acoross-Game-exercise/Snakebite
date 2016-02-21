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
			wdc.Rectangle(gs->Left, gs->Top, gs->Right, gs->Bottom);

			int h = gs->Height();
			int w = gs->Width();

			// TODO
			// ȭ��� game_session ũ�⸦ ����� ratio �� ���� ��,
			// ratio �� ���� ũ�⸦ �����ؼ� �׸���.
			
			auto& mo_list = gs->GetMovingObjects();
			for (auto& mo : mo_list)
			{
				const int radius = mo->GetRadius();

				auto pos = mo->GetPosition();
				auto ang = mo->GetAngle();

				int center_x = (int)pos.x;
				int center_y = (int)pos.y;

				wdc.Ellipse(center_x - radius, center_y - radius, 
					center_x + radius, center_y + radius);
			}
		}
	}

private:
	std::weak_ptr<GameSession> game_session_;
};

}
}
#endif