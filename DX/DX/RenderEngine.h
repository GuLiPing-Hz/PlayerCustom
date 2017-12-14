/*
	注释时间:2014-4-25
	author: glp
	封装一个CRenderEngine类，用于创建Direct3D设备，以及对设备的管理，实现自定义的VMR9的Allocator
	这样可以捕获视频的每一帧，保存在Texture中，在适当的时候把它渲染出来
*/

#if !defined(AFX_ALLOCATOR_H__F675D766_1E57_4269_A4B9_C33FB672B856__INCLUDED_)
#define AFX_ALLOCATOR_H__F675D766_1E57_4269_A4B9_C33FB672B856__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vmr9.h>
#include <Wxutil.h>
#include "../Opengl.h"

#pragma warning(push, 2)

// C4995'function': name was marked as #pragma deprecated
//
// The version of vector which shipped with Visual Studio .NET 2003 
// indirectly uses some deprecated functions.  Warning C4995 is disabled 
// because the file cannot be changed and we do not want to 
// display warnings which the user cannot fix.
#pragma warning(disable : 4995)

#include <vector>
#pragma warning(pop)
using namespace std;

class CGLVideo;
// #include "MultiVMR9.h"
class CRenderEngine  : public  IVMRSurfaceAllocator9,public IVMRImagePresenter9
{
public:
    CRenderEngine(HRESULT& hr,const std::string szIniFile, HWND wnd, HWND hHY,const GSize &winsize,CGLVideo* pglVideo,bool bWindowed,bool zBuffer=true);
    virtual ~CRenderEngine();

	ULONG ReleaseD3D()
	{
		int nCount;
		do
		{
			nCount = (int)m_D3DDev->Release();
		}while(nCount>0);

		return nCount;
	}
    
	// IVMRSurfaceAllocator9
	virtual HRESULT STDMETHODCALLTYPE InitializeDevice( 
		/* [in] */ DWORD_PTR dwUserID,
		/* [in] */ VMR9AllocationInfo *lpAllocInfo,
		/* [out][in] */ DWORD *lpNumBuffers);

	virtual HRESULT STDMETHODCALLTYPE TerminateDevice( 
		/* [in] */ DWORD_PTR dwID);

	virtual HRESULT STDMETHODCALLTYPE GetSurface( 
		/* [in] */ DWORD_PTR dwUserID,
		/* [in] */ DWORD SurfaceIndex,
		/* [in] */ DWORD SurfaceFlags,
		/* [out] */ IDirect3DSurface9 **lplpSurface);

	virtual HRESULT STDMETHODCALLTYPE AdviseNotify( 
		/* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify);

	// IVMRImagePresenter9
	virtual HRESULT STDMETHODCALLTYPE StartPresenting( 
		/* [in] */ DWORD_PTR dwUserID);

	virtual HRESULT STDMETHODCALLTYPE StopPresenting( 
		/* [in] */ DWORD_PTR dwUserID);

	virtual HRESULT STDMETHODCALLTYPE PresentImage( 
		/* [in] */ DWORD_PTR dwUserID,
		/* [in] */ VMR9PresentationInfo *lpPresInfo);
    
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        REFIID riid,
        void** ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

	//创建Direct3DDevice9
	HRESULT CreateDevice(const std::string szIniFile,bool bWindowed);

	void     onLostDevice();
	bool     initLost(bool fromLost);
	bool		reBeginScene(bool bHY,unsigned int color,void* pTarget);
	void		reEndScene(bool bHY,bool bPresent);

	void applyProjectionMatrix(const D3DXMATRIX& m);
	void applyViewMatrix(const D3DXMATRIX& m);
	void	applyWorldMatrix(const D3DXMATRIX& m);
	void applyColorSwf(unsigned long color);
	//获取私有的纹理，缓存视频帧
	IDirect3DTexture9* getPrivateTexture(){return m_pTexture1;}
protected:
	int _format_id(D3DFORMAT fmt);
    //删除表面
    void DeleteSurfaces();
	//设备关闭
	void Shutdown();
    bool NeedToHandleDisplayChange();
    // 处理捕获的纹理
    HRESULT PresentHelper(VMR9PresentationInfo *lpPresInfo);

protected:
    // needed to make this a thread safe object
	int																		m_nDeviceCreate;
    CCritSec																m_ObjectLock;
    HWND																	m_window;
	HWND																	m_hHY;
    long																		m_refCount;

	D3DPRESENT_PARAMETERS*		m_d3dpp;

	D3DPRESENT_PARAMETERS		m_d3dppHY;//幻影窗口
	D3DPRESENT_PARAMETERS		m_d3dppW;//窗口
	D3DPRESENT_PARAMETERS		m_d3dppFS;//全屏

	LPDIRECT3D9													m_D3D;//CComPtr<IDirect3D9>
	LPDIRECT3DSWAPCHAIN9							m_D3DSwapChain;
	LPDIRECT3DSURFACE9									m_pSwapChainBackBuffer;
    IVMRSurfaceAllocatorNotify9*						m_lpIVMRSurfAllocNotify;//CComPtr<IVMRSurfaceAllocatorNotify9>
    vector<CComPtr<IDirect3DSurface9> >		m_surfaces;

	bool																	m_bControlTexture;
    IDirect3DTexture9*										m_pTexture1;
	IDirect3DTexture9*										m_pTexture2;
	D3DPRESENT_PARAMETERS							m_d3dPresent;
	D3DCAPS9														m_d3dCaps;

	bool																	m_bInit;
	bool																	m_buseSameNotifyDev;

	int																	m_nScreenBPP;
	bool																	m_bFPSVSYNC;
	bool																	m_bZBuffer;
	GSize																m_winSize;
	std::string														m_szIniFile;

public:
	IDirect3DSurface9*										m_pRenderTarget;
	LPDIRECT3DDEVICE9										m_D3DDev;//CComPtr<IDirect3DDevice9>
	CGLVideo*														m_pglVideo;//视频
};

#endif // !defined(AFX_ALLOCATOR_H__F675D766_1E57_4269_A4B9_C33FB672B856__INCLUDED_)
