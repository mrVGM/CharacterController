#include "RenderWindow.h"

#include "PrimitiveTypes.h"
#include "JobSystemDef.h"

#include "JobSystem.h"
#include "Job.h"
#include "Jobs.h"

#include "ObjectValueContainer.h"

#include <hidusage.h>

namespace
{
	BasicObjectContainer<rendering::WindowTypeDef> m_wnd;

	bool m_classRegistered = false;
	const wchar_t* m_className = L"MyWindow";
}

rendering::WindowTypeDef::WindowTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "99EC3C91-412A-450B-AA61-721FFC77BF8B"),
	m_width("7E7854FF-F38C-47F1-8690-6FC16DB93882", IntTypeDef::GetTypeDef()),
	m_height("10A08A76-DB55-4F6C-B74B-8FA4DD263005", IntTypeDef::GetTypeDef()),
	m_windowUpdateJobSystem("BE6AE599-53DC-4932-9536-74D398FD1003", TypeTypeDef::GetTypeDef(jobs::JobSystemDef::GetTypeDef()))
{
	{
		m_width.m_name = "Width";
		m_width.m_category= "Setup";
		m_width.m_getValue = [](CompositeValue* obj) -> Value& {
			WindowObj* wnd = static_cast<WindowObj*>(obj);
			return wnd->m_width;
		};
		m_properties[m_width.GetId()] = &m_width;
	}

	{
		m_height.m_name = "Height";
		m_height.m_category = "Setup";
		m_height.m_getValue = [](CompositeValue* obj) -> Value& {
			WindowObj* wnd = static_cast<WindowObj*>(obj);
			return wnd->m_height;
		};
		m_properties[m_height.GetId()] = &m_height;
	}

	{
		m_windowUpdateJobSystem.m_name = "Window Update Job System";
		m_windowUpdateJobSystem.m_category = "Setup";
		m_windowUpdateJobSystem.m_getValue = [](CompositeValue* obj) -> Value& {
			WindowObj* wnd = static_cast<WindowObj*>(obj);
			return wnd->m_windowUpdateJobSystem;
		};
		m_properties[m_windowUpdateJobSystem.GetId()] = &m_windowUpdateJobSystem;
	}

	m_name = "Window";
	m_category = "Rendering";
}

rendering::WindowTypeDef::~WindowTypeDef()
{
}

void rendering::WindowTypeDef::Construct(Value& container) const
{
	WindowObj* obj = new WindowObj(WindowTypeDef::GetTypeDef());
	container.AssignObject(obj);
}

const rendering::WindowTypeDef& rendering::WindowTypeDef::GetTypeDef()
{
	if (!m_wnd.m_object)
	{
		m_wnd.m_object = new WindowTypeDef();
	}

	return *m_wnd.m_object;
}

rendering::WindowObj::WindowObj(const ReferenceTypeDef& typeDef) :
	ObjectValue(typeDef),
	m_width(WindowTypeDef::GetTypeDef().m_width.GetType(), this),
	m_height(WindowTypeDef::GetTypeDef().m_height.GetType(), this),
	m_windowUpdateJobSystem(WindowTypeDef::GetTypeDef().m_windowUpdateJobSystem.GetType(), this),
	m_windowUpdateJS(jobs::JobSystemDef::GetTypeDef(), this)
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

	jobs::Job* init = jobs::Job::CreateByLambda([=]() {
		jobs::JobSystem* js = GetWindowUpdateJS();
		jobs::Job* updateJob = jobs::Job::CreateByLambda([=]() {
			Create();
			while (m_hwnd)
			{
				WindowTick();
			}
		});

		js->ScheduleJob(updateJob);
	});

	jobs::RunSync(init);
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
	m_inputMutex.lock();

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
		break;
	}

	case WM_KEYUP:
	{
		m_inputInfo.m_keysDown.erase(wParam);
		break;
	}

	case WM_LBUTTONDOWN:
	{
		m_inputInfo.m_leftMouseButtonDown = true;
		break;
	}
	case WM_LBUTTONUP:
	{
		m_inputInfo.m_leftMouseButtonDown = false;
		break;
	}

	case WM_RBUTTONDOWN:
	{
		m_inputInfo.m_rightMouseButtonDown = true;
		break;
	}
	case WM_RBUTTONUP:
	{
		m_inputInfo.m_rightMouseButtonDown = false;
		break;
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

				m_inputInfo.m_mouseAxis[0] += raw->data.mouse.lLastX / 10.0;
				m_inputInfo.m_mouseAxis[1] += raw->data.mouse.lLastY / 10.0;
			}
			else if (raw->data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE) {
				m_inputInfo.m_mouseAxis[0] += (raw->data.mouse.lLastX - m_inputInfo.m_mouseMovement[0]) / 10.0;
				m_inputInfo.m_mouseAxis[1] += (raw->data.mouse.lLastY - m_inputInfo.m_mouseMovement[1]) / 10.0;

				m_inputInfo.m_mouseMovement[0] = raw->data.mouse.lLastX;
				m_inputInfo.m_mouseMovement[1] = raw->data.mouse.lLastY;
			}
		}
		break;
	}

	}

	m_inputMutex.unlock();

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

void rendering::WindowObj::GetInputInfo(InputInfo& outInputInfo)
{
	m_inputMutex.lock();

	outInputInfo = m_inputInfo;
	m_inputInfo.m_mouseAxis[0] = 0;
	m_inputInfo.m_mouseAxis[1] = 0;

	m_inputMutex.unlock();
}

jobs::JobSystem* rendering::WindowObj::GetWindowUpdateJS()
{
	jobs::JobSystem* js = m_windowUpdateJS.GetValue<jobs::JobSystem*>();
	if (js)
	{
		return js;	
	}

	const TypeDef* jsDef = m_windowUpdateJobSystem.GetType<const TypeDef*>();
	ObjectValueContainer::GetObjectOfType(*jsDef, m_windowUpdateJS);
	js = m_windowUpdateJS.GetValue<jobs::JobSystem*>();
	js->Start();

	return js;
}