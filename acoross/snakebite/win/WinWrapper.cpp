#include "WinWrapper.h"
#include "Resource.h"

namespace acoross {
namespace Win {

Window::Window(HINSTANCE hInstance)
	: hInst_(hInstance)
{
	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	::LoadStringW(hInst_, IDS_APP_TITLE, szTitle_, MAX_LOADSTRING);
	::LoadStringW(hInst_, IDC_SNAKEBITEM1, szWindowClass_, MAX_LOADSTRING);
}

//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM Window::MyRegisterClass(WndProcFunc* func)
{
	wcex_.cbSize = sizeof(WNDCLASSEX);

	wcex_.style = CS_HREDRAW | CS_VREDRAW;
	wcex_.lpfnWndProc = func;
	wcex_.cbClsExtra = 0;
	wcex_.cbWndExtra = 0;
	wcex_.hInstance = hInst_;
	wcex_.hIcon = LoadIcon(hInst_, MAKEINTRESOURCE(IDI_SNAKEBITEM1));
	wcex_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex_.lpszMenuName = MAKEINTRESOURCEW(IDC_SNAKEBITEM1);
	wcex_.lpszClassName = szWindowClass_;
	wcex_.hIconSm = LoadIcon(wcex_.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassExW(&wcex_);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL Window::InitInstance(int nCmdShow)
{
	HWND hWnd = CreateWindowW(szWindowClass_, szTitle_, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst_, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);

	return TRUE;
}

// �⺻ �޽��� �����Դϴ�.
int Window::PeekMessegeLoop(LoopFunc func)
{
	HACCEL hAccelTable = LoadAccelerators(hInst_, MAKEINTRESOURCE(IDC_SNAKEBITEM1));
	MSG msg;

	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		func(msg);
	}

	return (int)msg.wParam;
}

}
}
