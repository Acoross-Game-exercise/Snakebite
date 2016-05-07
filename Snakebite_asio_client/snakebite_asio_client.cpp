#include <acoross/snakebite/win/Resource.h>
#include <acoross/snakebite/win/WinWrapper.h>
#include <Windowsx.h>
#include <shellapi.h>

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

int last_mouse_pos_x = -1;
int last_mouse_pos_y = -1;

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
			auto game_client = g_game_client.lock();
			if (!game_client)
			{
				break;
			}

			if (wParam == VK_LEFT)
			{
				game_client->SetKeyDown(PK_LEFT);
			}
			else if (wParam == VK_RIGHT)
			{
				game_client->SetKeyDown(PK_RIGHT);
			}
			else if (wParam == VK_F5)
			{
				game_client->InitPlayer();
			}
			else if (wParam == 'Q')
			{
				game_client->SetScreenCenterToPlayerPos();
			}
			else if (wParam == 'F')
			{
				game_client->FollowPlayerOnOff();
			}
			else if (wParam == 'Q')
			{
				game_client->SetScreenCenterToPlayerPos();
			}
			else if (wParam == 'O')
			{
				::InvalidateRect(hWnd, nullptr, true);
				game_client->SetScreenOnOff();
			}
			else if (wParam == 'A') // auto
			{
				game_client->SetAutoOnOff();
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
	case WM_MOUSEMOVE:
		{
			if (last_mouse_pos_x == -1 || last_mouse_pos_y == -1)
				break;

			if (wParam & MK_LBUTTON)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				if (auto game_client = g_game_client.lock())
				{
					game_client->FetchMoveScreen(last_mouse_pos_x - x, last_mouse_pos_y - y);

					last_mouse_pos_x = x;
					last_mouse_pos_y = y;
				}
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			last_mouse_pos_x = GET_X_LPARAM(lParam);
			last_mouse_pos_y = GET_Y_LPARAM(lParam);
		}
		break;
	case WM_LBUTTONUP:
		{
			::InvalidateRect(hWnd, nullptr, true);
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

class ConsoleParser final
{
public:
	// only true false
	void RegisterToken(std::wstring token, bool default_value)
	{
		token_to_result_[token] = default_value;
	}

	void Run(LPWSTR* lpArgList, int args)
	{
		for (int i = 1; i + 1 < args; i += 2)
		{
			std::wstring arg_type(lpArgList[i]);
			std::wstring arg_contents(lpArgList[i + 1]);

			parse_cmdline(arg_type, arg_contents);
		}
	}

	bool GetResult(std::wstring arg_type) const
	{
		auto ret = token_to_result_.find(arg_type);
		if (ret == token_to_result_.end())
		{
			char buf[1000]{ 0, };
			::StringCchPrintfA(buf, _countof(buf), "GetResult: invalid argument: %S", arg_type.c_str());
			throw(std::exception(buf));
			return false;
		}

		return ret->second;
	}

private:
	void parse_cmdline(std::wstring& arg_type, std::wstring& arg_contents)
	{
		auto ret = token_to_result_.find(arg_type);
		if (ret == token_to_result_.end())
		{
			char buf[1000]{ 0, };
			::StringCchPrintfA(buf, _countof(buf), "invalid argument: %S %S", arg_type.c_str(), arg_contents.c_str());
			throw(std::exception(buf));
		}

		if (arg_contents.compare(L"true") == 0)
		{
			ret->second = true;
		}
		else if (arg_contents.compare(L"false") == 0)
		{
			ret->second = false;
		}
		else
		{
			char buf[1000]{ 0, };
			::StringCchPrintfA(buf, _countof(buf), "invalid argument: %S %S", arg_type.c_str(), arg_contents.c_str());
			throw(std::exception(buf));
		}
	}

	std::map<std::wstring, bool> token_to_result_;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	bool follow_player = false;
	bool draw_screen = true;
	bool auto_player = false;

	ConsoleParser parser;
	parser.RegisterToken(L"-f", false);
	parser.RegisterToken(L"-d", true);
	parser.RegisterToken(L"-a", false);

	int args = 0;
	auto lpArgList = ::CommandLineToArgvW(GetCommandLineW(), &args);

	try
	{
		parser.Run(lpArgList, args);
		follow_player = parser.GetResult(L"-f");
		draw_screen = parser.GetResult(L"-d");
		auto_player = parser.GetResult(L"-a");
	}
	catch (std::exception& ex)
	{
		::MessageBoxA(nullptr, ex.what(), "console parser error", MB_OK);
		abort();
	}

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

		auto game_client = std::make_shared<GameClient>(
			io_service, std::move(socket),
			auto_player, draw_screen, follow_player);
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

		if (draw_screen == false)
		{
			nCmdShow = SW_MINIMIZE;
		}

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