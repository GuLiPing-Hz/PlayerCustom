#include "StdAfx.h"

#include "OpenGLWindow.h"

#include "resource.h"
#include "Opengl.h"
#include "staff/object/SceneMgr.h"
#include "GLVideo.h"
#include "Florid/FloridMgr.h"
#include "sound/soundmgr.h"
#include "Dialog/ListDlg.h"
#include "DX.h"
#include <assert.h>
#include "staff/render/ScoreFrame.h"
#include "staff/render/OS_NotifyMessage.h"
#include "file/FileUtil.h"
#include "swf/SwfMgr.h"
#include "staff/render/TopNotifyRender.h"
#include "progress/Wmcopydata.h"

#define FILEBUFFER	(1024*1024)

//录音
CWaveUnit*						g_WavRecorder=NULL;
HANDLE								g_hEventEval=NULL;
HANDLE								g_hSemaphoreRender1=NULL;
HANDLE								g_hSemaphoreRender2=NULL;

bool		g_keys[256];			// Array Used For The Keyboard Routine
bool		g_active=TRUE;		// Window Active Flag Set To TRUE By Default
volatile bool	g_bGLThread = true;//控制绘画线程函数
volatile bool	g_bGLThreadStart = false;
volatile bool   g_bGLThreadEnd = true;
CCritSec g_csSingleton;
COpenGLWindow* cls_gl = COpenGLWindow::getWindowSingleton();

extern bool					g_bPboSupported;
extern int					g_pboMode;	
extern CListDialog*	g_listDlg;
extern HANDLE			g_hThread;
extern HWND				g_hWnd;
#ifdef _GWINXP
extern int g_bCountToCreateListener;
extern bool g_bNeedToReleaseListener;
#endif
//////////////////////////////////////////////////////////////////////////


DWORD WINAPI VThread(LPVOID pParam)//绘画线程
{
	COpenGLWindow * pcopenGLWindow = cls_gl;

	CTimerEx * ptimeEx = pcopenGLWindow->getTimerEx();
	if (!ptimeEx)
	{
		return -1;
	}
	ITimerListener* timerListener = ptimeEx->getListener();
	if (!timerListener)
	{
		return -1;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//循环 查看流程特效
	//glp debug
	//char * buf_singerpic = new char[260];
	//strcpy(buf_singerpic,"C:\\Users\\Administrator\\Desktop\\林宥嘉.png");
	//::PostMessage(g_hWnd,WM_PLAY_SINGERPIC,(WPARAM)buf_singerpic,0);
	//::PostMessage(g_hWnd,WM_DEBUG,0,0);
	//::PostMessage(g_hWnd,WM_PLAY_FINALSCORE,0,0);
	ptimeEx->startTimer();
	while(g_bGLThread)
	{
		ptimeEx->processTimer();
	}
	ptimeEx->closeTimer();
	//////////////////////////////////////////////////////////////////////////
	WLOGH_INFO << L"VThread step out while";
	SceneMgr* sceneMgr = pcopenGLWindow->m_pSceneMgr;
	if (sceneMgr)
	{
		sceneMgr->endModel();//结束
	}
	timerListener->endThread();

	WLOGH_INFO << L"VThread Finished";
	return 0;
}

bool COpenGLWindow::createVideoEvalThread()
{
	DWORD threadID;
	if (g_hThread)
	{
		SAFE_CLOSEHANDLE(g_hThread);
	}
	g_hThread = ::CreateThread(NULL,0,VThread,(LPVOID)this,0,&threadID);
	if (g_hThread == NULL)
	{
		return FALSE;
	}
	::SetThreadPriority(g_hThread,THREAD_PRIORITY_HIGHEST);

	m_pDrawListener->createEvalThread();
	return TRUE;
}

void	COpenGLWindow::updateVideo2Texture()
{
	CAutoLock lock(&m_csVideo);

	Imageset* pImgset = NULL;

	ImgsetMgr* pImgsetMgr = getImgsetMgr();
	if(m_sHostVideo.changed)//有更新？
	{
		pImgset = pImgsetMgr->getImageset(L"vHost");
		if (pImgset)
			pImgset->updateTexture(m_sHostVideo.p,m_nVideoSize[0].width,m_nVideoSize[0].height);
		SAFE_DELETE(m_sHostVideo.p);
		m_sHostVideo.changed = false;
	}

	if(m_sSingVideo.changed)
	{
		pImgset = pImgsetMgr->getImageset(L"vSing");
		if (pImgset)
			pImgset->updateTexture(m_sSingVideo.p,m_nVideoSize[1].width,m_nVideoSize[1].height);
		SAFE_DELETE(m_sSingVideo.p);
		m_sSingVideo.changed = false;
	}
}

void COpenGLWindow::ProcessIdle()
{
	CThreadMgr::ProcessIdle();
	
	if(m_vectmonitor.size() <= 1)
	{
		CDXApp::prepareWindow(this,m_winSize);
	}
}

void COpenGLWindow::ProcessMessage(int id,void* pData)
{
	CSafePointer safePointer(pData);
	switch(id)
	{
	case MSGGL_SCAN:
		{
			DoubleWString* pDS = (DoubleWString*) pData;
			if(pDS)
			{
				m_downloadUrl = pDS->str1;
				m_enterRoomUrl = pDS->str2;
			}
			break;
		}
	case MSGGL_PERISTATUS:
		{
			SingleInt* pSI = (SingleInt*)pData;
			if(pSI)
			{
				CTopNotifyRender* pNotify = m_pSceneMgr->getTopNotify();
				if(pNotify)
					pNotify->startNotify(pSI->wparam);
			}
			break;
		}
	case MSGGL_STUPDATE:
		{
			STUpdate* pST = (STUpdate*)pData;
			if(pST)
			{
				static char tail[200];
				if(pST->overed)
					sprintf(tail,"<txt space=\"0.9\" txt=\"分,超过了您,您现在是第\"/>"
					"<space width=\"5\"/><txt space=\"0.9\" color=\"ffffdd10\" txt=\"%d\"/><space width=\"5\"/>"
					"<txt space=\"0.9\" txt=\"名,加油哦!\"/>",pST->rank);
				else
					sprintf(tail,"<txt space=\"0.9\" txt=\"分,当前排行第\"/>"
					"<space width=\"5\"/><txt space=\"0.9\" color=\"ffffdd10\" txt=\"%d\"/><space width=\"5\"/>"
					"<txt space=\"0.9\" txt=\"名,鼓掌!\"/><space width=\"10\"/>",pST->rank);
				static char msg[400];
				sprintf(msg,"<txt space=\"0.9\" txt=\"包厢 %s 演唱《%s》获得\"/>"
					"<space width=\"10\"/><txt space=\"0.9\" color=\"ffffdd10\" txt=\"%0.1f\"/><space width=\"10\"/>%s"
					"<space width=\"10\"/>",pST->room,pST->song,pST->score/10.0f,tail);

				BarrageInfo bi = {0};
				bi.type = 1;
				bi.saytype = 1;//天天门店抢榜
				strcpy(bi.str,msg);
				CAutoLock lock(&m_csBarrage);
				m_listBarrage.push_back(bi);
			}
			break;
		}
	case MSGGL_SHOPTOP:
		{
			ShopTopTip* pST = (ShopTopTip*)pData;
			if(pST)
			{
				if(!pST->top[0].valid)
					break;

				SpecialInfo info = {0};
				sprintf(info.str,"<line><img space=\"20\" src=\"DailyRankingPic\"/><txt txt=\"当前前三甲\"/></line>"
					"<line indentation=\"30\" yoffset=\"20\"><txt color=\"0xffdd10\" space=\"0.8\" size=\"0.5\" txt=\"坚持到%s,前三甲有\"/>"
					"<space width=\"15\"/><txt space=\"0.8\" size=\"0.5\" txt=\"%s\"/><space width=\"10\"/>"
					"<txt color=\"0xffdd10\" space=\"0.8\" size=\"0.5\" txt=\"奖励\"/></line>"
					"<line indentation=\"15\" yoffset=\"20\"><txt size=\"0.7\" txt=\"第一名\"/><space width=\"20\"/><txt size=\"0.7\" txt=\"包厢%s\"/>"
					"<space width=\"30\"/><txt color=\"0x00ffdd10\" size=\"0.7\" txt=\"%0.1f\"/><txt size=\"0.7\" txt=\"分\"/></line>"
					,pST->time,pST->gift,pST->top[0].room,pST->top[0].score/10.0f);
				if(pST->top[1].valid)
				{
					sprintf(info.str,"%s<line indentation=\"15\"><txt size=\"0.7\" txt=\"第二名\"/><space width=\"20\"/><txt size=\"0.7\" txt=\"包厢%s\"/>"
						"<space width=\"30\"/><txt color=\"0x00ffdd10\" size=\"0.7\" txt=\"%0.1f\"/><txt size=\"0.7\" txt=\"分\"/></line>"
						,info.str,pST->top[1].room,pST->top[1].score/10.0f);
					if(pST->top[2].valid)
					{
						sprintf(info.str,"%s<line indentation=\"15\"><txt size=\"0.7\" txt=\"第三名\"/><space width=\"20\"/><txt size=\"0.7\" txt=\"包厢%s\"/>"
							"<space width=\"30\"/><txt color=\"0x00ffdd10\" size=\"0.7\" txt=\"%0.1f\"/><txt size=\"0.7\" txt=\"分\"/></line>"
							,info.str,pST->top[2].room,pST->top[2].score/10.0f);
					}
				}

				info.type = 1;
				CAutoLock lock(&m_csSpecial);
				m_listSpecialMsg.push_back(info);
			}
			break;
		}
	case MSGGL_GIFTSEND:
		{
			break;
		}
	case MSGGL_GIFTEXCHANGE:
		{
			GiftExchange* pGE = (GiftExchange*)pData;
			if (pGE)
			{
				SpecialInfo info = {0};
				sprintf(info.str,"<line><txt color=\"0xffffffff\" space=\"0.7\" size=\"0.48\" txt=\"%s%s包厢 消耗\"/>"
					"<txt color=\"0xffffdd10\" space=\"0.5\" size=\"0.75\" txt=\"%d\"/><space width=\"10\"/>"
					"<txt color=\"0xffffffff\" space=\"0.8\" size=\"0.48\" txt=\"金币\"/> </line>"
					"<line indentation=\"425\"><txt color=\"0xffffffff\" space=\"0.7\" size=\"0.48\" txt=\"兑换\"/>"
					"<txt color=\"0xffffdd10\" space=\"0.7\" txt=\"%s\" /><space width=\"5\"/>"
					"<txt color=\"0xffffffff\" size=\"0.48\" txt=\"%d%s\" /></line>",pGE->shop
					,pGE->room,pGE->money,pGE->giftname,pGE->count,pGE->giftunit);

				CAutoLock lock(&m_csSpecial);
				m_listSpecialMsg.push_back(info);
			}
			break;
		}
	case MSGGL_UPDATEMONEY:
		{
			SingleInt* pSi = (SingleInt*)pData;
			if(pSi)
			{
				CAutoLock lock(&m_csMoney);
				m_nMoney = pSi->wparam;
			}
			break;
		}
	case MSGGL_SELECTSTATE:
		{
			SingleInt* pSi = (SingleInt*)pData;
			if(pSi)
			{
				if(m_pDrawListener)
					m_pDrawListener->steStopSelect(!! pSi->wparam);
			}
			break;
		}
	case MSGGL_CATEGORY:
		{
			SingleString* pSs = (SingleString*)pData;
			if(pSs)
			{
				std::string tmp = pSs->str;
				MAPCATEGORY::iterator it = m_mapCategory.find(tmp);
				if(it == m_mapCategory.end())
					m_strCurCategory = "";
				else
					m_strCurCategory = pSs->str;

				switchSelect();
			}
			break;
		}
	case MSGGL_PROGRESS:
		{
			SingleInt* pSI = (SingleInt*)pData;
			if(m_pGraph)
			{
				ulong duration = 0;
				m_pGraph->getDuration(duration);
				m_pGraph->setCurPosition(duration*pSI->wparam/100);
			}
			break;
		}
	case MSGGL_OTHERGRADE:
		{
			CAutoLock lock(&m_otherRGLock);
			OtherGrade* pOG = (OtherGrade*)pData;

			GLRealtimeGrade grg;
			ZeroMemory(&grg,sizeof(grg));
			grg.accumulate_score = pOG->accumulate_score;
			grg.cur_db = pOG->cur_db;
			grg.cur_pitch = pOG->cur_pitch;
			grg.cur_tm = pOG->cur_tm;
			grg.realtime_score = pOG->realtime_score;
			grg.sentence_index = pOG->sentence_index;
			m_nOhterCurTime = pOG->cur_tm;
			if(pOG->sentence_switch)
			{
				GLSentenceGrade gsg;
				ZeroMemory(&gsg,sizeof(gsg));
				gsg.sentence_switch = pOG->sentence_switch;
				gsg.sentence_pitch_score = pOG->sentence_pitch_score;
				gsg.sentence_level = (GLSentenceLevel)pOG->sentence_level;
				gsg.sentence_lyric_score =  pOG->sentence_lyric_score;
				m_listOtherSG.push_back(gsg);
			}
			m_listOtherRG.push_back(grg);
			break;
		}
	case MSGGL_RANKINGLIST:
		{
			{
				CAutoLock lock(&m_csSleep);
				assert( m_nRankingSleep >= 0);
				m_nRankingSleep = 0;

				RankingInfo* pRI = (RankingInfo*) pData;
				if (pRI->ps[0].valid)
				{
					CScoreFrame* sf = m_pSceneMgr->getScoreRender(true);
					if(sf)
						sf->updateRankingInfo(&pRI->ps[0]);
				}

				if(pRI->ps[1].valid)
				{
					CScoreFrame* sf = m_pSceneMgr->getScoreRender(false);
					if(sf)
						sf->updateRankingInfo(&pRI->ps[1]);
				}
			}
			break;
		}
	case MSGGL_INITOPEN:
		{
			m_bInitOpenRoom = true;
			break;
		}
	case MSGGL_VIDEO_INFO:
		{
			VideoInfo* pVI = (VideoInfo*) pData;
			if (pVI && (pVI->type==0 || pVI->type==1))
			{
				m_mapVideoControl[(eVideoType)pVI->type] = pVI->operation;
				if(pVI->bname)
				{
					std::wstring wName;
					Str2Wstr(pVI->name,wName);
					m_mapVideoName[(eVideoType)pVI->type] = wName;
				}
				
				{
					CAutoLock lock(&m_csVideo);

					if(m_nVideoSize[pVI->type].width == 0 || m_nVideoSize[pVI->type].height == 0)
					{
						m_nVideoSize[pVI->type].width = pVI->width;
						m_nVideoSize[pVI->type].height = pVI->height;
					}
					else if(m_nVideoSize[pVI->type].width != pVI->width || m_nVideoSize[pVI->type].height != pVI->height)
					{
						WLOGH_ERRO << L"memory size is [" << pVI->width << L"*" << pVI->height << L"] not the same ["
							<< m_nVideoSize[pVI->type].width << L"*" << m_nVideoSize[pVI->type].height << L"],please check the video";
						SAFE_DELETE(pVI->buffer);
						goto fail;
					}

					m_nBytesPerPixel = pVI->bytes_pixel;

					if (pVI->size)
					{
						if (m_nVideoBytes[pVI->type] == 0)
							m_nVideoBytes[pVI->type] = pVI->size;

						if (pVI->size != m_nVideoBytes[pVI->type])
						{
							WLOGH_ERRO << L"memory size is not the same,please check the video";
							SAFE_DELETE(pVI->buffer);
							goto fail;
						}

						if (pVI->type == 0)
						{
							m_sHostVideo.changed = true;
							SAFE_DELETE(m_sHostVideo.p);//可能共享内存比较快，所以要把之前没有释放的释放掉
							m_sHostVideo.p = pVI->buffer;
						}
						else if (pVI->type == 1)
						{
							m_sSingVideo.changed = true;
							SAFE_DELETE(m_sSingVideo.p);
							m_sSingVideo.p = pVI->buffer;
						}
						else
							WLOGH_ERRO << L"receive a wrong video type";
					}
				}
				
			}
			break;
		}
	case MSGGL_DXPLAYER_ONLINE:
		{
			SingleInt* pInt = (SingleInt*)pData;
			if (pInt)
			{
				if (pInt->wparam)
					m_bOnline = true;
				else
					m_bOnline = false;
			}
			break;
		}
	case MSGGL_DXPLAYER_EFFECTNOTIFY:
		{
			SingleInt* pInt = (SingleInt*)pData;
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			if(pInt)
			{
				if(!m_bShowEndScore)
				{
					if (notify)
						notify->setLeftCurrentNotify(pInt->wparam+NOTIFY_MAGICAL_FAST);
				}
			}
			break;
		}
	case MSGGL_DXPLAYER_ATMOSPHERE:
		{
			SingleInt* pInt = (SingleInt*)pData;
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			if (pInt)
			{
				if (!m_bShowEndScore)
				{
					if (notify)
					{
						notify->setCenterNotifyAtomosphere((pInt->wparam)+NOTIFY_APPLAUD);
					}

					//停止之前的音效
					m_pSoundMgr->stopAll();
					switch(pInt->wparam)
					{
					case 0://鼓掌
						{
							if (m_pSoundApplaud)
								m_pSoundApplaud->soundPlay();
							break;
						}
					case 1://喝彩
						{
							if (m_pSoundCheer)
								m_pSoundCheer->soundPlay();
							break;
						}
					}
				}
				//delete pInt;
			}
			break;
		}
	case MSGGL_DXPLAYER_FIREWARNING:
		{
			m_pSoundMgr->stopAll();
			m_pSoundMgr->soundSetGStreamVol(10000);
			if (m_pSoundFireWarning)
				m_pSoundFireWarning->soundPlay();
			m_bFireWarning = true;
			break;
		}
	case MSGGL_DXPLAYER_NOFIREWARNING:
		{
			if (m_pSoundFireWarning)
				m_pSoundFireWarning->soundStop();
			m_pSoundMgr->soundSetGStreamVol(m_nSoundVolume);
			m_bFireWarning = false;
			break;
		}
	case MSGGL_DXPLAYER_MESSAGE:
		{
			PMessage * pMessage = (PMessage*)pData;
			if (pMessage)
			{
				if (pMessage->type == 0)
				{
					CAutoLock lock(&m_csLeftMsg);
					m_listLeffMsg.push_back(std::string(pMessage->str));
				}
				else if (pMessage->type == 1)
				{
					BarrageInfo bi = {0};
					bi.type = 1;
					bi.mode = pMessage->mode;
					bi.sayid = pMessage->sayid;
					wcscpy(bi.sayname,pMessage->sayname);
					static char buf[1024] = {0};
					float fSize = pMessage->fsize/5.0f;
					sprintf(buf,"<txt space=\"0.8\" size=\"%0.2f\" color=\"%x\" txt=\"%s\"/>",fSize
						,pMessage->argb,pMessage->str);
					strcpy(bi.str,buf);
					CAutoLock lock(&m_csBarrage);
					m_listBarrage.push_back(bi);
				}
				else if(pMessage->type == 2)
				{//星光里面需要配置下
					BarrageInfo bi = {0};
					bi.type = 1;
					bi.mode = pMessage->mode;
					bi.sayid = pMessage->sayid;
					wcscpy(bi.sayname,pMessage->sayname);
					strcpy(bi.str,pMessage->str);
					CAutoLock lock(&m_csBarrage);
					m_listBarrage.push_back(bi);
				}
			}
			break;
		}
	case MSGGL_DXPLAYER_OVERNINITY:
		{
			OverNinety* pO = (OverNinety*)pData;
			if (pO)
			{
				SpecialInfo info = {0};
				sprintf(info.str,"<line><txt color=\"ffffff\" space=\"0.7\" size=\"0.48\" txt=\"恭喜%s%s包厢演唱《%s》获得\"/>"
					"<space width=\"5\"/><txt color=\"ffdd10\" space=\"0.5\" txt=\"%0.1f\"/><space width=\"5\"/>"
					"<txt color=\"ffffff\" txt=\"分\"/> </line>"
					"<line indentation=\"92\" yoffset=\"-20\">"
					"<txt color=\"ffffff\" space=\"0.7\" size=\"0.48\" txt=\"本首排名No.\"/>"
					"<txt color=\"ffdd10\" space=\"0.5\" size=\"0.6\" txt=\"%d\" /><space width=\"5\"/>"
					"<txt color=\"ffffff\" space=\"0.7\" size=\"0.48\" txt=\",今日本店排名No.\" />"
					"<txt color=\"ffdd10\" space=\"0.5\" size=\"0.6\" txt=\"%d\"/><space width=\"5\"/>"
					"<txt color=\"ffffff\" space=\"0.7\" size=\"0.48\" txt=\",今日全国排名No.\"/>"
					"<txt color=\"ffdd10\" space=\"0.5\" size=\"0.6\" txt=\"%d\"/></line>",pO->shop
					,pO->room,pO->song,pO->score/10.0f,pO->ps.noInSong
					,pO->ps.noInHistory,pO->ps.noInCountry);

				CAutoLock lock(&m_csSpecial);
				m_listSpecialMsg.push_back(info);
			}
			break;
		}
	case MSGGL_DXPLAYER_MIC_RANGE:
		{
			DoubleInt* pdInt = (DoubleInt*) pData;
			if (pdInt)
			{
				m_cSound.min_mic = pdInt->wparam;
				m_cSound.max_mic = pdInt->lparam;
			}
			else
			{
				WLOGH_ERRO << L"MIC volume range read error";
			}
			break;
		}
	case MSGGL_DXPLAYER_MUSIC_RANGE:
		{
			DoubleInt* pdInt = (DoubleInt*) pData;
			if (pdInt)
			{
				m_cSound.min_music = pdInt->wparam;
				m_cSound.max_music = pdInt->lparam;
			}
			else
			{
				WLOGH_ERRO << L"Music volume range read error";
			}
			break;
		}
	case MSGGL_DXPLAYER_TONE_RANGE:
		{
			DoubleInt* pdInt = (DoubleInt*) pData;
			if (pdInt)
			{
				m_cSound.min_tone = pdInt->wparam;
				m_cSound.max_tone = pdInt->lparam;
			}
			else
			{
				WLOGH_ERRO << L"Tone value range read error";
			}
			break;
		}
	case MSGGL_DXPLAYER_OPENHY:
		{
			setHYWindow(m_hHY,true);
			break;
		}
	case MSGGL_DXPLAYER_CLOSEHY:
		{
			setHYWindow(m_hHY,false);
			break;
		}
	case MSGGL_DXPLAYER_NEXT_SONGNAME:
		{
			SingleWString* pStr = (SingleWString*) pData;
			if (pStr)
			{
				m_next_songName = pStr->str;
				m_bChangedNextSN = true;
			}
			break;
		}
	//切换歌曲，第一次可以当成播放
	//wparam歌曲名，包含完整路径和后缀名mpg/avi(char*)
	//lparam曲谱高度(int)
	case MSGGL_DXPLAYER_SWITCHSONG:
		{
			SwitchInfo* pSwitchSong = (SwitchInfo*)pData;
			if (pSwitchSong)
			{
				if (m_bWindowed)
				{
// 					s_gl->m_bWindowed = false;
// 					s_gl->adjustWindow(g_hWnd,s_gl->m_bWindowed);
				}
				wchar_t* fileName = pSwitchSong->str;
				CurPlayInfo lCurPlayInfo = {0};
				wcscpy(lCurPlayInfo.m_strMdmName,pSwitchSong->mdm); 
				int  nInfo = pSwitchSong->wparam;
				lCurPlayInfo.m_nStaffHeight1 = (int((nInfo>>24)&0xff)) * 10;
				lCurPlayInfo.m_bGradeSong = ((nInfo>>23)&0x1);
				
				m_cSound.cur_music = ((nInfo>>15)&0xff);
				//m_cSound.cur_mic = ((nInfo>>15)&0xf);
				//m_cSound.cur_tone = ((nInfo>>11)&0xf);

				lCurPlayInfo.m_isMV = ((nInfo>>14)&0x1);
				lCurPlayInfo.m_bAdvertisement = ((nInfo>>13)&0x1);
				lCurPlayInfo.m_bJump = ((nInfo>>12)&0x1);

				m_cSound.cur_tone = 0;//音调默认置为0
				lCurPlayInfo.m_bReverseAudio = ((nInfo>>10)&0x1);
				lCurPlayInfo.m_bAntiphonalSinging = ((nInfo>>9)&0x1);
				lCurPlayInfo.m_nStaffHeight2 = (int((nInfo>>1)&0xff))*10;
				lCurPlayInfo.m_bGrade = bool(nInfo&0x1);
				wcscpy(lCurPlayInfo.m_cur_songFile,fileName);
				wcscpy(lCurPlayInfo.m_cur_singerName,m_cur_singerName.c_str());
				wcscpy(lCurPlayInfo.m_cur_songName,m_cur_songName.c_str());
				wcscpy(lCurPlayInfo.m_cur_singerpic,m_cur_singerpic.c_str());
				wcscpy(lCurPlayInfo.m_cur_backupSinger,m_cur_backupSinger.c_str());

				try
				{
					dealSwitchSong(&lCurPlayInfo);
				}
				catch (CPlayerException& ex)
				{
					postRealEndSongMessage();
					WLOGH_ERRO << ex.getWWhat();
					SingleWString* pSws = new SingleWString;
					wcscpy(pSws->str,ex.getWWhat());
					m_smtHPMgr.PostMessageOS(MSG_ERRORLOG,pSws);
				}
			}
			break;
		}
	//暂停
	case MSGGL_DXPLAYER_CONTROL_PAUSE:
		{
			CTimerEx * timer = getTimerEx();
			if (timer)
			{
				SAFEPOINTER_CALL(m_pGraph)pausePlayer();//2
				if(g_WavRecorder)
					g_WavRecorder->setRecordFlag(false);
				COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
				if (notify)
				{
					notify->setLeftCurrentNotify(NOTIFY_PLAY_STOPPING);
				}
			}	
			break;
		}
	//播放
	case MSGGL_DXPLAYER_CONTROL_RESUME:
		{
			CTimerEx * timer = getTimerEx();
			if (timer)
			{
				if(g_WavRecorder)
					g_WavRecorder->setRecordFlag(true);
				if(m_pGraph)
					m_pGraph->resumePlayer();
				COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
				if (notify)
				{
					notify->setLeftCurrentNotify(NOTIFY_PLAY_RUNNING);
				}
			}	
			break;
		}

	//切换评分状态，开启/关闭
	case MSGGL_DXPLAYER_GRADE:
		{
			SingleInt* pInt = (SingleInt*) pData;
			if (pInt)
			{
				{
					CAutoLock lock(&m_csSleep);
					m_nRankingSleep = pInt->wparam;
					if (m_nRankingSleep >= 2)
					{
						m_nRankingSleep = 2;
					}
				}
				m_eShowType = eShowWave(pInt->wparam);					
				m_bChangedShowType = true;
			}
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			if (notify)
			{
				notify->setCenterNotifyGrade((int)m_eShowType+NOTIFY_CLOSE_GRADE);
			}
			m_pFloridMgr->stopAll();
			break;
		}

	//切换伴奏状态，开启/关闭
	case MSGGL_DXPLAYER_ACCOMPANY:
		{
			m_bAccompany = !m_bAccompany;
			CTimerEx * timer = getTimerEx();
			if(m_pGraph)
			{
				if (m_gCurPlayInfo.m_bReverseAudio)
					m_pGraph->switchAudioEx(m_bAccompany);
				else
					m_pGraph->switchAudioEx(!m_bAccompany);
			}
			
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			if (notify)
			{
				if (m_bAccompany)
					notify->setLeftCurrentNotify(NOTIFY_ORIGIN_CLOSE);
				else
					notify->setLeftCurrentNotify(NOTIFY_ORIGIN_OPEN);
			}
			break;
		}

	//歌曲信息
	//wparam歌手名(char*),lparam歌名(char*)
	case MSGGL_DXPLAYER_SONGINFO:
		{
			ThreeWString* pdStr = (ThreeWString*)pData;
			if (pdStr)
			{
				m_cur_singerName = pdStr->str1;
				m_cur_songName = pdStr->str2;
				m_cur_backupSinger = pdStr->str3;
			}
			break;
		}

	//歌手图片 ！！！！！！（需要在CMD_DXPLAYER_SONGINFO之前发送）
	//wparam歌手图片(包含路径,后缀)
	case MSGGL_DXPLAYER_SINGERPIC:
		{
			SingleWString* pStr = (SingleWString*)pData;
			if (pStr)
			{
				HANDLE hF = CreateFileW( pStr->str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if(hF == INVALID_HANDLE_VALUE)
				{
					WLOGH_WARN << L"singer picture open error or no file[" << m_gCurPlayInfo.m_cur_singerpic << L"]";
					m_cur_singerpic = L"";
					goto fail;
				}

				m_cur_singerpic = pStr->str;
				CloseHandle(hF);
			}
			break;
		}

	//设置歌曲的起始秒跟结束秒
	//wparam起始(int)lparam结束(int)
	case MSGGL_DXPLAYER_SONGSTARTEND:
		{
			DoubleInt* pdInt = (DoubleInt*)pData;
			if (pdInt)
			{
				if (((pdInt->wparam)>=0) && ((pdInt->lparam)>=0))
				{
					m_gCurPlayInfo.m_song_start_ms = pdInt->wparam *1000;
					m_gCurPlayInfo.m_song_end_ms = pdInt->lparam *1000;
				}
			}
			else
				WLOGH_ERRO << L"song time read error, point is null";
			break;
		}
	case MSGGL_DXPLAYER_GETTONEVALUE:
		{
			SingleInt* pVolume = new SingleInt;
			pVolume->wparam = m_cSound.cur_tone;
			m_smtHPMgr.PostMessageOS(MSG_TONEVALUE,(void*)pVolume);
			break;
		}
	case MSGGL_DXPLAYER_SUBTONEVALUE:
		{
			m_cSound.cur_tone--;
			if (m_cSound.cur_tone<m_cSound.min_tone)
				m_cSound.cur_tone = m_cSound.min_tone;
			SAFEPOINTER_CALL(m_pGraph)changeCurPitch(m_cSound.cur_tone);
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			if (notify)
				notify->setCenterCurrentNotify(NOTIFY_TONE_SET,m_cSound.cur_tone);
			break;
		}
	case MSGGL_DXPLAYER_ADDTONEVALUE:
		{
			m_cSound.cur_tone++;
			if (m_cSound.cur_tone>m_cSound.max_tone)
				m_cSound.cur_tone = m_cSound.max_tone;
			SAFEPOINTER_CALL(m_pGraph)changeCurPitch(m_cSound.cur_tone);
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			if (notify)
				notify->setCenterCurrentNotify(NOTIFY_TONE_SET,m_cSound.cur_tone);
			break;
		}
	case MSGGL_DXPLAYER_GETMICVOLUME:
		{
			long lVolume=0;
			SingleInt* pVolume = new SingleInt;
			pVolume->wparam = (int) lVolume;
			m_smtHPMgr.PostMessageOS(MSG_MICVOLUME,(void*)pVolume);
			break;
		}
	case MSGGL_DXPLAYER_SETMICVOLUME:
		{
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			SingleInt* pVolume = (SingleInt*) pData;
			if (pVolume)
			{
				if (notify)
				{
					notify->setCenterCurrentNotify(NOTIFY_MICVOLUME_SET,pVolume->wparam);
				}
			}
			break;
		}
	case MSGGL_DXPLAYER_MUTE_OPEN:
		{
			SAFEPOINTER_CALL(m_pGraph)setVolume(-10000);
			m_pSoundMgr->soundSetGStreamVol(0);
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			m_bMute = true;
			if (notify)
			{
				notify->setLeftCurrentNotify(NOTIFY_MUTE_OPEN);
			}
			break;
		}
	case MSGGL_DXPLAYER_MUTE_CLOSE:
		{
			float f = 1.0f*(m_cSound.cur_music-m_cSound.min_music)
				/(m_cSound.max_music-m_cSound.min_music);
			SAFEPOINTER_CALL(m_pGraph)setVolume((long)(f*10000-10000));
			m_pSoundMgr->soundSetGStreamVol(m_nSoundVolume);
			COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
			m_bMute = false;
			if (notify)
			{
				notify->setLeftCurrentNotify(NOTIFY_MUTE_CLOSE);
			}
			break;
		}
	//获取当前播放器音量
	case MSGGL_DXPLAYER_GETMUSICVOLUME:
		{
// 			long lVolume;
// 			SAFEPOINTER_CALL(m_pGraph)getVolume(lVolume);
// 			SingleInt* pVolume = new SingleInt;
// 			pVolume->wparam = (int) lVolume;
// 			m_smtHPMgr.PostMessageOS(MSG_MUSICVOLUME,(void*)pVolume);
			break;
		}
	//设置播放器声音 -10000到0: -10000最小，0最大.(静音传-10000)
	//wparam音量(int)
	case MSGGL_DXPLAYER_SETMUSICVOLUME:
		{
			SingleInt* pVolume = (SingleInt*) pData;
			if (pVolume)
			{
				m_cSound.cur_music = pVolume->wparam;
				float f = 1.0f*(m_cSound.cur_music-m_cSound.min_music)/(m_cSound.max_mic-m_cSound.min_music);
				long lVolume = (long)10000*f;
				SAFEPOINTER_CALL(m_pGraph)setVolume(lVolume-10000);
				COS_NotifyMessage* notify = m_pSceneMgr->getNotifyMsg();
				if (notify)
				{
					notify->setCenterCurrentNotify(NOTIFY_MUSICVOLUME_SET,m_cSound.cur_music);
				}
			}
			break;
		}
	}

fail:
	return ;
}

void COpenGLWindow::waitForRenderThread()
{
// 	HWND hWndFlash = ::FindWindowA(CLS_WINDOW_FLASH,NULL);
// 	if(hWndFlash)
// 	{
// 		COPYDATASTRUCT MyCDS = {0};
// 		MyCDS.dwData = WMDATA_STOP;
// 		::SendMessage(hWndFlash,WM_COPYDATA,(WPARAM)(HWND) g_hWnd,(LPARAM) (LPVOID) &MyCDS);
// 	}

	if (g_hThread)
	{
		if (m_pDrawListener)
			m_pDrawListener->setBlackCurtain(true);
		SAFEPOINTER_CALL(m_pGraph)setVolume(-10000);
		Sleep(2000);//glp
	}

	g_bGLThread = false;//结束线程
	WLOGH_INFO << L"g_hThread is waitting";
	WaitForSingleObject(g_hThread,INFINITE);
	WLOGH_INFO << L"g_hThread is passed";
	g_bGLThread = true;
}

void	COpenGLWindow::dealSwitchSong(CurPlayInfo* pCPI,bool bMy) throw(CPlayerException)
{
	CAutoLock lock(&m_ObjectLock);
	if(m_isFinish)
		return;

	std::wstring error_msg;

	assert(pCPI != NULL);

	static wchar_t buflog[300]={0};
	swprintf(buflog,L"[%s],[%s],CurMusicVolume=%d,CurMicVolume=%d,CurTone=%d,[%s],[%s],[%s],[%s&%s]"
		,bMy?L"显示器异常切歌":L"正常切歌",pCPI->m_bGradeSong?L"评分歌曲":L"非评分歌曲",m_cSound.cur_music
		,m_cSound.cur_mic,m_cSound.cur_tone,pCPI->m_bReverseAudio?L"音轨对调":L"正常音轨"
		,pCPI->m_bAntiphonalSinging?L"对唱模式":L"普通模式"
		,pCPI->m_bGrade?L"最后显示评测结果":L"最后不显示评测结果"
		,pCPI->m_isMV?L"MV":L"非MV",pCPI->m_bAdvertisement?L"广告":L"非广告");
	WLOGH_INFO << L"song file = " << pCPI->m_cur_songFile << L",mdm file = " << pCPI->m_strMdmName;
	WLOGH_INFO <<buflog;

	if(pCPI->m_isMV || pCPI->m_bAdvertisement)//如果是播放MV或者AD，则需要去判断文件是否存在
	{
		HANDLE hF = CreateFileW( pCPI->m_cur_songFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if(hF == INVALID_HANDLE_VALUE)
		{
			error_msg = std::wstring(L"文件无法读取，检查共享文件服务器 或者 该歌曲文件不存在:[") + std::wstring(pCPI->m_cur_songFile) + std::wstring(L"]");
			throw CPlayerException(error_msg.c_str());
			return ;
		}
		WLOGH_INFO << L"check file is passed ok";
		CloseHandle(hF);
	}
	
	waitForRenderThread();

	//必须在Render Thread结束之后才能赋值
	memcpy(&m_gCurPlayInfo,pCPI,sizeof(CurPlayInfo));

	if(m_isNeedResetShowType)
	{
		if(m_gCurPlayInfo.m_bGradeSong)//如果是评分歌曲
		{
			if (m_gCurPlayInfo.m_bAntiphonalSinging)
				m_eShowType = SHOW_THREE;
			else
				m_eShowType = SHOW_ONE;//根据产品需求，每次切换都要换成单人模式
		}
		else//非评分
			m_eShowType = SHOW_NONE;
	}
	else
	{
		if(m_gCurPlayInfo.m_bGradeSong)
		{
			if(m_gCurPlayInfo.m_bAntiphonalSinging)
			{
				if(m_bInitOpenRoom)//初始化开房
				{
					m_eShowType = SHOW_THREE;
					m_bInitOpenRoom = false;
				}
				else
				{
					//之前的设定
					if(m_eShowType==SHOW_ONE||m_eShowType==SHOW_TWO)
					{
						m_eShowType = SHOW_THREE;
					}
				}
			}
			else
			{
				if(m_bInitOpenRoom)
				{
					m_eShowType = SHOW_ONE;
					m_bInitOpenRoom = false;
				}
				else
				{
					//之前的设定
					if(m_eShowType == SHOW_THREE||m_eShowType==SHOW_TWO)
					{
						m_eShowType = SHOW_ONE;
					}
				}
			}
		}
		else
		{
			m_eShowType = SHOW_NONE;
		}
		m_bChangedShowType = true;
	}

	{
		CAutoLock lock(&m_csSleep);
		switch(m_eShowType)
		{
		case SHOW_THREE:
			{
				m_nRankingSleep = 2;
				break;
			}
		case SHOW_TWO:
		case SHOW_ONE:
			{
				m_nRankingSleep = 1;
				break;
			}
		case SHOW_NONE:
			{
				m_nRankingSleep = 0;
				break;
			}
		}
	}

	try
	{
		switchSong(m_gCurPlayInfo.m_cur_songFile,m_gCurPlayInfo.m_isMV?NULL:m_gCurPlayInfo.m_strMdmName
			,m_gCurPlayInfo.m_nStaffHeight1,m_gCurPlayInfo.m_nStaffHeight2);
	}
	catch(CPlayerException& e)
	{
		throw e;
		return ;
	}
	WLOGH_INFO << L"switch song is passed";

	if (m_gCurPlayInfo.m_cur_singerpic[0] == 0)
		WLOGH_WARN << L"no singer picture";

	if(!bMy)
		m_smtHPMgr.PostMessageOS(MSG_ENABLEB,NULL);
}

void COpenGLWindow::uninitSwitchSong()
{
	m_pSwfMgr->uninitSwfRender();
	//m_pSwfMgr->clearSwf();
	m_pSoundMgr->stopAll();
	m_pSceneMgr->resetDurationMS();
	//////////////////////////////////////////////////////////////////////////效果,初始化,由于设备重新生成，效果也必须重新生成。
	m_pSceneMgr->removeAllObject();
	m_pFloridMgr->stopAll();
	m_pFloridMgr->clearFlorid();
	m_pImgsetMgr->clearMapImageset();
	SAFEPOINTER_CALL(m_pGraph)gCloseGraph();

	//just for graph when is null
	SAFE_RELEASE(m_pDrawListener);

	CAutoLock lock(&m_endScoreLock);
	m_bShowEndScore = false;
	m_bGettingGold = false;
}

bool	COpenGLWindow::switchSelect()
{//暂停开放，，
	return false;
}

bool COpenGLWindow::switchSong(const wchar_t* fileName,const wchar_t* mdmName,const int nSPH1,const int nSPH2) throw(CPlayerException)
{
	std::wstring error_msg;
	uninitSwitchSong();
	m_bSelectSong = false;
	
	CGLVideo*	pGlVideo = NULL;
	pGlVideo = new CGLVideo();
	if(!pGlVideo)
	{
		error_msg = L"init TimerListener failed";
		goto failed;
	}

	if (!initTimerListener(g_hWnd,pGlVideo))//3d 设备初始化
	{
		error_msg = L"init TimerListener failed";
		goto failed;
	}

	bool b = m_pImgsetMgr->initAllImagesets(false);
	if (!b)
	{
		error_msg = L"Textures init error";
		goto failed;
	}
	
	if(!m_pSceneMgr->initFont())
	{
		error_msg = L"scene font main init failed";
		goto failed;
	}

	m_pFloridMgr->setEnvArg(getszIniFile(),getAppDirW());
	if(!cls_gl->m_pFloridMgr->initAllFlorid())
	{
		error_msg = L"miss some psi file";
		goto failed;
	}

	CRenderEngine*					pAllocator = getAllocator();
	try
	{
		m_pSceneMgr->preparePlay(g_hWnd,fileName,m_winSize,pAllocator);
	}
	catch(CPlayerException& e)
	{
		m_pSceneMgr->removeAllObject();
		m_pFloridMgr->clearFlorid();
		m_pImgsetMgr->clearMapImageset();
		SAFEPOINTER_CALL(m_pGraph)errorUninit();
		SAFE_RELEASE(m_pDrawListener);

		throw e;
		return false;
	}

	try
	{
		m_pSceneMgr->initSceneObj();
	}
	catch(CPlayerException& e)
	{
		m_pSceneMgr->removeAllObject();
		m_pFloridMgr->clearFlorid();
		m_pImgsetMgr->clearMapImageset();
		SAFEPOINTER_CALL(m_pGraph)errorUninit();
		SAFE_RELEASE(m_pDrawListener);

		throw e;
		return false;
	}

	//如果需要重置为伴唱
	if(m_isNeedResetAccompany)
		m_bAccompany = true;//切歌默认为伴唱

	if(!m_pSceneMgr->getFont()->loadFont(m_winSize,true,FONT_BANDING))
	{
		error_msg = L"Load font failed";
		goto failed;
	}

	if(m_pDrawListener->initLost())
	{
		error_msg = L"device init lost failed";
		goto failed;
	}

	m_pDrawListener->initGraph(m_nDurationMs);
	m_pSceneMgr->readyPlay(m_bAccompany,m_bMute);
	if ((!createVideoEvalThread()))
	{
		error_msg = L"CreateGLThread failed";
		goto failed;
	}

	//成功的话，释放掉
	//pAllocator->Release();
	return true;

failed:

	m_pSceneMgr->removeAllObject();
	m_pFloridMgr->clearFlorid();
	m_pImgsetMgr->clearMapImageset();
	SAFEPOINTER_CALL(m_pGraph)errorUninit();
	SAFE_RELEASE(m_pDrawListener);

	throw CPlayerException(error_msg.c_str());
	return false;
}

//////////////////////////////////////////////////////////////////////
COpenGLWindow* COpenGLWindow::cls_window = NULL;
COpenGLWindow* COpenGLWindow::getWindowSingleton()
{
	CAutoLock lock(&g_csSingleton);
	if (!cls_window)
	{
		cls_window = new COpenGLWindow;
	}
	return cls_window;
}

void COpenGLWindow::releaseWindowSingleton()
{
	SAFE_DELETE(cls_window);
}

LRESULT  CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static COpenGLWindow* s_gl = COpenGLWindow::getWindowSingleton();
	switch (message)									// Check For Windows Messages
	{
	case WM_CREATE:
		{
			break;
		}
	case WM_WINDOWPOSCHANGED:
		{
			HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
			if (s_gl->m_hMonitor!=NULL && s_gl->m_hMonitor!=monitor)
			{
				CDXApp::prepareWindow(s_gl,s_gl->getWinSize());
				try
				{
					s_gl->dealSwitchSong(&s_gl->m_gCurPlayInfo,true);
				}
				catch(CPlayerException& ex)
				{
					WLOGH_ERRO << ex.getWWhat();
				}
			}
			break;
		}
	case WM_COMMAND:
		{
			int wmId    = LOWORD(wParam);
			int wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_ACCELERATOR_C_F:
				{
					::MessageBox(/*m_*/hWnd,"CTRL+F","Message",MB_OK);
					break;
				}
			}
			break;
		}
	case WM_PLAY_FINALSCORE:
		{
			//Sleep(1000);
			if(s_gl->m_gCurPlayInfo.m_isMV)//是MV的时候才需要传递分数
			{
				if(s_gl->m_gCurPlayInfo.m_bGradeSong)
				{
					int nTotalScoreF=0;
					int nTotalScoreS=0;
					s_gl->getSceneMgr()->getFinalScoreStar(nTotalScoreF,nTotalScoreS);
					//nTotalScoreF = 999;
					DoubleInt* pdInt = new DoubleInt;
					assert(pdInt != NULL);
					s_gl->m_gCurPlayInfo.m_nScoreFirst = nTotalScoreF;
					s_gl->m_gCurPlayInfo.m_nScoreSecond = nTotalScoreS;
					pdInt->wparam = nTotalScoreF;//999;//;
					pdInt->lparam = nTotalScoreS;//998;//;
					s_gl->m_smtHPMgr.PostMessageOS(MSG_SCORESTARNUM,(void*)pdInt);

					if(s_gl->m_nRankingSleep)
					{
						SingleInt* pInt = new SingleInt;
						assert(pInt != NULL);
						pInt->wparam = s_gl->m_nRankingSleep;
						s_gl->m_smtHPMgr.PostMessageOS(MSG_NEEDRANKINGNUM,(void*)pInt);
					}

					COS_NotifyMessage* pNM = s_gl->m_pSceneMgr->getNotifyMsg();
					if(pNM)
						pNM->initGettingGold(max(nTotalScoreF,nTotalScoreS));
					s_gl->m_bGettingGold = true;
				}
			}
			break;
		}
	case WM_GRAPHNOTIFY:
		{
			CGraph* s_graph = s_gl->m_pGraph;
			if(!s_graph)
				break;

			LONG evCode = EC_SYSTEMBASE;
			LONG evParam1 = 0;
			LONG evParam2 = 0;
			while(SUCCEEDED(s_graph->getGraphEvent(evCode,evParam1,evParam2)))
			{
				//LOG_IFN3(evCode,evParam1,evParam2);
				s_graph->freeGraphEvent(evCode,evParam1,evParam2);
				switch (evCode)
				{
				case EC_COMPLETE:
					{//播放结束
						{
							//如果正在提示正在连接美女主播中
							if(s_gl->m_isShowWaitingPeri)
							{
								s_graph->pausePlayer();
								break;
							}

							if(s_gl->m_gCurPlayInfo.m_bAdvertisement)//如果是播放广告，则直接通知中心结束
							{
								s_gl->postRealEndSongMessage();
								break;
							}

							//如果强制显示评分，或者是评分歌曲，当前需要评分
							if ((s_gl->m_isForceShowEndEval || (s_gl->m_eShowType != SHOW_NONE && s_gl->m_gCurPlayInfo.m_bGradeSong))
								&& s_gl->m_bOnline)
							{
								s_gl->m_smtHPMgr.PostMessageOS(MSG_DISABLEB,NULL);
								int nTotalScoreF=0;
								int nTotalScoreS=0;
								if (s_gl->m_pDrawListener)
								{
									if(s_gl->m_eShowType == SHOW_NONE)
									{
										if(s_gl->m_gCurPlayInfo.m_bAntiphonalSinging)
											s_gl->m_eShowType = SHOW_TWO;
										else
											s_gl->m_eShowType = SHOW_ONE;
									}
								}

								//while(s_gl->m_nRankingSleep)
								{
									Sleep(1);
								}

								//重置状态，显示到最后的评测结果状态
								CAutoLock lock(&s_gl->m_endScoreLock);
								s_gl->m_bShowEndScore = true;
								s_gl->m_pFloridMgr->stopAll();
							}
							else
							{
								if (!s_gl->m_bOnline)
									s_gl->getSceneMgr()->endModel();
								s_gl->postRealEndSongMessage();
							}
						}
						break;
					}
				case EC_BUFFERING_DATA:
					{
						if (evParam1 == 0)
						{//结束缓冲
						}
						else if(evParam1 == 1)
						{//开始缓冲
						}
						break;
					}
				case EC_END_OF_SEGMENT:
					{
						break;
					}
				}
			}
			break;
		}
	case WM_DEBUG:
		{
			Sleep(3000);
			SAFEPOINTER_CALL(s_gl->m_pGraph)pausePlayer();
			int nTotalScoreF=0;
			int nTotalScoreS=0;
			if (s_gl->m_pDrawListener)
			{
				if(s_gl->m_eShowType == SHOW_NONE)
				{
					if(s_gl->m_gCurPlayInfo.m_bAntiphonalSinging)
					{
						s_gl->m_eShowType = SHOW_TWO;
					}
					else
					{
						s_gl->m_eShowType = SHOW_ONE;
					}
				}
				s_gl->getSceneMgr()->getFinalScoreStar(nTotalScoreF,nTotalScoreS);
			}

			//while(s_gl->m_nRankingSleep)
			{
				Sleep(1);
			}
			PlayerScore ps;
			ps.valid = true;
			ps.noInCountry = 1;
			ps.noInHistory = 35;
			ps.noInSong = 90;
			ps.percentCountry = 40;
			ps.percentHistory = 50;
			s_gl->m_pSceneMgr->getScoreRender(true)->updateRankingInfo(&ps);
			ps.noInHistory = 20;
			ps.percentCountry = 20;
			ps.percentHistory = 20;
			s_gl->m_pSceneMgr->getScoreRender(false)->updateRankingInfo(&ps);
			

			CAutoLock lock(&s_gl->m_endScoreLock);
			s_gl->m_bShowEndScore = true;
			s_gl->m_pFloridMgr->stopAll();
			//s_gl->m_pDrawListener->switchGrade(s_gl->m_eShowType);
			break;
		}
	case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				g_active=TRUE;						// Program Is Active
			}
			else
			{
				g_active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}
	case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

	case WM_CLOSE:
		{
			CAutoLock lock(&s_gl->m_ObjectLock);
			s_gl->m_isFinish = true;
			if (g_hThread)
			{
// 				if (s_gl->m_pDrawListener)
// 				{
// 					s_gl->m_pDrawListener->setBlackCurtain(true);
// 				}
				SAFEPOINTER_CALL(s_gl->m_pGraph)setVolume(-10000);
				//Sleep(2000);
			}

			g_bGLThread = false;//结束线程
			WLOGH_INFO << L"WM_CLOSE g_hThread is waitting";
			WaitForSingleObject(g_hThread,INFINITE);
			WLOGH_INFO << L"WM_CLOSE g_hThread is passed";
			SAFE_CLOSEHANDLE(g_hThread);

 			s_gl->uninitSwitchSong();
 			s_gl->m_smtHPMgr.ReleaseThread();
			PostQuitMessage(0);
			return 0;
		}

	case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			g_keys[wParam] = TRUE;					// If So, Mark It As TRUE
			switch(wParam)
			{
			case 'G':
				{
// 					SceneObject* cur_object = NULL;
// 					if (s_gl->m_pDrawListener)
// 					{
// 						cur_object = s_gl->m_pDrawListener->getSceneMgr()->getCurFirstSceneObject();
// 					}
// 					if (cur_object)
// 					{
// 						cur_object->setScoreShow(!cur_object->getScoreShow());
// 					}
					break;
				}
			case VK_ESCAPE:
				{
					break;
					::PostMessage(/*m_*/hWnd,WM_CLOSE,0,0);
					break;
				}
			case VK_SPACE:
				{
					break;
					if(g_bPboSupported)
					{
						bool bbreak = false;
						if (g_pboMode&0x4)
						{
							g_pboMode = 1;
							bbreak = true;
						}
						if (!bbreak)
						{
							g_pboMode <<= 1;
						}
					}
					break;
				}
			case 'L':
				{
					break;
					if (!g_listDlg)
					{
						g_listDlg = new CListDialog;
						if(g_listDlg->Create(IDD_DX_DIALOG,(CWnd*)s_gl))
						{
							g_listDlg->ShowWindow(SW_SHOW);
							g_listDlg->UpdateWindow();
						}
					}
					break;
				}
			case 'S':
				{
					break;
					if (GetKeyState(VK_CONTROL)&0x8000)
					{
						if (s_gl->getTimeRunning())
						{
							s_gl->setTimerRunning(false);
							CTimerEx * timer = s_gl->getTimerEx();
							if (timer)
							{
								timer->getListener()->m_bRunning = false;
								timer->stopTimer();//1
								SAFEPOINTER_CALL(s_gl->m_pGraph)pausePlayer();//2
// 								if (g_ WavRecorder)//录音停止3
// 								{
// 									g_ WavRecorder->RecordPause();
// 								}
							}	
						}
						else
						{
							s_gl->setTimerRunning(true);
							CTimerEx * timer = s_gl->getTimerEx();
							if (timer)
							{
								timer->getListener()->m_bRunning = true;
// 								if (g_ WavRecorder)//录音开始1
// 								{
// 									g_ WavRecorder->RecordContinue();
// 								}
								SAFEPOINTER_CALL(s_gl->m_pGraph)resumePlayer();//2
								timer->startTimer();//3								
							}	
						}
					}
					else
					{
						CTimerEx * timer = s_gl->getTimerEx();
						timer->stopTimer();//1
						//CGraph::getGraphInstance()->switchAudio();
						timer->startTimer();
					}
					break;
				}
			}
			return 0;								// Jump Back
		}

	case WM_KEYUP:								// Has A Key Been Released?
		{
			g_keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

	case WM_LBUTTONDOWN://用来拖动窗口，(发送非客户区左键按下)
		{
			//::SendMessage(g_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			break;
			s_gl->m_bWindowed = !s_gl->m_bWindowed;
			s_gl->adjustWindow(g_hWnd,s_gl->m_bWindowed);
			break;
		}

	}

	// Pass All Unhandled Messages To DefWindowProc
	return ::DefWindowProc(hWnd,message,wParam,lParam);
	//return CWnd::WindowProc(message,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////
COpenGLWindow::COpenGLWindow(ITimerListener* pTimerListener)
:m_pcTimeEx(NULL)
//,g_hRC(NULL)
//,m_pFrameData(NULL)
,m_bTimerRunning(true)
,m_pDrawListener(NULL)
,hBass(NULL)
,bSilent(false)
,streams(NULL)
,nSampleRate(44100)
,nFXVolume(100)
,nMusVolume(100)
,nStreamVolume(100)
,m_bWindowed(true)
,m_bAccompany(true)
,m_bMute(false)
,m_next_songName(L"下首")
,m_hHY(NULL)
,m_pSoundMgr(NULL)
,m_pSceneMgr(NULL)
,m_pFloridMgr(NULL)
,m_pImgsetMgr(NULL)
,m_pSwfMgr(NULL)
,m_pSoundFireWarning(NULL)
,m_pSoundCheer(NULL)
,m_pSoundApplaud(NULL)
,m_bFireWarning(false)
,m_bShowEndScore(false)
,m_eShowType(SHOW_ONE)
,m_bOnline(true)
,m_nBytesPerPixel(0)
,m_nRankingSleep(0)
,m_pGraph(NULL)
,m_nSoundVolume(5000)
,m_hMonitor(NULL)
,m_bSelectSong(false)
,m_bUpdateVideo(true)
,m_seed(0)
,m_nDurationMs(0)
,m_uRealtimeGradeMs(0)
,m_bChangedShowType(false)
,m_bChangedNextSN(false)
,m_nMoney(0)
,m_bGettingGold(false)
,m_isNeedResetAccompany(true)
,m_isShowWaitingPeri(false)
,m_isFinish(false)
,m_bInitOpenRoom(true)
{	
	memset(&m_gCurPlayInfo,0,sizeof(m_gCurPlayInfo));
	m_gCurPlayInfo.m_bGrade = true;
	m_gCurPlayInfo.m_nStaffHeight1 = 300;
	m_gCurPlayInfo.m_nStaffHeight2 = 160;
	wcscpy(m_gCurPlayInfo.m_cur_singerName,L"歌手");
	wcscpy(m_gCurPlayInfo.m_cur_songName,L"歌曲");

	CFileUtil::ReadCSV("randomlist.csv",m_mapCategory);

	memset(&m_cSound,0,sizeof(m_cSound));
	m_cSound.max_mic = 10;
	m_cSound.min_music = -10000;
	m_cSound.min_tone = -5;
	m_cSound.max_tone = 5;

	int nSize = sizeof(m_currentDir);
	ZeroMemory(m_currentDir,nSize);
	CFileUtil::GetModuleDir(m_currentDir,nSize);
	CFileUtil::GetModuleDirW(m_currentDirW,nSize);
	sprintf_s(m_szIniFile,"%s\\config.ini",m_currentDir);//定义配置文件
	swprintf(m_szIniFileW,L"%s\\config.ini",m_currentDirW);
	sprintf_s(m_szIniFilter,"%s\\filter.ini",m_currentDir);
	swprintf(m_wsFilterDir,L"%s\\ktvenv",m_currentDirW);
	GSize win_size(1280,720);
	setWinSize(win_size);

	m_isNeedResetAccompany = !! Ini_GetInt(m_szIniFile,"PARAM","NEEDRESETACCOMPANY",1);
	m_isNeedResetShowType = !! Ini_GetInt(m_szIniFile,"PARAM","NEEDRESETSHOWTYPE",1);
	m_isForceShowEndEval = !! Ini_GetInt(m_szIniFile,"PARAM","FORCESHOWENDEVAL",1);
	
	g_WavRecorder = new CWaveUnit;
	if (g_WavRecorder)
		g_WavRecorder->SetWaveInConfig(2,44100,16);//设置几声道，采样率，位
	else
		WLOGH_ERRO << L"wave recorder new error";

	m_pcTimeEx = new CTimerEx(m_szIniFile,60);
	m_pcTimeEx->setListener(pTimerListener);
	
	char logFile[260] = {0};
	sprintf_s(logFile,259,"%s\\player.log",m_currentDir);//定义日志文件
	LogHelp::InitLogHelp(logFile);

	CMixerDev::EnumMixerDevIdUnit(m_mapmx_str_id,m_vect_mxc);//枚举录音设备
	
	//m_hEvent = CreateEvent(NULL,false,false,NULL);
	//m_smtMgr.setEventHandle(m_hEvent);

	m_smtHPMgr.CreateThread(THREAD_PRIORITY_LOWEST);

	SingleString* pStr = new SingleString;
	strncpy(pStr->str,"GL",259);
	m_smtHPMgr.PostMessageOS(MSG_OPENSHAREMEMORY,(void*)pStr);

	g_hEventEval = CreateEvent(NULL,FALSE,FALSE,NULL);
	//g_hSemaphoreRender1 = CreateSemaphore(NULL,0,50,NULL);
	//g_hSemaphoreRender2 = CreateSemaphore(NULL,0,50,NULL);

	m_mapVideoControl[vtHost] = 1;
	m_mapVideoControl[vtSing] = 1;

	m_pGraph = new CGraph(this);

	Random_Seed();
}

COpenGLWindow::~COpenGLWindow(void)
{
	SAFE_DELETE(m_pSwfMgr);
	SAFE_DELETE(m_pImgsetMgr);
	SAFE_DELETE(m_pSceneMgr);
	SAFE_DELETE(m_pFloridMgr);
	SAFE_DELETE(m_pSoundMgr);

	SAFE_DELETE(m_sHostVideo.p);
	SAFE_DELETE(m_sSingVideo.p);
	SAFE_DELETE(m_pGraph);
	SAFE_CLOSEHANDLE(g_hSemaphoreRender2);
	SAFE_CLOSEHANDLE(g_hSemaphoreRender1);
	SAFE_CLOSEHANDLE(g_hEventEval);

	SAFE_DELETE(m_pcTimeEx);
	SAFE_DELETE(g_WavRecorder);
	SAFE_DELETE(g_listDlg);
	//CGLVideo::releaseInstance();
}

bool COpenGLWindow::initTimerListener(HWND hWnd,CGLVideo* pglVideo)
{
	
#ifdef _GWINXP
	if ((g_bCountToCreateListener%10000) == 0)
	{
		if (g_bCountToCreateListener != 0)
		{
			g_bNeedToReleaseListener = true;
		}
#endif				
		//如果创建不同的DrawListener 需要重新连接所有的filter；
		//如果是相同的，则只需要重新连接跟Source Filter相关的filter

		{
			HRESULT hr;
			//if (!m_drawListener)
			{
				m_pDrawListener = new CDrawListener(hr,hWnd,pglVideo,true);
				if (!m_pDrawListener)
				{
					SAFE_DELETE(pglVideo);
					WLOGH_ERRO << L"drawListener new error";
					return false;
				}
				if (FAILED(hr))
				{
					if(D3DERR_DEVICELOST == hr)
						WLOGH_ERRO << L"DX 3d device init failed hr = " << hex <<(unsigned int)hr <<"(Device Lost!)";
					else
						WLOGH_ERRO << L"DX 3d device init failed hr = " << hex <<(unsigned int)hr;
					return false;
				}
				m_pcTimeEx->setListener((ITimerListener*)m_pDrawListener);//设置绘画监听
			}

			m_pDrawListener->initListener();
		}
#ifdef _GWINXP
	}
	else
	{
		m_pcTimeEx->getListener()->setGLFlorid(pglFL);
	}
	g_bCountToCreateListener ++;
#endif
	
	return true;
}

void COpenGLWindow::_InitPowerStatus()//初始化电源状态获取函数
{
	hKrnl32 = LoadLibrary("kernel32.dll");

	if(hKrnl32 != NULL)
		lpfnGetSystemPowerStatus = (GetSystemPowerStatusFunc)GetProcAddress(hKrnl32, "GetSystemPowerStatus");

	_UpdatePowerStatus();
}


void COpenGLWindow::_UpdatePowerStatus()//更新当前系统的电源状态
{
	SYSTEM_POWER_STATUS ps;

	if(lpfnGetSystemPowerStatus != NULL && lpfnGetSystemPowerStatus(&ps))
	{
		if(ps.ACLineStatus == 1)//使用电线
		{
			nPowerStatus = HGEPWR_AC;
		}
		else if(ps.BatteryFlag < 128)//使用电池
		{
			nPowerStatus = ps.BatteryLifePercent;//当前电量所占总量百分比
		}
		else
		{
			nPowerStatus = HGEPWR_UNSUPPORTED;
		}
	}
	else
	{
		nPowerStatus = HGEPWR_UNSUPPORTED;
	}
}


void COpenGLWindow::_DonePowerStatus()//释放库
{
	if(hKrnl32 != NULL) FreeLibrary(hKrnl32);
}

void COpenGLWindow::adjustWindow(HWND hWnd,bool bWindowed,bool needNotify)
{
	RECT *rc;
	LONG style;
 
	if(bWindowed) {rc=&m_rectW; style=m_styleW; }
	else  {rc=&m_rectFS; style=m_styleFS; }
	::SetWindowLong(hWnd/*m_hWnd*/, GWL_STYLE, style);

	style=GetWindowLong(hWnd/*m_hWnd*/, GWL_EXSTYLE);
	if(bWindowed)
	{
		::SetWindowLong(hWnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
		::SetWindowPos(hWnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
	else
	{
		::SetWindowLong(hWnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
		::SetWindowPos(hWnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
		//::SetWindowLong(hWnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
		//::SetWindowPos(hWnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}

	static bool s_first = true;
	if(s_first || needNotify)
	{
		//通知到notify.exe变化位置
		NotifyPos* pNP = new NotifyPos;
		pNP->left = rc->left;
		pNP->top = rc->top;
		pNP->right = rc->right;
		pNP->bottom = rc->bottom;
		m_smtHPMgr.PostMessageOS(MSG_NOTIFY_POS,(void*)pNP);
		s_first = false;
	}
}

void COpenGLWindow::setHYWindow(HWND hWnd,bool bTop)
{
	RECT *rc = &m_rectHY;
	LONG style;

	::SetWindowLong(hWnd/*m_hWnd*/, GWL_STYLE, m_styleFS);
	style=GetWindowLong(hWnd/*m_hWnd*/, GWL_EXSTYLE);

	if (bTop)
	{
		::SetWindowLong(hWnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
		::SetWindowPos(hWnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
	else
	{
		::SetWindowLong(hWnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
		::SetWindowPos(hWnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
}

void COpenGLWindow::setWinStyleRect()
{
	// window argument

	int width=m_winSize.width + GetSystemMetrics(SM_CXFIXEDFRAME)*2;//左右边框
	int height=m_winSize.height + GetSystemMetrics(SM_CYFIXEDFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);//上下边框+标题框

	m_rectW.left=(GetSystemMetrics(SM_CXSCREEN)-width)/2;
	m_rectW.top=(GetSystemMetrics(SM_CYSCREEN)-height)/2;
	m_rectW.right=m_rectW.left+width;
	m_rectW.bottom=m_rectW.top+height;
	m_styleW=WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE; //WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;

	m_rectFS.left=0;
	m_rectFS.top=0;
	m_rectFS.right=m_winSize.width;
	m_rectFS.bottom=m_winSize.height;
	m_styleFS=WS_POPUP|WS_VISIBLE; //WS_POPUP|WS_VISIBLE
	//m_styleFS = WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_POPUP;
	//m_styleFS = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
}

void COpenGLWindow::setWinRect(const RECT rectFS)
{
	m_rectFS.left=rectFS.left;
	m_rectFS.top=rectFS.top;
	m_rectFS.right=rectFS.right;
	m_rectFS.bottom=rectFS.bottom;

	m_rectW.left=rectFS.left;
	m_rectW.top=rectFS.top;
	m_rectW.right=rectFS.right;
	m_rectW.bottom=rectFS.bottom;
}

void  COpenGLWindow::Random_Seed(int seed)
{
	if(!seed) m_seed=timeGetTime();
	else m_seed=seed;
}

int  COpenGLWindow::Random_Int(int min, int max)
{
	m_seed=214013*m_seed+2531011;
	return min+(m_seed ^ m_seed>>15)%(max-min+1);
}

float  COpenGLWindow::Random_Float(float min, float max)
{
	m_seed=214013*m_seed+2531011;
	//return min+g_seed*(1.0f/4294967295.0f)*(max-min);
	return min+(m_seed>>16)*(1.0f/65535.0f)*(max-min);
}

bool COpenGLWindow::initMgr(HWND hwnd)
{
	m_pSoundMgr = new CGLSoundMgr();//声音管理器
	m_pSceneMgr= new SceneMgr(hwnd);//场景对象管理器
	m_pFloridMgr= new CFloridMgr();//效果管理器
	m_pImgsetMgr= new ImgsetMgr();//纹理管理器
	m_pSwfMgr = new CSwfMgr();

	if(!m_pSwfMgr || !m_pImgsetMgr || !m_pFloridMgr || !m_pSceneMgr || !m_pSoundMgr)
		return false;

	m_pSoundMgr->setEnvArg(hwnd,getszIniFile(),getAppDir());
	if (!m_pSoundMgr->initAllSound())
		return false;

	m_pSoundFireWarning = m_pSoundMgr->getGLSound("firewarning");
	m_pSoundCheer = m_pSoundMgr->getGLSound("cheer");
	m_pSoundApplaud = m_pSoundMgr->getGLSound("applaud");

	m_nSoundVolume = Ini_GetInt(getszIniFile(),"SOUNDWIDGET","VOLUME",10000);
	m_pSoundMgr->soundSetGStreamVol(m_nSoundVolume);

	return true;
}

void	COpenGLWindow::postRealEndSongMessage()
{
	m_smtHPMgr.PostMessageOS(MSG_ENDSONG,0);
}

