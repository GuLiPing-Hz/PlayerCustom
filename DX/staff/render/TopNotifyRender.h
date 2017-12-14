#ifndef TOPNOTIFYRENDER__H__
#define TOPNOTIFYRENDER__H__

/************************************************************************/
/* 
	@Author Glp
	@Time 2014-11-24
*/
/************************************************************************/
#include "IComRender.h"

class CTopNotifyRender : public IComRender
{
public:

	CTopNotifyRender(bool& isInit,ISceneObject* pObj);
	virtual ~CTopNotifyRender();

	//IComRender
	//更新
	virtual void update(const ulong delta_ms);
	//描绘多边形
	virtual void drawGraphBuffer();

	void startNotify(int index){m_index = index;m_cur_ms=0;m_isNotified=false;}
	void stopNotify();
private:
	void drawNotify(const wchar_t* notify);
private:
	RenderEImage* m_pImgBackground;
	CGLFont*		   m_pFontNotify;

	bool				       m_isNotified;
	unsigned long   m_cur_ms;
	int					   m_index;
};

#endif//TOPNOTIFYRENDER__H__