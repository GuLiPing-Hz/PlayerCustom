
#include "stdafx.h"
#include "sound.h"

#define WIDTH_TM 600

QWORD g_loop[2];//循环起始点，结束点

void CALLBACK LoopSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(channel,g_loop[0],BASS_POS_BYTE)) // try seeking to loop start
		BASS_ChannelSetPosition(channel,0,BASS_POS_BYTE); // failed, go to start of file instead
}


CGLSound::CGLSound()
//:m_bInit(false)
:m_hsam(NULL)
,m_nchan(0)
{
}

CGLSound::~CGLSound()
{
	if (m_hsam)
	{
		BASS_StreamFree(m_hsam);
	}
	if (m_nchan)
	{
		BASS_StreamFree(m_nchan);
	}
}

bool	CGLSound::soundSetGobalVolume(const float v)
{
	if (v>=0 && v<=1)
	{
		BASS_SetVolume(v);
		return true;
	}
	else
	{
		return false;
	}

}

bool	CGLSound::soundSetChannelVolume(const float v)
{
	if (v>=0 && v<=1)
	{
		if (m_nchan)
		{
			BASS_ChannelSetAttribute(m_nchan,BASS_ATTRIB_MUSIC_VOL_CHAN+1,v);
		}
		if (m_hsam)
		{
			BASS_ChannelSetAttribute(m_hsam,BASS_ATTRIB_MUSIC_VOL_CHAN+1,v);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool	CGLSound::soundLoad(const char* file,bool bloop)
{
	if (bloop)
	{
// 		if (!m_bInit)
// 		{
// 			return false;
// 		}
		if (!(m_nchan=BASS_StreamCreateFile(FALSE,file,0,0,0))
			/*&& !(m_nchan=BASS_MusicLoad(FALSE,file,0,0,BASS_MUSIC_RAMPS|BASS_MUSIC_POSRESET|BASS_MUSIC_PRESCAN,1))*/)
		{
			WLOGH_ERRO << L"music file open error";
			return false;
		}
		m_bpp = (DWORD)BASS_ChannelGetLength(m_nchan,BASS_POS_BYTE)/WIDTH_TM;
		if (m_bpp<BASS_ChannelSeconds2Bytes(m_nchan,0.02))
		{
			m_bpp = (DWORD)BASS_ChannelSeconds2Bytes(m_nchan,0.02);
		}
		BASS_ChannelSetSync(m_nchan,BASS_SYNC_END|BASS_SYNC_MIXTIME,0,LoopSyncProc,0);
	}
	else
	{
		m_hsam = BASS_StreamCreateFile(FALSE,file,0,0,0);
		if (!m_hsam)
			WLOGH_ERRO <<"music file open error";
	}

	return true;
}

bool	CGLSound::soundPlay()
{
// 	if (!m_bInit)
// 	{
// 		return false;
// 	}
	if (m_nchan)
	{
		BASS_ChannelPlay(m_nchan,TRUE);
	}
	if (m_hsam)
	{
		BASS_ChannelPlay(m_hsam,TRUE);
	}
	return true;
}
bool	CGLSound::soundPause()
{
// 	if (!m_bInit)
// 	{
// 		return false;
// 	}
	if (m_nchan)
	{
		BASS_ChannelPause(m_nchan);
	}
	if (m_hsam)
	{
		BASS_ChannelPause(m_hsam);
	}
	return true;
}

bool	CGLSound::soundStop()
{
// 	if (!m_bInit)
// 	{
// 		return false;
// 	}
	if (m_nchan)
	{
		BASS_ChannelStop(m_nchan);
	}
	if (m_hsam)
	{
		BASS_ChannelStop(m_hsam);
	}
	return true;
}

bool	CGLSound::soundSetLoopStart(QWORD pos)
{
	g_loop[0] = pos;
	return true;
}

bool	CGLSound::soundSetLoopEnd(QWORD pos)
{
// 	if (!m_bInit)
// 	{
// 		return false;
// 	}
	g_loop[1] = pos;
	BASS_ChannelRemoveSync(m_nchan,m_lsync);
	m_lsync = BASS_ChannelSetSync(m_nchan,BASS_SYNC_POS|BASS_SYNC_MIXTIME,g_loop[1],LoopSyncProc,0);
	return true;
}
