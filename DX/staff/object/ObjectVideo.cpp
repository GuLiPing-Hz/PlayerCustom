#include "stdafx.h"
#include "ObjectVideo.h"
#include "../render/VSingerRender.h"

CVideoObject::CVideoObject(bool& bInit)
:ISceneObject(XGVIDEOOBJ)
,vsinger_render(NULL)
{
	bool tmp_bInit = true;
	vsinger_render = new CVSingerRender(tmp_bInit,this);
	if (!vsinger_render)
	{
		SceneMgr::s_error_msg = L"vsinger_render new failed";
		bInit = false ;
	}
	if(!tmp_bInit)
		bInit = false;
}
CVideoObject::~CVideoObject()
{
	SAFE_DELETE(vsinger_render);
}

//唤起，初始化操作
bool CVideoObject::awake(void* info)
{
	CGLFont* pFont = cls_gl->m_pSceneMgr->m_pGLFont;
	if(vsinger_render)
		vsinger_render->setFont(pFont);

	return ISceneObject::awake(info);
}
//设置当前时间值，
void CVideoObject::setCurMs(unsigned int ms)
{

}
//更新操作场景对象
//@param ms:逝去的时间，单位（毫秒）
bool CVideoObject::update(unsigned int ms)
{
	if(!ISceneObject::update(ms))
		return false;

	if(vsinger_render)
		vsinger_render->update(ms);

	return true;
}
//渲染对象
bool CVideoObject::render()
{
	if(!ISceneObject::render())
		return false;

	if (vsinger_render)//描绘视频信息
		vsinger_render->drawGraphBuffer();
	return true;
}
