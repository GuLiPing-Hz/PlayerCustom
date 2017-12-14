//------------------------------------------------------------------------------
// File: Allocator.cpp
//
// Desc: DirectShow sample code - implementation of the CAllocator class
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
// 使用两个宏控制纹理的使用方式 DEBUG_USEDOUBLETEXTURE DEBUG_USEGETTEXTURE
#include "stdafx.h"
#include "util.h"
#include "RenderEngine.h"
#include "../Opengl.h"
#include "../GLVideo.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderEngine::CRenderEngine(HRESULT& hr,const std::string szIniFile, HWND wnd,HWND hHY,const GSize &winsize
							 , CGLVideo* pglVideo,bool bWindowed,bool zBuffer)
:m_refCount(1)
,m_D3D(NULL)
,m_D3DDev(NULL)
,m_window( wnd )
,m_hHY(hHY)
,m_pglVideo(pglVideo)
,m_bInit(false)
,m_lpIVMRSurfAllocNotify(NULL)
,m_buseSameNotifyDev(true)
,m_nScreenBPP(32)
,m_bFPSVSYNC(true)
,m_d3dpp(NULL)
,m_bZBuffer(zBuffer)
,m_winSize(winsize)
,m_D3DSwapChain(NULL)
,m_pSwapChainBackBuffer(NULL)
,m_bControlTexture(false)
,m_pTexture1(NULL)
,m_pTexture2(NULL)
,m_pRenderTarget(NULL)
{
    CAutoLock Lock(&m_ObjectLock);
    hr = E_FAIL;

    if( IsWindow( wnd ) == FALSE )
    {
        hr = E_INVALIDARG;
        return;
    }

    if( m_D3D == NULL )
    {	
        m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
		//coun 1
        if (m_D3D == NULL) {
            hr = E_FAIL;
            return;
        }
    }

    if( m_D3DDev == NULL )
    {
		m_szIniFile = szIniFile;
        hr = CreateDevice(m_szIniFile,bWindowed);
		//count 1
    }

	if (m_D3DDev)
	{
		if (m_pglVideo)
		{
			m_pglVideo->setOwnListener(this);
		}
	}
	
}

CRenderEngine::~CRenderEngine()
{
	//CGLVideo::releaseInstance();
	SAFE_DELETE(m_pglVideo);
    DeleteSurfaces();
	Shutdown();
}


int CRenderEngine::_format_id(D3DFORMAT fmt)
{
	switch(fmt) {
		case D3DFMT_R5G6B5:		return 1;
		case D3DFMT_X1R5G5B5:	return 2;
		case D3DFMT_A1R5G5B5:	return 3;
		case D3DFMT_X8R8G8B8:	return 4;
		case D3DFMT_A8R8G8B8:	return 5;
		default:				return 0;
	}
}

HRESULT CRenderEngine::CreateDevice(const std::string szIniFile,bool bWindowed)
{
	// Get adapter info
	//D3DADAPTER_IDENTIFIER9 AdID;
	UINT nModes, i;
	D3DFORMAT Format=D3DFMT_UNKNOWN;
//////////////////////////////////////////////////////////////////////////
    HRESULT hr;
    D3DDISPLAYMODE dm;
	// Set up Window presentation parameters
    hr = m_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dm);

	//窗口参数
	ZeroMemory(&m_d3dppW, sizeof(m_d3dppW));

	m_d3dppW.BackBufferWidth  = m_winSize.width;
	m_d3dppW.BackBufferHeight = m_winSize.height;
	m_d3dppW.BackBufferFormat = dm.Format;
	m_d3dppW.BackBufferCount  = 1;
	m_d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
	m_d3dppW.hDeviceWindow    = m_window;
	m_d3dppW.Windowed         = TRUE;
	//垂直同步
	if(m_bFPSVSYNC/*==HGEFPS_VSYNC*/)
		m_d3dppW.PresentationInterval=/*D3DPRESENT_INTERVAL_IMMEDIATE*/D3DPRESENT_INTERVAL_ONE;
	else
		m_d3dppW.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
	//else					  
	m_d3dppW.SwapEffect = /*D3DSWAPEFFECT_DISCARD*//*D3DSWAPEFFECT_COPY*/D3DSWAPEFFECT_FLIP;

	if(m_bZBuffer)
	{
		m_d3dppW.EnableAutoDepthStencil = TRUE;
		m_d3dppW.AutoDepthStencilFormat = D3DFMT_D24S8;//创建一个32位深度/模板缓存，其中24位为深度缓存，8位为模板缓存。
	}
//////////////////////////////////////////////////////////////////////////
	// Set up Full Screen presentation parameters
	int nAdapterCount = m_D3D->GetAdapterCount();
	//w riteLog("[DXLOG CAllocator::CreateDevice] : nAdapterCount:%d",nAdapterCount);
	int nDeviceCreate = 0;
	if ( nAdapterCount>=2 )//如果有2个显示器，则在副显示器创建device
	{
		uint nDeviceId = Ini_GetInt(szIniFile.c_str(),"ADAPTERSELECT","ADAPTERID",1);
		nModes=m_D3D->GetAdapterModeCount(nDeviceId,dm.Format);
		for(i=0; i<nModes; i++)
		{
			m_D3D->EnumAdapterModes(nDeviceId,dm.Format, i, &dm);
			if(dm.Width != (UINT)m_winSize.width || dm.Height != (UINT)m_winSize.height) continue;
			if(m_nScreenBPP==16 && (_format_id(dm.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if(_format_id(dm.Format) > _format_id(Format)) Format=dm.Format;
		}
		nDeviceCreate = nDeviceId;
	}
	else
	{
		nModes=m_D3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,dm.Format);
		for(i=0; i<nModes; i++)
		{
			m_D3D->EnumAdapterModes(D3DADAPTER_DEFAULT,dm.Format, i, &dm);
			if(dm.Width != (UINT)m_winSize.width || dm.Height != (UINT)m_winSize.height) continue;
			if(m_nScreenBPP==16 && (_format_id(dm.Format) > _format_id(D3DFMT_A1R5G5B5))) continue;
			if(_format_id(dm.Format) > _format_id(Format)) Format=dm.Format;
		}
	}

	m_D3D->GetDeviceCaps(nDeviceCreate,D3DDEVTYPE_HAL,&m_d3dCaps);

	if(Format == D3DFMT_UNKNOWN)
	{
		WLOGH_INFO << L"Can't find appropriate full screen video mode";
		if(!bWindowed) 
			return false;
	}
	//全屏参数
	ZeroMemory(&m_d3dppFS, sizeof(m_d3dppFS));

	m_d3dppFS.BackBufferWidth  = m_winSize.width;
	m_d3dppFS.BackBufferHeight = m_winSize.height;
	m_d3dppFS.BackBufferFormat = Format;
	m_d3dppFS.BackBufferCount  = 1;
	m_d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
	m_d3dppFS.hDeviceWindow    = m_window;
	m_d3dppFS.Windowed         = FALSE;

	m_d3dppFS.SwapEffect       = D3DSWAPEFFECT_FLIP;
	m_d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//垂直同步
	if(m_bFPSVSYNC)
		m_d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else		
		m_d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(m_bZBuffer)
	{
		m_d3dppFS.EnableAutoDepthStencil = TRUE;
		m_d3dppFS.AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	m_d3dpp = bWindowed ? &m_d3dppW : &m_d3dppFS;

	if(_format_id(m_d3dpp->BackBufferFormat) < 4) m_nScreenBPP=16;
	else m_nScreenBPP=32;
	//////////////////////////////////////////////////////////////////////////
	int vp = 0;
	if( m_d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) 
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
	m_nDeviceCreate = nDeviceCreate;
    FAIL_RET( m_D3D->CreateDevice(  nDeviceCreate,/*D3DADAPTER_DEFAULT*/
                                    D3DDEVTYPE_HAL,m_window,vp,m_d3dpp,
                                    &m_D3DDev) );
	/*D3DERR_DEVICELOST*/
	m_D3DDev->ShowCursor(FALSE);

// 	if (m_hHY)
// 	{
// 		memcpy(&m_d3dppHY,m_d3dpp,sizeof(m_d3dppHY));
// 		m_d3dppHY.hDeviceWindow = m_hHY;
// 		SAFE_RELEASE(m_D3DSwapChain);
// 		hr = m_D3DDev->CreateAdditionalSwapChain(&m_d3dppHY,&m_D3DSwapChain);
// 		if (FAILED(hr))
// 		{
// 			w riteLog("[DXLOG CAllocator::CreateDevice] : CreateAdditionalSwapChain error");
// 		}
// 	}
    return hr;
}
void CRenderEngine::onLostDevice()
{
	SAFE_RELEASE(m_pRenderTarget);
}
bool	CRenderEngine::initLost(bool fromLost)
{
	SAFE_RELEASE(m_pRenderTarget);
	m_D3DDev->GetRenderTarget( 0, &m_pRenderTarget );//设备丢失时，需要先释放，再重新创建，如果是设备切换(全屏/窗口)，需要全部重新创建一个设备
	return true;
}

bool CRenderEngine::reBeginScene(bool bHY,unsigned int color,void* pTarget)
{
	HRESULT hr = S_OK;
	if(!pTarget)
	{
		if (bHY)
		{
			if (m_pSwapChainBackBuffer)
				hr = m_D3DDev->SetRenderTarget(0,m_pSwapChainBackBuffer);
			// 	m_glVideoHY.drawVideo(m_D3DDev,texture);
		}
		else
		{
			if (m_pRenderTarget)
				hr = m_D3DDev->SetRenderTarget( 0, m_pRenderTarget );
		}
	}
	else
		hr = m_D3DDev->SetRenderTarget( 0, (IDirect3DSurface9*)pTarget );

	if(FAILED(hr))
	{
		WLOGH_INFO << L"target can't be setted!";
		return false;
	}
	hr = m_D3DDev->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , color, 1.0f, 0L );
	return SUCCEEDED(m_D3DDev->BeginScene());
}

void CRenderEngine::reEndScene(bool bHY,bool bPresent)
{
	m_D3DDev->EndScene();
	if(!bPresent)
		return ;

	if (bHY)
	{
		if (m_D3DSwapChain)
			m_D3DSwapChain->Present(NULL,NULL,NULL,NULL,0);
	}
	else
		m_D3DDev->Present (NULL, NULL, NULL, NULL); 
}


void CRenderEngine::applyProjectionMatrix(const D3DXMATRIX& m)
{
	if(m_D3DDev)
		m_D3DDev->SetTransform(D3DTS_PROJECTION, &m);
}
void CRenderEngine::applyViewMatrix(const D3DXMATRIX& m)
{
	if(m_D3DDev)
		m_D3DDev->SetTransform(D3DTS_VIEW, &m);
}

void	CRenderEngine::applyWorldMatrix(const D3DXMATRIX& m)
{
	if(m_D3DDev)
		m_D3DDev->SetTransform(D3DTS_WORLD, &m);
}

void CRenderEngine::applyColorSwf(unsigned long color)
{
	if(m_D3DDev)
		m_D3DDev->SetRenderState( D3DRS_TEXTUREFACTOR, color);
}

void CRenderEngine::Shutdown()
{
	int n=0;
	SAFE_RELEASE(m_pSwapChainBackBuffer);
	SAFE_RELEASE(m_D3DSwapChain);
	SAFE_RELEASE(m_pRenderTarget);

	if(m_D3DDev != NULL) 
		n = (int)m_D3DDev->Release();
// 	assert(n == 0);
// 	if (n)
// 		ReleaseD3D();
	SAFE_RELEASE(m_D3D);
	m_D3DDev = NULL;
}

void CRenderEngine::DeleteSurfaces()
{
    CAutoLock Lock(&m_ObjectLock);
	//Clear stream source
    // clear out the private texture
	SAFE_RELEASE(m_pTexture1);
	SAFE_RELEASE(m_pTexture2);

    for( size_t i = 0; i < m_surfaces.size(); ++i ) 
    {
        m_surfaces[i] = NULL;
    }
}


// IVMRSurfaceAllocator9
HRESULT CRenderEngine::InitializeDevice( 
            /* [in] */ DWORD_PTR dwUserID,
            /* [in] */ VMR9AllocationInfo *lpAllocInfo,
            /* [out][in] */ DWORD *lpNumBuffers)
{
    D3DCAPS9 d3dcaps;
    DWORD dwWidth = 1;
    DWORD dwHeight = 1;
    float fTU = 1.f;
    float fTV = 1.f;

    if( lpNumBuffers == NULL )
    {
        return E_POINTER;
    }

    if( m_lpIVMRSurfAllocNotify == NULL )
    {
        return E_FAIL;
    }

    HRESULT hr = S_OK;
	//count 9 
	//D3DCAPS2_DYNAMICTEXTURES支持动态纹理
    m_D3DDev->GetDeviceCaps( &d3dcaps );
	// 9
	//////////////////////////////////////////////////////////////////////////
	//m_D3DDev->Release();//glp
	//////////////////////////////////////////////////////////////////////////
    if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        while( dwWidth < lpAllocInfo->dwWidth )
            dwWidth = dwWidth << 1;
        while( dwHeight < lpAllocInfo->dwHeight )
            dwHeight = dwHeight << 1;

        fTU = (float)(lpAllocInfo->dwWidth) / (float)(dwWidth);
        fTV = (float)(lpAllocInfo->dwHeight) / (float)(dwHeight);
        m_pglVideo->setSrcRect( fTU, fTV );
        lpAllocInfo->dwWidth = dwWidth;
        lpAllocInfo->dwHeight = dwHeight;
    }

    // NOTE:
    // we need to make sure that we create textures because
    // surfaces can not be textured onto a primitive.
    lpAllocInfo->dwFlags = VMR9AllocFlag_OffscreenSurface;//VMR9AllocFlag_TextureSurface;

    DeleteSurfaces();
    m_surfaces.resize(*lpNumBuffers);
    hr = m_lpIVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(0) );

	SAFE_RELEASE(m_pTexture1);
	SAFE_RELEASE(m_pTexture2);
	//这里使用获取的方式，下面那种是因为之前有闪帧的问题，弄的双纹理缓冲,结果没有解决，其实是因为其他的问题导致的
	//这里已经解决了，通过创建一张离屏纹理表面 VMR9AllocFlag_OffscreenSurface，
	//以前用的只是纹理表面VMR9AllocFlag_TextureSurface
#ifdef DEBUG_USEGETTEXTURE
	D3DDISPLAYMODE dm; 
	FAIL_RET( m_D3DDev->GetDisplayMode(NULL,  & dm ) );
	// create the private texture
	
	FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
		1, 
		D3DUSAGE_RENDERTARGET, 
		dm.Format, 
		D3DPOOL_DEFAULT  /* default pool - usually video memory */, 
		& m_pTexture1, NULL ) );
#endif

#ifdef DEBUG_USEDOUBLETEXTURE
	FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
		1, 
		D3DUSAGE_RENDERTARGET, 
		dm.Format, 
		D3DPOOL_DEFAULT /* default pool - usually video memory */, 
		& m_pTexture2.p, NULL ) );
 
	m_pglVideo->setTexture(m_pTexture1,m_pTexture2);
#endif

    // If we couldn't create a texture surface and 
    // the format is not an alpha format,
    // then we probably cannot create a texture.
    // So what we need to do is create a private texture
    // and copy the decoded images onto it.
    if(FAILED(hr) && !(lpAllocInfo->dwFlags & VMR9AllocFlag_3DRenderTarget))
    {

        DeleteSurfaces();            
		m_surfaces.resize(*lpNumBuffers);

        // is surface YUV ?
        if (lpAllocInfo->Format > '0000') 
        {           
            D3DDISPLAYMODE dm; 
            FAIL_RET( m_D3DDev->GetDisplayMode(NULL,  & dm ) );

            // create the private texture
            FAIL_RET( m_D3DDev->CreateTexture(lpAllocInfo->dwWidth, lpAllocInfo->dwHeight,
                                    1, 
                                    D3DUSAGE_RENDERTARGET, 
                                    dm.Format, 
                                    D3DPOOL_DEFAULT /* default pool - usually video memory */, 
                                    & m_pTexture1, NULL ) );
        }

        
        lpAllocInfo->dwFlags &= ~VMR9AllocFlag_TextureSurface;
        lpAllocInfo->dwFlags |= VMR9AllocFlag_OffscreenSurface;
		//lpAllocInfo->dwFlags = VMR9AllocFlag_OffscreenSurface;

        FAIL_RET( m_lpIVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_surfaces.at(0) ) );
    }
	
    return NOERROR;//m_scene.Init(m_D3DD
}
            
HRESULT CRenderEngine::TerminateDevice( 
        /* [in] */ DWORD_PTR dwID)
{
    DeleteSurfaces();
	//m_D3DDev->Release();
    return S_OK;
}
    
HRESULT CRenderEngine::GetSurface( 
        /* [in] */ DWORD_PTR dwUserID,
        /* [in] */ DWORD SurfaceIndex,
        /* [in] */ DWORD SurfaceFlags,
        /* [out] */ IDirect3DSurface9 **lplpSurface)
{
    if( lplpSurface == NULL )
    {
        return E_POINTER;
    }

    if (SurfaceIndex >= m_surfaces.size() ) 
    {
        return E_FAIL;
    }

    CAutoLock Lock(&m_ObjectLock);

    return m_surfaces[SurfaceIndex].CopyTo(lplpSurface) ;
}
    
HRESULT CRenderEngine::AdviseNotify( 
        /* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify)
{
    CAutoLock Lock(&m_ObjectLock);

    HRESULT hr;
	int n;
	if (m_lpIVMRSurfAllocNotify/*!m_buseSameNotifyDev*/)
	{
		n = m_lpIVMRSurfAllocNotify->Release();
	}
    m_lpIVMRSurfAllocNotify = lpIVMRSurfAllocNotify;

	if(!m_lpIVMRSurfAllocNotify)
		return S_FALSE;

	assert(m_D3D != NULL);
	HMONITOR hMonitor = m_D3D->GetAdapterMonitor( m_nDeviceCreate );
	if (!hMonitor)
	{
		WLOGH_INFO << L"Adapter Monitor acquires failed";
		return E_FAIL;
	}
    FAIL_RET( m_lpIVMRSurfAllocNotify->SetD3DDevice( m_D3DDev, hMonitor ) );
	//count 8
    return hr;
}

HRESULT CRenderEngine::StartPresenting( 
    /* [in] */ DWORD_PTR dwUserID)
{
    CAutoLock Lock(&m_ObjectLock);

    ASSERT( m_D3DDev );
    if( m_D3DDev == NULL )
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CRenderEngine::StopPresenting( 
    /* [in] */ DWORD_PTR dwUserID)
{
    return S_OK;
}

HRESULT CRenderEngine::PresentImage( 
    /* [in] */ DWORD_PTR dwUserID,
    /* [in] */ VMR9PresentationInfo *lpPresInfo)
{
    HRESULT hr;
    CAutoLock Lock(&m_ObjectLock);

    // if we are in the middle of the display change
    if( NeedToHandleDisplayChange() )
    {
        // NOTE: this piece of code is left as a user exercise.  
        // The D3DDevice here needs to be switched
        // to the device that is using another adapter
    }

    hr = PresentHelper( lpPresInfo );

    // IMPORTANT: device can be lost when user changes the resolution
    // or when (s)he presses Ctrl + Alt + Delete.
    // We need to restore our video memory after that
    if( hr == D3DERR_DEVICELOST)
    {
		WLOGH_INFO << L"[DXLOG CAllocator::PresentImage] device lost";
        if (m_D3DDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) 
        {
			Shutdown();
            DeleteSurfaces();
            FAIL_RET( CreateDevice(m_szIniFile,false) );

            HMONITOR hMonitor = m_D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

            FAIL_RET( m_lpIVMRSurfAllocNotify->ChangeD3DDevice( m_D3DDev, hMonitor ) );

        }

        hr = S_OK;
    }

    return hr;
}

HRESULT CRenderEngine::PresentHelper(VMR9PresentationInfo *lpPresInfo)
{
    // parameter validation
    if( lpPresInfo == NULL )
    {
        return E_POINTER;
    }
    else if( lpPresInfo->lpSurf == NULL )
    {
        return E_POINTER;
    }

    CAutoLock Lock(&m_ObjectLock);
    HRESULT hr;

    // if we created a  private texture
    // blt the decoded image onto the texture.
    if((m_pTexture1 != NULL) && (m_pTexture2 != NULL))
    {   
		if (m_pglVideo->m_bUseTexture1)
		{
			CComPtr<IDirect3DSurface9> surface;
			FAIL_RET( m_pTexture2->GetSurfaceLevel( 0 , & surface.p ) );
			// copy the full surface onto the texture's surface
			FAIL_RET( m_D3DDev->StretchRect( lpPresInfo->lpSurf, NULL,surface, NULL,D3DTEXF_NONE ) );
			//m_pglVideo->setTexture(m_pTexture1,m_pTexture2);
			m_pglVideo->m_bUseTexture1 = false;
		}
		else
		{
			CComPtr<IDirect3DSurface9> surface;
			FAIL_RET( m_pTexture1->GetSurfaceLevel( 0 , & surface.p ) );
			// copy the full surface onto the texture's surface
			FAIL_RET( m_D3DDev->StretchRect( lpPresInfo->lpSurf, NULL,surface, NULL,D3DTEXF_NONE ) );
			//m_pglVideo->setTexture(m_pTexture1,m_pTexture2);
			m_pglVideo->m_bUseTexture1 = true;
		}
    }
	else if(m_pTexture1 != NULL)
	{
		IDirect3DDevice9* pSampleDevice = NULL;
		lpPresInfo->lpSurf->GetDevice( &pSampleDevice );
		CComPtr<IDirect3DSurface9> surface;
		FAIL_RET( m_pTexture1->GetSurfaceLevel( 0 , & surface.p ) );
		// copy the full surface onto the texture's surface
		FAIL_RET( pSampleDevice->StretchRect( lpPresInfo->lpSurf, NULL,surface, NULL,D3DTEXF_NONE ) );
		SAFE_RELEASE(pSampleDevice);
	}
    else // this is the case where we have got the textures allocated by VMR
         // all we need to do is to get them from the surface
    {
        IDirect3DTexture9* texture;//CComPtr<>
		
        FAIL_RET( lpPresInfo->lpSurf->GetContainer( IID_IDirect3DTexture9, (LPVOID*) & texture ) );    
		m_pglVideo->updateGLTextures(texture);
    }

    return hr;
}


bool CRenderEngine::NeedToHandleDisplayChange()
{
    if( ! m_lpIVMRSurfAllocNotify )
    {
        return false;
    }

    D3DDEVICE_CREATION_PARAMETERS Parameters;

    if( FAILED( m_D3DDev->GetCreationParameters(&Parameters) ) )
    {
        ASSERT( false );
        return false;
    }

	HMONITOR currentMonitor = m_D3D->GetAdapterMonitor( Parameters.AdapterOrdinal );

    HMONITOR hMonitor = m_D3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

    return hMonitor != currentMonitor;
}

// IUnknown
HRESULT CRenderEngine::QueryInterface( 
        REFIID riid,
        void** ppvObject)
{
    HRESULT hr = E_NOINTERFACE;

    if( ppvObject == NULL ) {
        hr = E_POINTER;
    } 
    else if( riid == __uuidof(IVMRSurfaceAllocator9) ) {
        *ppvObject = static_cast<IVMRSurfaceAllocator9*>( this );
        AddRef();
        hr = S_OK;
    }
	else if( riid == __uuidof(IVMRImagePresenter9) ) {
		*ppvObject = static_cast<IVMRImagePresenter9*>( this );
		AddRef();
		hr = S_OK;
	}
    else if( riid == IID_IUnknown ) {
        *ppvObject = 
            static_cast<IUnknown*>( 
            static_cast<IVMRSurfaceAllocator9*>( this ) );
        AddRef();
        hr = S_OK;    
    }

    return hr;
}

ULONG CRenderEngine::AddRef()
{
    return InterlockedIncrement(& m_refCount);
}

ULONG CRenderEngine::Release()
{
    ULONG ret = InterlockedDecrement(& m_refCount);
    if( ret == 0 )
    {
        delete this;
    }
    return ret;
}
