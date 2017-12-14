#include "StdAfx.h"
#include "ShareMemoryThreadMgr.h"

#include "../OpenGLWindow.h"
extern HWND			g_hWnd;

CShareMemoryThreadMgr::CShareMemoryThreadMgr(void)
: m_bC2PShareMemory( false )
, m_bP2CShareMemory( false )
,m_bSmVideo(false)
, m_hC2PEvent( NULL )
, m_hP2CEvent( NULL )
{

}

CShareMemoryThreadMgr::~CShareMemoryThreadMgr(void)
{
	SAFE_CLOSEHANDLE(m_hC2PEvent);
	SAFE_CLOSEHANDLE(m_hP2CEvent);
}

CShareMemoryThreadMgr & CShareMemoryThreadMgr::getSingleton()
{
	static CShareMemoryThreadMgr instance;
	return instance;
}

//----------------------------------------------------------------------------
// 消息处理
//void CShareMemoryThreadMgr::ProcessMessage( int id, void * wParam, void * lParam )
void CShareMemoryThreadMgr::ProcessMessage(int id,void* pData)
{
	CSafePointer safepointer(pData);
	switch ( id )
	{
	case MSG_OPENSHAREMEMORY:
		{
			SingleString * lpszName = (SingleString*)pData;

			std::string strC2PName( C2P );
			strC2PName += lpszName->str;
			std::string strP2CName( P2C );
			strP2CName += lpszName->str;

			//HANDLE hEvent = ::OpenEvent(EVENT_ALL_ACCESS,FALSE,"GL");
			m_hC2PEvent = ::OpenEventA( EVENT_ALL_ACCESS, FALSE, strC2PName.c_str() );
			m_hP2CEvent = ::OpenEventA( EVENT_ALL_ACCESS, FALSE, strP2CName.c_str() );

			m_bC2PShareMemory = m_C2PShareMemory.Open( strC2PName.c_str() );
			m_bP2CShareMemory = m_P2CShareMemory.Open( strP2CName.c_str() );
			m_bSmVideo = m_smVideo.Open(VIDEO);
			LOGH_INFO << "MSG_OPENSHAREMEMORY: C2S[" << (m_bC2PShareMemory ? "TRUE" : "FALSE") <<"] S2C["<<(m_bP2CShareMemory ? "TRUE" : "FALSE")<<"].\n"; 
			LOGH_INFO << "VIDEO[" << (m_bSmVideo?"TRUE":"FALSE") <<"]\n";
			break;	
		}
	case MSG_NOTIFY_POS:
		{
			NotifyPos* pNP = (NotifyPos*)pData;
			if(m_bP2CShareMemory)
			{
				pNP->cmd = CMD_NOTIFYPOS_FROM_PLAYER;
				if (!m_P2CShareMemory.WriteStruct(pNP))
				{
					WLOGH_ERRO << L"CMD_NOTIFY_WND_POS param write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_ERRORLOG:
		{
			SingleWString* pSS = (SingleWString*)pData;
			if(m_bP2CShareMemory)
			{
				pSS->cmd = CMD_CENTER_ERROR_LOG;
				bool b = m_P2CShareMemory.WriteStruct(pSS);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_CENTER_ERROR_LOG param write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_NEEDRANKINGNUM:
		{
			SingleInt * pInt = (SingleInt*)pData;
			if (m_bP2CShareMemory)
			{
				pInt->cmd = CMD_CENTER_RANKING_NUM;
				bool b = m_P2CShareMemory.WriteStruct(pInt);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_RANKING_NUM wparam write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_DISABLEB:
		{
			if (m_bP2CShareMemory)
			{
				bool b = m_P2CShareMemory.WriteInt(CMD_CENTER_DISABLE_BUTTON);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPALYER_DISABLE_BUTTON write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_ENABLEB:
		{
			if (m_bP2CShareMemory)
			{
				bool b = m_P2CShareMemory.WriteInt(CMD_CENTER_ENABLE_BUTTON);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPLAYER_ENABLE_BUTTON write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_TONEVALUE:
		{
			SingleInt * pInt = (SingleInt*)pData;
			if (m_bP2CShareMemory)
			{
				pInt->cmd = CMD_CENTER_TONE_CURRENT;
				bool b = m_P2CShareMemory.WriteStruct(pInt);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPLAYER_TONE_CURRENT wparam write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_MICVOLUME:
		{
			SingleInt * pInt = (SingleInt*)pData;
			if (m_bP2CShareMemory)
			{
				pInt->cmd = CMD_CENTER_MICVOLUME_CURRENT;
				bool b = m_P2CShareMemory.WriteStruct(pInt);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPLAYER_MICVOLUME_CURRENT wparam write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_MUSICVOLUME:
		{
			SingleInt * pInt = (SingleInt*)pData;
			if (m_bP2CShareMemory)
			{
				pInt->cmd = CMD_CENTER_MUSICVOLUME_CURRENT;
				bool b = m_P2CShareMemory.WriteStruct(pInt);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPLAYER_MUSICVOLUME_CURRENT wparam write failed";
					goto fail;
				}
				informServer();
			}
			break;
		}
	case MSG_SCORESTARNUM:
		{
			DoubleInt* pdInt = (DoubleInt*)pData;
			if (m_bP2CShareMemory)
			{
				pdInt->cmd = CMD_CENTER_SCORE_STARNUM;
				bool b = m_P2CShareMemory.WriteStruct(pdInt);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPLAYER_SCORE_STARNUM struct write failed";
					goto fail;
				}
				informServer();//通知另一个进程
			}
			break;
		}
	case MSG_ENDSONG:
		{
			if (m_bP2CShareMemory)
			{
				bool b =m_P2CShareMemory.WriteInt(CMD_CENTER_ENDSONG);
				if (!b)
				{
					WLOGH_ERRO << L"CMD_DXPLAYER_ENDSONG write failed";
					goto fail;
				}
				WLOGH_INFO << L"inform the center end of song";
				informServer();//通知另一个进程
			}
			break;
		}
	}

fail:
	return ;
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

	if ( !m_bC2PShareMemory || !cls_gl )
	{
		Sleep( 50 );
		return;
	}

	if ( WAIT_OBJECT_0 != WaitForSingleObject( m_hC2PEvent, 50 ) )
	{
		return;
	}

	int id = 0;
	while ( m_C2PShareMemory.ReadInt( id ) )
	{
		switch(id)
		{		
		case CMD_DXPLAYER_SCAN:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SCAN";
				DoubleWString* pDS = new DoubleWString;
				if(pDS&&m_C2PShareMemory.ReadStruct(pDS))
					cls_gl->PostMessageOS(MSGGL_SCAN,pDS);
				else
					SAFE_DELETE(pDS);
				break;
			}
		case CMD_DXPLAYER_PERISTATUS:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_PERISTATUS";
				SingleInt* pSI = new SingleInt;
				if(pSI&&m_C2PShareMemory.ReadStruct(pSI))
					cls_gl->PostMessageOS(MSGGL_PERISTATUS,pSI);
				else
					SAFE_DELETE(pSI);
				break;
			}
		case CMD_NOTIFY_STUPDATE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_STUPDATE";
				STUpdate* pST = new STUpdate;
				if(pST&&m_C2PShareMemory.ReadStruct(pST))
					cls_gl->PostMessageOS(MSGGL_STUPDATE,pST);
				else
					SAFE_DELETE(pST);
				break;
			}
		case CMD_NOTIFY_SHOPTOP:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SHOPTOP";
				ShopTopTip* pST = new ShopTopTip;
				if(pST&&m_C2PShareMemory.ReadStruct(pST))
					cls_gl->PostMessageOS(MSGGL_SHOPTOP,pST);
				else
					SAFE_DELETE(pST);
				break;
			}
		case CMD_NOTIFY_GIFTSEND:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_GIFTSEND";
				GiftSend* pGS = new GiftSend;
				if(pGS&&m_C2PShareMemory.ReadStruct(pGS))
					cls_gl->PostMessageOS(MSGGL_GIFTSEND,pGS);
				else
					SAFE_DELETE(pGS);
				break;
			}
		case CMD_NOTIFY_GIFTEXCHANGE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_GIFTEXCHANGE";
				GiftExchange* pGE = new GiftExchange;
				if(pGE&&m_C2PShareMemory.ReadStruct(pGE))
					cls_gl->PostMessageOS(MSGGL_GIFTEXCHANGE,pGE);
				else
					SAFE_DELETE(pGE);
				break;
			}
		case CMD_NOTIFY_OVERNINETY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_OVERNINETY";
				OverNinety* pO = new OverNinety;
				if (pO && m_C2PShareMemory.ReadStruct(pO))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_OVERNINITY,(void*)pO);
				else
					SAFE_DELETE(pO);
				break;
			}
		case CMD_DXPLAYER_RANKINGLIST:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_RANKINGLIST";
				RankingInfo* pRI = new RankingInfo;
				ZeroMemory(pRI,sizeof(RankingInfo));
				if (pRI && m_C2PShareMemory.ReadStruct(pRI))
					cls_gl->PostMessageOS(MSGGL_RANKINGLIST,(void*)pRI);
				else
					SAFE_DELETE(pRI);
				break;
			}
		case CMD_DXPLAYER_INITOPEN:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_INITOPEN";
				m_C2PShareMemory.ReadInt(id,false);
				cls_gl->PostMessageOS(MSGGL_INITOPEN,NULL);
				break;
			}
		case CMD_DXPLAYER_UPDATEMONEY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_UPDATEMONEY";
				SingleInt* pSi = new SingleInt;
				if(pSi&&m_C2PShareMemory.ReadStruct(pSi))
					cls_gl->PostMessageOS(MSGGL_UPDATEMONEY,(void*)pSi);
				else
					SAFE_DELETE(pSi);
				break;
				break;
			}
		case CMD_DXPLAYER_SELECTSTATE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SELECTSTATE";
				SingleInt* pSi = new SingleInt;
				if(pSi&&m_C2PShareMemory.ReadStruct(pSi))
					cls_gl->PostMessageOS(MSGGL_SELECTSTATE,(void*)pSi);
				else
					SAFE_DELETE(pSi);
				break;
			}
		case CMD_DXPLAYER_CATEGORY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_CATEGORY";
				SingleString* pSs = new SingleString;
				if(pSs && m_C2PShareMemory.ReadStruct(pSs))
					cls_gl->PostMessageOS(MSGGL_CATEGORY,(void*)pSs);
				else
					SAFE_DELETE(pSs);
				break;
			}
		case CMD_DXPLAYER_PROGRESS:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_PROGRESS";
				SingleInt* pSI = new SingleInt;
				if(pSI && m_C2PShareMemory.ReadStruct(pSI))
					cls_gl->PostMessageOS(MSGGL_PROGRESS,(void*)pSI);
				else
					SAFE_DELETE(pSI);
				break;
			}
		case CMD_DXPLAYER_OTHERGRADE:
			{
				static unsigned int nOtherCount = 0;
				nOtherCount ++;
				if(nOtherCount%100 == 0)//控制日志数量
					WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_OTHERGRADE 100 times";
				OtherGrade* pOG = new OtherGrade;
				if(pOG&&m_C2PShareMemory.ReadStruct(pOG))
					cls_gl->PostMessageOS(MSGGL_OTHERGRADE,(void*)pOG);
				else
					SAFE_DELETE(pOG);
				break;
			}
		case CMD_DXPLAYER_VIDEO_INFO:
			{
				static unsigned int nVideoCount = 0;
				if(nVideoCount%100 == 0)//控制日志数量
					WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_VIDEO_INFO 50 times";
				nVideoCount ++;

				VideoInfo* pVI = new VideoInfo;
				ZeroMemory(pVI,sizeof(VideoInfo));
				if (pVI && m_C2PShareMemory.ReadStruct(pVI))
				{
					if (pVI->size)
					{
						unsigned char* buffer = new unsigned char[pVI->size];
						if (!buffer)
						{
							WLOGH_ERRO << L"memory new failed";
							SAFE_DELETE(pVI);
							break;
						}
						if (!m_smVideo.readVideo(pVI->type,buffer,pVI->size))
						{
							WLOGH_ERRO << L"video memory read failed";
							SAFE_DELETE(buffer);
							SAFE_DELETE(pVI);
							break;
						}
						pVI->buffer = buffer;
					}

					//LOG_IFN2(pVI->width,pVI->height);
					cls_gl->PostMessageOS(MSGGL_VIDEO_INFO,(void*)pVI);
				}
				else
				{
					SAFE_DELETE(pVI);
				}
				break;
			}
		case CMD_DXPLAYER_ONLINE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_ONLINE";
				SingleInt* pInt = new SingleInt;
				if (pInt && m_C2PShareMemory.ReadStruct(pInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_ONLINE,(void*)pInt);
				else
					SAFE_DELETE(pInt);
				break;
			}
		case CMD_DXPLAYER_EFFECTNOTIFY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_EFFECTNOTIFY";
				SingleInt* pInt = new SingleInt;
				if(pInt&&m_C2PShareMemory.ReadStruct(pInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_EFFECTNOTIFY,(void*)pInt);
				else
					SAFE_DELETE(pInt);
				break;
			}
		case CMD_DXPLAYER_ATMOSPHERE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_ATMOSPHERE";
				SingleInt* pInt = new SingleInt;
				if (pInt && m_C2PShareMemory.ReadStruct(pInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_ATMOSPHERE,(void*)pInt);
				else
					SAFE_DELETE(pInt);
				break;
			}
		case CMD_DXPLAYER_FIREWARNING:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_FIREWARNING";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_FIREWARNING,0);
				break;
			}
		case CMD_DXPLAYER_NOFIREWARNING:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_NOFIREWARNING";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_NOFIREWARNING,0);
				break;
			}
		case CMD_DXPLAYER_MESSAGE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_MESSAGE";
				PMessage* pStr = new PMessage;
				if (pStr && m_C2PShareMemory.ReadStruct(pStr))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_MESSAGE,(void*)pStr);
				else
					SAFE_DELETE(pStr);
				break;
			}
		case CMD_DXPLAYER_MUSIC_RANGE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_MUSIC_RANGE";
				DoubleInt* pdInt = new DoubleInt;
				if (pdInt && m_C2PShareMemory.ReadStruct(pdInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_MUSIC_RANGE,(void*)pdInt);
				else
					SAFE_DELETE(pdInt);
				break;
			}
		case CMD_DXPLAYER_MIC_RANGE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_MIC_RANGE";
				DoubleInt* pdInt = new DoubleInt;
				if (pdInt && m_C2PShareMemory.ReadStruct(pdInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_MIC_RANGE,(void*)pdInt);
				else
					SAFE_DELETE(pdInt);
				break;
			}
		case CMD_DXPLAYER_TONE_RANGE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_TONE_RANGE";
				DoubleInt* pdInt = new DoubleInt;
				if (pdInt && m_C2PShareMemory.ReadStruct(pdInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_TONE_RANGE,(void*)pdInt);
				else
					SAFE_DELETE(pdInt);
				break;
			}
		case CMD_DXPLAYER_CLOSEHY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_CLOSEHY";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_OPENHY,0);
				break;
			}
		case CMD_DXPLAYER_OPENHY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_OPENHY";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_CLOSEHY,0);
				break;
			}
		case CMD_DXPLAYER_NEXT_SONGNAME:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_NEXT_SONGNAME";
				SingleWString* pStr = new SingleWString;
				if (m_C2PShareMemory.ReadStruct(pStr))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_NEXT_SONGNAME,(void*)pStr);
				else
					SAFE_DELETE(pStr);
				break;
			}
		case CMD_DXPLAYER_GETMICVOLUME:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_GETMICVOLUME";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_GETMICVOLUME,0);
				break;
			}
		case CMD_DXPLAYER_SETMICVOLUME:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SETMICVOLUME";
				SingleInt* pInt = new SingleInt;
				if (pInt && m_C2PShareMemory.ReadStruct(pInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_SETMICVOLUME,(void*)pInt);
				else
					SAFE_DELETE(pInt);
				break;
			}
		case CMD_DXPLAYER_GETTONEVALUE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_GETTONEVALUE";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_GETTONEVALUE,0);
				break;
			}
		case CMD_DXPLAYER_SUBTONEVALUE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SUBTONEVALUE";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_SUBTONEVALUE,0);
				break;
			}
		case CMD_DXPLAYER_ADDTONEVALUE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_ADDTONEVALUE";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_ADDTONEVALUE,0);
				break;
			}
		case CMD_DXPLAYER_GETMUSICVOLUME:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_GETMUSICVOLUME";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_GETMUSICVOLUME,0);
				break;
			}
		case CMD_DXPLAYER_MUTE_CLOSE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_MUTE_CLOSE";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_MUTE_CLOSE,0);
				break;
			}
		case CMD_DXPLAYER_MUTE_OPEN:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_MUTE_OPEN";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_MUTE_OPEN,0);
				break;
			}
		case CMD_DXPLAYER_SETMUSICVOLUME:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SETMUSICVOLUME";
				SingleInt* pInt = new SingleInt;
				if (pInt && m_C2PShareMemory.ReadStruct(pInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_SETMUSICVOLUME,(void*)pInt);
				else
					SAFE_DELETE(pInt);
				break;
			}
		case CMD_DXPLAYER_SONGSTARTEND:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SONGSTARTEND";
				DoubleInt* pdInt = new DoubleInt;
				if (pdInt && m_C2PShareMemory.ReadStruct(pdInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_SONGSTARTEND,(void*)pdInt);
				else
					SAFE_DELETE(pdInt);
				break;
			}
		case CMD_DXPLAYER_CLOSE://程序退出
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_CLOSE";
				m_C2PShareMemory.ReadInt( id,false );
				::PostMessage( g_hWnd, WM_CLOSE, 0, 0 );
				break;
			}
		case CMD_DXPLAYER_SWITCHSONG://切换歌曲
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SWITCHSONG";
				SwitchInfo* pStrInt = new SwitchInfo;
				if (pStrInt && m_C2PShareMemory.ReadStruct(pStrInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_SWITCHSONG,(void*)pStrInt);
				else
					SAFE_DELETE(pStrInt);
				break;
			}
		case CMD_DXPLAYER_CONTROL_PAUSE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_CONTROL_PAUSE";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_CONTROL_PAUSE,0);
				break;
			}
			//播放，或暂停，第一次为暂停
		case  CMD_DXPLAYER_CONTROL_RESUME:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_CONTROL_RESUME";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_CONTROL_RESUME,0);
				break;
			}
			//切换评分状态，
		case CMD_DXPLAYER_GRADETYPE:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_GRADETYPE";
				SingleInt* pInt = new SingleInt;
				if (pInt && m_C2PShareMemory.ReadStruct(pInt))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_GRADE,(void*)pInt);
				else
					SAFE_DELETE(pInt);
				break;
			}
			//切换伴奏状态，开启/关闭
		case CMD_DXPLAYER_ACCOMPANY:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_ACCOMPANY";
				m_C2PShareMemory.ReadInt( id,false );
				cls_gl->PostMessageOS(MSGGL_DXPLAYER_ACCOMPANY,0);
				break;
			}
		case CMD_DXPLAYER_SINGERPIC:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SINGERPIC";
				SingleWString* pStr = new SingleWString;
				if (pStr && m_C2PShareMemory.ReadStruct(pStr))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_SINGERPIC,(void*)pStr);
				else
					SAFE_DELETE(pStr);
				break;
			}
		case CMD_DXPLAYER_SONGINFO:
			{
				WLOGH_INFO << L"player read cmd : CMD_DXPLAYER_SONGINFO";
				ThreeWString* pdStr = new ThreeWString;
				if (pdStr && m_C2PShareMemory.ReadStruct(pdStr))
					cls_gl->PostMessageOS(MSGGL_DXPLAYER_SONGINFO,(void*)pdStr);
				else
					SAFE_DELETE(pdStr);
				break;
			}
		default:
			{
				OutputDebugStringA("player: unknow cmd\n");
			}
		}
	}
}
