#include <iostream>

#include "Audio.hpp"

#define VOLUME_WALK 0.2f
#define VOLUME_RUN 0.3f
#define VOLUME_BREAK 0.5f
#define VOLUME_CHAIR 0.7f
#define VOLUME_LAND 0.5f
#define VOLUME_MUSIC 0.03f

#define print(x) std::cout << x << "\n"

using namespace irrklang;

Audio::Audio()
{
	// init device
	engine = createIrrKlangDevice();
	if (!engine) {
		std::cerr << "Failed to create IrrKlang device";
	}

	// init sound effects
	ISoundSource* sound_teleport = engine->addSoundSourceFromFile("resources/sounds/teleport.mp3");
	sound_teleport->setDefaultVolume(0.15f);
	sound_list.insert({ "sound_teleport", sound_teleport });

	ISoundSource* sound_land = engine->addSoundSourceFromFile("resources/sounds/jump_land.wav");
	sound_land->setDefaultVolume(0.35f);
	sound_list.insert({ "sound_land", sound_land });

	ISoundSource* sound_glass = engine->addSoundSourceFromFile("resources/sounds/glass_break.wav");
	sound_list.insert({ "sound_glass", sound_glass });

	ISoundSource* sound_chair = engine->addSoundSourceFromFile("resources/sounds/chair_move.wav");
	sound_list.insert({ "sound_chair", sound_chair });

	ISoundSource* sound_shoot = engine->addSoundSourceFromFile("resources/sounds/shoot.wav");
	sound_shoot->setDefaultVolume(0.20f);
	sound_list.insert({ "sound_shoot", sound_shoot });

	// init footstep sound

	walking_sound = engine->play2D("resources/sounds/walk.wav", true, true, true);
	if (walking_sound) {
		walking_sound->setVolume(0.45f);
	}

	running_sound = engine->play2D("resources/sounds/run.wav", true, true, true);
	if (running_sound) {
		running_sound->setVolume(0.25f);
	}

	// init chair sound

	chair_sound = engine->play3D("resources/sounds/chair_move.wav", irrklang::vec3df(0, 0, 0), true, true, true);
	if (chair_sound) {
		chair_sound->setMinDistance(1.0f);
		chair_sound->setVolume(3.0f);
	}

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

void Audio::PlayBgMusic()
{
	//bg_music = engine->play3D("resources/sounds/music.mp3", irrklang::vec3df(2, 2, 2), true, false, true);
	bg_music = engine->play2D("resources/sounds/music.mp3", true, false, true);
	print(bg_music);
	if (bg_music) {
		//bg_music->setMinDistance(5.0f);
		bg_music->setVolume(VOLUME_MUSIC);
	}
}

void Audio::PlayFootstepSound(bool runningNotWalking)
{
	if (runningNotWalking) {
		running_sound->setIsPaused(false);
		walking_sound->setIsPaused(true);
	}
	else {
		walking_sound->setIsPaused(false);
		running_sound->setIsPaused(true);
	}
}


void Audio::UpdateChairPosition(glm::vec3 position)
{
	if (chair_sound->getIsPaused()) {
		chair_sound->setIsPaused(false);
;	}
	irrklang::vec3df pos(position.x, position.y, position.z);
	chair_sound->setPosition(pos);
}

void Audio::StopFootstepSound()
{
	walking_sound->setIsPaused(true);
	running_sound->setIsPaused(true);
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

Audio::~Audio()
{
	if (bg_music) {
		bg_music->stop();
		bg_music->drop();
	}
	if (walking_sound) {
		walking_sound->stop();
		walking_sound->drop();
	}
	if (running_sound) {
		running_sound->stop();
		running_sound->drop();
	}
	if (engine) {
		engine->drop();
	}
}
