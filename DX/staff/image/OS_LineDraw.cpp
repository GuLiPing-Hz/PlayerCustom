#include "StdAfx.h"
#include "OS_LineDraw.h"

COS_LineDraw::COS_LineDraw(const GSize  &win_size)
:m_bline_init(false)
,m_vertexBuffer(NULL)
,m_batchPoint(NULL)
,m_line_points(0)
,m_windowsize(win_size)
{
}

COS_LineDraw::~COS_LineDraw(void)
{
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
	}
}

void COS_LineDraw::drawPoint(const LINEVERTEX* batchPoint,int nPoints)
{
	static CGFloat half_width = m_windowsize.width / 2.0f;
	static CGFloat half_height = m_windowsize.height / 2.0f;

	int nSize = sizeof(LINEVERTEX);
	m_line_points = nPoints;
	memcpy(m_batchPoint,batchPoint,nSize*nPoints);
	for(int i=0;i<m_line_points;i++)
	{
		m_batchPoint[i].point.x = (m_batchPoint[i].point.x - half_width)/half_width;
		m_batchPoint[i].point.y = (half_height - m_batchPoint[i].point.y)/half_height;
	}
}

void COS_LineDraw::initLine(IDirect3DDevice9* d3dDev)
{
	if (m_bline_init)
	{
		return ;
	}
	//第一个参数是大小
	HRESULT hr = d3dDev->CreateVertexBuffer(BATCH_LINE_BUFFERSIZE * sizeof(LINEVERTEX), D3DUSAGE_DYNAMIC/*D3DUSAGE_WRITEONLY*/,
		D3DFVF_LINEVERTEX, D3DPOOL_MANAGED, &m_vertexBuffer, NULL);
	m_line_points = 0;

	m_bline_init = true;
}

void COS_LineDraw::renderLine(IDirect3DDevice9* d3dDev)
{
	d3dDev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	d3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Set stream source to batch buffer
	d3dDev->SetStreamSource (0, m_vertexBuffer, 0, sizeof(LINEVERTEX));

	//d3dDev->SetVertexShader(NULL);
	d3dDev->SetFVF( D3DFVF_LINEVERTEX);
	//d3dDev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,cur_geo_idx*2) ;
	if (m_line_points>1)
	{
		d3dDev->DrawPrimitive (/*D3DPT_TRIANGLELIST*/D3DPT_LINESTRIP, 0,m_line_points-1 );
	}
}

