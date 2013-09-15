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
		throw std::runtime_error("Failed to initialize XAudio2");
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
		throw std::runtime_error("Failed to create a mastering voice");
	}

	return mastering_voice;
}

/******************************************************************************
	SoundDevice
******************************************************************************/

SoundDevice::SoundDevice() try :
	m_XAudio(InitXAudio()),
	m_MasterVoice(InitMasteringVoice(m_XAudio))
{}
catch(const std::runtime_error &e)
{
	std::wstringstream ss(L"");
	ss << e.what();

	MessageBox(nullptr, ss.str().c_str(), L"Error", MB_ICONERROR | MB_OK);

	exit(-1);
}