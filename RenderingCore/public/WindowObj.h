#pragma once

#include "WindowTypeDef.h"
#include "CompositeValue.h"

#include "Value.h"

#include <Windows.h>

#include <set>

namespace rendering
{
	class WindowObj;

	struct InputInfo
	{
		WindowObj* m_source = nullptr;
		std::set<WPARAM> m_keysDown;
		bool m_leftMouseButtonDown = false;
		bool m_rightMouseButtonDown = false;
		long m_mouseMovement[2];
	};

	class WindowObj : public ObjectValue
	{
	private:
		InputInfo m_inputInfo;

		void RegisterWindowClass();
		void Create();
		void Destroy();

		void RegisterRawInputDevice();
	public:
		Value m_width;
		Value m_height;
		Value m_windowUpdateJobSystem;

		HWND m_hwnd = nullptr;

		WindowObj(const CompositeTypeDef& typeDef, const CompositeValue* outer);
		virtual ~WindowObj();

		void Start();

		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void WindowTick();

		const InputInfo& GetInputInfo();
	};
}