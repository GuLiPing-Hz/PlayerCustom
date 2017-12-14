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

//���𣬳�ʼ������
bool CVideoObject::awake(void* info)
{
	CGLFont* pFont = cls_gl->m_pSceneMgr->m_pGLFont;
	if(vsinger_render)
		vsinger_render->setFont(pFont);

	return ISceneObject::awake(info);
}
//���õ�ǰʱ��ֵ��
void CVideoObject::setCurMs(unsigned int ms)
{

}
//���²�����������
//@param ms:��ȥ��ʱ�䣬��λ�����룩
bool CVideoObject::update(unsigned int ms)
{
	if(!ISceneObject::update(ms))
		return false;

	if(vsinger_render)
		vsinger_render->update(ms);

	return true;
}
//��Ⱦ����
bool CVideoObject::render()
{
	if(!ISceneObject::render())
		return false;

	if (vsinger_render)//�����Ƶ��Ϣ
		vsinger_render->drawGraphBuffer();
	return true;
}
