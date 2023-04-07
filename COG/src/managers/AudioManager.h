#pragma once
/*******************************************************************************************************************
	AudioManager.h, AudioManager.cpp
	Created by Kim Kane
	Last updated: 15/04/2018

	Singleton class that handles all in-game audio using the FMOD sound system.

	[Features]
	Supports FMOD audio library.
	Stores all sounds loaded and channels available into a memory cache, allowing re-use of sound files.
	Supports delaying of sounds (e.g. generate a delay in-between player walk cycles)
	
	[Upcoming]
	3D sound, support for events and sound banks.

	[Side Notes]
	Nothing at present.

*******************************************************************************************************************/
#include <pretty_glm/glm.hpp>
#include <string>
#include <map>
#include <pretty_fmod/fmod_studio.hpp>
#include <pretty_fmod/fmod.hpp>
#include "utilities/Singleton.h"

class AudioManager {

public:
	friend class Singleton<AudioManager>;

public:
	bool Initialize();
	void Update();
	void Shutdown();

public:
	bool LoadAudio(const std::string& tag, const std::string& soundFile, bool isLooping, float delayCallback = 0.0f, bool isStream = false, bool is3Dsound = false);
	bool PlayAudio(const std::string& tag, float volume, const glm::vec3& position = glm::vec3(0.0f));
	void UnloadAudio(const std::string& tag);

public:
	bool IsChannelPlaying(const std::string& tag);
	void PauseChannel(const std::string& tag, bool paused);
	void StopChannel(const std::string& tag);
	void StopAllChannels();

public:
	void SetChannel3DPosition(const std::string& tag, const glm::vec3& position);
	void SetChannelVolume(const std::string& tag, float volume);

private:
	float dBToVolume(float db);
	float VolumeTodB(float volume);

private:
	int GetErrors(FMOD_RESULT result);
	FMOD_VECTOR VectorToFmod(const glm::vec3& position);
	bool DelayCallbackElapsed(const std::string& tag);

private:
	AudioManager();
	AudioManager(const AudioManager&)				= delete;
	AudioManager& operator=(const AudioManager&)	= delete;

private:
	typedef std::map<std::string, FMOD::Sound*>				SoundCache;
	typedef std::map<std::string, FMOD::Channel*>			ChannelCache;
	typedef std::map<std::string, std::pair<float, float>>	DelayCallbacks;
private:
	FMOD::Studio::System*	m_studioSystem;
	FMOD::System*			m_lowLevelSystem;

private:
	SoundCache		m_sounds;
	ChannelCache	m_channels;
	DelayCallbacks	m_delays;
	bool			m_hasSoundDriver;

private:
	static const unsigned int s_maxChannels;
};

typedef Singleton<AudioManager> Audio;