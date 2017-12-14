#ifndef OBJECTNOTIFY__H__
#define OBJECTNOTIFY__H__

#include "ISceneObject.h"
#include "../swf/ISwfListener.h"

class COS_HeadMessage;
class COS_NotifyMessage;
class COpenGLWindow;
class CSwf;
class CNotifyObject : public ISceneObject,public ISwfListener
{
public:
	CNotifyObject(bool& bInit,std::wstring songName,std::wstring singName,std::wstring nextSongName
		,const StructSound& ss,bool bAccompany,bool bMute);
	virtual ~CNotifyObject();

	//唤起，初始化操作
	virtual bool awake(void* info);
	//设置当前时间值，
	virtual void setCurMs(unsigned int ms);
	//更新操作场景对象
	//@param ms:逝去的时间，单位（毫秒）
	virtual bool update(unsigned int ms);
	//渲染对象
	virtual bool render();

	COS_NotifyMessage* getNotifyMessageRender(){return notifymessage_render;}
	COS_HeadMessage*	getHeadMessageRender(){return headmessage_render;}
	void setFont(CGLFont* pFont){m_pFont = pFont;}

	//ISwfListener
	virtual void onSwfEnd();
	virtual void onSwfFrameRender(int n){}
	virtual void onSwfStart(){}
private:
	void drawSongInfo();
private:
	COS_HeadMessage*		headmessage_render;
	COS_NotifyMessage*		notifymessage_render;

	std::wstring						m_sNextSongName;
	std::wstring						m_sCurSongName;
	std::wstring						m_sCurSinger;

	CGLFont*						m_pFont;
	bool									m_bSIMoveUp;
	bool									m_bSIMoveRight;
	bool									m_bSIKeep;
	bool									m_bSIDisappear;
	bool									m_bShowSongInfo;
	ulong								m_nlast_songinfo_ms;
	RenderEImage*				m_ImgSongInfoBkg;
	RenderEImage*				m_ImgSongInfoBkg2;
	RenderEImage*				m_ImgSingerPic;
	CImgsetEx*						m_imgsetEx;

	CSwf*								m_pSwf;
};

#endif//OBJECTNOTIFY__H__
