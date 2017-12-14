#include "stdafx.h"
#include "Swf.h"

#ifdef TEST_PLAY
#include "glp.h"
#else 
#include "../OpenGLWindow.h"
#endif//TEST_PLAY

// #include "gameswf/gameswf_root.h"
// #include "gameswf/gameswf_player.h"


// static gameswf::render_handler* s_handler = NULL;
//tu_string flash_vars;

void CSwf::InitSwfRender()
{
// 	s_handler = gameswf::create_render_handler_d3d(cls_gl->m_pDrawListener->m_D3DDev);
// 	gameswf::set_render_handler(s_handler);
}
void CSwf::UninitSwfRender()
{
// 	gameswf::set_render_handler(NULL);
// 	SAFE_DELETE(s_handler);
}

CSwf::CSwf()
:m_pRoot(NULL)
//,m_pPlayer(NULL)
,m_bPlay(true)
,m_bLoop(false)
,m_pTex(NULL)
,m_pSwfListener(NULL)
{
// 	m_pPlayer = new gameswf::player();
// 	if(m_pPlayer)
// 	{
// 		m_pPlayer->set_force_realtime_framerate(false);
// 		m_pPlayer->set_flash_vars(flash_vars);
// 	}

	//默认居中，处于Notify层
// 	m_gQuad.v[0].point = GPoint(-0.5f,  0.5f, GL3D_Z_NOTIFY); // 左上
// 	m_gQuad.v[1].point = GPoint(-0.5f, -0.5f, GL3D_Z_NOTIFY); // 左下
// 	m_gQuad.v[2].point = GPoint( 0.5f,  0.5f, GL3D_Z_NOTIFY); // 右上
// 	m_gQuad.v[3].point = GPoint( 0.5f, -0.5f, GL3D_Z_NOTIFY); // 右下
// 	m_gQuad.v[0].colour = 0xffffffff; 
// 	m_gQuad.v[1].colour = 0xffffffff;
// 	m_gQuad.v[2].colour = 0xffffffff;
// 	m_gQuad.v[3].colour = 0xffffffff;
// 	m_gQuad.v[0].u = 0.0f; m_gQuad.v[0].v = 0.0f; // 左下
// 	m_gQuad.v[1].u = 0.0f; m_gQuad.v[1].v = 1.0f; // 左上
// 	m_gQuad.v[2].u = 1.0f; m_gQuad.v[2].v = 0.0f; // 右下
// 	m_gQuad.v[3].u = 1.0f; m_gQuad.v[3].v = 1.0f; // 右上
// 
// 	initTexture();
// 	m_gQuad.blend = BLEND_DEFAULT;
}

CSwf::~CSwf()
{
// 	uninitTexture();

// 	SAFE_DELETE(m_pRoot);
	//SAFE_DELETE(m_pPlayer);
}

bool CSwf::initTexture()
{
// 	HRESULT hr = cls_gl->m_pDrawListener->m_D3DDev->CreateTexture(cls_gl->m_winSize.width,cls_gl->m_winSize.height
// 		,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pTex,NULL);
// 	m_gQuad.tex = m_pTex;
// 
 	return true;
}

void CSwf::uninitTexture()
{
// 	SAFE_RELEASE(m_pTex);
}

bool CSwf::loadFile(gameswf::player* pPlayer,const char* swf)
{
// 	if( !pPlayer || !swf || *swf==0)
// 		return false;
// 
// 	m_pRoot = pPlayer->load_file(swf);
// 
// 	m_pRoot->set_display_viewport(0,0,cls_gl->m_winSize.width,cls_gl->m_winSize.height);
// 	m_pRoot->set_background_alpha(0.0f);
	return /*!!m_pRoot*/true;
}

void CSwf::resumePlay()
{
	m_bPlay = true;
}

void CSwf::update(unsigned int delta_ms)
{
// 	if(!m_pTex)
// 		return ;
// 
// 	if(!m_bPlay)
// 		return ;
// 
// 	IDirect3DSurface9* pTexTarget = NULL;
// 	HRESULT hr = m_pTex->GetSurfaceLevel(0,&pTexTarget);
// 
// 	cls_gl->m_pDrawListener->reBeginScene(false,0x0,pTexTarget);
// 	m_pRoot->display();
// 
// 	cls_gl->m_pDrawListener->reEndScene(false,false);
// 
// 	if(pTexTarget)
// 		pTexTarget->Release();
// 
// 	if(m_pSwfListener && m_pRoot->get_current_frame() == 0)
// 		m_pSwfListener->onSwfStart();
// 
// 	if(m_pRoot->get_current_frame()+1 >= m_pRoot->get_frame_count())
// 	{
// 		//设置可以重新开始的状态
// 		m_pRoot->restart_frame();
// 
// 		//只播放一次
// 		if(!m_bLoop)
// 		{
// 			m_bPlay = false;
// 
// 			if(m_pSwfListener)
// 				m_pSwfListener->onSwfEnd();
// 			return ;
// 		}
// 	}
// 	m_pRoot->advance(delta_ms/1000.0f);
}

void CSwf::render()
{
// 	if(!m_bPlay)
// 		return ;
// 
// 	if(m_pSwfListener)
// 		m_pSwfListener->onSwfFrameRender(m_pRoot->get_current_frame());
// // 	m->display();
// 	if(cls_gl->m_pDrawListener)
// 		cls_gl->m_pDrawListener->renderQuad(&m_gQuad);
}
