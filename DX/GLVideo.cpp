#include "StdAfx.h"
#include "GLVideo.h"

#include "DX/util.h"
#ifdef TEST_PLAY
#include "testlistener.h"
#include "glp.h"
#else
#include "DrawListener.h"
#endif

bool		g_bPboSupported = false;
int		g_pboMode = 0;			//PBO model


CGLVideo::CGLVideo()
:m_pvideoTextureData(NULL)
,m_videoHeight(0)
,m_videoWidth(0)
,m_textureX(0)
,m_textureY(0)
,m_pvideoData(NULL)
,m_bInit(false)
,m_switchTextureID(1)
,m_DATA_SIZE(0)
,m_texture1(NULL)
,m_texture2(NULL)
,m_vertexBuffer(NULL)
,m_bUseTexture1(true)
,m_owner(NULL)
{
//  #undef GL3D_Z_VIDEO
//  #define GL3D_Z_VIDEO 10.0f
	m_gQuad.v[0].point = GPoint(-1.0f,  1.0f, GL3D_Z_VIDEO); // 左上
	m_gQuad.v[1].point = GPoint(-1.0f, -1.0f, GL3D_Z_VIDEO); // 左下
	m_gQuad.v[2].point = GPoint( 1.0f,  1.0f, GL3D_Z_VIDEO); // 右上
	m_gQuad.v[3].point = GPoint( 1.0f, -1.0f, GL3D_Z_VIDEO); // 右下
	m_gQuad.v[0].colour = 0xffffffff; 
	m_gQuad.v[1].colour = 0xffffffff;
	m_gQuad.v[2].colour = 0xffffffff;
	m_gQuad.v[3].colour = 0xffffffff;
	m_gQuad.v[0].u = 0.0f; m_gQuad.v[0].v = 0.0f; // 左下
	m_gQuad.v[1].u = 0.0f; m_gQuad.v[1].v = 1.0f; // 左上
	m_gQuad.v[2].u = 1.0f; m_gQuad.v[2].v = 0.0f; // 右下
	m_gQuad.v[3].u = 1.0f; m_gQuad.v[3].v = 1.0f; // 右上

	m_gQuad.blend = BLEND_DEFAULT;
#ifdef SPECIAL_VERSION
	ZeroMemory(m_vertices,sizeof(m_vertices));
	m_vertices[0].point = GPoint(-1.0f,  1.0f, GL3D_Z_VIDEO); // 左上
	m_vertices[1].point = GPoint(-1.0f, -1.0f, GL3D_Z_VIDEO); // 左下
	m_vertices[2].point = GPoint( 1.0f,  1.0f, GL3D_Z_VIDEO); // 右上
	m_vertices[3].point = GPoint( 1.0f, -1.0f, GL3D_Z_VIDEO); // 右下

	// set up diffusion:
	m_vertices[0].colour = 0xffffffff;//argb
	m_vertices[1].colour = 0xffffffff;
	m_vertices[2].colour = 0xffffffff;
	m_vertices[3].colour = 0xffffffff;

	// set up texture coordinates
	m_vertices[0].u = 0.0f; m_vertices[0].v = 0.0f; // 左下
	m_vertices[1].u = 0.0f; m_vertices[1].v = 1.0f; // 左上
	m_vertices[2].u = 1.0f; m_vertices[2].v = 0.0f; // 右下
	m_vertices[3].u = 1.0f; m_vertices[3].v = 1.0f; // 右上
#endif//SPECIAL_VERSION
}

bool CGLVideo::getTextureIDInit()
{
	return m_bInit;
}

int CGLVideo::getTextureSwitch()
{
	return m_switchTextureID;
}

IDirect3DTexture9* CGLVideo::getTextureId()
{
	return m_texture1;
}

CGLVideo::~CGLVideo()
{
	SAFE_RELEASE(m_vertexBuffer);
}

void CGLVideo::init(IDirect3DDevice9* d3ddev)
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
	}
	//Create a vertex buffer and set stream source
	d3ddev->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY,D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_vertexBuffer, NULL);
}

BOOL CGLVideo::video2TextureMem(BYTE* pVideo,BYTE* pTexture,int format)
{
	if (!pVideo || !pTexture)
	{
		return FALSE;
	}
	BYTE *tmpVideo = pVideo;
	BYTE *tmpTexture = pTexture;
	if ((m_videoWidth > m_textureX) || (m_videoHeight > m_textureY))
	{
		return FALSE;
	}
	for (int i=0;i<m_videoHeight;i++)
	{
		memcpy(tmpTexture,tmpVideo,m_videoWidth*format);
		tmpTexture = tmpTexture + m_textureX*format;
		tmpVideo = tmpVideo + m_videoWidth*format;
	}
	return TRUE;
}

BOOL CGLVideo::loadGLTexturesVideo()
{
	BOOL Status=FALSE;									// Status Indicator					

	m_bInit = true;

	return Status;										// Return The Status
}

void	CGLVideo::setOwnListener(CRenderEngine* pListener)
{
	m_owner = pListener;
}

void CGLVideo::setTexture(IDirect3DTexture9* d3dTexture1,IDirect3DTexture9* d3dTexture2)
{
	m_texture1 = d3dTexture1;
	m_texture2 = d3dTexture2;

}

BOOL CGLVideo::updateGLTextures(IDirect3DTexture9* d3dTexture)
{
// 	if (m_texture)
// 	{
// 		m_texture->Release();
// 	}
	m_texture1 = d3dTexture;
	return TRUE;
}

void CGLVideo::setSrcRect( float fTU, float fTV )
{
	m_gQuad.v[0].u = 0.0f; m_gQuad.v[0].v = 0.0f; // low left
	m_gQuad.v[1].u = 0.0f; m_gQuad.v[1].v = fTV;  // high left
	m_gQuad.v[2].u = fTU;  m_gQuad.v[2].v = 0.0f; // low right
	m_gQuad.v[3].u = fTU;  m_gQuad.v[3].v = fTV;  // high right
#ifdef SPECIAL_VERSION
	m_vertices[0].u = 0.0f; m_vertices[0].v = 0.0f; // low left
	m_vertices[1].u = 0.0f; m_vertices[1].v = fTV;  // high left
	m_vertices[2].u = fTU;  m_vertices[2].v = 0.0f; // low right
	m_vertices[3].u = fTU;  m_vertices[3].v = fTV;  // high right
#endif//SPECIAL_VERSION
}

void CGLVideo::render()
{
	CAutoLock lock(&m_cs);
	IDirect3DTexture9* texture = NULL;

#ifdef DEBUG_USEDOUBLETEXTURE
	if (m_bUseTexture1)
		texture = m_texture1;
	else
		texture = m_texture2;
#elif DEBUG_USEGETTEXTURE
	int n;
	if (m_owner)
	{
		texture = m_owner->getPrivateTexture();
		if (texture)
			n = texture->AddRef();
	}
#else
	texture = m_texture1;
#endif
	//Get texture dimensions
	if (texture)
	{
#ifndef SPECIAL_VERSION
		m_gQuad.tex = texture;
		cls_gl->m_pDrawListener->renderQuad(&m_gQuad);
#else
		IDirect3DDevice9* d3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
		if(!d3dDevice)
		{
			texture->Release();
			return ;
		}
		HRESULT hr;
		FAIL_RET3( d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
		FAIL_RET3( d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

		FAIL_RET3( d3dDevice->SetTexture( 0, texture));
		//FAIL_RET3( d3dDevice->SetStreamSource(0, m_vertexBuffer, 0, sizeof(CUSTOMVERTEX)  ) );            //set next source ( NEW )
		FAIL_RET3( d3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX ) );
		//FAIL_RET3( d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2 ));  //draw quad 
		FAIL_RET3( d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,(void*)m_vertices,sizeof(CUSTOMVERTEX)) );  //draw quad 
		FAIL_RET3( d3dDevice->SetTexture( 0, NULL));
#endif//SPECIAL_VERSION
		texture->Release();
	}
}
