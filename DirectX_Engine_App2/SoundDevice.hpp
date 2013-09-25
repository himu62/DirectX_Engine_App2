/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: SoundDevice.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#include "Sound.hpp"

/******************************************************************************
	SoundDevice
******************************************************************************/

class SoundDevice : private boost::noncopyable
{
public:
	explicit SoundDevice();

private:
	const ComPtr<IXAudio2> m_XAudio;
	const MasteringVoice m_MasterVoice;
};

/******************************************************************************
	Submixer
******************************************************************************/

class Submixer : private boost::noncopyable
{
public:
	explicit Submixer(const ComPtr<IXAudio2> xaudio);

private:
	const SubmixVoice m_SubmixVoice;
};