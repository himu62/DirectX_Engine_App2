/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Sound.cpp
*	Author	: himu62
******************************************************************************/

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "Sound.hpp"

/******************************************************************************
	Initialize
******************************************************************************/

//*****************************************************************************
// Source Voice
SourceVoice InitSourceVoice(const ComPtr<IXAudio2> xaudio, const WAVEFORMATEX& format)
{
	assert(xaudio.Get());

	SourceVoice voice(nullptr);
	if(FAILED(xaudio->CreateSourceVoice(
		reinterpret_cast<IXAudio2SourceVoice**>(&voice),
		&format
		)))
	{
		throw runtime_error("Failed to create a source voice");
	}

	return voice;
}

SourceVoice InitSourceVoice(
	const ComPtr<IXAudio2> xaudio,
	const WAVEFORMATEX& format,
	std::unique_ptr<Sound_Vorbis> callback
	)
{
	assert(xaudio.Get());
	assert(callback.get());

	SourceVoice voice(nullptr);
	if(FAILED(xaudio->CreateSourceVoice(
		reinterpret_cast<IXAudio2SourceVoice**>(&voice),
		&format,
		0,
		XAUDIO2_DEFAULT_FREQ_RATIO,
		callback.get()
		)))
	{
		throw runtime_error("Failed to create a source voice");
	}

	return voice;
}

/******************************************************************************
	SoundBuffer
******************************************************************************/

SoundBuffer_PCM::SoundBuffer_PCM(const std::wstring& file_name)
{
	assert(&file_name);

	ZeroMemory(&m_Format, sizeof(m_Format));

	vector<char> file_data = ReadFile(file_name);
	if(file_data.empty())
	{
		throw runtime_error("Failed to read a file");
	}

	vector<char> read_data;

	for(int i=0; i<4; ++i)
	{
		read_data.push_back(file_data[i]);
	}
	if(!memcmp("RIFF", &read_data.front(), 4))
	{
		throw runtime_error("Failed to read a file");
	}

	read_data.clear();
	for(int i=0; i<4; ++i)
	{
		read_data.push_back(file_data[i+8]);
	}
	if(!memcmp("WAVE", &read_data.front(), 4))
	{
		throw runtime_error("Failed to read a file");
	}

	read_data.clear();
	for(int i=0; i<4; ++i)
	{
		read_data.push_back(file_data[i+12]);
	}
	if(!memcmp("fmt ", &read_data.front(), 4))
	{
		throw runtime_error("Failed to read a file");
	}

	uint32_t format_size;

}