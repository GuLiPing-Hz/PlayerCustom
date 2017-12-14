/*
	注释时间:2014-4-25
	author: glp
	通用渲染的接口
*/
#pragma once

#ifndef COMRENDER_H_
#define COMRENDER_H_

#include "../../GLListDef.h"
#include "../../font/font.h"
#include "../../Florid/FloridMgr.h"
#include "../../sound/soundmgr.h"
#include "../../effect/Alpha.h"
#include "../../effect/AnimaNumber.h"
#include "../../fto/ftotype.h"
#include "../../particlesystem/hgeparticlesys.h"
#include "../../Timer.h"

#include "../image/ImgsetMgr.h"

class IComRender
{
public:
	IComRender(ISceneObject* pObj):m_pOwner(pObj){}
	virtual ~IComRender(){}
	//更新
	virtual void update(const ulong delta_ms) = 0;
	//描绘多边形
	virtual void drawGraphBuffer() = 0;
protected:
	ISceneObject*			m_pOwner;
};

#endif//COMRENDER_H_

