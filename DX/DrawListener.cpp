#include "StdAfx.h"
#include "DrawListener.h"
#include "GLBlackCurtain.h"
#include "GLListDef.h"
#include "staff/image/ImgsetMgr.h"
#include "staff/object/SceneMgr.h"
#include "dx/util.h"
#include "DX/Grapha.h"
#include "wave/WaveUnit.h"
// #include "staff/KKType.h"
#include "fto/eval.h"

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <math.h>

#include "OpenGLWindow.h"

extern CWaveUnit* g_WavRecorder;
COS_Timer g_Timer;//测试cpu占用时间

CCritSec								g_csGrade;
LISTGLREALTIMEGRADE		g_listRGradeLeft;
LISTGLREALTIMEGRADE		g_listRGradeRight;
LISTGLSENTENCEGRADE		g_listSGradeLeft;
LISTGLSENTENCEGRADE		g_listSGradeRight;
bool										g_bUpdateWait = true;
bool										g_bEvalThread = true;
bool										g_bRenderThread = true;
extern HANDLE					g_hEventEval;
extern HANDLE					g_hSemaphoreRender1;
extern HANDLE					g_hSemaphoreRender2;

#undef LEVEL_SCORE
#define LEVEL_SCORE(level,score) \
{\
	if((level) >= 2) \
	score = (score) * 1.3f; \
	if((score)>900) \
	score = ((score)-900)/4.0f+900;\
}


DWORD WINAPI EvalThread(LPVOID pParam)
{
	CDrawListener* pListenter = (CDrawListener*) pParam;
	assert(pListenter != NULL);
	SceneMgr* pSceneMgr = cls_gl->m_pSceneMgr;
	GLRealtimeGrade		realtimeGrade1;
	ZeroMemory(&realtimeGrade1,sizeof(realtimeGrade1));
	GLRealtimeGrade		realtimeGrade2;
	ZeroMemory(&realtimeGrade2,sizeof(realtimeGrade2));
	GLSentenceGrade	sentenceGrade1;
	ZeroMemory(&sentenceGrade1,sizeof(sentenceGrade1));
	GLSentenceGrade	sentenceGrade2;
	ZeroMemory(&sentenceGrade2,sizeof(sentenceGrade2));
	g_listRGradeLeft.clear();
	g_listRGradeRight.clear();
	g_listSGradeLeft.clear();
	g_listSGradeRight.clear();

	while (g_bEvalThread)
	{
		float accumulate_score1 = 0.0f;//音高评分
		float lyric_score1 = 0.0f;//歌词评分
		bool switch_sentence1 = false;

		//从线上过来的评分数据
		if(!cls_gl->m_gCurPlayInfo.m_isMV)//其实已经废弃了。。。先留着万一以后还用到
		{
			memset(&realtimeGrade1,0,sizeof(realtimeGrade1));
			GLSentenceLevel sentence_level = GLSENTENCEGRADELEVEL_NONE;

			{
				CAutoLock lock(&(cls_gl->m_otherRGLock));
				LISTGLREALTIMEGRADE& listOtherRG = cls_gl->m_listOtherRG;
				if(!listOtherRG.empty())
				{
					memcpy(&realtimeGrade1,&(cls_gl->m_listOtherRG.back()),sizeof(realtimeGrade1));
					listOtherRG.clear();
				}

				LISTGLSENTENCEGRADE& listOtherSG = cls_gl->m_listOtherSG;
				if(!listOtherSG.empty())
				{
					switch_sentence1 = listOtherSG.back().sentence_switch;
					accumulate_score1 = listOtherSG.back().sentence_pitch_score;
					sentence_level = listOtherSG.back().sentence_level;
					lyric_score1 = listOtherSG.back().sentence_lyric_score;
					listOtherSG.clear();
				}
			}

			{
				CAutoLock lock(&g_csGrade);
				if ( switch_sentence1 )
				{
					if(cls_gl->m_pSceneMgr->CONFIG_MATH_ADD == 1)
					{
						LEVEL_SCORE(sentence_level,accumulate_score1);
					}
					sentenceGrade1.sentence_pitch_score = accumulate_score1;
					sentenceGrade1.sentence_lyric_score = lyric_score1;
					g_listSGradeLeft.push_back(sentenceGrade1);
				}
				g_listRGradeLeft.push_back(realtimeGrade1);
			}
		}
		//播放器录音数据评分
		else if (g_WavRecorder && (g_WavRecorder->IsRecording()))
		{
			static CWave wave;
			
			int iResult = g_WavRecorder->getWaveForLyricByNumSamples(WAVEIN_SAMPLE_NUM,wave);
			if (iResult==WAVEIN_SAMPLE_NUM)
			{
				int nBufferSize = iResult*wave.GetFormat().wBitsPerSample/8;
				BYTE* pBuf1 = NULL;
				int length1 = 0;
				BYTE* pBuf2 = NULL;
				int length2 = 0;
				CWaveUnit::GetOneChannelOfSteroWaveData(wave,'l',pBuf1,length1,realtimeGrade1.cur_db);
				CWaveUnit::GetOneChannelOfSteroWaveData(wave,'r',pBuf2,length2,realtimeGrade2.cur_db);

				float pitchdiff1 = 1000.0f;
				int rank1 = 0;
				float fCombo1;
				_eSentenceGradeLevel sglevel1 = SENTENCEGRADELEVEL_NONE;
				//数据1 进行评分
				pSceneMgr->m_pIEval1->realtimeEval( pBuf1
					,nBufferSize
					,pSceneMgr->m_durationMS
					,realtimeGrade1.cur_pitch,pitchdiff1
					,realtimeGrade1.realtime_score,rank1
					,realtimeGrade1.sentence_index
					,accumulate_score1
					,switch_sentence1,fCombo1
					,sentenceGrade1.sentence_lyric_score,sglevel1,false);

				realtimeGrade1.cur_tm = pSceneMgr->m_durationMS;

				float pitchdiff2 = 1000.0f;
				int rank2 = 0;
				float fCombo2;
				_eSentenceGradeLevel sglevel2 = SENTENCEGRADELEVEL_NONE;
				bool switch_sentence2 = false;
				float accumulate_score2 = 0.0f;
				//数据2 进行评分
				pSceneMgr->m_pIEval2->realtimeEval( pBuf2
					,nBufferSize
					,pSceneMgr->m_durationMS
					,realtimeGrade2.cur_pitch,pitchdiff2
					,realtimeGrade2.realtime_score,rank2
					,realtimeGrade2.sentence_index
					,accumulate_score2
					,switch_sentence2,fCombo2
					,sentenceGrade2.sentence_lyric_score,sglevel2,false);

				realtimeGrade2.cur_tm = pSceneMgr->m_durationMS;


				//释放内存
				SAFE_DELETE(pBuf1);
				SAFE_DELETE(pBuf2);

				{
					CAutoLock lock(&g_csGrade);
					if ( switch_sentence1 )
					{
						if(cls_gl->m_pSceneMgr->CONFIG_MATH_ADD == 1)
						{
							LEVEL_SCORE(sglevel1,accumulate_score1);
						}
						//for debug;
						sentenceGrade1.sentence_pitch_score = accumulate_score1;
						g_listSGradeLeft.push_back(sentenceGrade1);
					}
					g_listRGradeLeft.push_back(realtimeGrade1);

					if ( switch_sentence2 )
					{
						if(cls_gl->m_pSceneMgr->CONFIG_MATH_ADD == 1)
						{
							LEVEL_SCORE(sglevel2,accumulate_score2);
						}
						sentenceGrade2.sentence_pitch_score = accumulate_score2;
						g_listSGradeRight.push_back(sentenceGrade2);
					}
					g_listRGradeRight.push_back(realtimeGrade2);
				}
			}
		}
		else
		{
			break;
		}
		WaitForSingleObject(g_hEventEval,50);
	}
	return 0;
}

CDrawListener::CDrawListener(HRESULT& hr,HWND wnd,CGLVideo* pglVideo,bool zBuffer)
:CRenderEngine(hr,cls_gl->m_szIniFile,wnd,cls_gl->m_hHY,cls_gl->m_winSize,pglVideo,cls_gl->m_bWindowed,zBuffer)
,m_pSelectPage(NULL)
,m_hThreadEval(NULL)
,m_indexBatchBuffer(NULL)
,m_vertexBatchBuffer(NULL)
,m_nVertexBatchBufferSize(0)
,m_nCurPrimCount(0)
,m_eCurPrimType(REPRIM_QUADS)
,m_nCurBlendMode(BLEND_DEFAULT)
,m_pBatchVertices(NULL)
,m_bD3DThingsInit(false)
,m_pCurTexture(NULL)
,m_nCurVertexCount(0)
,m_cur_time_delta_ms(0)
{
	assert(cls_gl != NULL);
}

CDrawListener::~CDrawListener(void)
{
	m_pBatchVertices = NULL;
	if(m_vertexBatchBuffer)
	{
		m_D3DDev->SetStreamSource( 0, NULL,0, sizeof(CUSTOMVERTEX) );
		m_vertexBatchBuffer->Release();
	}

	if (m_indexBatchBuffer)
	{
		if (m_D3DDev)
			m_D3DDev->SetIndices(NULL);
		m_indexBatchBuffer->Release();
	}
	SAFE_CLOSEHANDLE(m_hThreadEval);
	SAFE_DELETE(m_pSelectPage);
}

void CDrawListener::initListener()
{
	ASSERT(cls_gl != NULL);

	setLastTime(cls_gl->m_gCurPlayInfo.m_song_start_ms);
	setEndTime(cls_gl->m_gCurPlayInfo.m_song_end_ms);

	m_bAntiphonalSinging = cls_gl->m_gCurPlayInfo.m_bAntiphonalSinging;
	m_startTime = 0;
	m_lastElapsedTime = 0;
	m_bBlackCurtain = false;
	m_pre_ms = 0;
	m_cur_ms = 0;
	m_timer_curms = 0;
	m_duration_ms = 0;
	m_end_ms = 0;

	if (m_pglVideo)
		m_pglVideo->updateGLTextures(NULL);
	SAFE_DELETE(m_pSelectPage);
	m_pSelectPage = new CSelectPage(cls_gl->m_winSize);
}

CGLVideo*	CDrawListener::getGLVideo()
{
	return m_pglVideo;
}

void CDrawListener::setStarTime(unsigned int start_ms)
{
	m_startTime = start_ms;
}
void CDrawListener::setStopTime(unsigned int elapsed_ms)
{
	m_lastElapsedTime += elapsed_ms-m_startTime;
}

void CDrawListener::resetTime()
{
	m_startTime = 0;
	m_lastElapsedTime = 0;
}

HRESULT CDrawListener::fillIndexBuffer (uint batch_buffer_size)
{
	HRESULT hr = E_FAIL;
	int index = 0;
	short* indices = NULL;

	uint all_buffer_size = batch_buffer_size;
	//Lock index buffer
	hr = m_indexBatchBuffer->Lock (0,all_buffer_size  * 3, (void**) &indices, 0);
	if (FAILED(hr))
	{
		WLOGH_ERRO << L"index batch buffer lock failed";
		return hr;
	}
	//设定顺时针描绘，0左上，1左下，2右上，3右下
	for (uint vertex = 0; vertex < all_buffer_size; vertex += 4)
	{
		indices[index] = vertex;
		indices[index + 1] = vertex + 2;
		indices[index + 2] = vertex + 3;
		indices[index + 3] = vertex;
		indices[index + 4] = vertex + 3;
		indices[index + 5] = vertex + 1;
		index += 6;
	}

	//Unlock index buffer
	m_indexBatchBuffer->Unlock ();
	return S_OK;
}

HRESULT CDrawListener::initIndexVertexBuffer(IDirect3DDevice9* d3dDev, uint max_batch_buffer_size)
{
	HRESULT hr = E_FAIL;

	m_nVertexBatchBufferSize = max_batch_buffer_size;
	hr = d3dDev->CreateVertexBuffer(m_nVertexBatchBufferSize * sizeof(CUSTOMVERTEX)
		, /*D3DUSAGE_WRITEONLY*/D3DUSAGE_DYNAMIC,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_vertexBatchBuffer, NULL);
	if (FAILED(hr))
	{
		WLOGH_ERRO << L"create first vertexbuffer failed";
		return hr;
	}

	hr = d3dDev->CreateIndexBuffer ((max_batch_buffer_size) / 4* 6 * sizeof(short),D3DUSAGE_WRITEONLY
		, D3DFMT_INDEX16,D3DPOOL_MANAGED, &m_indexBatchBuffer, NULL);
	if (FAILED(hr))
	{
		WLOGH_ERRO << L"CreateIndexBuffer failed";
	}
	hr = d3dDev->SetIndices(m_indexBatchBuffer);
	if (FAILED(hr))
	{
		WLOGH_ERRO << L"SetIndices failed";
	}
	if(FAILED(hr = fillIndexBuffer(max_batch_buffer_size)))
	{
		WLOGH_ERRO << L"fillIndexBuffer failed";
		return hr;
	}

	d3dDev->SetFVF(D3DFVF_CUSTOMVERTEX);
	d3dDev->SetStreamSource( 0, m_vertexBatchBuffer,0, sizeof(CUSTOMVERTEX) );

	return S_OK;
}

void CDrawListener::endThread()
{
	if (cls_gl->m_gCurPlayInfo.m_bGradeSong)
	{
		g_bEvalThread = false;
		SetEvent(g_hEventEval);
		if(WaitForSingleObject(m_hThreadEval,50) == WAIT_TIMEOUT)
		{//如果阻塞中
			TerminateThread(m_hThreadEval,1);
		}
		SAFE_CLOSEHANDLE(m_hThreadEval);
	}
}

void	CDrawListener::initGraph(ulong ms)
{
	m_duration_ms = ms;
}

void	CDrawListener::onLostDevice()
{
	CRenderEngine::onLostDevice();

#ifdef SPECIAL_VERSION
	//cls_gl->m_pFloridMgr->clearFlorid();
	cls_gl->getImgsetMgr()->onLostDevice();
#endif
	

	m_pBatchVertices = NULL;
	if(m_vertexBatchBuffer)
	{
		m_D3DDev->SetStreamSource( 0, NULL,0, sizeof(CUSTOMVERTEX) );
		m_vertexBatchBuffer->Release();
	}

	if (m_indexBatchBuffer)
	{
		if (m_D3DDev)
			m_D3DDev->SetIndices(NULL);
		m_indexBatchBuffer->Release();
	}

	HRESULT hr = m_D3DDev->Reset(m_d3dpp);
	if(FAILED(hr))
		WLOGH_ERRO << L"device reset failed";
}

bool	CDrawListener::initLost(bool fromLost)
{
	CRenderEngine::initLost(fromLost);

// 	if(fromLost)
// 	{
// 		if(!cls_gl->m_pFloridMgr->initAllFlorid())
// 		{
// 			WLOGH_ERRO << L"miss some psi file";
// 			return false;
// 		}
// 	}

	if(fromLost)
	{
		bool b = cls_gl->getImgsetMgr()->initAllImagesets(true);
		if (!b)
		{
			WLOGH_ERRO << L"Textures init error";
			return false;
		}
	}

	HRESULT hr = initIndexVertexBuffer(m_D3DDev);
	if (FAILED(hr))
	{
		WLOGH_ERRO << L"initIndexBuffer failed";
		return false;
	}

	return initViewD3D();
}

void	CDrawListener::initProjectionWorldView()
{
	if(m_bZBuffer)
	{
		HRESULT hr;
		FAIL_RET3(m_D3DDev->SetRenderState(D3DRS_ZENABLE,TRUE));
		FAIL_RET3(m_D3DDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS));  //将深度测试函数设为D3DCMP_LESS
	}

	D3DXMATRIX matIdentity;
	//Setup an orthographic perspective
	if(m_bZBuffer)
	{
		D3DXMATRIX matPerspect;
		D3DXMatrixPerspectiveFovLH(&matPerspect,D3DXToRadian(10.5f),1.0f,0.0f,100.0f);
		applyProjectionMatrix(matPerspect);
		//m_D3DDev->SetTransform(D3DTS_PROJECTION,&matPerspect);
	}
	else
	{
		D3DXMATRIX matOrtho;
		D3DXMatrixOrthoLH (&matOrtho, (float) m_winSize.width, (float) m_winSize.height, 0.0f, 100.0f);
		applyProjectionMatrix(matOrtho);
	}
	D3DXMatrixIdentity (&matIdentity);
	applyWorldMatrix(matIdentity);

	D3DXVECTOR3 from( 0.0f, 0.0f, -1.0f );			//视点坐标
	D3DXVECTOR3 at( 0.0f, 0.0f, 0.0f );        		//目标点坐标
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );				//当前世界坐标系向上方向向量，通常是(0,1,0)
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, & from, & at, & up);
	applyViewMatrix(matView);
	//applyViewMatrix(matIdentity);
}

bool CDrawListener::initViewD3D()
{
	HRESULT hr;
	FAIL_RET2(hr = m_D3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));//
	FAIL_RET2(hr = m_D3DDev->SetRenderState(D3DRS_LIGHTING, FALSE));

	FAIL_RET2(hr = m_D3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
	FAIL_RET2(hr = m_D3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
	FAIL_RET2(hr = m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

	//FAIL_RET3(hr = m_D3DDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));alpha测试，类似于深度缓冲区的测试，
	//FAIL_RET3(hr = m_D3DDev->SetRenderState(D3DRS_ALPHAREF, 0x10));//对小于这个值的纹理不会描绘，当透明处理
	//FAIL_RET3(hr = m_D3DDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER));

	FAIL_RET2(hr = m_D3DDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));//设置u坐标， 纹理包装寻址方式
	FAIL_RET2(hr = m_D3DDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));//设置v坐标，纹理包装寻址方式
	FAIL_RET2(hr = m_D3DDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));//放大采用的方法
	FAIL_RET2(hr = m_D3DDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));//缩小采用的方法,
	//FAIL_RET2(hr = m_D3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));//MIP采用的方法

	//默认选取第一个Alpha值 D3DTOP_SELECTARG1
	FAIL_RET2(hr =m_D3DDev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE)); //设定纹理的alpha运算
	//FAIL_RET3(hr =m_D3DDev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE));//纹理颜色默认相乘

	if (m_hHY)
	{
		memcpy(&m_d3dppHY,m_d3dpp,sizeof(m_d3dppHY));
		m_d3dppHY.hDeviceWindow = m_hHY;
		SAFE_RELEASE(m_pSwapChainBackBuffer);
		SAFE_RELEASE(m_D3DSwapChain);
		hr = m_D3DDev->CreateAdditionalSwapChain(&m_d3dppHY,&m_D3DSwapChain);
		if (SUCCEEDED(hr))
		{
			m_D3DSwapChain->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&m_pSwapChainBackBuffer);
		}
		else
		{
			WLOGH_ERRO << L"CreateAdditionalSwapChain failed";
		}
	}

	m_pCurTexture = NULL;
	m_nCurPrimCount = 0;
	m_nCurVertexCount = 0;
	m_nCurBlendMode = BLEND_DEFAULT;
	m_pBatchVertices = NULL;

	initProjectionWorldView();
	//setBlendMode(m_nCurBlendMode);
	return S_OK;
}

void	CDrawListener::createEvalThread()
{
	if(m_hThreadEval != NULL)
		return ;

	if (cls_gl->m_gCurPlayInfo.m_isMV && cls_gl->m_gCurPlayInfo.m_bGradeSong)
	{
		g_bEvalThread = true;
		DWORD nID;
		m_hThreadEval = ::CreateThread(NULL,0,EvalThread,(void*)this,0,&nID);
	}	
}
void adjustVertex(CUSTOMVERTEX* pVertex,const GPoint& lefttop,const GPoint& leftbottom,const GPoint& rightbottom
				  ,const GPoint& righttop,unsigned long ltc,unsigned long lbc,unsigned long rbc,unsigned long rtc)
{
	//逆时针
	pVertex[0].point = lefttop; // 左上
	pVertex[1].point = leftbottom; // 左下
	pVertex[2].point = rightbottom; // 右下
	pVertex[3].point = lefttop; // 左上
	pVertex[4].point = rightbottom; // 右下
	pVertex[5].point = righttop; // 右上

	pVertex[0].colour = ltc;//argb
	pVertex[1].colour = lbc;
	pVertex[2].colour = rbc;
	pVertex[3].colour = ltc;//argb
	pVertex[4].colour = rbc;
	pVertex[5].colour = rtc;
}

bool	CDrawListener::reBeginScene(bool bHY,unsigned int color,void* pTarget)
{
	if(!CRenderEngine::reBeginScene(bHY,color,pTarget))
		return false;
#ifdef SPECIAL_VERSION
	return true;
#else
	if(!m_vertexBatchBuffer)
		return false;
	HRESULT hr = m_vertexBatchBuffer->Lock(0,0,(void **)&m_pBatchVertices,D3DLOCK_DISCARD);
	return !!m_pBatchVertices;
#endif//SPECIAL_VERSION
}

void CDrawListener::renderLine(float x1, float y1, float x2, float y2, DWORD color, float z)
{
	if(m_pBatchVertices)
	{
		if((m_eCurPrimType!=REPRIM_LINES) || ((m_nCurVertexCount+REPRIM_LINES)>m_nVertexBatchBufferSize) 
			|| m_pCurTexture || (m_nCurBlendMode!=BLEND_DEFAULT))
		{
			renderBatch();

			m_eCurPrimType=REPRIM_LINES;
			if(m_nCurBlendMode != BLEND_DEFAULT) setBlendMode(BLEND_DEFAULT);
			if(m_pCurTexture) { m_D3DDev->SetTexture(0, 0); m_pCurTexture=0; }
		}

		int i=m_nCurPrimCount*REPRIM_LINES;
		m_pBatchVertices[i].point.x = x1; 
		m_pBatchVertices[i+1].point.x = x2;
		m_pBatchVertices[i].point.y = y1; 
		m_pBatchVertices[i+1].point.y = y2;
		m_pBatchVertices[i].point.z   = m_pBatchVertices[i+1].point.z = z;
		m_pBatchVertices[i].colour    = m_pBatchVertices[i+1].colour = color;
		m_pBatchVertices[i].u = m_pBatchVertices[i+1].u = m_pBatchVertices[i].v = m_pBatchVertices[i+1].v = 0.0f;

		m_nCurPrimCount++;
		m_nCurVertexCount += REPRIM_LINES;
	}
}

void	CDrawListener::renderLineStrip(const CUSTOMVERTEX* vertex,int count,int blend,bool direct)
{
	if(m_pBatchVertices)
	{
		if((m_eCurPrimType!=REPRIM_LINESTRIP) || ((m_nCurVertexCount+count)>m_nVertexBatchBufferSize) 
			|| m_pCurTexture || (m_nCurBlendMode!=blend))
		{
			renderBatch();

			m_eCurPrimType=REPRIM_LINESTRIP;
			if(m_nCurBlendMode != blend) 
				setBlendMode(blend);
			if(m_pCurTexture) 
			{ 
				m_D3DDev->SetTexture(0, 0); 
				m_pCurTexture=0; 
			}
		}

		assert(count <= m_nVertexBatchBufferSize);
		memcpy(&m_pBatchVertices[m_nCurVertexCount],vertex,count*sizeof(CUSTOMVERTEX));

		m_nCurPrimCount += (count-1);
		m_nCurVertexCount += count;

		if(direct)
			renderBatch();
	}
}

void	CDrawListener::renderTriangleStrip(const reVertex* batch,bool direct)
{
	if(m_pBatchVertices&&batch)
	{
		if(batch->count < 3)
			return ;

		if((m_eCurPrimType!=REPRIM_TRIANGLESTRIP) || ((m_nCurVertexCount+(int)batch->count)>m_nVertexBatchBufferSize) 
			|| (m_pCurTexture!=batch->tex) || (m_nCurBlendMode!=batch->blend))
		{
			renderBatch();

			m_eCurPrimType=REPRIM_TRIANGLESTRIP;
			if(m_nCurBlendMode != batch->blend)
				setBlendMode(batch->blend);
			if(batch->tex != m_pCurTexture) 
			{
				m_D3DDev->SetTexture( 0, (IDirect3DBaseTexture9*)batch->tex );
				m_pCurTexture = batch->tex;
			}
		}

		memcpy(&m_pBatchVertices[m_nCurVertexCount], batch->v, sizeof(CUSTOMVERTEX)*batch->count);
		m_nCurPrimCount += (batch->count-2);
		m_nCurVertexCount += batch->count;

		if(direct)
			renderBatch();
	}
}

void	CDrawListener::renderTriangle(const reVertex* batch)
{
	if(m_pBatchVertices)
	{
		if(batch->count%3 != 0)
			return ;

		if((m_eCurPrimType!=REPRIM_TRIANGLES) || ((m_nCurVertexCount+(int)batch->count)>m_nVertexBatchBufferSize) 
			|| (m_pCurTexture!=batch->tex) || (m_nCurBlendMode!=batch->blend))
		{
			renderBatch();

			m_eCurPrimType=REPRIM_TRIANGLES;
			if(m_nCurBlendMode != batch->blend)
				setBlendMode(batch->blend);
			if(batch->tex != m_pCurTexture) 
			{
				m_D3DDev->SetTexture( 0, (IDirect3DTexture9*)batch->tex );
				m_pCurTexture = batch->tex;
			}
		}

		memcpy(&m_pBatchVertices[m_nCurVertexCount], batch->v, sizeof(CUSTOMVERTEX)*batch->count);
		m_nCurPrimCount += (batch->count/3);
		m_nCurVertexCount += batch->count;
	}
}
void CDrawListener::renderTriangle(const reTriangle *triangle)
{
	if(m_pBatchVertices)
	{
		if((m_eCurPrimType!=REPRIM_TRIANGLES) || ((m_nCurVertexCount+REPRIM_TRIANGLES)>m_nVertexBatchBufferSize)
			|| (m_pCurTexture!=triangle->tex) || (m_nCurBlendMode!=triangle->blend))
		{
			renderBatch();

			m_eCurPrimType=REPRIM_TRIANGLES;
			if(m_nCurBlendMode != triangle->blend) 
				setBlendMode(triangle->blend);
			if(triangle->tex != m_pCurTexture) {
				m_D3DDev->SetTexture( 0, (LPDIRECT3DTEXTURE9)triangle->tex );
				m_pCurTexture = triangle->tex;
			}
		}

		memcpy(&m_pBatchVertices[m_nCurVertexCount], triangle->v, sizeof(CUSTOMVERTEX)*REPRIM_TRIANGLES);
		m_nCurPrimCount++;
		m_nCurVertexCount += REPRIM_TRIANGLES;
	}
}

void CDrawListener::renderQuad(const reQuad *quad,bool direct)
{
	if(m_pBatchVertices)
	{
		if((m_eCurPrimType!=REPRIM_QUADS) || ((m_nCurVertexCount+REPRIM_QUADS)>m_nVertexBatchBufferSize) 
			|| (m_pCurTexture!=quad->tex) || (m_nCurBlendMode!=quad->blend))
		{
			renderBatch();

			m_eCurPrimType=REPRIM_QUADS;
			if(m_nCurBlendMode != quad->blend) setBlendMode(quad->blend);
			if(quad->tex != m_pCurTexture)
			{
				HRESULT hr = m_D3DDev->SetTexture( 0, (LPDIRECT3DTEXTURE9)quad->tex );
				if(FAILED(hr))
					int glp = 1;
				m_pCurTexture = quad->tex;
			}
		}
		memcpy(&m_pBatchVertices[m_nCurVertexCount], quad->v, sizeof(CUSTOMVERTEX)*REPRIM_QUADS);
		m_nCurPrimCount++;
		m_nCurVertexCount += REPRIM_QUADS;

// 		if(direct)
// 			renderBatch();
	}
}

void	CDrawListener::reEndScene(bool bHY,bool bPresent)
{
	renderBatch(true);
	CRenderEngine::reEndScene(bHY,bPresent);
}

void CDrawListener::setBlendMode()
{
	switch(m_nCurBlendMode & BLEND_COLORMASK)
	{
	case BLEND_COLORMUL:
		{
			m_D3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			break;
		}
	case BLEND_COLORADD:
		{
			m_D3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
			break;
		}
	case BLEND_COLORSELECT2:
		{
			m_D3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			break;
		}
	}

	switch(m_nCurBlendMode & BLEND_ALPHAMASK)
	{
	case BLEND_ALPHAADD:
		{
			m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			break;
		}
	case BLEND_ALPHABLEND:
		{
			m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			break;
		}
	}
}
void CDrawListener::setBlendMode(int blend)
{
	int nColorBlend = blend & BLEND_COLORMASK;
	if(nColorBlend != (m_nCurBlendMode & BLEND_COLORMASK))
	{
		switch(nColorBlend)
		{
		case BLEND_COLORMUL:
			{
				m_D3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				break;
			}
		case BLEND_COLORADD:
			{
				m_D3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
				break;
			}
		case BLEND_COLORSELECT2:
			{
				m_D3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
				break;
			}
		}
	}

	int nAlphaBlend = blend & BLEND_ALPHAMASK;
	if(nAlphaBlend != (m_nCurBlendMode & BLEND_ALPHAMASK))
	{
		if(nAlphaBlend&BLEND_ALPHABLEND)
			m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		else
			m_D3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	m_nCurBlendMode = blend;
}
void CDrawListener::renderBatch(bool bEndScene)
{
	if(m_pBatchVertices)
	{
		m_vertexBatchBuffer->Unlock();
		m_pBatchVertices = NULL;
		HRESULT hr;
		if(m_nCurPrimCount)
		{
			switch(m_eCurPrimType)
			{
			case REPRIM_QUADS:
				hr = m_D3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nCurPrimCount<<2, 0, m_nCurPrimCount<<1);
				break;
			case REPRIM_TRIANGLES:
				m_D3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_nCurPrimCount);
				break;
			case REPRIM_LINES:
				m_D3DDev->DrawPrimitive(D3DPT_LINELIST, 0, m_nCurPrimCount);
				break;
			case REPRIM_LINESTRIP:
				m_D3DDev->DrawPrimitive(D3DPT_LINESTRIP,0,m_nCurPrimCount);
				break;
			case REPRIM_TRIANGLESTRIP:
				m_D3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,m_nCurPrimCount);
				break;
			}

			m_nCurPrimCount=0;
			m_nCurVertexCount = 0;
		}

		if(!bEndScene) 
		{
			HRESULT hr = m_vertexBatchBuffer->Lock( 0, 0, (void**)&m_pBatchVertices, D3DLOCK_DISCARD );
		}
	}
}

void CDrawListener::playSelect(unsigned int timer_deltams)
{
// 	reBeginScene(false,0xff000000);
// 
// 	if (cls_gl->m_pSceneMgr)
// 		cls_gl->m_pSceneMgr->renderScene(m_D3DDev);
// 	m_pSelectPage->drawGraphBuffer();
// 
// 	reEndScene(false);
}

void CDrawListener::playMV(unsigned int timer_deltams)
{
	//timer_deltams = 16;
	bool bNeedAdjust = true;
	if(!cls_gl->m_gCurPlayInfo.m_isMV)//如果不是MV则不需要调整五线谱的展示
	{
		bNeedAdjust = false;
// 		if(!cls_gl->m_gCurPlayInfo.m_bAdvertisement)
// 		{
// 			m_pre_ms = m_cur_ms;
// 			m_cur_ms = cls_gl->m_nOhterCurTime;
// 			if(m_pre_ms == m_cur_ms)
// 				bNeedAdjust = false;
// 		}
	}
	else
	{
		m_pre_ms = m_cur_ms;
		SAFEPOINTER_CALL(cls_gl->m_pGraph)getPosition(m_cur_ms);
	}

	ulong elapsed_ms;
	elapsed_ms = m_cur_ms - m_pre_ms;

	if (elapsed_ms<=0 && cls_gl->m_gCurPlayInfo.m_isMV)//已经结束,或暂停,特殊情况，
	{
		if ((m_end_ms &&(m_cur_ms+1 >= m_end_ms)) ||  (m_cur_ms==m_duration_ms))
		{
			//elapsed_ms = timer_deltams;
		}
	}
	else
	{
		m_timer_curms += timer_deltams;
	}

	if(bNeedAdjust)
	{
		//如果时间相差变大，则调整时间
		if (fabsl(((float)m_timer_curms) - ((float)m_cur_ms)) > 100.0f)
		{
// 			char buf[260];
// 			sprintf(buf,"adjust time : m_timer_curms %d m_cur_ms %d\n",m_timer_curms,m_cur_ms);
// 			OutputDebugString(buf);
			m_timer_curms = m_cur_ms;
		}
	}

	elapsed_ms = timer_deltams;

#ifndef SPECIAL_VERSION
	if(reBeginScene(false,0xff000000))
	{
		//如果是MV的话
		if(cls_gl->m_gCurPlayInfo.m_isMV)//是MV才需要进行最后的结算
		{
			if (!cls_gl->m_bShowEndScore)//是否开启最后评分画面
			{
				if (m_pglVideo)
					m_pglVideo->render();
			}
			else
			{
				cls_gl->m_pSceneMgr->addObjectToRemove(SINGOBJF);
				cls_gl->m_pSceneMgr->addObjectToRemove(SINGOBJS);
			}
		}
		else if(cls_gl->m_gCurPlayInfo.m_bAdvertisement)
		{
			if (m_pglVideo)
				m_pglVideo->render();
		}
		
		cls_gl->m_pSceneMgr->realRender();
		reEndScene(false);
	}
#else//SPECIAL_VERSION
	if(reBeginScene(false,0xff000000))
	{
		CAutoLock lock(&cls_gl->m_endScoreLock);
		if(cls_gl->m_gCurPlayInfo.m_isMV)//是MV才需要进行最后的结算
		{
			if (!cls_gl->m_bShowEndScore)//是否开启最后评分画面
			{
				if (m_pglVideo)
					m_pglVideo->render();
			}
			else
			{
				cls_gl->m_pSceneMgr->addObjectToRemove(SINGOBJF);
				cls_gl->m_pSceneMgr->addObjectToRemove(SINGOBJS);
			}
		}
		else if(cls_gl->m_gCurPlayInfo.m_bAdvertisement)
		{
			if (m_pglVideo)
				m_pglVideo->render();
		}
		
		if (cls_gl->m_pSceneMgr)
			cls_gl->m_pSceneMgr->renderScene();
		reEndScene(false);
	}
	cls_gl->m_pImgsetMgr->clearAllQuad();
	cls_gl->m_pImgsetMgr->lockAll();

	cls_gl->m_pSceneMgr->realRender();
	cls_gl->m_pFloridMgr->renderAll();

	cls_gl->m_pImgsetMgr->unlockAll();
#endif//SPECIAL_VERSION

	if (cls_gl->m_gCurPlayInfo.m_bGradeSong)
	{
		if (cls_gl->m_pSceneMgr)
		{
			eShowWave eShowType = cls_gl->m_eShowType;
			switch(eShowType)
			{
			case SHOW_NONE:
				{
					//如果是对唱，需要开启第二评分
					cls_gl->m_pSceneMgr->realtimeEval(eShowType,m_timer_curms,m_bAntiphonalSinging);
					if(cls_gl->m_bChangedShowType)
					{
						cls_gl->m_pFloridMgr->stopAll();
						cls_gl->m_pSceneMgr->sleepObj(SINGOBJF);
						cls_gl->m_pSceneMgr->sleepObj(SINGOBJS);
						cls_gl->m_bChangedShowType = false;
					}
					break;
				}
			case SHOW_ONE:
				{
					cls_gl->m_pSceneMgr->realtimeEval(eShowType,m_timer_curms,false);
					if(cls_gl->m_bChangedShowType)
					{
						cls_gl->m_pSceneMgr->resumeObj(SINGOBJF);
						cls_gl->m_pSceneMgr->sleepObj(SINGOBJS);
						cls_gl->m_bChangedShowType = false;
					}
					break;
				}
			case SHOW_TWO:
				{
					cls_gl->m_pSceneMgr->realtimeEval(eShowType,m_timer_curms,true);
					if(cls_gl->m_bChangedShowType)
					{
						cls_gl->m_pSceneMgr->resumeObj(SINGOBJF);
						cls_gl->m_pSceneMgr->resumeObj(SINGOBJS);
						cls_gl->m_bChangedShowType = false;
					}
					break;
				}
			case SHOW_THREE:
				{
					cls_gl->m_pSceneMgr->realtimeEval(eShowType,m_timer_curms,true);
					if(cls_gl->m_bChangedShowType)
					{
						cls_gl->m_pSceneMgr->resumeObj(SINGOBJF);
						cls_gl->m_pSceneMgr->resumeObj(SINGOBJS);
						cls_gl->m_bChangedShowType = false;
					}
					break;
				}
			}
		}
	}
	else
	{
		if (cls_gl->m_gCurPlayInfo.m_isMV && m_cur_ms >= (m_duration_ms-2000))//是MV才需要进行最后的渐渐变黑
			m_bBlackCurtain = true;
	}

	if (cls_gl->m_pSceneMgr)
	{
		//渲染循环
		cls_gl->m_pSceneMgr->gameLoopUpdate(m_cur_ms,elapsed_ms);
	}
}

void CDrawListener::onTimer( unsigned int timer_deltams )//毫秒
{
	///////////更新
	m_cur_time_delta_ms = timer_deltams;
	char buf[260] = {0};
	if (m_bRunning)
	{
		switch( m_D3DDev->TestCooperativeLevel())
		{
		case D3D_OK:
			{
				if(cls_gl->m_bSelectSong)
					playSelect(timer_deltams);
				else
					playMV(timer_deltams);
				break;
			}
		case D3DERR_DEVICELOST:
			{
				WLOGH_ERRO << L"device lost!";
				break;
			}
		case D3DERR_DEVICENOTRESET:
			{
				WLOGH_ERRO << L"device not reset!";
				//onLostDevice();
				//initLost();
				cls_gl->postRealEndSongMessage();
				break;
			}
		 default:
			 {
				 //QuitGame();
				 break;
			 }
		}
	}
}
