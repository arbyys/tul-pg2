#pragma once
#include <map>

#include <glm/glm.hpp>
#include <irrKlang/irrKlang.h>

using namespace irrklang;

class Audio
{
public:
	Audio();

	void UpdateListenerPosition(glm::vec3 position, glm::vec3 front, glm::vec3 world_up);
	void Play2DOneShot(std::string sound_name);
	void Play3DOneShot(std::string sound_name, glm::vec3 position);
	void PlayFootstepSound(bool runningNotWalking);
	void StopFootstepSound();
	void UpdateChairPosition(glm::vec3 position);
	void PlayBgMusic();

	~Audio();
private:
	ISoundEngine* engine = nullptr;

	std::map<std::string, ISoundSource*> sound_list;

	irrklang::ISound* bg_music = nullptr;
	irrklang::ISound* walking_sound = nullptr;
	irrklang::ISound* running_sound = nullptr;
	irrklang::ISound* chair_sound = nullptr;
	
	bool footstepSoundPlaying = false;
};
