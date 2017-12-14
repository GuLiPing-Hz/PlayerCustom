#include "stdafx.h"
#include "ObjectStatistics.h"
#include "../render/ScoreFrame.h"

CStatisticsObject::CStatisticsObject(bool& bInit,bool bFirst)
:ISceneObject(bFirst?STATISTICSOBJF:STATISTICSOBJS)
,scoreframe_render(NULL)
{
	bInit = true;
	
	bool tmp_bInit = true;
	scoreframe_render = new CScoreFrame(tmp_bInit,this,bFirst);
	if (!scoreframe_render)
	{
		SceneMgr::s_error_msg = L"scoreframe_render new failed";
		bInit = false ;
	}

	if(!tmp_bInit)
		bInit = false;
}
CStatisticsObject::~CStatisticsObject()
{
	SAFE_DELETE(scoreframe_render);
}

//���𣬳�ʼ������
bool CStatisticsObject::awake(void* info)
{
	if(!info)
	{
		SceneMgr::s_error_msg = L"CStatisticsObject awake info error";
		return false;
	}

	AwakeStatistics* pInfo = (AwakeStatistics*)info;
	if (scoreframe_render)
	{
		//pInfo->m_ave_dest_score = 990;//glp debug �鿴���Ľ�����Ч
		scoreframe_render->setFont(cls_gl->m_pSceneMgr->m_pFontRankinfo);
		//��Ҫ�ѷ��������ύ��SceneMgr
		scoreframe_render->initScoreFrame(pInfo->m_ave_dest_score,pInfo->m_star_number
			,pInfo->m_wcur_singer_name,pInfo->m_wcur_song_name,pInfo->eType);
	}

	return ISceneObject::awake(info);
}
//���õ�ǰʱ��ֵ��
void CStatisticsObject::setCurMs(unsigned int ms)
{

}
//���²�����������
//@param ms:��ȥ��ʱ�䣬��λ�����룩
bool CStatisticsObject::update(unsigned int ms)
{
	if(!ISceneObject::update(ms))
		return false;

	if (scoreframe_render)
	{
		if (scoreframe_render->getScoreFrameAllInit())
			scoreframe_render-> update(ms);
	}

	return true;
}
//��Ⱦ����
bool CStatisticsObject::render()
{
	if(!ISceneObject::render())
		return false;
	
	if (scoreframe_render)
	{
		if (scoreframe_render->getScoreFrameAllInit())
			scoreframe_render-> drawGraphBuffer();
	}
	return true;
}
