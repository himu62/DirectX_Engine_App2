/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: deleter.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#pragma warning(disable: 4005)
#include <cassert>

#include <XAudio2.h>

#include <vorbisfile.h>

/******************************************************************************
	delete
******************************************************************************/

struct VoiceDeleter
{
	void operator()(IXAudio2Voice* ptr)
	{
		assert(ptr);
		ptr->DestroyVoice();
	}
};

struct SourceVoiceDeleter
{
	void operator()(IXAudio2SourceVoice* ptr)
	{
		assert(ptr);
		ptr->Stop();
		ptr->DestroyVoice();
	}
};

struct VorbisDeleter
{
	void operator()(OggVorbis_File* ptr)
	{
		assert(ptr);
		ov_clear(ptr);
		delete ptr;
	}
};