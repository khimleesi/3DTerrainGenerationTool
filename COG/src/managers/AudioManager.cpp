#include <pretty_fmod/fmod_errors.h>
#include <vector>
#include "managers/AudioManager.h"
#include "utilities/Log.h"
#include "managers/GameManager.h"

/*******************************************************************************************************************
	Constructor with initializer list to set default values of data members
*******************************************************************************************************************/
AudioManager::AudioManager()
	:	m_studioSystem(nullptr),
		m_lowLevelSystem(nullptr),
		m_hasSoundDriver(true)
{
	COG_LOG("[AUDIO MANAGER CONSTRUCT]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Function that initializes the FMOD sound system
*******************************************************************************************************************/
bool AudioManager::Initialize() {

	//--- Create our FMOD studio handle
	if (GetErrors(FMOD::Studio::System::create(&m_studioSystem)))													{ return false; }
	
	//--- Initialize FMOD studio
	if (GetErrors(m_studioSystem->initialize(s_maxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr)))	{ return false; }
	
	//--- Get the low level FMOD handle
	if (GetErrors(m_studioSystem->getLowLevelSystem(&m_lowLevelSystem)))											{ return false; }

	int numDrivers = 0;

	//--- Make sure the user has speakers
	GetErrors(m_lowLevelSystem->getNumDrivers(&numDrivers));

	if (numDrivers == 0)
	{
		if (GetErrors(m_lowLevelSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND))) { return false; }
		COG_LOG("[FMOD AUDIO] No sound drivers found, cannot play sounds", COG_LOG_EMPTY, LOG_WARN);
		m_hasSoundDriver = false;
		return false;
	}

	return true;
}


/*******************************************************************************************************************
	Function that updates the FMOD sound system every frame
*******************************************************************************************************************/
void AudioManager::Update() {
	
	if (m_hasSoundDriver) {

		std::vector<ChannelCache::iterator> stoppedChannels;

		//--- Check what channels are no longer playing
		for (auto it = m_channels.begin(); it != m_channels.end(); it++) {

			bool isPlaying = false;
			it->second->isPlaying(&isPlaying);
			if (!isPlaying) { stoppedChannels.emplace_back(it); }
		}

		//--- Remove them from the channel cache
		for (auto& it : stoppedChannels) {
			m_channels.erase(it);
		}

		GetErrors(m_studioSystem->update());
	}
}


/*******************************************************************************************************************
	Function that shuts down the FMOD sound system and cleans up all memory usage
*******************************************************************************************************************/
void AudioManager::Shutdown() {

	static unsigned int soundCount = m_sounds.size();
	
	COG_LOG("[FMOD AUDIO] Size of sounds cache before sounds destroyed: ", soundCount, LOG_RESOURCE);
	
	for (auto& sound : m_sounds) { 
		COG_LOG("[FMOD AUDIO] Destroying sound: ", sound.first.c_str(), LOG_RESOURCE);
		GetErrors(sound.second->release());
		soundCount--;
		if (soundCount == 0) { break; }
	}
	COG_LOG("[FMOD AUDIO] Size of sounds cache after sounds destroyed: ", soundCount, LOG_RESOURCE);

	GetErrors(m_studioSystem->release());
	m_lowLevelSystem	= nullptr;
	m_studioSystem		= nullptr;

	COG_LOG("[AUDIO MANAGER SHUT DOWN]", COG_LOG_EMPTY, LOG_BREAK);
}


/*******************************************************************************************************************
	Function that loads in a sound file and stores the sound in the sound cache
*******************************************************************************************************************/
bool AudioManager::LoadAudio(const std::string& tag, const std::string& soundFile, bool isLooping, float delayCallback, bool isStream, bool is3Dsound)
{
	auto found = m_sounds.find(tag);
	if (found != m_sounds.end())
	{
		COG_LOG("[FMOD AUDIO] Sound file already exists, re-using: ", tag.c_str(), LOG_RESOURCE);
		return false;
	}

	FMOD_MODE mode = FMOD_DEFAULT;

	mode |= (int)isLooping	? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= (int)isStream	? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;
	mode |= (int)is3Dsound	? FMOD_3D : FMOD_2D;

	FMOD::Sound* soundHandle = nullptr;

	std::string src = "Assets\\Sounds\\" + soundFile;

	if (GetErrors(m_lowLevelSystem->createSound(src.c_str(), mode, nullptr, &soundHandle))) { return false; }

	if (soundHandle) { 
		m_sounds.try_emplace(tag, soundHandle);
		if (delayCallback != 0.0f) { m_delays.try_emplace(tag, std::make_pair(0.0f, delayCallback)); }
		COG_LOG("[FMOD AUDIO] New sound loaded successfully: ", src.c_str(), LOG_RESOURCE);
	}

	return true;
}


/*******************************************************************************************************************
	Function that unloads a sound file and removes the sound from the sound cache
*******************************************************************************************************************/
void AudioManager::UnloadAudio(const std::string& tag)
{
	auto found = m_sounds.find(tag);

	if (found != m_sounds.end()) {

		GetErrors((found->second->release()));
		m_sounds.erase(found);
		COG_LOG("[FMOD AUDIO] Sound destroyed from memory and removed from sound cache: ", tag.c_str(), LOG_RESOURCE);
	}
	else { COG_LOG("[FMOD AUDIO] Can't unload sound, sound not found: ", tag.c_str(), LOG_RESOURCE); }
}


/*******************************************************************************************************************
	Function that plays a sound in the sound cache
*******************************************************************************************************************/
bool AudioManager::PlayAudio(const std::string& tag, float volume, const glm::vec3& position)
{
	auto found = m_sounds.find(tag);
	
	if (found == m_sounds.end()) {
		COG_LOG("[FMOD AUDIO] Trying to play a sound that hasn't been loaded: ", tag.c_str(), LOG_ERROR);
		return false;
	}

	//--- Make sure channel isn't already playing
	if (IsChannelPlaying(tag)) { return false; }
	
	//--- If user wants a delay callback (sound is delayed for a certain amount of time - useful for walking/shooting/running sounds)
	auto delay = m_delays.find(tag);
	if (delay != m_delays.end()) { if (!DelayCallbackElapsed(tag)) { return false; } }

	FMOD::Channel* channel = nullptr;

	if (GetErrors(m_lowLevelSystem->playSound(found->second, nullptr, true, &channel))) { return false; }

	if (channel) {

		FMOD_MODE mode = FMOD_DEFAULT;

		found->second->getMode(&mode);

		if (mode & FMOD_3D) {

			FMOD_VECTOR fmodPosition = VectorToFmod(position);
			GetErrors(channel->set3DAttributes(&fmodPosition, nullptr));
		}
	
		GetErrors(channel->setVolume(dBToVolume(volume)));
		GetErrors(channel->setPaused(false));

		m_channels.try_emplace(found->first, channel);
	}

	return true;
}


/*******************************************************************************************************************
	Function that returns a set delay amount to be added to the user defined delay
*******************************************************************************************************************/
bool AudioManager::DelayCallbackElapsed(const std::string& tag)
{
	m_delays[tag].first += 1.0f * Game::Instance()->GetDeltaTime();

	if (m_delays[tag].first <= m_delays[tag].second)	{ return false; }
	else												{ m_delays[tag].first = 0.0f; }

	return true;
}


/*******************************************************************************************************************
	Function that checks if a sound channel is currently playing
*******************************************************************************************************************/
bool AudioManager::IsChannelPlaying(const std::string& tag)
{
	bool isPlaying = false;
	auto channel = m_channels.find(tag);
	
	if (channel != m_channels.end()) { channel->second->isPlaying(&isPlaying); }

	return isPlaying;
}


/*******************************************************************************************************************
	Function that stops playing a sound channel
*******************************************************************************************************************/
void AudioManager::StopChannel(const std::string& tag)
{
	auto found = m_channels.find(tag);
	if (found != m_channels.end()) { GetErrors(found->second->stop()); }
	else { COG_LOG("[FMOD AUDIO] Couldn't find sound to stop channel: ", tag.c_str(), LOG_WARN); }
}


/*******************************************************************************************************************
	Function that sets the 3D position of a sound channel
*******************************************************************************************************************/
void AudioManager::SetChannel3DPosition(const std::string& tag, const glm::vec3& position)
{
	auto found = m_channels.find(tag);

	if (found != m_channels.end()) {
		FMOD_VECTOR Fmodposition = VectorToFmod(position);
		GetErrors(found->second->set3DAttributes(&Fmodposition, nullptr));
	}
	else { COG_LOG("[FMOD AUDIO] Couldn't find sound to set channels 3D position: ", tag.c_str(), LOG_WARN); }
}


/*******************************************************************************************************************
	Function that sets the volume of a sound channel
*******************************************************************************************************************/
void AudioManager::SetChannelVolume(const std::string& tag, float volume)
{
	auto found = m_channels.find(tag);

	if (found != m_channels.end()) { GetErrors(found->second->setVolume(dBToVolume(volume))); }
	else { COG_LOG("[FMOD AUDIO] Couldn't find sound to set channels volume: ", tag.c_str(), LOG_WARN); }
}


/*******************************************************************************************************************
	Function that pauses/unpauses a sound channel
*******************************************************************************************************************/
void AudioManager::PauseChannel(const std::string& tag, bool paused)
{
	auto found = m_channels.find(tag);

	if (found != m_channels.end()) { GetErrors(found->second->setPaused(paused)); }
	else { COG_LOG("[FMOD AUDIO] Couldn't find sound to set channels pause state: ", tag.c_str(), LOG_WARN); }
}


/*******************************************************************************************************************
	Function that stops playing all sound channels
*******************************************************************************************************************/
void AudioManager::StopAllChannels()
{
	for (auto& channel : m_channels) { GetErrors(channel.second->stop()); }
}


/*******************************************************************************************************************
	Function that converts a glm::vec3 to an FMOD_VECTOR for 3D sound
*******************************************************************************************************************/
FMOD_VECTOR AudioManager::VectorToFmod(const glm::vec3& position) {
	FMOD_VECTOR fmodPosition;
	fmodPosition.x = position.x;
	fmodPosition.y = position.y;
	fmodPosition.z = position.z;
	return fmodPosition;
}


/*******************************************************************************************************************
	Function that converts decibel to a volume amount
*******************************************************************************************************************/
float  AudioManager::dBToVolume(float dB)
{
	return powf(10.0f, 0.05f * dB);
}


/*******************************************************************************************************************
	Function that converts a volume amount in to decibel
*******************************************************************************************************************/
float  AudioManager::VolumeTodB(float volume)
{
	return 20.0f * log10f(volume);
}


/*******************************************************************************************************************
	Function that returns any errors discovered during the use of the FMOD sound system
*******************************************************************************************************************/
int AudioManager::GetErrors(FMOD_RESULT result) {
	
	if (result != FMOD_OK) {
		COG_LOG("[FMOD AUDIO] ", FMOD_ErrorString(result), LOG_WARN);
		return 1;
	}

	return 0;
}


/*******************************************************************************************************************
	Static variables and functions
*******************************************************************************************************************/
unsigned const int AudioManager::s_maxChannels = 32;