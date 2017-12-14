#include "stdafx.h"
#include "ObjectFir.h"
#include "../../GLBlackCurtain.h"
#include "../render/TopNotifyRender.h"

CFirObject::CFirObject(bool& isInit)
:ISceneObject(FIROBJ)
,m_pGLBlackCurtain(NULL)
{
	assert(cls_gl != NULL);

	isInit = true;
	m_pGLBlackCurtain = new CGLBlackCurtain();
	if(!m_pGLBlackCurtain)
		isInit = false;

	m_pTopNotify = new CTopNotifyRender(isInit,this);
	if(!m_pTopNotify)
		isInit = false;
}
CFirObject::~CFirObject()
{
	SAFE_DELETE(m_pTopNotify);
	SAFE_DELETE(m_pGLBlackCurtain);
}

//���𣬳�ʼ������
bool CFirObject::awake(void* info)
{
	if(!ISceneObject::awake(info))
		return false;

	return m_pGLBlackCurtain->init();
}
//���õ�ǰʱ��ֵ��
void CFirObject::setCurMs(unsigned int ms)
{

}
//���²�����������
//@param ms:��ȥ��ʱ�䣬��λ�����룩
bool CFirObject::update(unsigned int ms)
{
	if(!ISceneObject::update(ms))
		return false;

	if(m_pTopNotify)
		m_pTopNotify->update(ms);

	if (m_pGLBlackCurtain)
	{
		if(cls_gl->m_pDrawListener->m_bBlackCurtain)
			m_pGLBlackCurtain->update(ms);
	}
	return true;
}
//��Ⱦ����
bool CFirObject::render()
{
	if(!ISceneObject::render())
		return false;

	//���ںܶ�ͼ��ĸ��ǣ������ȡ�����ķ���������
#ifndef SPECIAL_VERSION
	cls_gl->m_pFloridMgr->renderAll();
#endif

	if(m_pTopNotify)
		m_pTopNotify->drawGraphBuffer();

	if (m_pGLBlackCurtain)
	{
		if(cls_gl->m_pDrawListener->m_bBlackCurtain)
			m_pGLBlackCurtain->drawBlackCurtain();
		if(cls_gl->m_bFireWarning)
			m_pGLBlackCurtain->drawFireWarning();
	}
	return true;
}
