#include <iostream>

#include "Audio.hpp"

#define VOLUME_WALK 0.2f
#define VOLUME_RUN 0.3f
#define VOLUME_BREAK 0.5f
#define VOLUME_CHAIR 0.7f
#define VOLUME_LAND 0.5f

using namespace irrklang;

Audio::Audio()
{
	// init device
	engine = createIrrKlangDevice();
	if (!engine) {
		std::cerr << "Failed to create IrrKlang device";
	}

	// init sound effects
	ISoundSource* sound_walk = engine->addSoundSourceFromFile("resources/sounds/walk.wav");
	sound_walk->setDefaultVolume(0.25f);
	sound_list.insert({ "sound_walk", sound_walk });

	ISoundSource* sound_run = engine->addSoundSourceFromFile("resources/sounds/run.wav");
	sound_run->setDefaultVolume(0.25f);
	sound_list.insert({ "sound_run", sound_run });

	ISoundSource* sound_teleport = engine->addSoundSourceFromFile("resources/sounds/teleport.mp3");
	sound_teleport->setDefaultVolume(0.15f);
	sound_list.insert({ "sound_teleport", sound_teleport });

	ISoundSource* sound_land = engine->addSoundSourceFromFile("resources/sounds/jump_land.wav");
	sound_land->setDefaultVolume(0.45f);
	sound_list.insert({ "sound_land", sound_land });

	ISoundSource* sound_glass = engine->addSoundSourceFromFile("resources/sounds/glass_break.wav");
	sound_list.insert({ "sound_glass", sound_glass });

	ISoundSource* sound_chair = engine->addSoundSourceFromFile("resources/sounds/chair_move.wav");
	sound_list.insert({ "sound_chair", sound_chair });
}

void Audio::UpdateListenerPosition(glm::vec3 position, glm::vec3 front, glm::vec3 world_up)
{
	// position of the listener
	vec3df _position(position.x, position.y, position.z);
	// the direction the listener looks into
	vec3df _look_direction(-front.x, front.y, -front.z);
	// only relevant for doppler effects
	vec3df _vel_per_second(0, 0, 0);
	// where 'up' is in your 3D scene
	vec3df _up_vector(world_up.x, world_up.y, world_up.z);

	engine->setListenerPosition(_position, _look_direction, _vel_per_second, _up_vector);
}

void Audio::Play2DOneShot(std::string sound_name)
{
	engine->play2D(sound_list[sound_name]);
}

void Audio::Play3DOneShot(std::string sound_name, glm::vec3 position)
{
	vec3df _position(position.x, position.y, position.z);
	engine->play3D(sound_list[sound_name], _position);
}

void Audio::PlayWalk()
{
	engine->play2D(sound_list[isCharacterRunning ? "sound_run" : "sound_walk"]);
	isCharacterRunning = !isCharacterRunning;
}

Audio::~Audio()
{
	/*if (music) {
		music->stop();
		music->drop();
	}
	if (jetpack) {
		jetpack->stop();
		jetpack->drop();
	}
	*/
	if (engine) {
		engine->drop();
	}
}
