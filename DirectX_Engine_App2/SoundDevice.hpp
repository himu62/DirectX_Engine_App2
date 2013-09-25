/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: SoundDevice.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#pragma warning(disable: 4005)
#include <memory>
#include <vector>
#include <string>

#include <XAudio2.h>

#include <wrl/client.h>
#include <boost/noncopyable.hpp>

#include "deleter.hpp"

#pragma comment(lib, "xapobase.lib")
#ifdef _DEBUG
#pragma comment(lib, "xapobased.lib")
#endif

using namespace Microsoft::WRL;

typedef std::unique_ptr<IXAudio2MasteringVoice, VoiceDeleter> MasteringVoice;
typedef std::unique_ptr<IXAudio2SubmixVoice, VoiceDeleter> SubmixVoice;
typedef std::unique_ptr<IXAudio2SourceVoice, SourceVoiceDeleter> SourceVoice;

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