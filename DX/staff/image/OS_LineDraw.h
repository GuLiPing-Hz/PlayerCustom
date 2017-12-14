/*
	◊¢ Õ ±º‰:2014-4-25
	author: glp
	œ»‰÷»æ
*/
#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "../GLDef.h"

#ifndef SPECIAL_VERSION

class COS_LineDraw
{
public:
	COS_LineDraw(const GSize  &win_size);
	virtual ~COS_LineDraw(void);

public:
	virtual void initLine(IDirect3DDevice9* d3dDev);
	virtual void renderLine(IDirect3DDevice9* d3dDev);
	virtual void drawPoint(const LINEVERTEX* batchPoint,int nPoints);

protected:
	bool											m_bline_init;
	IDirect3DVertexBuffer9*		m_vertexBuffer;
	LINEVERTEX*							m_batchPoint;
	int											m_line_points;

	GSize										m_windowsize;

};
#else
#include <../Player/DX/staff/image/OS_LineDraw.h>
#endif//SPECIAL_VERSION
