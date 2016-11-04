#include "stdafx.h"
#include "FFmod.h"

#define HZRANGE (20000 - 160)

FFmod::FFmod()
{
}


FFmod::~FFmod()
{
}

void FFmod::Init()
{
	result = FMOD::System_Create(&system);
	FMODErrorCheck(result);

	result = system->getVersion(&version);
	FMODErrorCheck(result);
	if (version < FMOD_VERSION)
		std::cout << "old version of FMOD";

	result = system->getNumDrivers(&numDrivers);
	FMODErrorCheck(result);

	if (numDrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		FMODErrorCheck(result);
	}
	else
	{
		result = system->getDriverInfo(0, 0, 0, 0, &sampleRate, &currSpeakerMode, 0);
		FMODErrorCheck(result);
	}

	result = system->init(1024, FMOD_INIT_NORMAL, 0);
	FMODErrorCheck(result);

	result = system->createChannelGroup("BGM", &m_MusicChannel);
	FMODErrorCheck(result);

	result = system->createChannelGroup("GSFX", &m_GameSFXChannel);
	FMODErrorCheck(result);

	result = system->createChannelGroup("MSFX", &m_MenuSFXChannel);
	FMODErrorCheck(result);

	result = system->getMasterChannelGroup(&m_MasterChannel);
	FMODErrorCheck(result);

	result = m_MasterChannel->addGroup(m_MusicChannel);
	FMODErrorCheck(result);

	result = m_MasterChannel->addGroup(m_GameSFXChannel);
	FMODErrorCheck(result);

	result = m_MasterChannel->addGroup(m_MenuSFXChannel);
	FMODErrorCheck(result);

//  	result = system->set3DSettings(1.0f, 1, 100.0f);
//  	FMODErrorCheck(result);

	result = system->createDSPByType(FMOD_DSP_TYPE_FFT, &m_pScanningDSP);
	FMODErrorCheck(result);

	result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS_SIMPLE, &m_pWhipDSP);
	FMODErrorCheck(result);

	int windowSize = 256;
	result =  m_pScanningDSP->setParameterInt(FMOD_DSP_FFT_WINDOWSIZE, windowSize);
	FMODErrorCheck(result);

	m_MusicChannel->addDSP(0, m_pScanningDSP);

	m_szBasePath = "Assets/Sounds/";
	m_szSuffix = ".ogg";

	m_fBGMVol = m_fSFXVol = m_fMasterVol = 1;

	m_bBGMDampened = false;

	m_UniformDist = std::uniform_int_distribution<int>(49, 51);


	m_fCamOffset = 120;
}

void FFmod::Shutdown()
{

}

void FFmod::FMODErrorCheck(FMOD_RESULT _res)
{
	if (_res != FMOD_OK)
	{
		std::cout << "ERROR: " << _res << FMOD_ErrorString(_res) << "\n";
		std::cin.ignore();
		PostQuitMessage(-1);
	}
}

void FFmod::Update(float _dt)
{
	system->update();

// 	float val;
// 	m_MusicChannel->getVolume(&val);
// 	std::cout << val;
// 	m_pBGM->getVolume(&val); 
// 	std::cout << " " << val << '\n';
}

void FFmod::LoadSound(std::string _name, FMOD_MODE _mode )
{
	FMOD::Sound* audio;
	std::string szComplete = m_szBasePath + _name + m_szSuffix;
	result = system->createSound(szComplete.c_str(), _mode, 0, &audio);
	FMODErrorCheck(result);
	m_vSounds[_name] = audio;
}

void FFmod::LoadStream(std::string _name, FMOD_MODE _mode)
{
	FMOD::Sound* audio;
	std::string szComplete = m_szBasePath + _name + m_szSuffix;
	result = system->createStream(szComplete.c_str(), _mode, 0, &audio);
	FMODErrorCheck(result);
	m_vStreams[_name] = audio;
}

void FFmod::PlaySound(std::string _name, GameObject* _obj, bool _2D /*= false*/, bool _Loop /*= false*/)
{
	FMOD::Channel* tmpCHannel;
	FMOD_VECTOR tmpPos;

	result = system->playSound(m_vSounds[_name], m_GameSFXChannel, false, &tmpCHannel);
	FMODErrorCheck(result);

	if (_Loop)
 		tmpCHannel->setLoopCount(-1);

	if (_2D)
		tmpCHannel->setMode(FMOD_3D_HEADRELATIVE);

	tmpPos.x = _obj->GetTransform().GetPosition()->x;
	tmpPos.y = _obj->GetTransform().GetPosition()->y;
	tmpPos.z = _obj->GetTransform().GetPosition()->z;

	tmpCHannel->set3DAttributes(&tmpPos, 0);
	tmpCHannel->setMode(FMOD_3D_LINEARROLLOFF);
	tmpCHannel->set3DMinMaxDistance(m_fCamOffset, m_fCamOffset + 75);
	_obj->SetChannel(tmpCHannel);
}

void FFmod::PlaySound(std::string _name)
{
	FMOD::Channel* tmpCHannel;
	result = system->playSound(m_vSounds[_name], m_MenuSFXChannel, false, &tmpCHannel);
	FMODErrorCheck(result);

	tmpCHannel->setMode(FMOD_3D_HEADRELATIVE);
}

void FFmod::PlayBGM(std::string _name)
{
	

	result = system->playSound(m_vStreams[_name], m_MusicChannel, false, &m_pBGM);
	FMODErrorCheck(result);

	m_pBGM->setLoopCount(-1);

	m_pBGM->setMode(FMOD_3D_HEADRELATIVE);


}

void FFmod::UpdateListener(GameObject* _cam)
{
	FMOD_VECTOR camPos, camUp, camZ;

	camPos.x = _cam->GetTransform().GetPosition()->x;
	camPos.y = _cam->GetTransform().GetPosition()->y;
	camPos.z = _cam->GetTransform().GetPosition()->z;

	camUp.x = _cam->GetTransform().GetYAxis()->x;
	camUp.y = _cam->GetTransform().GetYAxis()->y;
	camUp.z = _cam->GetTransform().GetYAxis()->z;

	camZ.x = _cam->GetTransform().GetZAxis()->x;
	camZ.y = _cam->GetTransform().GetZAxis()->y;
	camZ.z = _cam->GetTransform().GetZAxis()->z;

	system->set3DListenerAttributes(0, &camPos, 0, &camZ, &camUp);
}

void FFmod::StopBGM()
{
	if (m_pBGM)
		m_pBGM->stop();
}

void FFmod::SetChannelVolume(int _whichChannel, int _newVol)
{
	float conversion = (float)_newVol;
	conversion *= 0.01f;
	switch (_whichChannel)
	{
	case 0:
		m_MasterChannel->setVolume(conversion);
		m_fMasterVol = conversion;
		break;

	case 1:
		m_GameSFXChannel->setVolume(conversion);
		m_MenuSFXChannel->setVolume(conversion);
		m_fSFXVol = conversion;
		break;

	case 2:
		m_MusicChannel->setVolume(conversion);
		m_fBGMVol = conversion;
		break;
	}
}

void FFmod::ResetListener()
{
	FMOD_VECTOR tmpVec;
	tmpVec.x = 0;
	tmpVec.y = 0;
	tmpVec.z = 0;

	system->set3DListenerAttributes(0, &tmpVec, 0, 0, 0);
}

void FFmod::StopAllSounds()
{
	m_MasterChannel->stop();
	
}

void FFmod::PauseAllSounds()
{ 
	m_GameSFXChannel->setPaused(true);
}

void FFmod::ResumeAllSounds()
{
	m_GameSFXChannel->setPaused(false);
}

void FFmod::ModulateWhip(FMOD::Channel* _source, float _speed)
{
	_speed *= 2;
	if (_speed > 1.0f)
		_speed = 1.0f;
	//_source->setPitch(_speed * 5);
	//_source->
	//std::cout << _speed << "\n";

	//m_pWhipDSP->setParameterFloat()

}

void FFmod::DampenBGM()
{
	m_MusicChannel->setVolume(m_fBGMVol * 0.5f);
	m_bBGMDampened = true;
}

void FFmod::UnDampenBGM()
{
	m_MusicChannel->setVolume(m_fBGMVol );
	m_bBGMDampened = false;
}

void FFmod::GrabFreq()
{
	float val = 0;
	m_pScanningDSP->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**)&fft, 0, 0, 0);

// 	for (int channel = 0; channel < fft->numchannels; channel++)
// 	{
// 		for (int bin = 0; bin < fft->length; bin++)
// 		{
// 			float val = fft->spectrum[channel][bin];
// 		}
// 	}
}

bool FFmod::BeatCheck()
{
	float val = (fft->spectrum[0][1] + fft->spectrum[1][1]) * 0.5f;
	//static unsigned int count = 0;
	if (val > BEAT_THRESHOLD * m_fBGMVol)
	{
		//std::cout << "Bass " << count++ << "\n";
		return true;
	}

	return false;
}

void FFmod::ConnectWhipDSP(FMOD::Channel* _source)
{
	_source->addDSP(0, m_pWhipDSP);
}

void FFmod::PlayRandomBGM()
{
	std::string tmpStr = "RATS_BGM_Normal_0";
	// 49 = 1 in ascii
	int help = rand() % 3 + 49;//m_UniformDist(m_RandEng);
	tmpStr += help;
	std::cout << tmpStr.c_str() << "\n";
	PlayBGM(tmpStr);
}


