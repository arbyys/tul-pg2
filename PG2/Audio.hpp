#pragma once
#include <map>

#include <glm/glm.hpp>
#include <irrKlang/irrKlang.h>

using namespace irrklang;

class Audio
{
public:
	Audio();

	// All 3D sound
	void UpdateListenerPosition(glm::vec3 position, glm::vec3 front, glm::vec3 world_up);

	// Sound effects
	void Play2DOneShot(std::string sound_name);
	void Play3DOneShot(std::string sound_name, glm::vec3 position);
	void PlayFootstepSound();
	void PlayBgMusic();

	~Audio();
private:
	ISoundEngine* engine = nullptr;

	std::map<std::string, ISoundSource*> sound_list;

	irrklang::ISound* bg_music = nullptr;

	bool isCharacterRunning = false;
};
