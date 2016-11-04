#pragma once
#include "fmod.hpp"
#include "fmod_errors.h"
#include <unordered_map>
#include <string>
#include <random>
#include "../Object Manager/GameObject.h"


#define SampleSize 128
#define BEAT_THRESHOLD 0.3f

class FFmod
{
	FMOD::System* system;
	FMOD_RESULT result;
	unsigned version;
	int numDrivers;
	FMOD_SPEAKERMODE currSpeakerMode;
	//FMOD_CAPS caps;
	char name[256];
	int sampleRate;
	std::string m_szBasePath;
	std::string m_szSuffix;

	float specLeft[SampleSize];
	float specRight[SampleSize];
	float spec[SampleSize];
	FMOD_DSP_PARAMETER_FFT* fft;
	FMOD::DSP* m_pScanningDSP;
	FMOD::DSP* m_pWhipDSP;

	std::unordered_map<std::string, FMOD::Sound*> m_vSounds;
	std::unordered_map<std::string, FMOD::Sound*> m_vStreams;

	FMOD::ChannelGroup* m_MasterChannel;
	FMOD::ChannelGroup* m_MusicChannel;
	FMOD::ChannelGroup* m_GameSFXChannel;
	FMOD::ChannelGroup* m_MenuSFXChannel;

	FMOD::Channel* m_pBGM;

	float m_fMasterVol,
		m_fSFXVol,
		m_fBGMVol;

	bool m_bBGMDampened;

	std::default_random_engine m_RandEng;
	std::uniform_int_distribution<int> m_UniformDist;

	float m_fCamOffset;

public:
	FFmod();
	~FFmod();

	void Init();
	void Shutdown();
	void Update(float _dt);
	void FMODErrorCheck(FMOD_RESULT _res);
	//void LoadSound(char* _path, FMOD_MODE _mode = FMOD_DEFAULT);
	void LoadSound(std::string _name, FMOD_MODE _mode = FMOD_DEFAULT);
	void LoadStream(std::string _name, FMOD_MODE _mode = FMOD_DEFAULT);
	//void LoadStream(char* _path, FMOD_MODE _mode = FMOD_DEFAULT);
	//void PlayStream(std::string _name, )
	//void PlaySound(std::string _name, )
	void PlaySound(std::string _name, GameObject* _obj, bool _2D = false, bool _Loop = false);

	// NOTE: DEPRICATED
	// -- If you need a one shot, create an Audio_OneSHot obj
	// -- and use the overload with the gameobject*.  make sure to either
	// -- set the obj's position or set the sound to 2D.  
	void PlaySound(std::string _name);


	void PlayBGM(std::string _name);
	void StopBGM();
	void UpdateListener(GameObject* _cam);
	void ResetListener();
	void StopAllSounds();
	void PauseAllSounds();
	void ResumeAllSounds();
	void ModulateWhip(FMOD::Channel* _source, float _speed);

	void DampenBGM();
	void UnDampenBGM();


	void GrabFreq();
	bool BeatCheck();

	// 0 - MASTER, 1 - SFX, 2 - BGM
	void SetChannelVolume(int _whichChannel, int _newVol);

	bool IsBGMDampened() { return m_bBGMDampened; }

	void ConnectWhipDSP(FMOD::Channel* _source);

	void PlayRandomBGM();

	void SetCameraDistance(float _val) { m_fCamOffset = _val; }

};

