/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: GraphicDevice.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "GraphicDevice.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Direct2D Factory
ComPtr<ID2D1Factory> InitD2DFactory()
{
	ComPtr<ID2D1Factory> factory(nullptr);
	if(FAILED(::D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), static_cast<void**>(&factory)
		)))
	{
		throw std::runtime_error("Failed to create a Direct2D factory");
	}

	return factory;
}

//*****************************************************************************
// Direct3D11 Device
ComPtr<ID3D10Device1> InitD3DDevice()
{
	UINT f_device = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	f_device |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	const D3D10_DRIVER_TYPE types[] = {
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_WARP,
		D3D10_DRIVER_TYPE_REFERENCE
	};
	const UINT n_types = sizeof(types) / sizeof(D3D_DRIVER_TYPE);

	const D3D10_FEATURE_LEVEL1 levels[] = {
		D3D10_FEATURE_LEVEL_10_1,
		D3D10_FEATURE_LEVEL_10_0,
		D3D10_FEATURE_LEVEL_9_3,
		D3D10_FEATURE_LEVEL_9_2,
		D3D10_FEATURE_LEVEL_9_1
	};
	const UINT n_levels = sizeof(levels) / sizeof(D3D_FEATURE_LEVEL);

	ComPtr<ID3D10Device1> d3d_device(nullptr);
	HRESULT hr = S_OK;
	for(int i=0; i<n_types; ++i)
	{
		for(int j=0; j<n_levels; ++j)
		{
			hr = ::D3D10CreateDevice1(
				nullptr,
				types[i],
				nullptr,
				f_device,
				levels[j],
				D3D10_1_SDK_VERSION,
				&d3d_device
				);

			if(SUCCEEDED(hr))
				break;
		}
	}
	if(FAILED(hr))
	{
		throw std::runtime_error("Failed to create a Direct3D device");
	}

	return d3d_device;
}

//*****************************************************************************
// Swap Chain
ComPtr<IDXGISwapChain> InitSwapChain(
	const HWND window_handle, const ComPtr<ID3D10Device1> d3d_device
	)
{
	assert(window_handle);
	assert(d3d_device.Get());

	ComPtr<IDXGIDevice> dxgi_device(nullptr);
	d3d_device.CopyTo(__uuidof(IDXGIDevice), static_cast<void**>(&dxgi_device));

	ComPtr<IDXGIAdapter> adapter(nullptr);
	dxgi_device->GetAdapter(&adapter);

	ComPtr<IDXGIFactory> dxgi_factory(nullptr);
	adapter->GetParent(__uuidof(IDXGIFactory), static_cast<void**>(&dxgi_factory));

	RECT rc;
	GetClientRect(window_handle, &rc);

	DXGI_SWAP_CHAIN_DESC sc_desc;
	sc_desc.BufferDesc.Width					= rc.right - rc.left;
	sc_desc.BufferDesc.Height					= rc.bottom - rc.top;
	sc_desc.BufferDesc.RefreshRate.Numerator	= 60;
	sc_desc.BufferDesc.RefreshRate.Denominator	= 1;
	sc_desc.BufferDesc.Format					= DXGI_FORMAT_B8G8R8A8_UNORM;
	sc_desc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc_desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	sc_desc.SampleDesc.Count					= 1;
	sc_desc.SampleDesc.Quality					= 0;
	sc_desc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc.BufferCount							= 1;
	sc_desc.OutputWindow						= window_handle;
	sc_desc.Windowed							= true;
	sc_desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	sc_desc.Flags								= 0;

	ComPtr<IDXGISwapChain> swap_chain(nullptr);
	if(FAILED(dxgi_factory->CreateSwapChain(d3d_device.Get(), &sc_desc, &swap_chain)))
	{
		throw std::runtime_error("Failed to create a swap chain");
	}

	return swap_chain;
}

//*****************************************************************************
// Render Target View
ComPtr<ID3D10RenderTargetView> InitRenderTargetView(
	const ComPtr<ID3D10Device> d3d_device,
	const ComPtr<IDXGISwapChain> swap_chain
	)
{
	assert(d3d_device.Get());
	assert(swap_chain.Get());

	ComPtr<ID3D10Resource> buffer_resource(nullptr);
	swap_chain->GetBuffer(0, __uuidof(ID3D10Resource), static_cast<void**>(&buffer_resource));

	D3D10_RENDER_TARGET_VIEW_DESC render_desc;
	render_desc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
	render_desc.ViewDimension		= D3D10_RTV_DIMENSION_TEXTURE2D;
	render_desc.Texture2D.MipSlice	= 0;

	ComPtr<ID3D10RenderTargetView> rt_view(nullptr);
	if(FAILED(d3d_device->CreateRenderTargetView(buffer_resource.Get(), &render_desc, &rt_view)))
	{
		throw std::runtime_error("Failed to create a render target view");
	}

	return rt_view;
}

//*****************************************************************************
// Depth Stencil View
ComPtr<ID3D10DepthStencilView> InitDepthStencilView(
	const ComPtr<ID3D10Device> d3d_device,
	const ComPtr<IDXGISwapChain> swap_chain
	)
{
	assert(d3d_device.Get());
	assert(swap_chain.Get());

	DXGI_SWAP_CHAIN_DESC sc_desc;
	swap_chain->GetDesc(&sc_desc);

	D3D10_TEXTURE2D_DESC tex_desc;
	tex_desc.Width			= sc_desc.BufferDesc.Width;
	tex_desc.Height			= sc_desc.BufferDesc.Height;
	tex_desc.MipLevels		= 1;
	tex_desc.ArraySize		= 1;
	tex_desc.Format			= DXGI_FORMAT_D16_UNORM;
	tex_desc.SampleDesc		= sc_desc.SampleDesc;
	tex_desc.Usage			= D3D10_USAGE_DEFAULT;
	tex_desc.BindFlags		= D3D10_BIND_DEPTH_STENCIL;
	tex_desc.CPUAccessFlags	= 0;
	tex_desc.MiscFlags		= 0;

	ComPtr<ID3D10Texture2D> depth_stencil(nullptr);
	if(FAILED(d3d_device->CreateTexture2D(&tex_desc, nullptr, &depth_stencil)))
	{
		throw std::runtime_error("Failed to create a depth stencil");
	}

	D3D10_DEPTH_STENCIL_VIEW_DESC dv_desc;
	dv_desc.Format				= DXGI_FORMAT_D16_UNORM;
	dv_desc.ViewDimension		= D3D10_DSV_DIMENSION_TEXTURE2D;
	dv_desc.Texture2D.MipSlice	= 0;

	ComPtr<ID3D10DepthStencilView> ds_view(nullptr);
	if(FAILED(d3d_device->CreateDepthStencilView(depth_stencil.Get(), &dv_desc, &ds_view)))
	{
		throw std::runtime_error("Failed to create a depth stencil view");
	}

	return ds_view;
}

//*****************************************************************************
// Back Buffer
ComPtr<ID2D1RenderTarget> InitBackBuffer(
	const ComPtr<ID2D1Factory> d2d_factory,
	const ComPtr<ID3D10Device1> d3d_device,
	const ComPtr<IDXGISwapChain> swap_chain,
	const ComPtr<ID3D10RenderTargetView> rt_view,
	const ComPtr<ID3D10DepthStencilView> ds_view
	)
{
	assert(d2d_factory.Get());
	assert(d3d_device.Get());
	assert(swap_chain.Get());
	assert(rt_view.Get());
	assert(ds_view.Get());

	std::vector<ID3D10RenderTargetView*> targets;
	targets.push_back(rt_view.Get());
	d3d_device->OMSetRenderTargets(1, &targets.front(), ds_view.Get());

	DXGI_SWAP_CHAIN_DESC sc_desc;
	swap_chain->GetDesc(&sc_desc);

	D3D10_VIEWPORT view_port;
	view_port.TopLeftX	= 0;
	view_port.TopLeftY	= 0;
	view_port.Width		= sc_desc.BufferDesc.Width;
	view_port.Height	= sc_desc.BufferDesc.Height;
	view_port.MinDepth	= 0.f;
	view_port.MaxDepth	= 1.f;

	d3d_device->RSSetViewports(1, &view_port);

	ComPtr<IDXGISurface> surface(nullptr);
	swap_chain->GetBuffer(0, __uuidof(IDXGISurface), &surface);
	
	float dpiX, dpiY;
	d2d_factory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props;
	props.type			= D2D1_RENDER_TARGET_TYPE_DEFAULT;
	props.pixelFormat	= PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
	props.dpiX			= dpiX;
	props.dpiY			= dpiY;
	props.usage			= D2D1_RENDER_TARGET_USAGE_NONE;
	props.minLevel		= D2D1_FEATURE_LEVEL_DEFAULT;

	ComPtr<ID2D1RenderTarget> back_buffer(nullptr);
	if(FAILED(d2d_factory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, &back_buffer)))
	{
		throw std::runtime_error("Failed to create a render target");
	}

	return back_buffer;
}

/******************************************************************************
	GraphicDevice
******************************************************************************/

GraphicDevice::GraphicDevice(const HWND window_handle) try :
	m_d2dFactory(InitD2DFactory()),
	m_d3dDevice(InitD3DDevice()),
	m_SwapChain(InitSwapChain(window_handle, m_d3dDevice)),
	m_RenderTargetView(InitRenderTargetView(m_d3dDevice, m_SwapChain)),
	m_DepthStencilView(InitDepthStencilView(m_d3dDevice, m_SwapChain)),
	m_BackBuffer(InitBackBuffer(m_d2dFactory, m_d3dDevice, m_SwapChain, m_RenderTargetView, m_DepthStencilView))
{}
catch(std::runtime_error const &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}

//*****************************************************************************
// Start of rendering
void GraphicDevice::BeginDraw() const
{
	assert(m_d3dDevice.Get());
	assert(m_BackBuffer.Get());

	m_d3dDevice->ClearDepthStencilView(
		m_DepthStencilView.Get(),
		D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL,
		1.f,
		0
		);

	m_BackBuffer->BeginDraw();

	m_BackBuffer->SetTransform(Matrix3x2F::Identity());
	m_BackBuffer->Clear(ColorF(0xffffff));
}

//*****************************************************************************
// End of rendering
void GraphicDevice::EndDraw() const
{
	assert(m_BackBuffer.Get());

	m_BackBuffer->EndDraw();

	m_SwapChain->Present(1, 0);
}