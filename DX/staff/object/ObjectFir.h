#ifndef OBJECTFIR__H__
#define OBJECTFIR__H__

#include "ISceneObject.h"

class CGLBlackCurtain;
class CTopNotifyRender;
class CFirObject : public ISceneObject
{
public:
	CFirObject(bool& isInit);
	virtual ~CFirObject();

	//唤起，初始化操作
	virtual bool awake(void* info);
	//设置当前时间值，
	virtual void setCurMs(unsigned int ms);
	//更新操作场景对象
	//@param ms:逝去的时间，单位（毫秒）
	virtual bool update(unsigned int ms);
	//渲染对象
	virtual bool render();

	CTopNotifyRender* getTopNotify(){return m_pTopNotify;}
private:
	CGLBlackCurtain*			m_pGLBlackCurtain;//黑幕
	CTopNotifyRender*		m_pTopNotify;//顶端字幕提示
};

#endif//OBJECTFIR__H__
