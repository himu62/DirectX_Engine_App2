/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Texture.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include <webp/decode.h>

#include "Texture.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Caluculate hash
std::size_t hash_value(const TextureKey &tex)
{
	std::size_t hash = 0;
	boost::hash_combine(hash, tex.m_FileName);
	boost::hash_combine(hash, tex.m_RenderTarget.Get());

	return hash;
}

//*****************************************************************************
// Bitmap
ComPtr<ID2D1Bitmap> InitBitmap(
	const ComPtr<ID2D1RenderTarget> render_target,
	const std::wstring &file_name
	)
{
	assert(render_target.Get());
	assert(&file_name);

	std::vector<uint8_t> file = ReadFile(file_name);
	if(file.empty())
	{
		throw runtime_error("Failed to read a file");
	}

	unsigned int file_size = file.size();

	ComPtr<ID2D1Bitmap> bitmap(nullptr);

	std::vector<char> header;
	for(int i=0; i<4; ++i)
	{
		header.push_back(file[i]);
	}
	if(memcmp("RIFF", &header.front(), 4))
	{
		header.clear();
		for(int i=0; i<4; ++i)
		{
			header.push_back(file[i+8]);
		}
		if(memcmp("WEBP", &header.front(), 4))
		{
			// This file is WebP

			int width, height;
			if(!::WebPGetInfo(&file.front(), file.size(), &width, &height))
			{
				throw runtime_error("Failed to decode a image file");
			}

			std::vector<uint8_t> data(4 * width * height);
			if(!::WebPDecodeBGRAInto(&file.front(), file.size(), &data.front(), data.size(), 4 * width))
			{
				throw runtime_error("Failed to decode a image file");
			}

			float dpiX, dpiY;
			render_target->GetDpi(&dpiX, &dpiY);

			D2D1_BITMAP_PROPERTIES props;
			props.pixelFormat	= PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
			props.dpiX			= dpiX;
			props.dpiY			= dpiY;

			if(FAILED(render_target->CreateBitmap(
				SizeU(width, height),
				&data.front(),
				4 * width,
				&props,
				&bitmap
				)))
			{
				throw runtime_error("Failed to create a bitmap");
			}
		}
	}
	else
	{
		header.clear();
		for(int i=0; i<8; ++i)
		{
			header.push_back(file[i]);
		}
		if(header[0] == 0x89)
		if(header[1] == 0x50)
		if(header[2] == 0x4e)
		if(header[3] == 0x47)
		if(header[4] == 0x0d)
		if(header[5] == 0x0a)
		if(header[6] == 0x1a)
		if(header[7] == 0x0a)
		{
			// This file is PNG
		}
		else
		{
			header.clear();
			for(int i=0; i<2; ++i)
			{
				header.push_back(file[i]);
			}
			if(memcmp("BM", &header.front(), 2))
			{
				// This file is BMP
			}
		}
	}

	return bitmap;
}

/******************************************************************************
	TextureKey
******************************************************************************/

TextureKey::TextureKey(
	const ComPtr<ID2D1RenderTarget> render_target,
	const std::wstring &file_name
	) :
	m_RenderTarget(render_target),
	m_FileName(file_name)
{}

//*****************************************************************************
// Is equal
bool TextureKey::operator==(const TextureKey &rhs) const
{
	return (m_FileName == rhs.m_FileName) && (m_RenderTarget == rhs.m_RenderTarget);
}

/******************************************************************************
	TextureImpl
******************************************************************************/

TextureImpl::TextureImpl(const TextureKey &key) try :
	m_Bitmap(InitBitmap(key.m_RenderTarget, key.m_FileName)),
	m_Point(Point2F()),
	m_Scale(SizeF(1.f, 1.f)),
	m_Rotate(0.f)
{}
catch(const runtime_error &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(NULL, ss.str().c_str(), L"ERROR", MB_ICONERROR | MB_OK);

	exit(-1);
}

//*****************************************************************************
// Set properties
void TextureImpl::SetLocate(const D2D1_POINT_2F point)
{
	m_Point = point;
}

void TextureImpl::SetScale(const D2D1_SIZE_F scale)
{
	m_Scale = scale;
}

void TextureImpl::SetRotate(const float degree)
{
	m_Rotate = degree;
}

//*****************************************************************************
// Get bitmap rectangle
D2D1_RECT_F TextureImpl::GetRect() const
{
	const D2D1_SIZE_F size = m_Bitmap->GetSize();

	D2D1_RECT_F rect;
	rect.left	= m_Point.x;
	rect.right	= m_Point.x + size.width;
	rect.top	= m_Point.y;
	rect.bottom	= m_Point.y + size.height;

	return rect;
}