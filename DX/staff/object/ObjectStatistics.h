#ifndef OBJECTSTATISTICS__H__
#define OBJECTSTATISTICS__H__

#include "ISceneObject.h"

class CScoreFrame;
class CStatisticsObject : public ISceneObject
{
public:
	CStatisticsObject(bool& bInit,bool bFirst);
	virtual ~CStatisticsObject();

	//唤起，初始化操作
	virtual bool awake(void* info);
	//设置当前时间值，
	virtual void setCurMs(unsigned int ms);
	//更新操作场景对象
	//@param ms:逝去的时间，单位（毫秒）
	virtual bool update(unsigned int ms);
	//渲染对象
	virtual bool render();

	CScoreFrame*	 getScoreFrameRender(){return scoreframe_render;};
private:
	CScoreFrame*		scoreframe_render;
};


#endif//OBJECTSTATISTICS__H__
