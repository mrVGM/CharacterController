#include "WindowObj.h"

#include "WindowTypeDef.h"

#include "JobSystem.h"
#include "Job.h"
#include "JobSystemDef.h"
#include "Jobs.h"

#include "ObjectValueContainer.h"

#include <hidusage.h>

namespace
{
	bool m_classRegistered = false;
	const wchar_t* m_className = L"MyWindow";
}

rendering::WindowObj::WindowObj(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	ObjectValue(typeDef, outer),
	m_width(WindowTypeDef::GetTypeDef().m_width.GetType(), this),
	m_height(WindowTypeDef::GetTypeDef().m_height.GetType(), this),
	m_windowUpdateJobSystem(WindowTypeDef::GetTypeDef().m_windowUpdateJobSystem.GetType(), this)
{
}

rendering::WindowObj::~WindowObj()
{
	Destroy();

	if (m_hwnd != nullptr) {
		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;
	}
}

void rendering::WindowObj::Start()
{
	RegisterWindowClass();

	class WindowUpdateJob : public jobs::Job
	{
	private:
		WindowObj* m_wnd = nullptr;
	public:
		WindowUpdateJob(WindowObj* window) :
			m_wnd(window)
		{
		}
		void Do()
		{
			m_wnd->Create();
			while (m_wnd->m_hwnd)
			{
				m_wnd->WindowTick();
			}
		}
	};

	struct Context
	{
		WindowObj* m_self = nullptr;
	};

	class StartJob : public jobs::Job
	{
		Context m_ctx;
	public:
		StartJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			const jobs::JobSystemDef* jsDef = m_ctx.m_self->m_windowUpdateJobSystem.GetType<const jobs::JobSystemDef*>();

			ObjectValueContainer& container = ObjectValueContainer::GetContainer();
			std::list<ObjectValue*> tmp;
			container.GetObjectsOfType(*jsDef, tmp);

			jobs::JobSystem* js = static_cast<jobs::JobSystem*>(tmp.front());
			js->Start();

			js->ScheduleJob(new WindowUpdateJob(m_ctx.m_self));
		}
	};

	Context ctx{ this };
	jobs::RunSync(new StartJob(ctx));
}

void rendering::WindowObj::RegisterWindowClass()
{
	if (!m_classRegistered)
	{
		WNDCLASSEXW wcex;

		ZeroMemory(&wcex, sizeof(wcex));
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = &StaticWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = m_className;

		RegisterClassExW(&wcex);

		m_classRegistered = true;
	}
}

LRESULT rendering::WindowObj::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT data = (LPCREATESTRUCT)lParam;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data->lpCreateParams);
		auto* window = (WindowObj*)data->lpCreateParams;
		window->m_hwnd = hWnd;

		window->RegisterRawInputDevice();
	}

	// Process messages by window message function
	WindowObj* window = (WindowObj*) ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (window)
	{
		return window->WndProc(uMsg, wParam, lParam);
	}
	else
	{
		return static_cast<LRESULT>(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
}


void rendering::WindowObj::Create()
{
	DWORD dwStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
	DWORD dxExStyle = 0;

	RECT windowRect;
	windowRect.left = 50;
	windowRect.top = 50;
	windowRect.right = windowRect.left + m_width.Get<int>();
	windowRect.bottom = windowRect.top + m_height.Get<int>();

	AdjustWindowRect(&windowRect, dwStyle, FALSE);

	CreateWindow(
		m_className,
		L"Render Window",
		dwStyle,
		windowRect.left, windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL, NULL, GetModuleHandle(NULL), this);
}

void rendering::WindowObj::Destroy()
{
	if (m_hwnd != NULL) {
		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;
	}
}

LRESULT rendering::WindowObj::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			Destroy();
			break;
		}

	case WM_KEYDOWN:
	{
		m_inputInfo.m_keysDown.insert(wParam);
		return 0;
	}

	case WM_KEYUP:
	{
		m_inputInfo.m_keysDown.erase(wParam);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		m_inputInfo.m_leftMouseButtonDown = true;
		return 0;
	}
	case WM_LBUTTONUP:
	{
		m_inputInfo.m_leftMouseButtonDown = false;
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		m_inputInfo.m_rightMouseButtonDown = true;
		return 0;
	}
	case WM_RBUTTONUP:
	{
		m_inputInfo.m_rightMouseButtonDown = false;
		return 0;
	}

	case WM_INPUT:
	{
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				m_inputInfo.m_mouseMovement[0] += raw->data.mouse.lLastX;
				m_inputInfo.m_mouseMovement[1] += raw->data.mouse.lLastY;
			}
			else if (raw->data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE) {
				m_inputInfo.m_mouseMovement[0] = raw->data.mouse.lLastX;
				m_inputInfo.m_mouseMovement[1] = raw->data.mouse.lLastY;
			}
		}
		return 0;
	}

	}

	return static_cast<LRESULT>(DefWindowProc(m_hwnd, uMsg, wParam, lParam));
}

void rendering::WindowObj::WindowTick()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!GetMessage(&msg, NULL, 0, 0)) {
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void rendering::WindowObj::RegisterRawInputDevice()
{
	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = m_hwnd;
	RegisterRawInputDevices(rid, _countof(rid), sizeof(RAWINPUTDEVICE));
}

const rendering::InputInfo& rendering::WindowObj::GetInputInfo()
{
	return m_inputInfo;
}