#include <acoross/snakebite/win/Resource.h>
#include <acoross/snakebite/win/WinWrapper.h>

#include <iostream>
#include <thread>
#include <memory>
#include <SDKDDKVer.h>
#include <boost/asio.hpp>

#include <acoross/rpc/rpc_server.h>
#include "game_client.h"

#define ACOROSS_USE_TBB
#ifdef ACOROSS_USE_TBB
//#include "tbb/tbbmalloc_proxy.h"
#endif

using boost::asio::ip::tcp;
using namespace acoross::snakebite;

std::weak_ptr<GameClient> g_game_client;

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
			if (auto game_client = g_game_client.lock())
			{
				game_client->SetKeyDown(PK_LEFT);
			}
		}
		else if (wParam == VK_RIGHT)
		{
			if (auto game_client = g_game_client.lock())
			{
				game_client->SetKeyDown(PK_RIGHT);
			}
		}
		else if (wParam == VK_F5)
		{
			if (auto game_client = g_game_client.lock())
			{
				game_client->InitPlayer();
			}
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		if (auto game_client = g_game_client.lock())
		{
			short hword = HIWORD(wParam);
			short lword = LOWORD(wParam);
			if (lword == 0)
			{
				game_client->FetchAddScalePcnt(hword > 0 ? 2 : -2);
				::InvalidateRect(hWnd, nullptr, true);
			}
		}
	}
	break;
	case WM_KEYUP:
	{
		if (wParam == VK_LEFT)
		{
			if (auto game_client = g_game_client.lock())
			{
				game_client->SetKeyUp(PK_LEFT);
			}
		}
		else if (wParam == VK_RIGHT)
		{
			if (auto game_client = g_game_client.lock())
			{
				game_client->SetKeyUp(PK_RIGHT);
			}
		}
	}
	case WM_PAINT:
	{
		RECT client_rect;
		::GetClientRect(hWnd, &client_rect);

		PAINTSTRUCT ps;
		acoross::Win::WDC wdc(::BeginPaint(hWnd, &ps));

		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		{
			//MeanProcessTimeChecker mean_draw(mean_draw_time_ms);
			if (auto game_client = g_game_client.lock())
			{
				game_client->Draw(wdc, client_rect);
			}
		}

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

	try
	{
		// TODO: ���⿡ �ڵ带 �Է��մϴ�.
		using namespace ::boost::asio;
		io_service io_service;
		ip::tcp::socket socket(io_service);
		{
			tcp::resolver resolver(io_service);
			boost::asio::connect(socket, resolver.resolve({ "127.0.0.1", "22000" }));
		}
		
		auto game_client = std::make_shared<GameClient>(io_service, std::move(socket));
		g_game_client = game_client;

		game_client->start();

		std::thread game_thread(
			[&io_service]()
		{
			io_service.run();
		});

		// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
		acoross::Win::Window window(hInstance);
		window.MyRegisterClass(WndProc);
		if (!window.InitInstance(nCmdShow))
		{
			return FALSE;
		}

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
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
		::MessageBoxA(NULL, e.what(), "error", MB_OK);
	}

	return 0;
}