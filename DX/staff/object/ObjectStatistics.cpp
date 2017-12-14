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

//唤起，初始化操作
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
		//pInfo->m_ave_dest_score = 990;//glp debug 查看最后的结算特效
		scoreframe_render->setFont(cls_gl->m_pSceneMgr->m_pFontRankinfo);
		//需要把分数数据提交给SceneMgr
		scoreframe_render->initScoreFrame(pInfo->m_ave_dest_score,pInfo->m_star_number
			,pInfo->m_wcur_singer_name,pInfo->m_wcur_song_name,pInfo->eType);
	}

	return ISceneObject::awake(info);
}
//设置当前时间值，
void CStatisticsObject::setCurMs(unsigned int ms)
{

}
//更新操作场景对象
//@param ms:逝去的时间，单位（毫秒）
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
//渲染对象
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
