/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Application.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "Application.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Window Handle
HWND InitWindow(const std::wstring &caption, const int width, const int height)
{
	const std::wstring class_name = L"Engine Test Application";

	WNDCLASSEX window_class;
	window_class.cbSize			= sizeof(window_class);
	window_class.style			= CS_VREDRAW | CS_HREDRAW;
	window_class.lpfnWndProc	= ::DefWindowProc;
	window_class.cbClsExtra		= 0;
	window_class.cbWndExtra		= 0;
	window_class.hInstance		= ::GetModuleHandle(nullptr);
	window_class.hIcon			= ::LoadIcon(nullptr, IDI_APPLICATION);
	window_class.hCursor		= ::LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground	= nullptr;
	window_class.lpszMenuName	= nullptr;
	window_class.lpszClassName	= class_name.c_str();
	window_class.hIconSm		= ::LoadIcon(nullptr, IDI_APPLICATION);

	if(!(::RegisterClassEx(&window_class)))
	{
		throw std::runtime_error("Failed to register a window class");
	}

	RECT rc = {0, 0, width, height};
	const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	::AdjustWindowRect(&rc, style, false);

	const HWND window_handle = ::CreateWindow(
		class_name.c_str(),
		caption.c_str(),
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top,
		nullptr,
		nullptr,
		::GetModuleHandle(nullptr),
		nullptr
		);
	if(!window_handle)
	{
		throw std::runtime_error("Failed to create a window");
	}

	return window_handle;
}

/******************************************************************************
	CoInitter
******************************************************************************/

CoInitter::CoInitter()
{
	if(::CoInitializeEx(nullptr, COINIT_MULTITHREADED))
	{
		throw std::runtime_error("Failed to initialize COM");
	}
}

CoInitter::~CoInitter()
{
	::CoUninitialize();
}

/******************************************************************************
	Application
******************************************************************************/

Application::Application(
	const std::wstring &caption,
	const int width,
	const int height,
	const bool windowed
	) try :
	m_WindowHandle(InitWindow(caption, width, height)),
	m_Initter(),
	m_GraphicDevice(new GraphicDevice(m_WindowHandle)),
	m_SoundDevice(new SoundDevice())
{
	::SetWindowSubclass(m_WindowHandle, SubclassProcedure, reinterpret_cast<UINT_PTR>(this), 0);
	::SetTimer(m_WindowHandle, 0, 16, nullptr);

	::ShowWindow(m_WindowHandle, SW_SHOW);
}
catch(const std::runtime_error &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	::MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}

//*****************************************************************************
// Run
int Application::Run()
{
	MSG message;

	while(::GetMessage(&message, nullptr, 0, 0))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}

	::PostQuitMessage(0);

	return static_cast<int>(message.wParam);
}

//*****************************************************************************
// Exit
void Application::Exit()
{
	::PostQuitMessage(0);
}

//*****************************************************************************
// Process
void Application::Update()
{
	m_GraphicDevice->BeginDraw();

	m_GraphicDevice->EndDraw();
}

//*****************************************************************************
// Window Procedure
LRESULT CALLBACK Application::SubclassProcedure(
	const HWND window_handle,
	const UINT message,
	const WPARAM wp,
	const LPARAM lp,
	const UINT_PTR this_ptr,
	DWORD_PTR
	)
{
	Application* app = reinterpret_cast<Application*>(this_ptr);
	assert(app);

	switch(message)
	{
	case WM_TIMER:
	case WM_DISPLAYCHANGE:
		::InvalidateRect(window_handle, nullptr, false);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		::BeginPaint(window_handle, &ps);
		app->Update();
		::EndPaint(window_handle, &ps);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 1;

	default:
		return ::DefSubclassProc(window_handle, message, wp, lp);
	}

	return 0;
}