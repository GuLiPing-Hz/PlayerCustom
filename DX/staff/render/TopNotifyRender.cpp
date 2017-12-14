#include "stdafx.h"
#include "TopNotifyRender.h"

#define NOTIFY_WAITING L"����Ϊ����������,�����ĵȴ�..."
#define NOTIFY_SUCCESS L"����������ʹ����˷������������"
#define NOTIFY_TIMEOUT L"��Ǹ,���ӳ�ʱ,���Ժ�����"
#define NOTIFY_FAILED L"��Ǹ,�����쳣,���Ժ�����"

#define TIME_TO_NOTIFY 5000
#define TIME_TO_LIVE 6000

CTopNotifyRender::CTopNotifyRender(bool& isInit,ISceneObject* pObj)
:IComRender(pObj)
,m_pImgBackground()
,m_pFontNotify(NULL)
,m_cur_ms(TIME_TO_LIVE)
,m_index(-1)
,m_isNotified(false)
{
	isInit = true;

	m_pFontNotify = new CGLFont(30,L"zznotify");
	if(m_pFontNotify)
	{
		std::wstring wstr = NOTIFY_WAITING;
		wstr += NOTIFY_SUCCESS;
		wstr += NOTIFY_TIMEOUT;
		wstr += NOTIFY_FAILED;
		m_pFontNotify->addFont(wstr);
		m_pFontNotify->loadFont(cls_gl->m_winSize,true);
	}
	else 
		isInit = false;

	Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(L"zzmask");
	if(pImgset)
		m_pImgBackground = pImgset->getImage(L"BlackCurtain");
	else
		isInit = false;
	if(!m_pImgBackground)
		isInit = false;
}

CTopNotifyRender::~CTopNotifyRender()
{
	SAFE_DELETE(m_pFontNotify);
}
//IComRender
//����
void CTopNotifyRender::update(const ulong delta_ms)
{	
	if(m_index <= 0)
		return ;
	
	m_cur_ms+= delta_ms;

	//�����ʱ���ѳ�����������,��֪ͨ�и�.
	if(m_cur_ms >= TIME_TO_NOTIFY && !m_isNotified)
	{
		cls_gl->postRealEndSongMessage();
		m_isNotified = true;
	}
}

void CTopNotifyRender::drawNotify(const wchar_t* notify)
{
	if(!notify || m_pFontNotify || !m_pImgBackground)
		return ;

	m_pImgBackground->draw(GPoint(0.0f,0.0f,GL3D_Z_FIRALARM),cls_gl->m_winSize);
	int width = m_pFontNotify->getTextWidth(notify);
	float x = (cls_gl->m_winSize.width - width)/2.0f;
	static float y = cls_gl->m_winSize.height/2.0f;
	m_pFontNotify->drawText(notify,GPoint(x,y,GL3D_Z_FIRALARM));
}
//�������
void CTopNotifyRender::drawGraphBuffer()
{
	switch(m_index)
	{
	case 0:
		{
			drawNotify(NOTIFY_WAITING);
			break;
		}
	case 1:
		{
			drawNotify(NOTIFY_SUCCESS);
			break;
		}
	case 2:
		{
			drawNotify(NOTIFY_TIMEOUT);
			break;
		}
	case 3:
		{
			drawNotify(NOTIFY_FAILED);
			break;
		}
	}
}

void CTopNotifyRender::stopNotify()
{
	m_index = -1;
	m_cur_ms = TIME_TO_LIVE;
	m_isNotified = true;
}
