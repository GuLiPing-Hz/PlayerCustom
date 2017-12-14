/*
	注释时间:2014-4-25
	author: glp
	CDrawListener 管理整个绘画的进程，控制渲染的逻辑，包含对3D方面的初始化，创建等等。
	还包括顶层的渲染，以及对渲染对象的管理，还有评分的线程的管理
*/
#pragma once
#include "timerex.h"

#include "Opengl.h"
#include "Florid/FloridMgr.h"
#include "staff/object/SceneMgr.h"
#include "staff/image/ImgsetMgr.h"
#include "DX/RenderEngine.h"
#include "SelectPage.h"
// #include "dx/Wizard.h"

class CGLBlackCurtain;
class COpenGLWindow; 

class CDrawListener :public ITimerListener,public CRenderEngine/*,public CMultiVMR9Wizard*/
{
public:
	CDrawListener(HRESULT& hr,HWND wnd,CGLVideo* pglVideo,bool zBuffer);
	~CDrawListener(void);

public:
	//ITimerListener Implement
	virtual void				onTimer( unsigned int elapsed_tm );//毫秒
	virtual void 				setStarTime(uint start_ms);
	virtual void 				setLastTime(uint elapsed_ms){m_lastElapsedTime = elapsed_ms;}
	virtual void 				setEndTime(uint end_ms){m_end_ms = end_ms;}
	virtual void 				setStopTime(uint elapsed_ms);
	virtual void 				resetTime();
	virtual void				initGraph(ulong ms);
	void							onLostDevice();
	bool							initLost(bool fromLost=false);//设备丢失的处理

	//针对 播放swf时候需要做的特殊处理
	void							initProjectionWorldView();
	virtual bool 			initViewD3D();
	virtual void 				endThread();
	virtual void 				setBlackCurtain(bool b){m_bBlackCurtain=b;}

//////////////////////////////////////////////////////////////////////////
	void 						initListener();
	CGLVideo*				getGLVideo();
	CFloridMgr*			getFloridMgr();
	
	void							steStopSelect(bool bStop){if(m_pSelectPage)m_pSelectPage->setStop(bStop);}

	bool		reBeginScene(bool bHY,unsigned int color,void* pTarget=NULL);
	void		renderLine(float x1, float y1, float x2, float y2, DWORD color, float z);
	void		renderLineStrip(const CUSTOMVERTEX* vertex,int count,int blend,bool direct=true);//need to add
	void		renderTriangleStrip(const reVertex* batch,bool direct=true);
	void		renderTriangle(const reVertex* batch);
	void		renderTriangle(const reTriangle *triangle);
	void		renderQuad(const reQuad *quad,bool direct=true);
	void		reEndScene(bool bHY,bool bPresent=true);
	
	void     setBlendMode();
	void		createEvalThread();
protected:
	void     setBlendMode(int blend);
	void		renderBatch(bool bEndScene=false);
private:
	//播放选择歌曲画面（还没完全开发完）
	void				playSelect(unsigned int timer_deltams);
	//播放MV
	void				playMV(unsigned int timer_deltams); 
	HRESULT		fillIndexBuffer (uint batch_buffer_size/*,uint batch_buffer_size2*/);
	HRESULT		initIndexVertexBuffer(IDirect3DDevice9* d3dDev, uint max_batch_buffer_size=BATCH_BUFFER_SIZE);
public:
	uint 									m_startTime;//毫秒
	uint 									m_lastElapsedTime;
	uint									m_end_ms;

	unsigned int					m_cur_time_delta_ms;
private:
	IDirect3DIndexBuffer9*		m_indexBatchBuffer;
	int										m_nVertexBatchBufferSize;
	IDirect3DVertexBuffer9*	m_vertexBatchBuffer;
	bool										m_bD3DThingsInit;

	CUSTOMVERTEX*				m_pBatchVertices;//显存中的地址块 通常是后备缓冲区
	int										m_nCurPrimCount;//图元个数
	int										m_nCurVertexCount;//当前使用的顶点数量
	eDrawType							m_eCurPrimType;//描绘类型
	int										m_nCurBlendMode;//当前的渲染风格
	void*									m_pCurTexture;//当前使用的纹理

	HANDLE							m_hThreadEval;
	CGLBlackCurtain*			m_pGLBlackCurtain;//黑幕
	CSelectPage*					m_pSelectPage;//随机选择渲染

	ulong								m_pre_ms;
	ulong								m_cur_ms;
	ulong								m_timer_curms;
	ulong								m_duration_ms;

public:
	bool									m_bBlackCurtain;//是否开启黑幕
	bool									m_bAntiphonalSinging;
};
