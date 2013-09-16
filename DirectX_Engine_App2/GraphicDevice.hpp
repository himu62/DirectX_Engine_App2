/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: GraphicDevice.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#include <memory>
#include <vector>
#include <string>

#pragma warning(disable: 4005)
#include <D3D10_1.h>
#include <D2D1.h>
#include <D2D1Helper.h>
#include <DWrite.h>
#pragma warning(default: 4005)

#include <wrl/client.h>
#include <boost/noncopyable.hpp>

#pragma comment(lib, "d3d10_1.lib")
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
	const ComPtr<ID2D1Factory> m_d2dFactory;
	
	const ComPtr<ID3D10Device1> m_d3dDevice;
	const ComPtr<IDXGISwapChain> m_SwapChain;

	ComPtr<ID3D10RenderTargetView> m_RenderTargetView;
	ComPtr<ID3D10DepthStencilView> m_DepthStencilView;

	const ComPtr<ID2D1RenderTarget> m_BackBuffer;
};