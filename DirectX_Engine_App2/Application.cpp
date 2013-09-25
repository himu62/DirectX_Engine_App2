/******************************************************************************
	Project	: Direct3D11 Sample
	Version	: 0.0.0
	File	: Application.cpp
	Author	: himu62
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
	const std::wstring cls_name = L"DirectX Engine Application 2";

	WNDCLASSEX window_class;
	window_class.cbSize			= sizeof(WNDCLASSEX);
	window_class.style			= CS_VREDRAW | CS_HREDRAW;
	window_class.lpfnWndProc	= ::DefWindowProc;
	window_class.cbClsExtra		= 0;
	window_class.cbWndExtra		= 0;
	window_class.hInstance		= ::GetModuleHandle(nullptr);
	window_class.hIcon			= ::LoadIcon(nullptr, IDI_APPLICATION);
	window_class.hCursor		= ::LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground	= nullptr;
	window_class.lpszMenuName	= nullptr;
	window_class.lpszClassName	= cls_name.c_str();
	window_class.hIconSm		= ::LoadIcon(nullptr, IDI_APPLICATION);

	if(!(::RegisterClassEx(&window_class)))
	{
		throw runtime_error("Failed to register a window class");
	}

	RECT rect = {0, 0, width, height};
	const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	::AdjustWindowRect(&rect, style, false);

	const HWND window_handle = CreateWindow(
		cls_name.c_str(),
		caption.c_str(),
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		nullptr
		);

	if(!window_handle)
	{
		throw runtime_error("Failed to create a window");
	}

	::SetTimer(window_handle, 0, 16, nullptr);

	return window_handle;
}

/******************************************************************************
	CoInitter
******************************************************************************/

CoInitter::CoInitter()
{
	::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
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
	::SetWindowSubclass(
		m_WindowHandle,
		SubclassProcedure,
		reinterpret_cast<UINT_PTR>(this),
		0
		);

	::UpdateWindow(m_WindowHandle);
	::ShowWindow(m_WindowHandle, SW_SHOW);
}
catch(const runtime_error &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	::MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	::exit(-1);
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

	PostQuitMessage(0);

	return static_cast<int>(message.wParam);
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
	const WPARAM wprm,
	const LPARAM lprm,
	const UINT_PTR this_ptr,
	DWORD_PTR
	)
{
	Application* app = reinterpret_cast<Application*>(this_ptr);
	assert(app);

	switch(message)
	{
	//case WM_INPUT:

	case WM_PAINT:
		PAINTSTRUCT ps;
		::BeginPaint(window_handle, &ps);
		app->Update();
		::EndPaint(window_handle, &ps);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 1;

	case WM_SIZE:
		break;

	case WM_DISPLAYCHANGE:
	case WM_TIMER:
		::InvalidateRect(window_handle, nullptr, false);
		break;

	default:
		return ::DefSubclassProc(window_handle, message, wprm, lprm);
	}

	return 0;
}