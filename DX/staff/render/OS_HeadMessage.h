/*
	注释时间:2014-4-25
	author: glp
	文字通告渲染以及播放信息显示渲染
	弹幕渲染
*/
#pragma once

#include <list>
#include <string>

#include "IComRender.h"

typedef struct _tagMessageTime
{
	std::wstring message;//指定显示的字符串
	ulong	duration_ms;//内部填充
}MessageTime;

typedef std::list<MessageTime> LISTMESSAGETIME;

typedef struct _SpecialMessage
{
	/*
		intendation 指定缩进值
		1.演唱分数超过90分的系统广播，增加<line indentation yoffset></line> 指定一行文字 intendation是缩进 yoffset指定y偏移
		2.门店前三甲显示
	*/
	int type;
	//xml 字符串，
	/*
		color:字体颜色
		size:字体大小
		txt:字体文本 
	*/
	std::string msg;
	
	int txt_width;//内部填充 最大长度
	int txt_height;//最大高度

	unsigned int life;
}SpecialMessage;
typedef std::list<SpecialMessage> LISTSPECIALMSG;

typedef struct _BarrageMessage
{
	unsigned int mode;

	/*
	@saytype 指定显示的开始对象，
		0：代表显示用户名字+id的方式
		1：图片-天天抢榜
	*/
	int saytype;
	std::wstring sayid;
	std::wstring sayname;
	std::string message;//xml 风格的字符串
	int diffwidth;
	bool isnamelonger;

	bool islocation;
	std::wstring location;
	int locationwidth;

	float speed;
	float cur_x;
	float cur_y;
	float height;//message中最大的字符串高度 
	int width;//message的宽度，包含图片等,不包含说话者名字id的长度
	int display_width;
	bool isfull;//显示完全
	bool isold;//不需要显示了
	unsigned int age;//
}BarrageMessage;

typedef std::list<BarrageMessage> LISTBARRAGEMSG;

class COpenGLWindow;
class COS_HeadMessage : IComRender
{
public:
	COS_HeadMessage(bool& bInit,ISceneObject* pObj,const std::wstring cur_song,const std::wstring next_song);
	virtual ~COS_HeadMessage();
protected:
	//描绘左边字幕条
	void	drawMessageLeft();
	//描绘右边字幕条
	void drawMessageRight();
	//描绘弹幕
	void drawBarrageMessage();
	//描绘特殊通知
	void drawSpecialText(const SpecialMessage& sm,float cur_x,float cur_y,float cur_alpha);
	void drawSpecialMessage();
	//描绘门店排行
	void drawShopTop(const SpecialMessage& sm,float x,float y,float alpha);
	//描绘包厢金币信息
	void drawRoomMoney(const ulong delta_ms);

	void updateMessageLeft(ulong delta_ms);
	void updateMessageRight(ulong delta_ms);
	void updateMessageBarrage(ulong delta_ms);
	void updateMessageSpecial(ulong delta_ms);
public:
	virtual void update(const ulong delta_ms);
	virtual void drawGraphBuffer();
public:
	//更新字幕
	void updateRightStr(const std::string next_song,bool b=true);
	void updateRightStr(const std::wstring next_song,bool b=true);
	//更新通知信息
	void updateLeftStr(const std::wstring str);
	//更新弹幕
	bool upateBarrageStr(const BarrageInfo& info);
	//更新特殊提醒
	void updateCenterSpecial(SpecialMessage& sm);
	//重置时间
	void resetTime();
public:
	bool					m_bNeedCreateAgain;
	CGLFont*		m_pFontR;
	CGLFont*		m_pFontL;
	CGLFont*		m_pFontBarrage;
	CGLFont*		m_pFontBarrageBubble;
	CGLFont*		m_pFontSpecial;
protected:
	ulong				m_nleft_last_ms;
	ulong				m_nright_last_ms;
	ulong				m_nleft_total_ms;
	ulong				m_nright_total_ms;
	ulong				m_nspecial_last_ms;

	ulong				m_nbarrage_last_ms;
	ulong				m_nbarrate_total_ms;

	CAlpha			m_gMoneyAlpha;

	LISTMESSAGETIME	m_listMessage;
	LISTBARRAGEMSG		m_listBarrageMsg;
	LISTSPECIALMSG		m_listSpecialMsg;
	//LISTULONGMS			m_listTotalMS;
	std::wstring 				m_leftcurrent_wstr;
	std::wstring					m_rightcurrent_wstr;
	std::wstring					m_rightfuture_wstr;
	std::wstring					m_cur_songName;
	std::wstring					m_next_songName;
	//特效提醒
	RenderEImage*			m_ImgStar;
	bool								m_bVaild;
	float								m_fSpeed;
	float								m_fSpeedOut;
	RenderEImage*			m_ImgBanner;
	CGLFlorid*					m_FloridStar;
	//气泡
	RenderEImage*			m_ImgBubbleL;
	RenderEImage*			m_ImgBubbleM;
	RenderEImage*			m_ImgBubbleR;

	//天天门店榜
	RenderEImage*			m_ImgIconShopTop;
	RenderEImage*			m_ImgBubbleSTL;
	RenderEImage*			m_ImgBubbleSTM;
	RenderEImage*			m_ImgBubbleSTR;
	RenderEImage*			m_ImgBgST;

	RenderEImage*	m_ImgAppDownload;
	RenderEImage*  m_ImgEnterRoom;
};
