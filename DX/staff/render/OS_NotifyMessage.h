/*
	注释时间:2014-4-25
	author: glp
	提醒信息渲染
*/
#pragma once

#include "IComRender.h"
class CGLFlorid;
class COpenGLWindow;
class CGLSound;
class COS_NotifyMessage : IComRender
{
	typedef void (COS_NotifyMessage::*DRAW_NOTIFY)(const ulong delta_ms);
	typedef void (COS_NotifyMessage::*TIMEOUT_FUNC)();
public:
	COS_NotifyMessage(bool& bInit,ISceneObject* pObj,const StructSound& ss,bool bAccompany,bool bMute);
	virtual ~COS_NotifyMessage();

public:
	//设置左边的提示，主要是指一些静音，暂停，音效，原伴唱
	void setLeftCurrentNotify(const int n);
	//设置中间的提示，主要是指音乐音量，麦克风音量，音调
	void setCenterCurrentNotify(const int n,const int nValue);
	//设置评分模式提示
	void setCenterNotifyGrade(const int n);
	//设置气氛提示
	void setCenterNotifyAtomosphere(const int n);
	//初始化金币掉落
	void initGettingGold(int highScore);
public:
	//更新
	virtual void update(const ulong delta_ms);
	//描绘的接口
	virtual void drawGraphBuffer();
protected:
	void updateGoldGetting(ulong delta_ms);
	//描绘提示模板
	void updateNotifyTemplate(bool bNeedAddMS,ulong& nLastMS,const ulong delta_ms,const ulong total_ms,TIMEOUT_FUNC pFunc1=NULL);
	//描绘左边提示
	void	drawLeftNotify();
	//描绘中间提示
	//音量，音调
	void drawCenterNotify1();
	//评分模式
	void drawCenterNotify2();
	//气氛动画
	void drawCenterNotify3();
	//描绘槽
	void drawSlot(RenderEImage* img,const float fShedule,GPoint dest_point);
	//描绘槽的具体数值
	void drawValue(const int iValue,GPoint dest_point);
	//左边notify时间超出后的回调处理
	void dealLeftNotifTimeOut();
	//金币获得特效
	void drawGoldGetting();
public:
	Imageset*			m_imgset;
	bool						m_bDrawLeft;
	bool						m_bDrawCenter;
	bool						m_bDrawCenter2;
protected:
	bool						m_bLeftKeep;
	bool						m_bInitAccompany;
	bool						m_bneed_return;
	int						m_nReturn;

	int						m_nTop;
	int						m_arraHistory[2];//0 静音状态保存,1 暂停状态保存
	int						m_nPreCurrent1;
	int						m_nPreCurrent2;

	CCritSec				m_csLeft;
	CCritSec				m_csCenter;
	CCritSec				m_csCenter2;
	CCritSec				m_csCenter3;

	StructSound		m_structSound;
	int						m_nMicRange;
	int						m_nMusicRange;
	int						m_nToneRange;

	RenderEImage*				m_ImgGLSH;
	RenderEImage*				m_ImgMicVolumeBkg;
	RenderEImage*				m_ImgMusicVolumeBkg;
	RenderEImage*				m_ImgToneBkg;
	RenderEImage*				m_ImgNagetive;
	VECTORIMAGE	m_vectImgNotify;
	VECTORIMAGE	m_vectImgNumber;//////////////////////////////////////////////////////////////////////////
	VECTORIMAGE	m_vectImgAnimaApplaud;
	VECTORIMAGE	m_vectImgAnimaCheer;
	VECTORIMAGE	m_vectImgAnimaCom;
	uint						m_nAnimaI;

	RenderEImage*				m_ImgLeftCurrent1;
	RenderEImage*				m_ImgLeftCurrent2;
	RenderEImage*				m_ImgCenterCurrent;
	RenderEImage*				m_ImgCenterCurrent2;
	RenderEImage*				m_ImgCenterCurrentAnima;
	float						m_fPreShedule;
	float						m_fPreSheduleMic;
	float						m_fPreSheduleMusic;
	float						m_fPreSheduleTone;
	float						m_fCurSheduleMic;
	float						m_fCurSheduleMusic;
	float						m_fCurSheduleTone;
	float						m_fCurShedule;

	bool						m_bneed_addms;
	bool						m_bneed_addms_center;
	ulong					m_left_last_ms;
	ulong					m_center_last_ms;
	ulong					m_center_last_ms2;
	ulong					m_center_anima_last_ms;
	ulong					m_center_anima_singlems;

	int						m_nSheduleValue;

	//金币特效相关代码
	bool						m_bStartGoldGetting;
	RenderEImage*	m_ImgGoldBag;
	bool						m_bStartFlorid;
	CGLFlorid*			m_gFloridGold1;
	CGLFlorid*			m_gFloridGold2;
	CGLFlorid*			m_gFloridGold3;
	CGLFlorid*			m_gFloridGold4;
	CGLSound*			m_gSoundGold;

	float						m_fGoldBagFloridEndsize;
	float						m_fDeltaSize;

	CGLFlorid*			m_gFloridLight;
	VECTORIMAGE	m_vectImgGoldNumber;
	float						m_fFloridGoldMs;
	int						m_nGoldStep;

	float						m_fGoldPercent;
	int						m_nGoldCur;
	int						m_nGoldNext;
	int						m_nGoldDest;
	int						m_nDigit;
	VECTORINT			m_vectDest;
	VECTORINT			m_vectGoldCur;
	ulong					m_nNumberLastMs;
	ulong					m_nGoldEffectLastMs;
};
