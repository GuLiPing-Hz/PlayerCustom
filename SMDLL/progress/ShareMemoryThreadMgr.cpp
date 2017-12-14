#include "windows.h"
#include "ShareMemoryThreadMgr.h"
#include <processcommunicate.h>
#include "../SMHelp.h"
#include <assert.h>

#define FALSE_BREAK(b) \
if (!(b))\
{\
	assert(false);\
	break;\
}

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(h)         \
	if ((h) != NULL)                  \
{                               \
	CloseHandle((h));             \
	(h) = NULL;                   \
}
#endif

CShareMemoryThreadMgr::CShareMemoryThreadMgr(void)
: m_bC2PShareMemory( false )
, m_bP2CShareMemory( false )
, m_bSmVideo(false)
, m_hC2PEvent( NULL )
, m_hP2CEvent( NULL )
{
	ZeroMemory(&m_procinfoPlayer,sizeof(m_procinfoPlayer));
	ZeroMemory(&m_procinfoNotify,sizeof(m_procinfoNotify));
}

CShareMemoryThreadMgr::~CShareMemoryThreadMgr(void)
{
	SAFE_CLOSEHANDLE(m_hC2PEvent);
	SAFE_CLOSEHANDLE(m_hP2CEvent);
	SAFE_CLOSEHANDLE(m_procinfoPlayer.hProcess);
	SAFE_CLOSEHANDLE(m_procinfoPlayer.hThread);
	SAFE_CLOSEHANDLE(m_procinfoNotify.hProcess);
	SAFE_CLOSEHANDLE(m_procinfoNotify.hThread);
}

CShareMemoryThreadMgr & CShareMemoryThreadMgr::getSingleton()
{
	static CShareMemoryThreadMgr instance;
	return instance;
}

//----------------------------------------------------------------------------
// 消息处理
void CShareMemoryThreadMgr::ProcessMessage( int id, void * pData )
{
	CSafePointer safe_pointer(pData);
	switch ( id )
	{
	case MSG_CREATESHAREMEMORY:
		{
			DoubleString* pdStr = (DoubleString *)pData;
			char * lpszName = pdStr->str1;
			char* exeDir = pdStr->str2;

			std::string strC2PName( C2P );
			strC2PName += lpszName;
			std::string strP2CName( P2C );
			strP2CName += lpszName;
			std::string strC2NName(C2N);
			strC2NName += lpszName;

			m_hC2PEvent = ::CreateEventA( NULL, FALSE, FALSE, strC2PName.c_str() );
			m_hP2CEvent = ::CreateEventA( NULL, FALSE, FALSE, strP2CName.c_str() );
			m_hC2NEvent = ::CreateEventA(NULL, FALSE, FALSE, strC2NName.c_str());

			m_bC2PShareMemory = m_C2PShareMemory.Create( strC2PName.c_str() );
			m_bP2CShareMemory = m_P2CShareMemory.Create( strP2CName.c_str() );
			m_bC2NShareMemory = m_C2NShareMemory.Create(strC2NName.c_str());
			m_bSmVideo = m_smVideo.Create(VIDEO);

			//break;

			//唤起Player
			char exe[260] = {0};
			sprintf_s(exe,"%s\\player.exe",exeDir);
			STARTUPINFO stinfo; //启动窗口的信息 
			ZeroMemory(&stinfo,sizeof(stinfo));
			stinfo.cb = sizeof(stinfo);/*STARTF_FORCEONFEEDBACK*/
			CreateProcessA(NULL,exe,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,exeDir,&stinfo,&m_procinfoPlayer);

			//唤起Notify
			sprintf_s(exe,"%s\\notify\\notify.exe",exeDir);
			ZeroMemory(&stinfo,sizeof(stinfo));
			stinfo.cb = sizeof(stinfo);/*STARTF_FORCEONFEEDBACK*/
			CreateProcessA(NULL,exe,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,exeDir,&stinfo,&m_procinfoNotify);

			//m_procinof 中的句柄需要关闭
			break;
		}
	case MSG_MESSAGE:
		{
			PMessage* pPM = (PMessage *)pData;

			if(pPM->type == 0)
			{
				if (m_bC2PShareMemory)
				{
					pPM->cmd = CMD_DXPLAYER_MESSAGE;
					FALSE_BREAK(m_C2PShareMemory.WriteStruct(pPM));
					informClient();
				}
			}
			else
			{
				if(m_bC2NShareMemory)
				{
					pPM->cmd = CMD_NOTIFY_MESSAGE;
					FALSE_BREAK(m_C2NShareMemory.WriteStruct(pPM));
					informNotify();
				}
			}

			break;
		}
	case MSG_OVERNINETY:
		{
			OverNinety* pO = (OverNinety*)pData;
			pO->cmd = CMD_NOTIFY_OVERNINETY;
			if(m_bC2NShareMemory)
			{
				FALSE_BREAK(m_C2NShareMemory.WriteStruct(pO));
				informNotify();
			}
			break;
		}
	case MSG_STUPDATE:
		{
			STUpdate* pST = (STUpdate*)pData;
			pST->cmd = CMD_NOTIFY_STUPDATE;
			if(m_bC2NShareMemory)
			{
				FALSE_BREAK(m_C2NShareMemory.WriteStruct(pST));
				informNotify();
			}
			break;
		}
	case MSG_SHOPTOP:
		{
			ShopTopTip* pST = (ShopTopTip*)pData;
			pST->cmd = CMD_NOTIFY_SHOPTOP;
			if(m_bC2NShareMemory)
			{
				FALSE_BREAK(m_C2NShareMemory.WriteStruct(pST));
				informNotify();
			}
			break;
		}
	case MSG_GIFTSEND:
		{
			GiftSend* pGS = (GiftSend*)pData;
			pGS->cmd = CMD_NOTIFY_GIFTSEND;
			if(m_bC2NShareMemory)
			{
				FALSE_BREAK(m_C2NShareMemory.WriteStruct(pGS));
				informNotify();
			}
			break;
		}
	case MSG_GIFTEXCHANGE:
		{
			GiftExchange* pGE = (GiftExchange*)pData;
			pGE->cmd = CMD_NOTIFY_GIFTEXCHANGE;
			if(m_bC2NShareMemory)
			{
				FALSE_BREAK(m_C2NShareMemory.WriteStruct(pGE));
				informNotify();
			}
			break;
		}
	case MSG_NOTIFYPOS:
		{
			NotifyPos* pNp = (NotifyPos*)pData;
			pNp->cmd = CMD_NOTIFY_POS;
			if(m_bC2NShareMemory)
			{
				FALSE_BREAK(m_C2NShareMemory.WriteStruct(pNp));
				informNotify();
			}
			break;
		}
	case MSG_SCAN:
		{
			DoubleWString* pDS = (DoubleWString*)pData;
			if(m_bC2PShareMemory)
			{
				pDS->cmd = CMD_DXPLAYER_SCAN;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pDS));
				informClient();
			}
			break;
		}
	case MSG_PERISTATUS:
		{
			SingleInt* pSI = (SingleInt*)pData;
			if(m_bC2PShareMemory)
			{
				pSI->cmd = CMD_DXPLAYER_PERISTATUS;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pSI));
				informClient();
			}
			break;
		}
	case MSG_MICRANG:
		{
			DoubleInt* pDI = (DoubleInt*)pData;
			if(m_bC2PShareMemory)
			{
				pDI->cmd = CMD_DXPLAYER_MIC_RANGE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pDI));
				informClient();
			}
			break;
		}
	case MSG_INITOPEN:
		{
			if(m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_INITOPEN));
				informClient();
			}
			break;
		}
	case MSG_MUSICRANG:
		{
			DoubleInt* pDI = (DoubleInt*)pData;
			if(m_bC2PShareMemory)
			{
				pDI->cmd = CMD_DXPLAYER_MUSIC_RANGE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pDI));
				informClient();
			}
			break;
		}
	case MSG_UPDATEMONEY:
		{
			SingleInt* pSi = (SingleInt*)pData;
			if(m_bC2PShareMemory)
			{
				pSi->cmd = CMD_DXPLAYER_UPDATEMONEY;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pSi));
				informClient();
			}
			break;
		}
	case MSG_SELECTSTATE:
		{
			SingleInt* pSi = (SingleInt*)pData;
			if(m_bC2PShareMemory)
			{
				pSi->cmd = CMD_DXPLAYER_SELECTSTATE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pSi));
				informClient();
			}
			break;
		}
	case MSG_SELECTCATEGORY:
		{
			SingleString* pSs = (SingleString*)pData;
			if(m_bC2PShareMemory)
			{
				pSs->cmd = CMD_DXPLAYER_CATEGORY;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pSs));
				informClient();
			}
			break;
		}
	case MSG_PROGRESS:
		{
			SingleInt* pSI = (SingleInt*)pData;
			if(m_bC2PShareMemory)
			{
				pSI->cmd = CMD_DXPLAYER_PROGRESS;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pSI));
				informClient();
			}
			break;
		}
	case MSG_OTHERGRADE:
		{
			OtherGrade* pOG = (OtherGrade*)pData;
			if(m_bC2PShareMemory)
			{
				pOG->cmd = CMD_DXPLAYER_OTHERGRADE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pOG));
				informClient();
			}
			break;
		}
	case MSG_RANKING:
		{
			RankingInfo* pRI = (RankingInfo*)pData;
			if (m_bC2PShareMemory)
			{
				pRI->cmd = CMD_DXPLAYER_RANKINGLIST;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pRI));
				informClient();
			}
			break;
		}
	case MSG_VIDEOINFO:
		{
			//OutputDebugStringA("write video info\n");
			VideoInfo* pVI = (VideoInfo*)pData;
			if (m_bC2PShareMemory)
			{
				char buf[260] = {0};
				sprintf(buf,"width: %d,height: %d \n",pVI->width,pVI->height);
				OutputDebugStringA(buf);
				pVI->cmd = CMD_DXPLAYER_VIDEO_INFO;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pVI));
				informClient();
			}
			break;
		}
	case MSG_ONLINE:
		{
			SingleInt* pInt = (SingleInt*)pData;
			if (m_bC2PShareMemory)
			{
				pInt->cmd = CMD_DXPLAYER_ONLINE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pInt));
				informClient();
			}
			break;
		}
	case MSG_EFFECTNOTIFY:
		{
			SingleInt* pInt = (SingleInt *)pData;
			if (m_bC2PShareMemory)
			{
				pInt->cmd = CMD_DXPLAYER_EFFECTNOTIFY;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pInt));
				informClient();
			}
			break;
		}
	case MSG_ATOMOSPHERE:
		{
			SingleInt* pInt = (SingleInt *)pData;
			if (m_bC2PShareMemory)
			{
				pInt->cmd = CMD_DXPLAYER_ATMOSPHERE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pInt));
				informClient();
			}
			break;
		}
	case MSG_NEXTSONG:
		{
			SingleWString* pStr =  (SingleWString *)pData;
			if (m_bC2PShareMemory)
			{
				pStr->cmd = CMD_DXPLAYER_NEXT_SONGNAME;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pStr));
				informClient();
			}
			break;
		}
	case MSG_NOFIREWARNING:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_NOFIREWARNING));
				informClient();
			}
			break;
		}
	case MSG_FIREWARNING:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_FIREWARNING));
				informClient();
			}
			break;
		}
	case MSG_MUTE_OPEN:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_MUTE_OPEN));
				informClient();
			}
			break;
		}
	case MSG_MUTE_CLOSE:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_MUTE_CLOSE));
				informClient();
			}
			break;
		}
	case MSG_SET_SUB_TONE:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_SUBTONEVALUE));
				informClient();
			}
			break;
		}
	case MSG_SET_ADD_TONE:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_ADDTONEVALUE));
				informClient();
			}
			break;
		}
	case MSG_GET_TONE:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_GETTONEVALUE));
				informClient();
			}
			break;
		}
	case MSG_SET_MIC:
		{
			SingleInt *pInt = (SingleInt *)pData;
			if (m_bC2PShareMemory)
			{
				pInt->cmd = CMD_DXPLAYER_SETMICVOLUME;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pInt));
				informClient();
			}
			break;
		}
	case MSG_SET_VOLUME:
		{
			SingleInt *pInt = (SingleInt *)pData;
			if (m_bC2PShareMemory)
			{
				pInt->cmd = CMD_DXPLAYER_SETMUSICVOLUME;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pInt));
				informClient();
			}
			break;
		}
	case MSG_SWITCH_SONG:
		{
			SwitchInfo* pStrInt = (SwitchInfo*)pData;
			if (m_bC2PShareMemory)
			{
				pStrInt->cmd = CMD_DXPLAYER_SWITCHSONG;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pStrInt));
				informClient();//通知另一个进程
			}
			break;
		}
	case MSG_PLAY_GRADE:
		{
			SingleInt* pInt = (SingleInt*)pData;
			if (m_bC2PShareMemory)
			{
				pInt->cmd = CMD_DXPLAYER_GRADETYPE;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pInt));
				informClient();
			}
			break;
		}
	case MSG_PLAY_CONTROL_PAUSE:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_CONTROL_PAUSE));
				informClient();
			}
			break;
		}
	case MSG_PLAY_CONTROL_RESUME:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_CONTROL_RESUME));
				informClient();
			}
			break;
		}
	case MSG_PLAY_ACCOMPANY:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_ACCOMPANY));
				informClient();
			}
			break;
		}
	case MSG_DXPLAYER_CLOSE:
		{
			if (m_bC2PShareMemory)
			{
				FALSE_BREAK(m_C2PShareMemory.WriteInt(CMD_DXPLAYER_CLOSE));
				informClient();
			}
			break;
		}
	case MSG_SONG_STARTEND:
		{
			DoubleInt* pdInt = (DoubleInt*)pData;
			if (m_bC2PShareMemory)
			{
				pdInt->cmd = CMD_DXPLAYER_SONGSTARTEND;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pdInt));
				informClient();
			}
			break;
		}
	case MSG_SONG_INFO:
		{
			ThreeWString* pdStr = (ThreeWString*)pData;
			if (m_bC2PShareMemory)
			{
				pdStr->cmd = CMD_DXPLAYER_SONGINFO;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pdStr));
				informClient();
			}
			break;
		}
	case MSG_SINGER_PIC:
		{
			SingleWString* pStr = (SingleWString*)pData;
			if (m_bC2PShareMemory)
			{
				pStr->cmd = CMD_DXPLAYER_SINGERPIC;
				FALSE_BREAK(m_C2PShareMemory.WriteStruct(pStr));
				informClient();
			}
			break;
		}
	}
}

//----------------------------------------------------------------------------
// 空闲处理
void CShareMemoryThreadMgr::ProcessIdle()
{
	ReadShareMemory();
}

//----------------------------------------------------------------------------
// 读取ShareMemory
void CShareMemoryThreadMgr::ReadShareMemory()
{
	static char szReadBuffer[102400] = { 0 };
	static char szWriteBuffer[102400] = { 0 };

	if ( !m_bP2CShareMemory /*|| !g_hWnd */)
	{
		Sleep( 50 );
		return;
	}

	if ( WAIT_OBJECT_0 != WaitForSingleObject( m_hP2CEvent, 50 ) )
	{
		return;
	}

	int id = 0;
	while ( m_P2CShareMemory.ReadInt( id ) )
	{
		switch(id)
		{
		case CMD_NOTIFYPOS_FROM_PLAYER:
			{
				NotifyPos* pNP = new NotifyPos;
				if(m_P2CShareMemory.ReadStruct(pNP))
				{
					PostMessage(MSG_NOTIFYPOS,pNP);
				}
				break;
			}
		case CMD_CENTER_ERROR_LOG:
			{
				SingleWString ss;
				if(m_P2CShareMemory.ReadStruct(&ss))
				{
					if (CSMHelp::m_pDealMessage)
						(CSMHelp::m_pDealMessage)(CMD_CENTER_ERROR_LOG,-1,-1,(void*)ss.str);
				}
			}
		case CMD_CENTER_DISABLE_BUTTON:
			{
				if(m_P2CShareMemory.ReadInt(id,false))
				{
					if (CSMHelp::m_pDealMessage)
						(CSMHelp::m_pDealMessage)(CMD_CENTER_DISABLE_BUTTON,-1,-1,NULL);
				}
				break;
			}
		case CMD_CENTER_RANKING_NUM:
			{
				int nNum = -1;
				m_P2CShareMemory.ReadInt( id ,false);
				if (m_P2CShareMemory.ReadInt(nNum,false))
				{
					if (CSMHelp::m_pDealMessage)
						(CSMHelp::m_pDealMessage)(CMD_CENTER_RANKING_NUM,nNum,-1,NULL);
				}
				break;
			}
		case CMD_CENTER_ENDSONG:
			{
				m_P2CShareMemory.ReadInt( id ,false);
				if (CSMHelp::m_pDealMessage)
				{
					(CSMHelp::m_pDealMessage)(CMD_CENTER_ENDSONG,-1,-1,NULL);
				}
				break;
			}
		case CMD_CENTER_SCORE_STARNUM:
			{
				int nScore = -1;
				int nStarNum = -1;
				m_P2CShareMemory.ReadInt( id ,false);
				if (m_P2CShareMemory.ReadInt(nScore,false))
				{
					if (m_P2CShareMemory.ReadInt(nStarNum,false))
					{
						if (CSMHelp::m_pDealMessage)
							(CSMHelp::m_pDealMessage)(CMD_CENTER_SCORE_STARNUM,nScore,nStarNum,NULL);
					}
				}
				break;
			}
		case CMD_CENTER_MUSICVOLUME_CURRENT:
			{
				int nInt;
				m_P2CShareMemory.ReadInt( id ,false);
				if (m_P2CShareMemory.ReadInt(nInt,false))
				{
					if (CSMHelp::m_pDealMessage)
						(CSMHelp::m_pDealMessage)(CMD_CENTER_MUSICVOLUME_CURRENT,nInt,-1,NULL);
				}
				break;
			}
		case CMD_CENTER_MICVOLUME_CURRENT:
			{
				int nInt;
				m_P2CShareMemory.ReadInt( id ,false);
				if (m_P2CShareMemory.ReadInt(nInt,false))
				{
					if (CSMHelp::m_pDealMessage)
						(CSMHelp::m_pDealMessage)(CMD_CENTER_MICVOLUME_CURRENT,nInt,-1,NULL);
				}
				break;
			}
		case CMD_CENTER_TONE_CURRENT:
			{
				int nInt;
				m_P2CShareMemory.ReadInt( id ,false);
				if (m_P2CShareMemory.ReadInt(nInt,false))
				{
					if (CSMHelp::m_pDealMessage)
						(CSMHelp::m_pDealMessage)(CMD_CENTER_TONE_CURRENT,nInt,-1,NULL);
				}
				break;
			}
		case CMD_CENTER_ENABLE_BUTTON:
			{
				m_P2CShareMemory.ReadInt( id ,false);
				if (CSMHelp::m_pDealMessage)
					(CSMHelp::m_pDealMessage)(CMD_CENTER_ENABLE_BUTTON,-1,-1,NULL);
				break;
			}
		default:
			{
				m_P2CShareMemory.ReadInt( id ,false);
				char buf[260] = {0};
				sprintf(buf,"ReadShareMemory ERROR id = %d\n",id);
				OutputDebugStringA(buf);
			}
		}
	}
}
