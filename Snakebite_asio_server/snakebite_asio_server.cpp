//#include "SnakebiteM1.h"
#include <acoross/snakebite/win/Resource.h>
#include <acoross/snakebite/win/WinWrapper.h>

#include <memory>
#include <functional>
#include <thread>
#include <boost/asio.hpp>

#include <acoross/snakebite/game_session.h>
#include <acoross/snakebite/game_client.h>
#include "game_server.h"

using namespace acoross::snakebite;

std::shared_ptr<GameSession> g_game_session;
std::unique_ptr<GameClient> g_game_client;

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
				g_game_client->SetKeyDown(PK_LEFT);
			}
			else if (wParam == VK_RIGHT)
			{
				g_game_client->SetKeyDown(PK_RIGHT);
			}
			else if (wParam == VK_RETURN)
			{
				//g_game_session->AddSnakeNpc();
				g_game_session->AddSnake();
			}
			else if (wParam == VK_SPACE)
			{
				g_game_session->AddApple();
			}
			else if (wParam == VK_F5)
			{
				g_game_client->InitPlayer();
			}
		}
		break;
	case WM_KEYUP:
		{
			if (wParam == VK_LEFT)
			{
				g_game_client->SetKeyUp(PK_LEFT);
			}
			else if (wParam == VK_RIGHT)
			{
				g_game_client->SetKeyUp(PK_RIGHT);
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

			//g_game_drawer->Draw(memdc);
			g_game_client->Draw(memdc);

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
	g_game_session = std::make_shared<GameSession>();
	//g_game_drawer = std::make_unique<GameSessionDrawer>(*g_game_session.get());
	g_game_client = std::make_unique<GameClient>(*g_game_session.get());

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	acoross::Win::Window window(hInstance);
	window.MyRegisterClass(WndProc);
	if (!window.InitInstance(nCmdShow))
	{
		return FALSE;
	}

	boost::asio::io_service io_service;
	std::thread game_thread(
		[&io_service]()
		{
			GameServer s(io_service, 22000, g_game_session);
			io_service.run();
		}
	);

	auto loop = [](MSG& msg)
	{
		static DWORD lasttick2draw = ::GetTickCount();
		DWORD tick = ::GetTickCount();
		auto difftick2draw = (int64_t)tick - lasttick2draw;
		if (difftick2draw > 33)
		{
			InvalidateRect(msg.hwnd, nullptr, false);
			lasttick2draw = tick;
		}
	};
	window.PeekMessegeLoop(loop);

	io_service.stop();
	game_thread.join();

	return 0;
}