/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Texture.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#pragma warning(disable: 4005)
#include <string>
#include <vector>
#include <memory>

#include <D2D1.h>
#include <D2D1Helper.h>

#include <wrl/client.h>

#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>

using std::runtime_error;
using namespace D2D1;
using namespace Microsoft::WRL;

#pragma comment(lib, "d2d1.lib")

/******************************************************************************
	TextureKey
******************************************************************************/

class TextureKey
{
public:
	explicit TextureKey(
		const ComPtr<ID2D1RenderTarget> render_target,
		const std::wstring &file_name
		);
	bool operator==(const TextureKey &rhs) const;

private:
	friend std::size_t hash_value(const TextureKey &tex);
	friend class TextureImpl;

	const ComPtr<ID2D1RenderTarget> m_RenderTarget;
	const std::wstring m_FileName;
};

/******************************************************************************
	TextureImpl
******************************************************************************/

class TextureImpl
{
public:
	explicit TextureImpl(const TextureKey &key);

	void SetLocate(const D2D1_POINT_2F point);
	void SetScale(const D2D1_SIZE_F scale);
	void SetRotate(const float degree);

private:
	friend class GraphicDevice;

	D2D1_RECT_F GetRect() const;

	ComPtr<ID2D1Bitmap> m_Bitmap;

	D2D1_POINT_2F m_Point;
	D2D1_SIZE_F m_Scale;
	float m_Rotate;
};

typedef boost::flyweights::flyweight<boost::flyweights::key_value<TextureKey, TextureImpl>> Texture;