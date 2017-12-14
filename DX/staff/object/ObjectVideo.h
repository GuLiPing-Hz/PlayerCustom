#ifndef OBJECTVIDEO__H__
#define OBJECTVIDEO__H__

#include "ISceneObject.h"

class CVSingerRender;
class CVideoObject : public ISceneObject
{
public:
	CVideoObject(bool& bInit);
	virtual ~CVideoObject();

	//唤起，初始化操作
	virtual bool awake(void* info);
	//设置当前时间值，
	virtual void setCurMs(unsigned int ms);
	//更新操作场景对象
	//@param ms:逝去的时间，单位（毫秒）
	virtual bool update(unsigned int ms);
	//渲染对象
	virtual bool render();

private:
	CVSingerRender*			vsinger_render;
};


#endif//OBJECTVIDEO__H__
