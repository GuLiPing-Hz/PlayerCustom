#ifndef ISCENEOBJECT__H__
#define ISCENEOBJECT__H__

#include "ObjectDef.h"

struct IDirect3DDevice9;
class ImgsetMgr;
class Imageset;

class ISceneObject
{
public:
	ISceneObject(const char* name)
		:m_pNextObj(NULL),m_bActive(false),m_bAlive(true),m_sName(name){}
	virtual ~ISceneObject(){};

	//设置下一个描绘对象,结束的时候通知
	void setNextObj(ISceneObject* pObj){m_pNextObj=pObj;}
	//唤起，初始化操作
	virtual bool awake(void* info){m_bActive=true; return true;}
	//睡眠
	void sleep(){m_bActive=false;}
	//睡眠之后需要唤起的操作
	void resume(){m_bActive=true;}
	//设置当前时间值，
	virtual void setCurMs(unsigned int ms)=0;
	//更新操作场景对象
	//@param ms:逝去的时间，单位（毫秒）
	virtual bool update(unsigned int ms){return m_bActive;}
	//渲染对象
	virtual bool render(){return m_bActive;}
	//结束，资源等回收操作
	virtual void end(){sleep();m_bAlive=false;}
protected:
	//子类可以通过这种方式唤起另一个对象，一般是在自己结束的时候
	void informNext(void* info){if(m_pNextObj)m_pNextObj->awake(info);}
public:
	bool						m_bAlive;
	bool						m_bActive;
	std::string	        m_sName;
	ISceneObject*		m_pNextObj;
};

#endif//ISCENEOBJECT__H__
