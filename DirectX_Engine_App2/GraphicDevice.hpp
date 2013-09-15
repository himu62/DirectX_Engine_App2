/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: Core_0.0.0
*	File	: GraphicDevice.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#include <memory>
#include <vector>
#include <string>

#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <dwrite_1.h>

#include <wrl/client.h>
#include <boost/noncopyable.hpp>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace Microsoft::WRL;
using namespace D2D1;

/******************************************************************************
	GraphicDevice
******************************************************************************/

class GraphicDevice : private boost::noncopyable
{
public:
	explicit GraphicDevice(const HWND window_handle);

	void BeginDraw() const;
	void EndDraw() const;

private:
	const ComPtr<ID2D1Factory1> m_d2dFactory;
	
	const ComPtr<ID3D11Device> m_d3dDevice;
	const ComPtr<IDXGISwapChain> m_SwapChain;

	const ComPtr<ID2D1DeviceContext> m_d2dDeviceContext;
};