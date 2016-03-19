//#include "SnakebiteM1.h"
#include <acoross/snakebite/win/Resource.h>
#include <acoross/snakebite/win/WinWrapper.h>

#include <memory>
#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <string>
#include <sstream>
#include <strsafe.h>
#include <atomic>

#include <acoross/snakebite/game_session.h>
#include <acoross/snakebite/game_client.h>
#include "game_server.h"

using namespace acoross::snakebite;

std::shared_ptr<GameSession> g_game_session;
std::unique_ptr<GameClient> g_game_client;
std::weak_ptr<GameServer> g_game_server_wp;

//<test>
std::atomic<double> mean_draw_time_ms{ 0 };
//</test>

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
				g_game_session->AddSnakeNpc();
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

			double beforeDrawTick = static_cast<double>(::GetTickCount64());
				PAINTSTRUCT ps;
				acoross::Win::WDC wdc(::BeginPaint(hWnd, &ps));
				// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
				acoross::Win::WDC memdc(::CreateCompatibleDC(wdc.Get()));
				static HBITMAP hbitmap = ::CreateCompatibleBitmap(memdc.Get(), client_rect.right, client_rect.bottom);
				HBITMAP oldbit = (HBITMAP)::SelectObject(memdc.Get(), hbitmap);

				//g_game_drawer->Draw(memdc);

				g_game_client->Draw(memdc);
				
				if (auto game_server = g_game_server_wp.lock())
				{
					RECT rect{ 
						g_game_session->GetContainer().Right + 10,
						10,
						g_game_session->GetContainer().Right + 300,
						110
					};

					//::FillRect(wdc.Get(), &rect, ::GetSysColorBrush(0));

					wchar_t outBuf[1000] = { 0, };
					::StringCchPrintfW(outBuf, 1000,
						L"snakes: %d, apples: %d, \n"
						L"mean move time: %.4f(ms), \n"
						L"mean collision time: %.4f(ms)\n"
						L"mean draw time: %.4f(ms)\n",
						g_game_session->CalculateSnakeCount(),
						g_game_session->CalculateAppleCount(),
						game_server->mean_move_time_ms_.load(),
						game_server->mean_collision_time_ms_.load(),
						mean_draw_time_ms.load());

					size_t str_len = 0;
					::StringCchLengthW(outBuf, 1000, &str_len);

					::DrawTextW(memdc.Get(), outBuf, (int)str_len, &rect, DT_CENTER);
				}

				::BitBlt(wdc.Get(), 0, 0, client_rect.right, client_rect.bottom, memdc.Get(), 0, 0, SRCCOPY);

				::SelectObject(memdc.Get(), oldbit);
				::DeleteObject(memdc.Get());
			double afterDrawTick = static_cast<double>(::GetTickCount64());
			double new_mean_draw = mean_draw_time_ms.load() * 0.9 + (afterDrawTick - beforeDrawTick) * 0.1;
			mean_draw_time_ms.store(new_mean_draw);

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
			auto server = std::make_shared<GameServer>(io_service, 22000, g_game_session);
			g_game_server_wp = server;
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