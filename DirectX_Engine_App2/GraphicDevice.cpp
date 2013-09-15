/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: Core_0.0.0
*	File	: GraphicDevice.cpp
*	Author	: himu62
******************************************************************************/

#include "GraphicDevice.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Direct2D Factory
ComPtr<ID2D1Factory1> InitD2DFactory()
{
	ComPtr<ID2D1Factory1> factory(nullptr);
	if(FAILED(::D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), static_cast<void**>(&factory)
		)))
	{
		throw std::runtime_error("Failed to create a Direct2D factory");
	}

	return factory;
}

//*****************************************************************************
// Direct3D11 Device
ComPtr<ID3D11Device> InitD3DDevice()
{
	UINT f_device = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	f_device |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_DRIVER_TYPE types[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	const UINT n_types = sizeof(types) / sizeof(D3D_DRIVER_TYPE);

	const D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	const UINT n_levels = sizeof(levels) / sizeof(D3D_FEATURE_LEVEL);

	ComPtr<ID3D11Device> d3d_device(nullptr);
	HRESULT hr = S_OK;
	for(int i=0; i<n_types; ++i)
	{
		hr = ::D3D11CreateDevice(
			nullptr,
			types[i],
			nullptr,
			f_device,
			levels,
			n_levels,
			D3D11_SDK_VERSION,
			&d3d_device,
			nullptr,
			nullptr
			);

		if(SUCCEEDED(hr))
			break;
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
	const HWND window_handle, const ComPtr<ID3D11Device> d3d_device
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
	GetWindowRect(window_handle, &rc);

	DXGI_SWAP_CHAIN_DESC sc_desc;
	sc_desc.BufferDesc.Width					= rc.right - rc.left;
	sc_desc.BufferDesc.Height					= rc.bottom - rc.top;
	sc_desc.BufferDesc.RefreshRate.Numerator	= 0;
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
	sc_desc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ComPtr<IDXGISwapChain> swap_chain(nullptr);
	if(FAILED(dxgi_factory->CreateSwapChain(d3d_device.Get(), &sc_desc, &swap_chain)))
	{
		throw std::runtime_error("Failed to create a swap chain");
	}

	return swap_chain;
}

//*****************************************************************************
// Direct2D Device Context
ComPtr<ID2D1DeviceContext> InitD2DDeviceContext(
	const ComPtr<ID2D1Factory1> d2d_factory,
	const ComPtr<ID3D11Device> d3d_device,
	const ComPtr<IDXGISwapChain> swap_chain
	)
{
	assert(d2d_factory.Get());
	assert(d3d_device.Get());
	assert(swap_chain.Get());

	D3D11_RASTERIZER_DESC rs_desc;
	rs_desc.FillMode				= D3D11_FILL_SOLID;
	rs_desc.CullMode				= D3D11_CULL_NONE;
	rs_desc.FrontCounterClockwise	= false;
	rs_desc.DepthBias				= 0;
	rs_desc.DepthBiasClamp			= 0;
	rs_desc.SlopeScaledDepthBias	= 0;
	rs_desc.DepthClipEnable			= true;
	rs_desc.ScissorEnable			= false;
	rs_desc.MultisampleEnable		= false;
	rs_desc.AntialiasedLineEnable	= false;

	ComPtr<ID3D11RasterizerState> rs_state(nullptr);
	if(FAILED(d3d_device->CreateRasterizerState(&rs_desc, &rs_state)))
	{
		throw std::runtime_error("Failed to create a rasterizer state");
	}

	ComPtr<ID3D11DeviceContext> d3d_dc(nullptr);
	d3d_device->GetImmediateContext(&d3d_dc);

	d3d_dc->RSSetState(rs_state.Get());
	d3d_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXGI_SWAP_CHAIN_DESC sc_desc;
	swap_chain->GetDesc(&sc_desc);

	D3D11_TEXTURE2D_DESC tex_desc;
	tex_desc.Width			= sc_desc.BufferDesc.Width;
	tex_desc.Height			= sc_desc.BufferDesc.Height;
	tex_desc.MipLevels		= 1;
	tex_desc.ArraySize		= 1;
	tex_desc.Format			= DXGI_FORMAT_B8G8R8A8_UNORM;
	tex_desc.SampleDesc		= sc_desc.SampleDesc;
	tex_desc.Usage			= D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tex_desc.CPUAccessFlags	= 0;
	tex_desc.MiscFlags		= 0;

	ComPtr<ID3D11Texture2D> offscreen_tex(nullptr);
	if(FAILED(d3d_device->CreateTexture2D(&tex_desc, nullptr, &offscreen_tex)))
	{
		throw std::runtime_error("Failed to create an offscreen texture");
	}

	ComPtr<IDXGISurface> dxgi_surface(nullptr);
	offscreen_tex.CopyTo(__uuidof(IDXGISurface), static_cast<void**>(&dxgi_surface));

	ComPtr<ID2D1DeviceContext> d2d_dc(nullptr);
	if(FAILED(::D2D1CreateDeviceContext(
		dxgi_surface.Get(),
		CreationProperties(
			D2D1_THREADING_MODE_SINGLE_THREADED,
			D2D1_DEBUG_LEVEL_WARNING,
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE
			),
		&d2d_dc
		)))
	{
		throw std::runtime_error("Failed to create a Direct2D device context");
	}

	float dpiX, dpiY;
	d2d_factory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_BITMAP_PROPERTIES1 bitmap_props;
	bitmap_props.pixelFormat	= PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	bitmap_props.dpiX			= dpiX;
	bitmap_props.dpiY			= dpiY;
	bitmap_props.bitmapOptions	= D2D1_BITMAP_OPTIONS_TARGET;
	bitmap_props.colorContext	= nullptr;

	ComPtr<ID2D1Bitmap1> bitmap(nullptr);
	if(FAILED(d2d_dc->CreateBitmapFromDxgiSurface(dxgi_surface.Get(), &bitmap_props, &bitmap)))
	{
		throw std::runtime_error("Failed to create a bitmap from a DXGI surface");
	}

	d2d_dc->SetTarget(bitmap.Get());
	
	return d2d_dc;
}

/******************************************************************************
	GraphicDevice
******************************************************************************/

GraphicDevice::GraphicDevice(const HWND window_handle) try :
	m_d2dFactory(InitD2DFactory()),
	m_d3dDevice(InitD3DDevice()),
	m_SwapChain(InitSwapChain(window_handle, m_d3dDevice)),
	m_d2dDeviceContext(InitD2DDeviceContext(m_d2dFactory, m_d3dDevice, m_SwapChain))
{}
catch(std::runtime_error const &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(NULL, ss.str().c_str(), L"ERROR", MB_ICONERROR | MB_OK);

	exit(-1);
}

//*****************************************************************************
// Start of rendering
void GraphicDevice::BeginDraw() const
{
	assert(m_d2dDeviceContext.Get());

	m_d2dDeviceContext->BeginDraw();

	m_d2dDeviceContext->SetTransform(Matrix3x2F::Identity());
	m_d2dDeviceContext->Clear();
}

//*****************************************************************************
// End of rendering
void GraphicDevice::EndDraw() const
{
	assert(m_d2dDeviceContext.Get());

	if(m_d2dDeviceContext->EndDraw() == D2DERR_RECREATE_TARGET)
	{
		throw std::runtime_error("Error: ID2D1RenderTarget::EndDraw() failed");
	}
}