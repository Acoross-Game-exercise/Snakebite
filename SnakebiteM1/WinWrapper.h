#ifndef ACOROSS_WINRAPPER_H_
#define ACOROSS_WINRAPPER_H_

#include <Windows.h>
#include <functional>

//#include "Resource.h"

#define MAX_LOADSTRING 100

namespace acoross {
namespace Win {

class MemDC;

// wrapper for HDC
class WDC
{
public:
	WDC(HDC hdc) : hdc_(hdc) {}
	WDC(){}
	virtual ~WDC() {}

	HDC Get() const { return hdc_; };

	void Rectangle(int left, int top, int right, int bottom)
	{
		::Rectangle(hdc_, left, top, right, bottom);
	}

	void Ellipse(int left, int top, int right, int bottom)
	{
		::Ellipse(hdc_, left, top, right, bottom);
	}

protected:
	HDC hdc_;
};

//////////////////////////////
// class Window

typedef LRESULT(CALLBACK WndProcFunc)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR(CALLBACK AboutCallback)(HWND, UINT, WPARAM, LPARAM);

class Window
{
public:
	typedef std::function<void(MSG& msg)> LoopFunc;

	Window(Window&) = delete;
	Window& operator=(Window&) = delete;

public:
	Window(HINSTANCE hInstance);

	//
	//  �Լ�: MyRegisterClass()
	//
	//  ����: â Ŭ������ ����մϴ�.
	//
	ATOM MyRegisterClass(WndProcFunc* func);

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
	BOOL InitInstance(int nCmdShow);

	// �⺻ �޽��� �����Դϴ�.
	int PeekMessegeLoop(LoopFunc func);

	// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

private:
	WCHAR szTitle_[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
	WCHAR szWindowClass_[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
	HINSTANCE hInst_;
	WNDCLASSEXW wcex_;
};

}
}
#endif //ACOROSS_WINRAPPER_H_
