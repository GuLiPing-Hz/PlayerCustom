#include "stdafx.h"
#include "SMHelp.h"
#include "progress/ShareMemoryThreadMgr.h"
#include <string>
#include <ctype.h>
#include <TlHelp32.h>
#include <algorithm>

CShareMemoryThreadMgr g_smtMgr;
DEALMESSAGE CSMHelp::m_pDealMessage = NULL;

void CSMHelp::InitSMHelp(DEALMESSAGE pDealMessage)
{
	m_pDealMessage = pDealMessage;
}

bool CSMHelp::KillProcessFromName(const char* pProcessName)  
{  
	bool re = false;
	std::string strProcessName = pProcessName;
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)  
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  

	//PROCESSENTRY32进程快照的结构体  
	PROCESSENTRY32 pe;  

	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作  
	pe.dwSize = sizeof(PROCESSENTRY32);  


	//下面的IF效果同:  
	if((hSnapShot == INVALID_HANDLE_VALUE) || (!Process32First(hSnapShot,&pe)))
	{  
		SAFE_CLOSEFILEHANDLE(hSnapShot);
		goto final;
	}  

	//将字符串转换为小写  
	for_each(strProcessName.begin(),strProcessName.end(),tolower);
	//如果句柄有效  则一直获取下一个句柄循环下去  
	while (Process32Next(hSnapShot,&pe))
	{
		//pe.szExeFile获取当前进程的可执行文件名称  
		std::string scTmp = pe.szExeFile;  

		//将可执行文件名称所有英文字母修改为小写  
		for_each(scTmp.begin(),scTmp.end(),tolower);

		//比较当前进程的可执行文件名称和传递进来的文件名称是否相同  考虑到64位机器 (xxx.exe *32)
		if( 0 == scTmp.find(strProcessName))
		{  
			//从快照进程中获取该进程的PID(即任务管理器中的PID)  
			DWORD dwProcessID = pe.th32ProcessID;  
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE,dwProcessID);  
			::TerminateProcess(hProcess,0);  
			CloseHandle(hProcess);  
			goto final;
		}  
	}  

final:
	SAFE_CLOSEFILEHANDLE(hSnapShot);
	return re;
}  

CSMHelp::CSMHelp(const char* exeDir,const char* exeName)
{
	KillProcessFromName("notify.exe");
	g_smtMgr.CreateThread();
	DoubleString* pdStr = new DoubleString;
	if (pdStr)
	{
		strcpy_s(pdStr->str1,"GL");
		strcpy_s(pdStr->str2,exeDir);
		g_smtMgr.PostMessage(MSG_CREATESHAREMEMORY,(void*)pdStr);
	}
}

CSMHelp::~CSMHelp()
{
	g_smtMgr.ReleaseThread();
	KillProcessFromName("notify.exe");
}

bool CSMHelp::PostClosePlayer()
{
	g_smtMgr.PostMessage(MSG_DXPLAYER_CLOSE,NULL);
	return true;
}
bool CSMHelp::PostSwitchSong(const wchar_t* file,int info,const wchar_t* mdm)
{
	if (!file)
		return false;
	SwitchInfo* pStrInt = new SwitchInfo;
	if (pStrInt)
	{
		wcscpy(pStrInt->str,file);
		pStrInt->str[259] = 0;
		if(mdm)
		{
			wcscpy(pStrInt->mdm,mdm);
			pStrInt->mdm[259] = 0;
		}
		else
			pStrInt->mdm[0] = 0;
		pStrInt->wparam = info;
		g_smtMgr.PostMessage(MSG_SWITCH_SONG,(void*)pStrInt);
		return true;
	}
	return false;
}
bool CSMHelp::PostSingerPic(const wchar_t* file)
{
	if (!file)
		return false;
	SingleWString* pStr = new SingleWString;
	if (pStr)
	{
		wcscpy(pStr->str,file);
		g_smtMgr.PostMessage(MSG_SINGER_PIC,(void*)pStr);
		return true;
	}
	return false;
}
bool CSMHelp::PostControlResume()
{
	g_smtMgr.PostMessage(MSG_PLAY_CONTROL_RESUME,NULL);
	return true;
}
bool CSMHelp::PostControlPause()
{
	g_smtMgr.PostMessage(MSG_PLAY_CONTROL_PAUSE,NULL);
	return true;
}
bool CSMHelp::PostPlayGrade(unsigned int eType)
{
	//eType = eType %3;
	if (eType < 0 || eType >3)
		return false;
	SingleInt* pInt = new SingleInt;
	if (pInt)
	{
		pInt->wparam = eType;
		g_smtMgr.PostMessage(MSG_PLAY_GRADE,(void*)pInt);
		return true;
	}
	return false;
}
bool CSMHelp::PostPlayAccompany()
{
	g_smtMgr.PostMessage(MSG_PLAY_ACCOMPANY,NULL);
	return true;
}
bool CSMHelp::PostSongInfo(const ThreeWString* pParam)
{
	if (!pParam)
		return false;

	ThreeWString* pdStr = new ThreeWString;
	if (pdStr)
	{
		memcpy(pdStr,pParam,sizeof(ThreeWString));
		g_smtMgr.PostMessage(MSG_SONG_INFO,(void*)pdStr);
		return true;
	}
	return false;
}
bool CSMHelp::PostSongStartEnd(int start,int end)
{
	DoubleInt* pdInt = new DoubleInt;
	if (pdInt)
	{
		pdInt->wparam = start;
		pdInt->lparam = end;
		g_smtMgr.PostMessage(MSG_SONG_STARTEND,(void*)pdInt);
		return true;
	}
	return false;
}

bool CSMHelp::PostMicValue(int info)
{
	SingleInt* pInt = new SingleInt;
	if (pInt)
	{
		pInt->wparam = info;
		g_smtMgr.ProcessMessage(MSG_SET_MIC,(void*)pInt);
		return true;
	}
	return false;
}

bool CSMHelp::PostSetVolume(int info)
{
	SingleInt* pInt = new SingleInt;
	if (pInt)
	{
		pInt->wparam = info;
		g_smtMgr.PostMessage(MSG_SET_VOLUME,(void*)pInt);
		return true;
	}
	return false;
}
// void CSMHelp::PostGetVolume()
// {
// 	g_smtMgr.PostMessage(MSG_GET_VOLUME,NULL);
// }
bool CSMHelp::PostAddTone()
{
	g_smtMgr.PostMessage(MSG_SET_ADD_TONE,NULL);
	return true;
}
bool CSMHelp::PostSubTone()
{
	g_smtMgr.PostMessage(MSG_SET_SUB_TONE,NULL);
	return true;
}
bool CSMHelp::PostGetTone()
{
	g_smtMgr.PostMessage(MSG_GET_TONE,NULL);
	return true;
}
bool CSMHelp::PostMuteOpen()
{
	g_smtMgr.PostMessage(MSG_MUTE_OPEN,NULL);
	return true;
}
bool CSMHelp::PostMuteClose()
{
	g_smtMgr.PostMessage(MSG_MUTE_CLOSE,NULL);
	return true;
}
bool CSMHelp::PostMessage(PMessage* pMsg)
{
	if (!pMsg)
		return false;
	if(strlen(pMsg->str) > CONFIG_MAX_LEN)
		return false;
	PMessage* pPM = new PMessage;
	if (pPM)
	{
		memcpy(pPM,pMsg,sizeof(PMessage));
		pPM->fsize = pMsg->fsize< 1?1:pMsg->fsize>10?10:pMsg->fsize;
		g_smtMgr.PostMessage(MSG_MESSAGE,(void*)pPM);
		return true;
	}
	return false;
}
bool CSMHelp::PostSpecialMsg(const wchar_t* shop,const wchar_t* room,const wchar_t* song,int score,void* pPlayerScore)
{
	if (!shop || !room || !song || !pPlayerScore)
		return false;

	OverNinety* pO = new OverNinety;
	if (pO)
	{
		wcscpy(pO->shop,shop);
		wcscpy(pO->room,room);
		wcscpy(pO->song,song);
		pO->score = score;
		memcpy(&pO->ps,pPlayerScore,sizeof(PlayerScore));
		g_smtMgr.PostMessage(MSG_OVERNINETY,pO);
		return true;
	}
	return false;
}
bool CSMHelp::PostFireWarning()
{
	g_smtMgr.PostMessage(MSG_FIREWARNING,NULL);
	return true;
}
bool CSMHelp::PostNoFireWarning()
{
	g_smtMgr.PostMessage(MSG_NOFIREWARNING,NULL);
	return true;
}
bool CSMHelp::PostNextSong(const wchar_t* name)
{
	if (!name)
		return false;
	SingleWString* pStr = new SingleWString;
	if (pStr)
	{	
		wcscpy(pStr->str,name);
		g_smtMgr.PostMessage(MSG_NEXTSONG,(void*)pStr);
		return true;
	}
	return false;
}

bool CSMHelp::PostEffectNotify(unsigned int eInt)
{
	eInt = eInt%8;
	SingleInt* pInt = new SingleInt;
	if (pInt)
	{
		pInt->wparam = (int)eInt;
		g_smtMgr.PostMessage(MSG_EFFECTNOTIFY,(void*)pInt);
		return true;
	}
	return false;
}

bool CSMHelp::PostAtomosphere(unsigned int eInt)
{
	eInt = eInt%2;
	SingleInt* pInt = new SingleInt;
	if (pInt)
	{
		pInt->wparam = eInt;
		g_smtMgr.PostMessage(MSG_ATOMOSPHERE,(void*)pInt);
		return true;
	}
	return false;
}

bool CSMHelp::PostOnline(int eInt)
{
	eInt = eInt % 2;  
	SingleInt* pInt = new SingleInt;
	if (pInt)
	{
		pInt->wparam = eInt;
		g_smtMgr.PostMessage(MSG_ONLINE,(void*)pInt);
		return true;
	}
	return false;
}

bool CSMHelp::PostVideoInfo(int operation,int type,unsigned int width,unsigned int height,unsigned int bytes_pixel,unsigned int size
							,bool first,const char* name)
{
	VideoInfo* pVI = new VideoInfo;
	if (pVI)
	{
		pVI->operation = operation;
		pVI->bname = first;
		strcpy(pVI->name,name?name:"");
		pVI->type = type;
		pVI->width = width;
		pVI->height = height;
		pVI->bytes_pixel = bytes_pixel;
		pVI->size = size;
		pVI->buffer = NULL;

		g_smtMgr.PostMessage(MSG_VIDEOINFO,(void*)pVI);
		return true;
	}
	return false;
}

bool CSMHelp::PostRankingList(void* pPlayerScore,int nPlayer)
{
	if(!pPlayerScore || !nPlayer)
		return false;
	PlayerScore* pPs = (PlayerScore*) pPlayerScore;
	RankingInfo* pRI = new RankingInfo;
	if (pRI)
	{
		memset(pRI,0,sizeof(RankingInfo));
		memcpy(pRI->ps,pPs,sizeof(PlayerScore)*nPlayer);
		g_smtMgr.PostMessage(MSG_RANKING,(void*)pRI);
		return true;
	}
	return false;
}

bool CSMHelp::PostOhterGrade(const OtherGrade* pOG)
{
	if(!pOG)
		return false;
	OtherGrade* pData = new OtherGrade;
	if(!pData)
		return false;
	memcpy(pData,pOG,sizeof(OtherGrade));
	g_smtMgr.PostMessage(MSG_OTHERGRADE,(void*)pData);
	return true;
}

bool CSMHelp::PostProgress(int progress)
{
	SingleInt* pSI = new SingleInt;
	if(!pSI)
		return false;
	pSI->wparam = progress<0?0:progress>100?100:progress;
	g_smtMgr.PostMessage(MSG_PROGRESS,(void*)pSI);
	return true;
}

bool CSMHelp::PostCategory(const char* category)
{
	if(!category)
		return false;
	SingleString* pSs = new SingleString;
	if(!pSs)
		return false;
	strcpy_s(pSs->str,category);
	g_smtMgr.PostMessage(MSG_SELECTCATEGORY,(void*)pSs);
	return true;
}

bool CSMHelp::PostSelectState(bool bStop)
{
	SingleInt* pSi = new SingleInt;
	if(!pSi)
		return false;
	pSi->wparam = bStop;
	g_smtMgr.PostMessage(MSG_SELECTSTATE,(void*)pSi);
	return true;
}

bool CSMHelp::PostUpdateMoney(int money)
{
	SingleInt* pSi = new SingleInt;
	if(!pSi)
		return false;
	pSi->wparam = money;
	g_smtMgr.PostMessage(MSG_UPDATEMONEY,pSi);
	return true;
}

bool CSMHelp::PostGiftExchange(GiftExchange* pData)
{
	if(!pData)
		return false;
	GiftExchange* pGE = new GiftExchange;
	if(!pGE)
		return false;
	memcpy(pGE,pData,sizeof(GiftExchange));
	g_smtMgr.PostMessage(MSG_GIFTEXCHANGE,pGE);
	return true;
}

bool CSMHelp::PostGiftSend(GiftSend* pData)
{
	if(!pData)
		return false;
	GiftSend* pGS = new GiftSend;
	if(!pGS)
		return false;
	memcpy(pGS,pData,sizeof(GiftSend));
	g_smtMgr.PostMessage(MSG_GIFTSEND,pGS);
	return true;
}
//门店排名信息
bool CSMHelp::PostShopTop(ShopTopTip* pData)
{
	if(!pData)
		return false;
	ShopTopTip* pST = new ShopTopTip;
	if (!pST)
		return false;
	memcpy(pST,pData,sizeof(ShopTopTip));
	g_smtMgr.PostMessage(MSG_SHOPTOP,pST);
	return true;
}

//主播连接状态
bool CSMHelp::PostConnectStatus(int status)
{
	if(status < 0 || status > 3)
		return false;
	SingleInt* pSI = new SingleInt;
	if(!pSI)
		return false;
	pSI->wparam = status;
	g_smtMgr.PostMessage(MSG_PERISTATUS,pSI);
	return true;
}
//扫码信息
bool CSMHelp::PostScanInfo(const wchar_t* downloadUrl,const wchar_t* enterRoomUrl)
{
	if(!downloadUrl || !enterRoomUrl)
		return false;
	DoubleWString* pDS = new DoubleWString;
	if(!pDS)
		return false;

	wcscpy(pDS->str1,downloadUrl);
	wcscpy(pDS->str2,enterRoomUrl);

	g_smtMgr.ProcessMessage(MSG_SCAN,pDS);
	return true;
}
//门店排名更新信息
bool CSMHelp::PostSTUpdate(STUpdate* pData)
{
	if(!pData)
		return false;
	STUpdate* pST = new STUpdate;
	if (!pST)
		return false;
	memcpy(pST,pData,sizeof(STUpdate));
	g_smtMgr.PostMessage(MSG_STUPDATE,pST);
	return true;
}
//设置音乐音量范围'
bool CSMHelp::PostMusicRang(int min,int max)
{
	if(min >= max)
		return false;
	
	DoubleInt* pDI = new DoubleInt;
	if(!pDI)
		return false;
	pDI->wparam = min;
	pDI->lparam = max;

	g_smtMgr.PostMessage(MSG_MUSICRANG,pDI);
	return true;
}
//设置麦克风音量范围
bool CSMHelp::PostMicRang(int min,int max)
{
	if(min >= max)
		return false;
	DoubleInt* pDI = new DoubleInt;
	if(!pDI)
		return false;
	pDI->wparam = min;
	pDI->lparam = max;

	g_smtMgr.PostMessage(MSG_MICRANG,pDI);
	return true;
}

bool CSMHelp::PostInitOpenRoom()
{
	g_smtMgr.PostMessage(MSG_INITOPEN,NULL);
	return true;
}
