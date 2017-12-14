/*
	注释时间:2014-4-25
	author: glp
	SceneMgr 提供对场景对象的管理
	封装两个评分实例，对共用字体库的管理等等。
	封装对视频的准备工作，具体的播放时间在场景对象里面实现。
*/

#pragma once

#include <list>
#include <stdio.h>
#include <string.h>
#include <string>

#include "../Opengl.h"
#include "../particlesystem/hgedef.h"
#include "../fto/ftotype.h"
#include "../dat/AvGrade.h"
#include "../dat/DATExtractor.h"
#include "ISceneObject.h"
#include "../../exception/CPlayerException.h"

#include <map>

class COpenGLWindow;
class InputViewController;
class CRenderEngine;
class IEval;
class CGLFont;

typedef std::map<std::string,ISceneObject*> MAPSCENEOBJECT;
typedef std::list<ISceneObject*> LISTSCENEOBJ;
typedef std::list<std::string> LISTSCENEOBJNAME;

class CScoreFrame;
class COS_NotifyMessage;
class CTopNotifyRender;
class SceneMgr
{
public:
	SceneMgr(HWND hwnd,bool bLoop=true);
	~SceneMgr();

	static bool loadSongInfoFromDat( const std::string & file_name,int iDatOneLyricLength, _tDatOneLyric * fDatOneLyric
		, int iDatSongPitchLength, _tDatPitchGroup * fDatSongPitch, int iAvSongSentenceLineLength
		, unsigned long * plAvSongSentenceLine,unsigned int song_tm,_tSongInfo& song_info);

	static void parseTime(unsigned long start,unsigned long end);
	static bool loadSongInfo(const char* file,_tSongInfo& song_info);
	static bool loadSongInfo(const wchar_t* file,_tSongInfo& song_info);
	static void freeSongInfo();

	CScoreFrame* getScoreRender(bool first);
	COS_NotifyMessage* getNotifyMsg();
	CTopNotifyRender* getTopNotify();
	bool					initSceneObj() throw(CPlayerException);
	int					getFinalScoreStar(int &nFinalScoreF,int &nFinalScoreS);

	bool					initFont();
	CGLFont*		getFont();
	//void startScene();
	void	resetDurationMS();
	void	realRender();
	void	gameLoopUpdate(ulong durationMS,unsigned int lastEslapedTime);
	void renderScene();
	void startAnimation() ;
	void stopAnimation() ;

	bool preparePlay(HWND hWnd,const wchar_t* mpgFile,GSize windowSize,CRenderEngine* pallocator);
	bool readyPlay(bool bAccompany,bool bMute);
	//实时评测
	void realtimeEval(eShowWave eType,unsigned int nDurationMs,bool bSecond=true);
	//保存录音文件
	void endModel();

	//用于在发生错误时，方便的删除所有对象提供的方法。
	void removeAllObject();
	bool addObjectToAdd(ISceneObject* obj,bool bAwake=true);
	bool addObjectToRemove(const char* name);
	void sleepObj(const char* name);
	void resumeObj(const char* name);

	void			Input_GetMousePos(float *x, float *y);
	void			Input_SetMousePos(float x, float y);
	int			Input_GetMouseWheel();
	bool			Input_IsMouseOver();
	bool			Input_KeyDown(int key);
	bool			Input_KeyUp(int key);
	bool			Input_GetKeyState(int key);
	char*		Input_GetKeyName(int key);
	int			Input_GetKey();
	int			Input_GetChar();
	bool			Input_GetEvent(hgeInputEvent *event);
private:
	void			parseSongInfo(SingInfo& si1,SingInfo& si2,PlayInfo& pi1,PlayInfo& pi2);
	//实时更新
	void			realUpdate(unsigned int  ms);

	bool			loadWaveConfigure(const char* iniFile);
	void			getFileListFromDir(std::string dir, std::string pattern, std::list<std::string> &files);

	void			removeCurObject();
	void 		addObjectToScene();
	void 		removeObjectFromScene();

	template<class T>
	void clearListObject(T & listObject)
	{
		T::iterator i;
		for (i=listObject.begin();i!=listObject.end();i++)
		{
			SAFE_DELETE(*i);
		}
		listObject.clear();
	}
public:
	static std::wstring s_error_msg;

	static int sNalgorithm;

	WaveConfig   m_gWaveConfig;
	bool					m_bWaveConfig;
	// Input
	int					VKey;
	int					Char;
	int					Zpos;
	float					Xpos;
	float					Ypos;
	bool					bMouseOver;
	bool					bCaptured;
	char					keyz[256];
	CInputEventList*	queue;
	void				_UpdateMouse();
	void				_InputInit();
	void				_ClearQueue();
	void				_BuildEvent(int type, int key, int scan, int flags, int x, int y);

	HWND			m_hwnd;

private:
	CCritSec						m_csObjectAdd;
	CCritSec						m_csObjectRemove;
	CCritSec						m_csObjectSleep;
	CCritSec						m_csObjectResume;
	static _tSongInfo*		s_si;
	std::string					CONFIG_RECORD_DEVNAME;
	ulong							CONFIG_RECORD_SRCTYPE;

	MAPSCENEOBJECT		m_mapSceneObjs;
	LISTSCENEOBJNAME		m_listObjsToRemove;
	LISTSCENEOBJ				m_listObjsToAdd;
	LISTSCENEOBJNAME		m_listObjsToSleep;
	LISTSCENEOBJNAME		m_listObjsToResume;

	ulong						curTimeInterval;
	// 每一帧累计的精确到毫秒的值

	//控制循环
	bool								m_bLoop;
	std::string					m_record_dir;

	uint								m_nLimitRecord;
	int								CONFIG_FONT_SIZE;
public:
	IEval*							m_pIEval1;
	IEval*							m_pIEval2;
	unsigned int				m_durationMS;

	CGLFont*					m_pGLFont;//main font
	CGLFont*					m_pFontRankinfo;
	unsigned int				CONFIG_MONEYTOTWINKLE;

	int								CONFIG_LYRICSCORE_NOTIFY;
	int								CONFIG_MATH_ADD;
	float								CONFIG_PITCHSCOREPERCENT;
	float								CONFIG_LYRICSCOREPERCENT;
	bool								m_bEffectGold;

	static unsigned long			s_jump_begin_ms;
	static unsigned long			s_jump_end_ms;
};

