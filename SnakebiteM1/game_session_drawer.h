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
		auto gs = game_session_.lock();
		if (gs)
		{
			int h = gs->Height();
			int w = gs->Width();

			double ratio = 1.0;

			// TODO
			// ȭ��� game_session ũ�⸦ ����� ratio �� ���� ��,
			// ratio �� ���� ũ�⸦ �����ؼ� �׸���.
			
			auto& mo_list = gs->GetMovingObjects();
			for (auto& mo : mo_list)
			{
				auto pos = mo->GetPosition();
				auto ang = mo->GetAngle();

				wdc.Ellipse(100, 100, 110, 110);
			}
		}
	}

private:
	std::weak_ptr<GameSession> game_session_;
};

}
}
#endif