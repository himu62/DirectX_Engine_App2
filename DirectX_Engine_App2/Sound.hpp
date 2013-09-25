/******************************************************************************
*	Project	: DirectX_Engine_App2
*	Version	: 0.0.0
*	File	: Sound.hpp
*	Author	: himu62
******************************************************************************/
#pragma once

#pragma warning(disable: 4005)
#include <memory>
#include <vector>
#include <string>
#include <queue>

#include <XAudio2.h>

#include <vorbisfile.h>
#include <wrl/client.h>
#include <boost/noncopyable.hpp>
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>

#include "deleter.hpp"

#pragma comment(lib, "xapobase.lib")
#ifdef _DEBUG
#pragma comment(lib, "xapobased.lib")
#endif
#pragma comment(lib, "libogg.lib")
#pragma comment(lib, "libvorbis.lib")
#pragma comment(lib, "libvorbisfile.lib")

using std::runtime_error;
using std::vector;
using namespace Microsoft::WRL;

typedef std::unique_ptr<IXAudio2MasteringVoice, VoiceDeleter> MasteringVoice;
typedef std::unique_ptr<IXAudio2SubmixVoice, VoiceDeleter> SubmixVoice;
typedef std::unique_ptr<IXAudio2SourceVoice, SourceVoiceDeleter> SourceVoice;

/******************************************************************************
	SoundBuffer
******************************************************************************/

struct SoundBuffer_PCM
{
	explicit SoundBuffer_PCM(const std::wstring& file_name);

	vector<char> m_Buffer;
	WAVEFORMATEX m_Format;
};

struct SoundBuffer_Vorbis
{
	explicit SoundBuffer_Vorbis(const std::wstring& file_name);

	static size_t Read(void* buffer, size_t size, size_t max_count, void* stream);
	static int Seek(void* buffer, ogg_int64_t offset, int flag);
	static int Close(void* buffer);
	static int Tell(void* buffer);

	std::unique_ptr<OggVorbis_File, VorbisFileDeleter> m_File;
	WAVEFORMATEX m_Format;

	vector<char> m_Buffer;
	unsigned int m_FileSize;
	unsigned int m_Pos;
};

/******************************************************************************
	Sound
******************************************************************************/

class Sound
{
public:
	explicit Sound();

protected:
	void SetVoice(SourceVoice voice);
	void Submit(const XAUDIO2_BUFFER& buffer);

private:
	friend class SoundDevice;

	virtual void Init() = 0;

	SourceVoice m_Voice;
};

class Sound_PCM : public Sound
{
public:
	explicit Sound_PCM(const std::wstring& file_name, const ComPtr<IXAudio2> xaudio);

	void Play() const;
	void Stop();
	void Pause() const;

	void SetVolume(float volume);

private:
	void Init();

	boost::flyweights::flyweight<boost::flyweights::key_value<std::wstring, SoundBuffer_PCM>> m_Buffer;
	SourceVoice m_Voice;
};

class Sound_Vorbis : public Sound, public IXAudio2VoiceCallback
{
public:
	explicit Sound_Vorbis(
		const std::wstring& file_name,
		bool looped,
		uint32_t offset,
		const ComPtr<IXAudio2> xaudio
		);

	void Play() const;
	void Stop();
	void Pause() const;

	void SetVolume(float volume);

private:
	friend class SoundDevice;

	void WINAPI OnStreamEnd();
	void WINAPI OnVoiceProcessingPassStart(UINT32);
	void WINAPI OnVoiceProcessingPassEnd();
	void WINAPI OnBufferStart(void*);
	void WINAPI OnBufferEnd(void*);
	void WINAPI OnLoopEnd(void*);
	void WINAPI OnVoiceError(void*, HRESULT);

	void Init();
	void LoadBuffer();

	bool m_Looped;
	uint32_t m_Offset;

	std::queue<vector<uint8_t>> m_Buffer;
	boost::flyweights::flyweight<boost::flyweights::key_value<std::wstring, SoundBuffer_Vorbis>> m_Ogg;
};