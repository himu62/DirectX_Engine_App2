/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: SoundDevice.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "SoundDevice.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// XAudio2
ComPtr<IXAudio2> InitXAudio()
{
	UINT32 f_debug = 0;
#ifdef _DEBUG
	f_debug = 1;
#endif

	ComPtr<IXAudio2> xaudio(nullptr);
	if(FAILED(::XAudio2Create(&xaudio, f_debug)))
	{
		throw runtime_error("Failed to initialize XAudio2");
	}

	return xaudio;
}

//*****************************************************************************
// Mastering Voice
MasteringVoice InitMasteringVoice(const ComPtr<IXAudio2> xaudio)
{
	assert(xaudio.Get());

	MasteringVoice mastering_voice(nullptr);
	if(FAILED(xaudio->CreateMasteringVoice(
		reinterpret_cast<IXAudio2MasteringVoice**>(&mastering_voice)
		)))
	{
		throw runtime_error("Failed to create a mastering voice");
	}

	return mastering_voice;
}

//*****************************************************************************
// Submix Voice
SubmixVoice InitSubmixVoice(const ComPtr<IXAudio2> xaudio)
{
	assert(xaudio.Get());

	SubmixVoice submix_voice(nullptr);
	if(FAILED(xaudio->CreateSubmixVoice(
		reinterpret_cast<IXAudio2SubmixVoice**>(&submix_voice),
		2,
		44100,
		0,
		0,
		nullptr,
		nullptr
		)))
	{
		throw runtime_error("Failed to create a submix voice");
	}

	return submix_voice;
}

/******************************************************************************
	SoundDevice
******************************************************************************/

SoundDevice::SoundDevice() try :
	m_XAudio(InitXAudio()),
	m_MasterVoice(InitMasteringVoice(m_XAudio))
{}
catch(const runtime_error& e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}

/******************************************************************************
	Submixer
******************************************************************************/

Submixer::Submixer(const ComPtr<IXAudio2> xaudio) try :
	m_SubmixVoice(InitSubmixVoice(xaudio))
{}
catch(const runtime_error& e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}