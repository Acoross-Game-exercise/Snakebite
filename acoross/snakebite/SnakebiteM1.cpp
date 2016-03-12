#include "SnakebiteM1.h"

#include <functional>

#include <acoross/snakebite/win/WinWrapper.h>
#include "game_session.h"
#include "game_session_drawer.h"

std::shared_ptr<acoross::snakebite::GameSession> gamesession;
std::unique_ptr<acoross::snakebite::GameSessionDrawer> g_game_drawer;

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) // static
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_KEYDOWN:
		{
			if (wParam == VK_LEFT)
			{
				gamesession->SetPlayerKey(PK_LEFT);
			}
			else if (wParam == VK_RIGHT)
			{
				gamesession->SetPlayerKey(PK_RIGHT);
			}
			else if (wParam == VK_RETURN)
			{
				gamesession->AddSnake();
			}
			else if (wParam == VK_SPACE)
			{
				gamesession->AddApple();
			}
		}
		break;
	case WM_KEYUP:
		{
			if (wParam == VK_LEFT)
			{
				gamesession->SetKeyUp(PK_LEFT);
			}
			else if (wParam == VK_RIGHT)
			{
				gamesession->SetKeyUp(PK_RIGHT);
			}
		}
	case WM_PAINT:
	{
		RECT client_rect;
		::GetClientRect(hWnd, &client_rect);

		PAINTSTRUCT ps;
		acoross::Win::WDC wdc(::BeginPaint(hWnd, &ps));
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		acoross::Win::WDC memdc(::CreateCompatibleDC(wdc.Get()));
		static HBITMAP hbitmap = ::CreateCompatibleBitmap(memdc.Get(), client_rect.right, client_rect.bottom);
		HBITMAP oldbit = (HBITMAP)::SelectObject(memdc.Get(), hbitmap);

		g_game_drawer->Draw(memdc);
		::BitBlt(wdc.Get(), 0, 0, client_rect.right, client_rect.bottom, memdc.Get(), 0, 0, SRCCOPY);

		::SelectObject(memdc.Get(), oldbit);
		//::DeleteObject(hbitmap);
		::DeleteObject(memdc.Get());
		::EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	using namespace acoross::snakebite;
	gamesession = std::make_unique<GameSession>();
	g_game_drawer = std::make_unique<GameSessionDrawer>(*gamesession.get());

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	acoross::Win::Window window(hInstance);
	window.MyRegisterClass(WndProc);
	if (!window.InitInstance(nCmdShow))
    {
        return FALSE;
    }

	auto loop = [](MSG& msg)
	{
		{
			const DWORD frametickdiff = 33;

			static DWORD lasttick = ::GetTickCount();
			DWORD tick = ::GetTickCount();
			auto difftick = (int64_t)tick - lasttick;
			for (;difftick > frametickdiff; difftick -= frametickdiff)
			{
#if defined(_DEBUG)
				gamesession->UpdateMove(frametickdiff);
#else
				gamesession->UpdateMove(difftick);
#endif	
				gamesession->ProcessCollisions();
				lasttick = tick;
			}
		}

		{
			static DWORD lasttick2draw = ::GetTickCount();
			DWORD tick = ::GetTickCount();
			auto difftick2draw = (int64_t)tick - lasttick2draw;
			if (difftick2draw > 200)
			{
				InvalidateRect(msg.hwnd, nullptr, false);
				lasttick2draw = tick;
			}
		}		
	};

	return window.PeekMessegeLoop(loop);
}
