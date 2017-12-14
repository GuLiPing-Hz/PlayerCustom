#include "stdafx.h"
#include "soundmgr.h"

CGLSoundMgr::CGLSoundMgr()
:m_bInit(false)
{
	m_mapSound.clear();
}

CGLSoundMgr::~CGLSoundMgr()
{
	releaseSound();
	if (m_bInit)
	{
		soundUninit();
	}
}

void CGLSoundMgr::setEnvArg(HWND hwnd,const std::string iniFile,const std::string current_dir)
{
	m_hwnd=hwnd;
	m_szIniFile=iniFile;
	std::string tmp = Ini_GetString(m_szIniFile.c_str(),"DIR","SoundDir","");
	if (tmp == "")
		return ;
	m_sound_dir=current_dir+tmp;

	//m_bInit = true;
	soundInit(hwnd);
}

void	CGLSoundMgr::soundUninit()
{
	BASS_Free();
}


bool CGLSoundMgr::soundInit(HWND hWnd)
{
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		WLOGH_ERRO <<"An incorrect version of BASS.DLL was loaded";
		return false;
	}

	int a, enabled=0,count=0;
	BASS_DEVICEINFO info;
	for (a=1; BASS_GetDeviceInfo(a, &info); a++)//0 is no sound so from 1
	{
		count++;
		if (info.flags&BASS_DEVICE_ENABLED) // device is enabled?
			enabled++; // count it
	}

	if(!count)
	{
		WLOGH_ERRO << L"there is no vaild device in this PC";
		return false;
	}

	if(!enabled)//如果没有启用的
	{
		int n = 1;
		bool finish = false;
		do 
		{
			if(!BASS_SetDevice(n))
			{
				if(BASS_ErrorGetCode() == BASS_ERROR_DEVICE)
					WLOGH_ERRO << L"BASS_SetDevice : set device failed device is invalid";
				else if(BASS_ErrorGetCode() == BASS_ERROR_INIT)
					WLOGH_ERRO << L"BASS_SetDevice : set device failed The device has not been initialized";
			}
			else
			{
				finish = true;
				break;
			}
			n++;
		} while (n <= count);
		
		if(!finish)
		{
			WLOGH_ERRO << L"all device can't be used";
			return false;
		}
	}

	if (!BASS_Init(-1,44100,0,hWnd,NULL)) 
	{
		WLOGH_ERRO <<"Bass Init failed,Can't initialize device";
		switch(BASS_ErrorGetCode())
		{
		case BASS_ERROR_DX:
			{
				WLOGH_ERRO << L"BASS_ERROR_DX: DirectX (or ALSA on Linux or OpenSL ES on Android) is not installed.";
				break;
			}
		case BASS_ERROR_DEVICE: 
			{
				WLOGH_ERRO <<L"BASS_ERROR_DEVICE:device is invalid.";
				break;
			}
		case BASS_ERROR_ALREADY:
			{
				WLOGH_ERRO <<L"BASS_ERROR_ALREADY:The device has already been initialized. BASS_Free must be called before it can be initialized again."; 
				break;
			}
		case BASS_ERROR_DRIVER:
			{
				WLOGH_ERRO <<L"BASS_ERROR_DRIVER:There is no available device driver. The device may already be in use.";
				break;
			}
		case BASS_ERROR_FORMAT: 
			{
				WLOGH_ERRO <<L"BASS_ERROR_FORMAT:The specified format is not supported by the device. Try changing the freq and flags parameters.";
				break;
			}
		case BASS_ERROR_MEM:
			{
				WLOGH_ERRO <<L"BASS_ERROR_MEM:There is insufficient memory.";
				break;
			}
		case BASS_ERROR_NO3D:
			{
				WLOGH_ERRO <<L"BASS_ERROR_NO3D:Could not initialize 3D support.";
				break;
			}
		case BASS_ERROR_UNKNOWN:
			{
				WLOGH_ERRO <<L"BASS_ERROR_UNKNOWN:Some other mystery problem!";
				break;
			}
		}
		return false;
	}

	m_bInit = true;
	return m_bInit;
}


bool CGLSoundMgr::addGLSound(const std::string name,bool bLoop)
{
	if (!m_bInit)
	{
		return false;
	}
	MAPSTRGLSOUND::iterator i;
	i = m_mapSound.find(name);
	if (i!=m_mapSound.end())//已经在列表中
	{
		WLOGH_INFO <<"sound had in the map";
		return false;
	}
	CGLSound* pSound = new CGLSound;

	std::string sound_file = m_sound_dir + "\\" + name + ".mp3";
	if (pSound && pSound->soundLoad(sound_file.c_str(),bLoop))
	{
		m_mapSound.insert(std::pair<std::string,CGLSound*>(name,pSound));
		return true;
	}
	else
	{
		WLOGH_ERRO <<"load sound file failed!";
		SAFE_DELETE(pSound);
		return false;
	}
}
bool CGLSoundMgr::addGLSound(const std::string name,CGLSound* psound)
{
	MAPSTRGLSOUND::iterator i;
	i = m_mapSound.find(name);
	if (i!=m_mapSound.end())
	{
		SAFE_DELETE(i->second);
		m_mapSound.erase(i);
	}
	m_mapSound.insert(std::pair<std::string,CGLSound*>(name,psound));
	return true;
}

void CGLSoundMgr::releaseSound()
{
	MAPSTRGLSOUND::iterator i;
	for (i=m_mapSound.begin();i!=m_mapSound.end();i++)
	{
		if (i->second)
		{
			SAFE_DELETE(i->second);
		}
	}
	m_mapSound.clear();
}

CGLSound* CGLSoundMgr::getGLSound(const std::string name)
{
	MAPSTRGLSOUND::iterator i;
	i = m_mapSound.find(name);
	if (i!=m_mapSound.end())
	{
		return (i->second);
	}
	return NULL;
}

void CGLSoundMgr::stopAll()
{
	MAPSTRGLSOUND::iterator i;
	for (i=m_mapSound.begin();i!=m_mapSound.end();i++)
	{
		if (i->second)
			(i->second)->soundStop();
	}
}

bool CGLSoundMgr::initAllSound()
{
	bool b = addGLSound("bgm",true);
	b &= addGLSound("particle",true);
	b &= addGLSound("upgrade",false);
	b &= addGLSound("star",true);
	b &= addGLSound("clapping1",false);
	b &= addGLSound("clapping2",false);
	b &= addGLSound("clapping3",false);
	b &= addGLSound("firewarning",true);
	b &= addGLSound("cheer",false);
	b &= addGLSound("applaud",false);
	b &= addGLSound("goldfall",true);

	return b;
}

bool	CGLSoundMgr::soundSetGStreamVol(DWORD dwV)
{
	return !! BASS_SetConfig(BASS_CONFIG_GVOL_STREAM,dwV);
}
