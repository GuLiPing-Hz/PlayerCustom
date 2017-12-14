// gameswf_render_handler_d3d.cpp  Paul Kelly & Bob Ives, 2006

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// A gameswf::render_handler that uses DX8 or DX9.  
// Based on the xbox code by Thatcher Ulrich <http://tulrich.com> 2003

/*
modified by GLP 2014-09-02
bug fixed
*/

#include "stdafx.h"
#ifdef TEST_PLAY
#include "glp.h"
#else
#include "../../OpenGLWindow.h"
#endif
#include "gameswf/gameswf.h"
#include "gameswf/gameswf_log.h"
#include "gameswf/gameswf_types.h"
#include "base/image.h"
#include "base/container.h"

//#define _DX8

#if defined (_DX8)
# include <d3d8.h>
#else
# include <d3d9.h>
#endif

#if DIRECT3D_VERSION < 0x0900
# include <d3dx8tex.h>
# include <d3d8types.h>
typedef IDirect3D8              IDirect3D;
typedef IDirect3DDevice8        IDirect3DDevice;
typedef IDirect3DTexture8       IDirect3DTexture;
typedef IDirect3DBaseTexture8   IDirect3DBaseTexture;
typedef IDirect3DSurface8       IDirect3DSurface;
typedef IDirect3DVertexBuffer8  IDirect3DVertexBuffer;
typedef D3DVIEWPORT8            D3DVIEWPORT;
typedef BYTE                    tLock;
#else 
# include <d3dx9tex.h>
# include <d3d9types.h>
typedef IDirect3D9              IDirect3D;
typedef IDirect3DDevice9        IDirect3DDevice;
typedef IDirect3DTexture9       IDirect3DTexture;
typedef IDirect3DBaseTexture9   IDirect3DBaseTexture;
typedef IDirect3DSurface9       IDirect3DSurface;
typedef IDirect3DVertexBuffer9  IDirect3DVertexBuffer;
typedef D3DVIEWPORT9            D3DVIEWPORT;
typedef void                    tLock;
#endif

#include <string.h>


#define Z_DEPTH 1.0f

// Our custom FVF, which describes our custom vertex structure
//#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)

struct CUSTOMVERTEX2
{
	float x,y,z;    // position
	float tu, tv;   // texture coordinates
};

#define D3DFVF_CUSTOMVERTEX2 (D3DFVF_XYZ|D3DFVF_TEX1)


static inline float clamp( float x,float min,float max ) {
	union { float f; int hex; };
	f = x - min;
	hex &= ~hex>>31;
	f += min - max;
	hex &= hex>>31;
	f += max;
	return f;
}


// bitmap_info_d3d declaration
struct bitmap_info_d3d : public gameswf::bitmap_info
{
	unsigned int	m_texture_id;
	int m_width;
	int m_height;
	image::image_base* m_suspended_image;

	bitmap_info_d3d();
	~bitmap_info_d3d(){if(m_suspended_image)delete m_suspended_image;}
	void convert_to_argb(image::rgba* im);
	bitmap_info_d3d(image::rgb* im);
	bitmap_info_d3d(image::rgba* im);
	bitmap_info_d3d(int width, int height, Uint8* data);

	virtual int get_width() const { return m_width; }
	virtual int get_height() const { return m_height; }
};


struct render_handler_d3d : public gameswf::render_handler
{
	// Some renderer state.

	gameswf::matrix m_current_matrix;
	gameswf::cxform m_current_cxform;

	float m_display_width;
	float m_display_height;

	DWORD m_nMaxVertices;  // Max Vertices held in m_pVB
	//D3DVIEWPORT m_origVP;

	static D3DFORMAT m_FormatRGB;
	static D3DFORMAT m_FormatRGBA;
	static D3DFORMAT m_FormatA;
	static D3DXMATRIX m_ModelViewMatrix;
	static D3DXMATRIX m_ProjMatrix;
	static array<IDirect3DBaseTexture*> m_d3d_textures;

	void set_antialiased(bool enable)
	{
		// not supported
	}

	static void make_next_miplevel(int* width, int* height, Uint8* data)
		// Utility.  Mutates *width, *height and *data to create the
		// next mip level.
	{
		assert(width);
		assert(height);
		assert(data);

		int new_w = *width >> 1;
		int new_h = *height >> 1;
		if (new_w < 1) new_w = 1;
		if (new_h < 1) new_h = 1;

		if (new_w * 2 != *width  || new_h * 2 != *height)
		{
			// Image can't be shrunk along (at least) one
			// of its dimensions, so don't bother
			// resampling.  Technically we should, but
			// it's pretty useless at this point.  Just
			// change the image dimensions and leave the
			// existing pixels.
		}
		else
		{
			// Resample.  Simple average 2x2 --> 1, in-place.
			for (int j = 0; j < new_h; j++) {
				Uint8*  out = ((Uint8*) data) + j * new_w;
				Uint8*  in = ((Uint8*) data) + (j << 1) * *width;
				for (int i = 0; i < new_w; i++) {
					int a;
					a = (*(in + 0) + *(in + 1) + *(in + 0 + *width) + *(in + 1 + *width));
					*(out) = (Uint8) (a >> 2);
					out++;
					in += 2;
				}
			}
		}

		// Munge parameters to reflect the shrunken image.
		*width = new_w;
		*height = new_h;
	}

	struct fill_style
	{
		enum mode
		{
			INVALID,
			COLOR,
			BITMAP_WRAP,
			BITMAP_CLAMP,
			LINEAR_GRADIENT,
			RADIAL_GRADIENT,
		};
		mode  m_mode;
		gameswf::rgba m_color;
		const bitmap_info_d3d* m_bitmap_info;
		gameswf::matrix m_bitmap_matrix;
		gameswf::cxform m_bitmap_color_transform;
		bool  m_has_nonzero_bitmap_additive_color;

		fill_style()
			:
		m_mode(INVALID),
			m_has_nonzero_bitmap_additive_color(false)
		{
		}

		void  apply(const gameswf::matrix& current_matrix,int& blend,void*& tex) const
			// Push our style into D3D.
		{
			assert(m_mode != INVALID);

			IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
			if(!pD3dDevice)
				return ;

			if (m_mode == COLOR)
			{
				apply_color(m_color);

				blend = BLEND_ALPHABLEND|BLEND_COLORSELECT2;
				tex = NULL;

				//m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
				pD3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
			}
			else if (m_mode == BITMAP_WRAP
				|| m_mode == BITMAP_CLAMP)
			{
				assert(m_bitmap_info != NULL);

				apply_color(m_color);

				if (m_bitmap_info == NULL)
				{
					assert(0);

					blend = BLEND_ALPHABLEND|BLEND_COLORSELECT2;
					tex = NULL;
					//m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
					pD3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
				}
				else
				{
					// Set up the texture for rendering.

					// Do the modulate part of the color
					// transform in the first pass.  The
					// additive part, if any, needs to
					// happen in a second pass.

					//m_pd3dDevice->SetTexture(0, m_d3d_textures[m_bitmap_info->m_texture_id]);

					tex = m_d3d_textures[m_bitmap_info->m_texture_id];
					blend = BLEND_ALPHABLEND|BLEND_COLORMUL;

					//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					pD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

					if (m_mode == BITMAP_CLAMP)
					{
						pD3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
						pD3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
					}
					else
					{
						assert(m_mode == BITMAP_WRAP);
						pD3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
						pD3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
					}

					// Set up the bitmap matrix for texgen.
					float inv_width = 1.0f / m_bitmap_info->get_width();
					float inv_height = 1.0f / m_bitmap_info->get_height();

					gameswf::matrix m = m_bitmap_matrix;
					gameswf::matrix m_cm_inv;
					m_cm_inv.set_inverse(current_matrix);
					m.concatenate(m_cm_inv);

					D3DXMATRIXA16 mat;
					D3DXMatrixIdentity(&mat);

					mat._11 = m.m_[0][0] * inv_width;             mat._12 = m.m_[1][0] * inv_height;  mat._13 = 0.00f; mat._14 = 0.00f;
					mat._21 = m.m_[0][1] * inv_width;             mat._22 = m.m_[1][1] * inv_height;  mat._23 = 0.00f; mat._24 = 0.00f;
					mat._31 = 0.00f;                              mat._32 = 0.00f;                    mat._33 = 0.00f; mat._34 = 0.00f;
					mat._41 = m.m_[0][2] * inv_width;             mat._42 = m.m_[1][2] * inv_height;  mat._43 = 0.00f; mat._44 = 1.00f;

					pD3dDevice->SetTransform( D3DTS_TEXTURE0, &mat );
					pD3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
					pD3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
				}
			}
		}


		bool  needs_second_pass() const
			// Return true if we need to do a second pass to make
			// a valid color.  This is for cxforms with additive
			// parts.
		{
			if (m_mode == BITMAP_WRAP
				|| m_mode == BITMAP_CLAMP)
			{
				return m_has_nonzero_bitmap_additive_color;
			}
			else
			{
				return false;
			}
		}

		void  apply_second_pass(int& blend) const
			// Set D3D state for a necessary second pass.
		{
			assert(needs_second_pass());

			// Additive color.
			apply_color(gameswf::rgba(
				Uint8(m_bitmap_color_transform.m_[0][1]), 
				Uint8(m_bitmap_color_transform.m_[1][1]), 
				Uint8(m_bitmap_color_transform.m_[2][1]), 
				Uint8(m_bitmap_color_transform.m_[3][1])));

			blend = BLEND_ALPHAADD|BLEND_COLORSELECT2;
			//m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
			IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
			if(pD3dDevice)
				pD3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

			//       m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
			//       m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
		}

		void  cleanup_second_pass() const
		{
			//pD3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//pD3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}


		void  disable() { m_mode = INVALID; }
		void  set_color(gameswf::rgba color) 
		{ 
			m_mode = COLOR; 
			m_color = color; 
		}
		void  set_bitmap(const gameswf::bitmap_info* bi, const gameswf::matrix& m, bitmap_wrap_mode wm, const gameswf::cxform& color_transform)
		{
			m_mode = (wm == WRAP_REPEAT) ? BITMAP_WRAP : BITMAP_CLAMP;
			m_bitmap_info = (bitmap_info_d3d*)bi;
			m_bitmap_matrix = m;

			m_bitmap_color_transform.m_[0][0] = clamp(color_transform.m_[0][0], 0, 1);
			m_bitmap_color_transform.m_[1][0] = clamp(color_transform.m_[1][0], 0, 1);
			m_bitmap_color_transform.m_[2][0] = clamp(color_transform.m_[2][0], 0, 1);
			m_bitmap_color_transform.m_[3][0] = clamp(color_transform.m_[3][0], 0, 1);

			m_bitmap_color_transform.m_[0][1] = clamp(color_transform.m_[0][1], -255.0f, 255.0f);
			m_bitmap_color_transform.m_[1][1] = clamp(color_transform.m_[1][1], -255.0f, 255.0f);
			m_bitmap_color_transform.m_[2][1] = clamp(color_transform.m_[2][1], -255.0f, 255.0f);
			m_bitmap_color_transform.m_[3][1] = clamp(color_transform.m_[3][1], -255.0f, 255.0f);

			m_color = gameswf::rgba(
				Uint8(m_bitmap_color_transform.m_[0][0]*255.0f), 
				Uint8(m_bitmap_color_transform.m_[1][0]*255.0f), 
				Uint8(m_bitmap_color_transform.m_[2][0]*255.0f), 
				Uint8(m_bitmap_color_transform.m_[3][0]*255.0f));

			if (m_bitmap_color_transform.m_[0][1] > 1.0f
				|| m_bitmap_color_transform.m_[1][1] > 1.0f
				|| m_bitmap_color_transform.m_[2][1] > 1.0f
				|| m_bitmap_color_transform.m_[3][1] > 1.0f)
			{
				m_has_nonzero_bitmap_additive_color = true;
			}
			else
			{
				m_has_nonzero_bitmap_additive_color = false;
			}
		}
		bool  is_valid() const { return m_mode != INVALID; }
	};

	// Constructor.
	render_handler_d3d(IDirect3DDevice* device)
	{
		HRESULT hr;

		IDirect3DDevice9* pD3dDevice = device;
		if(!pD3dDevice)
			return ;
		pD3dDevice->AddRef();

		// Determine texture formats
		IDirect3D* l_pD3D;
		hr = pD3dDevice->GetDirect3D( &l_pD3D );
		assert(hr==S_OK);

#if DIRECT3D_VERSION < 0x0900
		D3DCAPS8 l_DeviceCaps;
#else
		D3DCAPS9 l_DeviceCaps;
#endif
		ZeroMemory( &l_DeviceCaps, sizeof(l_DeviceCaps) );  
		hr = pD3dDevice->GetDeviceCaps( &l_DeviceCaps );
		assert(hr==S_OK);

		D3DDISPLAYMODE l_DisplayMode;
		hr = pD3dDevice->GetDisplayMode( 
#if DIRECT3D_VERSION >= 0x0900
			0, 
#endif
			&l_DisplayMode );

		if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT1))) 
			m_FormatRGB = D3DFMT_DXT1;
		else if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_R8G8B8))) 
			m_FormatRGB = D3DFMT_R8G8B8;
		else       
			m_FormatRGB = D3DFMT_A8R8G8B8;

		if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT5))) 
			m_FormatRGBA = D3DFMT_DXT5;
		else
			m_FormatRGBA  = D3DFMT_A8R8G8B8;

		if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8))) 
			m_FormatA = D3DFMT_A8;
		else if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8L8))) 
			m_FormatA = D3DFMT_A8L8;
		else if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8P8))) 
			m_FormatA = D3DFMT_A8P8;
		else 
			m_FormatA = D3DFMT_A8R8G8B8;

		pD3dDevice->Release();
	}

	render_handler_d3d() :
	m_display_width(0),
		m_display_height(0)
	{
	}


	~render_handler_d3d()
		// Destructor.
	{
		for (int i = 0; i < m_d3d_textures.size(); i++)
		{
			if( m_d3d_textures[i] )
			{
				m_d3d_textures[i]->Release();
				m_d3d_textures[i] = 0;
			}
		}
	}

	void on_reset_device(IDirect3DDevice* device) 
	{
		HRESULT hr;

		IDirect3DDevice9* pD3dDevice = device;
		if(!pD3dDevice)
			return ;
		pD3dDevice->AddRef();

		// Determine texture formats
		IDirect3D* l_pD3D;
		hr = pD3dDevice->GetDirect3D( &l_pD3D );
		assert(hr==S_OK);

#if DIRECT3D_VERSION < 0x0900
		D3DCAPS8 l_DeviceCaps;
#else
		D3DCAPS9 l_DeviceCaps;
#endif
		ZeroMemory( &l_DeviceCaps, sizeof(l_DeviceCaps) );  
		hr = pD3dDevice->GetDeviceCaps( &l_DeviceCaps );
		assert(hr==S_OK);

		D3DDISPLAYMODE l_DisplayMode;
		hr = pD3dDevice->GetDisplayMode( 
#if DIRECT3D_VERSION >= 0x0900
			0, 
#endif
			&l_DisplayMode );

		if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT1))) 
			m_FormatRGB = D3DFMT_DXT1;
		else if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_R8G8B8))) 
			m_FormatRGB = D3DFMT_R8G8B8;
		else       
			m_FormatRGB = D3DFMT_A8R8G8B8;

		if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT5))) 
			m_FormatRGBA = D3DFMT_DXT5;
		else
			m_FormatRGBA  = D3DFMT_A8R8G8B8;

		if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8))) 
			m_FormatA = D3DFMT_A8;
		else if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8L8))) 
			m_FormatA = D3DFMT_A8L8;
		else if (SUCCEEDED(l_pD3D->CheckDeviceFormat(l_DeviceCaps.AdapterOrdinal, l_DeviceCaps.DeviceType, l_DisplayMode.Format, 0, D3DRTYPE_TEXTURE, D3DFMT_A8P8))) 
			m_FormatA = D3DFMT_A8P8;
		else 
			m_FormatA = D3DFMT_A8R8G8B8;

		pD3dDevice->Release();
	}

	void on_release_device() 
	{
	}

	// Style state.
	enum style_index
	{
		LEFT_STYLE = 0,
		RIGHT_STYLE,
		LINE_STYLE,

		STYLE_COUNT
	};
	fill_style  m_current_styles[STYLE_COUNT];

	gameswf::bitmap_info* create_bitmap_info_alpha(int w, int h, unsigned char* data)
	{
		return new bitmap_info_d3d(w, h, data);
	}


	gameswf::bitmap_info* create_bitmap_info_rgb(image::rgb* im)
		// Given an image, returns a pointer to a bitmap_info struct
		// that can later be passed to fill_styleX_bitmap(), to set a
		// bitmap fill style.
	{
		return new bitmap_info_d3d(im);
	}


	gameswf::bitmap_info* create_bitmap_info_rgba(image::rgba* im)
		// Given an image, returns a pointer to a bitmap_info struct
		// that can later be passed to fill_style_bitmap(), to set a
		// bitmap fill style.
		//
		// This version takes an image with an alpha channel.
	{
		return new bitmap_info_d3d(im);
	}


	gameswf::bitmap_info* create_bitmap_info_empty()
		// Creates and returns an empty bitmap_info structure.  Image data
		// can be bound to this info later, via set_alpha_image().
	{
		assert(0); //pk this function not implemented.
		//pk    return new bitmap_info_d3d(gameswf::bitmap_info::empty);
		return NULL;
	}


	void  set_alpha_image(gameswf::bitmap_info* bi, int w, int h, Uint8* data)
		// Set the specified bitmap_info so that it contains an alpha
		// texture with the given data (1 byte per texel).
		//
		// Munges *data (in order to make mipmaps)!!
	{
		assert(bi);
		assert(0);  // not tested

		//pk    bi->set_alpha_image(w, h, data);
	}


	void  delete_bitmap_info(gameswf::bitmap_info* bi)
		// Delete the given bitmap info struct.
	{
		delete bi;
	}

	void  begin_display(
		gameswf::rgba background_color,
		int viewport_x0, int viewport_y0,
		int viewport_width, int viewport_height,
		float x0, float x1, float y0, float y1)
	{
		if(!cls_gl->m_pDrawListener)
			return ;
		IDirect3DDevice* pD3dDev = cls_gl->m_pDrawListener->m_D3DDev;
		if(!pD3dDev)
			return ;

		D3DXMatrixIdentity(&m_ModelViewMatrix);
		D3DXMatrixIdentity(&m_ProjMatrix);

		// invert coordinate system from lower left to upper left

		float gsWidthDiv  = 1.0f / viewport_width;
		float gsHeightDiv = 1.0f / viewport_height;

		m_ModelViewMatrix._11 = 2.0f / (x1 - x0);
		m_ModelViewMatrix._22 = -2.0f / (y1 - y0);
		m_ModelViewMatrix._41 = -((x1 + x0) / (x1 - x0));
		m_ModelViewMatrix._42 = ((y1 + y0) / (y1 - y0));

		m_ProjMatrix._11 = viewport_width * gsWidthDiv;
		m_ProjMatrix._22 = viewport_height * gsHeightDiv;
		m_ProjMatrix._41 = -1.0f + viewport_x0 * 2.0f * gsWidthDiv + viewport_width * gsWidthDiv;
		m_ProjMatrix._42 = 1.0f - viewport_y0 * 2.0f * gsHeightDiv - viewport_height * gsHeightDiv;

		cls_gl->m_pDrawListener->applyProjectionMatrix(m_ModelViewMatrix);
		cls_gl->m_pDrawListener->applyViewMatrix(m_ProjMatrix);

		// Viewport.
		// 	  hr = pD3dDev->GetViewport(&m_origVP);
		// 	  assert(SUCCEEDED(hr));  // pure device?

		// 	  D3DVIEWPORT  vp;
		// 	  vp.X = viewport_x0;
		// 	  vp.Y = viewport_y0;
		// 	  vp.Width = viewport_width;
		// 	  vp.Height = viewport_height;
		// 	  vp.MinZ = 0.0f;
		// 	  vp.MaxZ = 100.0f;   // minZ = maxZ = 0 forces all polys to be in foreground
		// 	  pD3dDev->SetViewport(&vp);

		// Blending renderstates
		//pk not sure if we need these.
		// 	  pD3dDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		// 	  pD3dDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		// 	  pD3dDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		// 	  pD3dDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );


		pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

		// Textures off by default.
		pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

		// @@ for sanity's sake, let's turn off backface culling...
		// 	  pD3dDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);//xxxxx
		// 	  pD3dDev->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);  //xxxxx

		pD3dDev->SetVertexShader(NULL);
		// No pixel shader.
		pD3dDev->SetPixelShader(NULL);

		// Clear the background, if background color has alpha > 0.
		if (background_color.m_a > 0)
		{
			if( background_color.m_a==255 )
			{
				D3DRECT rect = {LONG(x0),LONG(y0),LONG(x1),LONG(y1)};
				pD3dDev->Clear(1, &rect,D3DCLEAR_TARGET,D3DCOLOR_XRGB(background_color.m_r, background_color.m_g, background_color.m_b),0.0f, 0 );
			}
			else
			{
				apply_color(background_color);
				set_matrix(gameswf::matrix::identity);
				apply_matrix(m_current_matrix);

				reQuad quad;
				memset(&quad,0,sizeof(quad));
				quad.blend = BLEND_DEFAULT;
				//quad.tex = NULL;
				quad.v[0].point = GPoint(-1.0f,  1.0f, Z_DEPTH); // 左上GPoint(x0,y1,Z_DEPTH);
				quad.v[1].point = GPoint(-1.0f, -1.0f, Z_DEPTH); // 左下GPoint(x0,y0,Z_DEPTH);
				quad.v[2].point = GPoint( 1.0f,  1.0f, Z_DEPTH); // 右上GPoint(x1,y1,Z_DEPTH);
				quad.v[3].point = GPoint( 1.0f, -1.0f, Z_DEPTH); // 右下GPoint(x1,y0,Z_DEPTH);

				if(cls_gl->m_pDrawListener)
					cls_gl->m_pDrawListener->renderQuad(&quad);
			}
		}
	}


	void  end_display()
	{

		if(!cls_gl->m_pDrawListener)//复位投影？
			return ;

		IDirect3DDevice* pD3dDev = cls_gl->m_pDrawListener->m_D3DDev;
		if(!pD3dDev)
			return ;

		pD3dDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);//依据drawlistener里面的设置
		pD3dDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		pD3dDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);//设置颜色采样参数
		pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		//pD3dDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);//设置颜色混合运算
		pD3dDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pD3dDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );//关闭从其他地方获取纹理uv坐标
		pD3dDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );//使用默认的纹理uv坐标
		cls_gl->m_pDrawListener->setBlendMode();

		//pD3dDev->SetViewport(&m_origVP);
		cls_gl->m_pDrawListener->initProjectionWorldView();
	}


	void  set_matrix(const gameswf::matrix& m)
		// Set the current transform for mesh & line-strip rendering.
	{
		m_current_matrix = m;
	}


	void  set_cxform(const gameswf::cxform& cx)
		// Set the current color transform for mesh & line-strip rendering.
	{
		m_current_cxform = cx;
	}

	void  apply_matrix(const gameswf::matrix& m)
		// Set the given transformation matrix.
	{
		D3DXMATRIX  mat;
		D3DXMatrixIdentity( &mat );
		// row 0
		mat._11 = m.m_[0][0];             mat._12 = m.m_[1][0];  mat._13 = 0.00f; mat._14 = 0.00f;
		mat._21 = m.m_[0][1];             mat._22 = m.m_[1][1];  mat._23 = 0.00f; mat._24 = 0.00f;
		mat._31 = 0.00f;                  mat._32 = 0.00f;                    mat._33 = 1.00f; mat._34 = 0.00f;
		mat._41 = m.m_[0][2];             mat._42 = m.m_[1][2];  mat._43 = 0.00f; mat._44 = 1.00f;

		if(cls_gl->m_pDrawListener)
			cls_gl->m_pDrawListener->applyWorldMatrix(mat);
	}

	static void apply_color(const gameswf::rgba& c)
		// Set the given color.
	{
		if(cls_gl->m_pDrawListener)
			cls_gl->m_pDrawListener->applyColorSwf(c.ucolor());
	}

	void  fill_style_disable(int fill_side)
		// Don't fill on the {0 == left, 1 == right} side of a path.
	{
		assert(fill_side >= 0 && fill_side < 2);

		m_current_styles[fill_side].disable();
	}


	void  line_style_disable()
		// Don't draw a line on this path.
	{
		m_current_styles[LINE_STYLE].disable();
	}


	void  fill_style_color(int fill_side,const gameswf::rgba& color)
		// Set fill style for the left interior of the shape.  If
		// enable is false, turn off fill for the left interior.
	{
		assert(fill_side >= 0 && fill_side < 2);

		m_current_styles[fill_side].set_color(m_current_cxform.transform(color));
	}


	void  line_style_color(gameswf::rgba color)
		// Set the line style of the shape.  If enable is false, turn
		// off lines for following curve segments.
	{
		m_current_styles[LINE_STYLE].set_color(m_current_cxform.transform(color));
	}


	void  fill_style_bitmap(int fill_side, gameswf::bitmap_info* bi, const gameswf::matrix& m, bitmap_wrap_mode wm, bitmap_blend_mode bm)
	{
		assert(fill_side >= 0 && fill_side < 2);
		m_current_styles[fill_side].set_bitmap(bi, m, wm, m_current_cxform);
	}

	void  line_style_width(float width)
	{
		// WK: what to do here???
	}
	//triangle list
	void draw_triangle_list(const void *coords,int vertex_count)
	{
		draw_mesh_primitive(D3DPT_TRIANGLELIST,coords,vertex_count);
	}
	//strips do the same as triangle
	void  draw_mesh_strip(const void* coords, int vertex_count)
	{
		draw_mesh_primitive(D3DPT_TRIANGLESTRIP,coords,vertex_count);
	}

	void draw_mesh_primitive(D3DPRIMITIVETYPE primitive_type, const void* coords, int vertex_count)
	{
		//return ;
		// Set up current style.
		reVertex vertexBatch;
		memset(&vertexBatch,0,sizeof(vertexBatch));
		m_current_styles[LEFT_STYLE].apply(m_current_matrix,vertexBatch.blend,vertexBatch.tex);

		apply_matrix(m_current_matrix);

		vertexBatch.v = new CUSTOMVERTEX[vertex_count];
		if(!vertexBatch.v)
			return ;
		vertexBatch.count = vertex_count;
		const short* pCoords = (const short*)coords;
		for (int i=0;i<vertex_count;i++)
		{
			vertexBatch.v[i].point = GPoint(pCoords[2*i],pCoords[2*i+1],Z_DEPTH);
			vertexBatch.v[i].colour = 0;
			vertexBatch.v[i].u = vertexBatch.v[i].v = 0;
		}

		if(D3DPT_TRIANGLESTRIP == primitive_type)
		{
			if(cls_gl->m_pDrawListener)
				cls_gl->m_pDrawListener->renderTriangleStrip(&vertexBatch);
		}
		else if(D3DPT_TRIANGLELIST == primitive_type)
		{
			if(cls_gl->m_pDrawListener)
				cls_gl->m_pDrawListener->renderTriangle(&vertexBatch);
		}

		if (m_current_styles[LEFT_STYLE].needs_second_pass())
		{
			// 2nd pass, if necessary.
			m_current_styles[LEFT_STYLE].apply_second_pass(vertexBatch.blend);

			if(D3DPT_TRIANGLESTRIP == primitive_type)
			{
				if(cls_gl->m_pDrawListener)
					cls_gl->m_pDrawListener->renderTriangleStrip(&vertexBatch);
			}
			else if(D3DPT_TRIANGLELIST == primitive_type)
			{
				if(cls_gl->m_pDrawListener)
					cls_gl->m_pDrawListener->renderTriangle(&vertexBatch);
			}

			m_current_styles[LEFT_STYLE].cleanup_second_pass();
		}

		SAFE_DELETE(vertexBatch.v);
	}

	void  draw_line_strip(const void* coords, int vertex_count)
		// Draw the line strip formed by the sequence of points.
	{
		assert(m_current_styles[LINE_STYLE].m_mode  == fill_style::COLOR);
		int blend;
		void* tex;
		m_current_styles[LINE_STYLE].apply(m_current_matrix,blend,tex);
		apply_matrix(m_current_matrix);

		CUSTOMVERTEX* pVertex = new CUSTOMVERTEX[vertex_count];
		if(!pVertex)
			return ;
		const Sint16* pPoint = (const Sint16*)coords;
		for (int i=0;i<vertex_count;i++)
		{
			pVertex[i].point.x = pPoint[2*i];
			pVertex[i].point.y = pPoint[2*i+1];
			pVertex[i].point.z = Z_DEPTH;
			pVertex[i].colour = 0;
			pVertex[i].u = 0;
			pVertex[i].v = 0;
		}
		if( cls_gl->m_pDrawListener)
			cls_gl->m_pDrawListener->renderLineStrip(pVertex,vertex_count,blend);

		SAFE_DELETE(pVertex);
	}

	void  draw_bitmap(
		const gameswf::matrix& m,
		gameswf::bitmap_info* bi,
		const gameswf::rect& coords,
		const gameswf::rect& uv_coords,
		gameswf::rgba color)
		// Draw a rectangle textured with the given bitmap, with the
		// given color.  Apply given transform; ignore any currently
		// set transforms.
		//
		// Intended for textured glyph rendering.
	{
		assert(bi);

		bitmap_info_d3d* bi_d3d = (bitmap_info_d3d*)bi;
		IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
		if(!pD3dDevice)
			return ;
		apply_color(color);
		D3DXMATRIX  mat;
		D3DXMatrixIdentity(&mat);
		pD3dDevice->SetTransform(D3DTS_WORLD, &mat);

		gameswf::point a, b, c, d;
		m.transform(&a, gameswf::point(coords.m_x_min, coords.m_y_min));
		m.transform(&b, gameswf::point(coords.m_x_max, coords.m_y_min));
		m.transform(&c, gameswf::point(coords.m_x_min, coords.m_y_max));
		d.m_x = b.m_x + c.m_x - a.m_x;
		d.m_y = b.m_y + c.m_y - a.m_y;

		// Set texture.
		//m_pd3dDevice->SetTexture(0, m_d3d_textures[bi_d3d->m_texture_id]);
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
		pD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);

		//m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		//m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		// No texgen; just pass through.
		pD3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pD3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

		// Draw the quad.
		reQuad quad;
		quad.blend  = BLEND_ALPHABLEND | BLEND_COLORSELECT2;
		quad.tex = m_d3d_textures[bi_d3d->m_texture_id];

		quad.v[0].point.x = a.m_x ;
		quad.v[0].point.y = a.m_y ;
		quad.v[0].point.z = Z_DEPTH;
		quad.v[0].u       = uv_coords.m_x_min ;
		quad.v[0].v       = uv_coords.m_y_min ;
		quad.v[0].colour = 0xffffffff;

		quad.v[1].point.x = c.m_x ;
		quad.v[1].point.y = c.m_y ;
		quad.v[1].point.z = Z_DEPTH;
		quad.v[1].u       = uv_coords.m_x_min ;
		quad.v[1].v       = uv_coords.m_y_max ;
		quad.v[1].colour = 0xffffffff;

		quad.v[2].point.x = b.m_x ;
		quad.v[2].point.y = b.m_y ;
		quad.v[2].point.z = Z_DEPTH;
		quad.v[2].u       = uv_coords.m_x_max ;
		quad.v[2].v       = uv_coords.m_y_min ;
		quad.v[2].colour = 0xffffffff;

		quad.v[3].point.x = d.m_x ;
		quad.v[3].point.y = d.m_y ;
		quad.v[3].point.z = Z_DEPTH;
		quad.v[3].u       = uv_coords.m_x_max ;
		quad.v[3].v       = uv_coords.m_y_max ;
		quad.v[3].colour = 0xffffffff;

		if(cls_gl->m_pDrawListener)
			cls_gl->m_pDrawListener->renderQuad(&quad);
	}

	void begin_submit_mask()
	{
		IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
		if(!pD3dDevice)
			return;
		HRESULT hr = pD3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);//we should disable color input by glp
		assert(hr == S_OK);

		pD3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE );

		// Enable stencil testing
		pD3dDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

		// Clear stencil buffer values to zero
		pD3dDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0);

		// Specify the stencil comparison function
		pD3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS );

		// Set the comparison reference value
		pD3dDevice->SetRenderState(D3DRS_STENCILREF, 1);

		//  Specify a stencil mask
		pD3dDevice->SetRenderState(D3DRS_STENCILMASK, 0x1);

		// A write mask controls what is written
		pD3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0x1);

		pD3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);

		pD3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		pD3dDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	}

	void end_submit_mask()
	{
		IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
		if(!pD3dDevice)
			return;

		// Specify when to write stencil data
		pD3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);

		pD3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		pD3dDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);

		// Specify the stencil comparison function
		pD3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

		pD3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		//we should enable color input by glp
		HRESULT hr = pD3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 
			D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
		assert(hr == S_OK);
	}

	void disable_mask()
	{
		IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
		if(pD3dDevice)
			pD3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}

	//TODO: Create a D3D video handler implementation
	gameswf::video_handler* create_video_handler()
	{
		return NULL;
	}

	bool is_visible(const gameswf::rect& bound)
	{
		gameswf::rect viewport;
		viewport.m_x_min = 0;
		viewport.m_y_min = 0;
		viewport.m_x_max = m_display_width;
		viewport.m_y_max = m_display_height;
		return viewport.bound_test(bound);
	}

	void open()
	{
		//doesn't need to open anything in directx
	}

	bool test_stencil_buffer(const gameswf::rect& bound, Uint8 pattern)
	{
		//TODO: implement stencil testing
		return false;
	}

};  // end struct render_handler_d3d


// bitmap_info_d3d implementation


bitmap_info_d3d::bitmap_info_d3d() : 
m_texture_id(0),
m_width(0),
m_height(0),
m_suspended_image(NULL)
{
}


bitmap_info_d3d::bitmap_info_d3d(image::rgb* im) :
m_width(im->m_width),
m_height(im->m_height)
// Image with no alpha.
{
	assert(im);

	int w = 1; while (w < im->m_width) { w <<= 1; }
	int h = 1; while (h < im->m_height) { h <<= 1; }

	// Need to insert a dummy alpha byte in the image data, for
	// D3DXLoadSurfaceFromMemory.
	// @@ this sucks :(
	Uint8*  expanded_data = new Uint8[m_width * m_height * 4];
	Uint8*  pdata = expanded_data;
	for (int y = 0; y < m_height; y++)
	{
		Uint8*  scanline = image::scanline(im, y);
		for (int x = 0; x < m_width; x++)
		{
			*pdata++ = scanline[x * 3 + 2]; // blue
			*pdata++ = scanline[x * 3 + 1]; // green
			*pdata++ = scanline[x * 3 + 0]; // red
			*pdata++ = 255; // alpha
		}
	}

	// Create the texture.
	render_handler_d3d::m_d3d_textures.push_back(NULL);
	m_texture_id = render_handler_d3d::m_d3d_textures.size() - 1;

	IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
	if(!pD3dDevice)
		return ;
	IDirect3DTexture*  tex;
	HRESULT result = pD3dDevice->CreateTexture(
		w, h, 1, 0,      // Usage
		render_handler_d3d::m_FormatRGB,  // Format
		D3DPOOL_MANAGED, &tex
#if DIRECT3D_VERSION >= 0x0900
		, NULL
#endif
		);

	if (render_handler_d3d::m_FormatRGB!=D3DFMT_A8R8G8B8 && result != S_OK)
	{
		result = pD3dDevice->CreateTexture(
			w, h, 1, 0,      // Usage
			D3DFMT_A8R8G8B8,  // Format
			D3DPOOL_MANAGED, &tex
#if DIRECT3D_VERSION >= 0x0900
			, NULL
#endif    
			);
	}
	if (result != S_OK)
	{
		gameswf::log_error("error: can't create texture\n");
		return;
	}
	assert(tex);
	render_handler_d3d::m_d3d_textures.back() = tex;

	IDirect3DSurface*  surf = NULL;
	result = tex->GetSurfaceLevel(0, &surf);
	if (result != S_OK)
	{
		gameswf::log_error("error: can't get surface\n");
		return;
	}
	assert(surf);

	RECT  source_rect;
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = m_width;
	source_rect.bottom  = m_height;
	result = D3DXLoadSurfaceFromMemory( surf, NULL, NULL, expanded_data, 
		D3DFMT_A8R8G8B8, m_width * 4, NULL, &source_rect, D3DX_DEFAULT, 0 );

	// test
	//D3DXSaveSurfaceToFile( "image.png", D3DXIFF_PNG, surf, NULL, NULL );

	delete [] expanded_data;
	if (result != S_OK)
		gameswf::log_error("error: can't load surface from memory, result = %d\n", result);

	surf->Release();

	m_suspended_image = new image::image_base(NULL,m_width,m_height,m_width * 4,image::image_base::RGB);
}

typedef struct
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} RGBA;

void bitmap_info_d3d::convert_to_argb(image::rgba* im)
{
	for (int h = 0; h < im->m_height; h++)
	{
		for (int w = 0; w < im->m_width; w++)
		{
			RGBA c;
			c.r = im->m_data[((h * im->m_width) + w ) * 4];
			c.g = im->m_data[(((h * im->m_width) + w ) * 4) + 1];
			c.b = im->m_data[(((h * im->m_width) + w ) * 4) + 2];
			c.a = im->m_data[(((h * im->m_width) + w ) * 4) + 3];
			im->m_data[((h * im->m_width) + w ) * 4 + 3] = c.a;
			im->m_data[(((h * im->m_width) + w ) * 4) + 2] = c.r;
			im->m_data[(((h * im->m_width) + w ) * 4) + 1] = c.g;
			im->m_data[(((h * im->m_width) + w ) * 4) + 0] = c.b;
		}
	}
}

bitmap_info_d3d::bitmap_info_d3d(image::rgba* im) : 
m_width(im->m_width),
m_height(im->m_height)
// Version of the constructor that takes an image with alpha.
{
	assert(im);

#if DIRECT3D_VERSION < 0x0900
	convert_to_argb(im);
#endif

	int w = 1; while (w < im->m_width) { w <<= 1; }
	int h = 1; while (h < im->m_height) { h <<= 1; }

	// Create the texture.
	render_handler_d3d::m_d3d_textures.push_back(NULL);
	m_texture_id = render_handler_d3d::m_d3d_textures.size() - 1;

	IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
	if(!pD3dDevice)
		return ;
	IDirect3DTexture*  tex;
	HRESULT result = pD3dDevice->CreateTexture(
		w, h, 1, 0,      // Usage
		render_handler_d3d::m_FormatRGBA,  // Format
		D3DPOOL_MANAGED, &tex
#if DIRECT3D_VERSION >= 0x0900
		, NULL
#endif    
		);

	if (render_handler_d3d::m_FormatRGBA!=D3DFMT_A8R8G8B8 && result != S_OK)
	{
		result = pD3dDevice->CreateTexture(
			w, h, 1, 0,      // Usage
			D3DFMT_A8R8G8B8,  // Format
			D3DPOOL_MANAGED, &tex
			, NULL
			);
	}

	if (result != S_OK)
	{ 
		gameswf::log_error("error: can't create texture\n");
		return;
	}
	assert(tex);
	render_handler_d3d::m_d3d_textures.back() = tex;

	IDirect3DSurface*  surf = NULL;
	result = tex->GetSurfaceLevel(0, &surf);
	if (result != S_OK)
	{
		gameswf::log_error("error: can't get surface\n");
		return;
	}
	assert(surf);

	RECT  source_rect;
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = m_width;
	source_rect.bottom  = m_height;

	// Set the actual data.
	result = D3DXLoadSurfaceFromMemory( surf, NULL, NULL, im->m_data,
		D3DFMT_A8B8G8R8, //D3DFMT_A8R8G8B8,
		im->m_pitch, NULL, &source_rect, D3DX_DEFAULT, 0);

	if (result != S_OK)
		gameswf::log_error("error: can't load surface from memory, result = %d\n", result);

	surf->Release();

	m_suspended_image = new image::image_base(NULL,m_width,m_height,im->m_pitch,image::image_base::RGBA);
}


bitmap_info_d3d::bitmap_info_d3d(int width, int height, Uint8* data)
// Initialize this bitmap_info to an alpha image
// containing the specified data (1 byte per texel).
//
// !! Munges *data in order to create mipmaps !!
{
	assert(data);

	// Create the texture.
	m_width = width;
	m_height = height;

	// You must use power-of-two dimensions!!
	int w = 1; while (w < width) { w <<= 1; }
	int h = 1; while (h < height) { h <<= 1; }

	render_handler_d3d::m_d3d_textures.push_back(NULL);
	m_texture_id = render_handler_d3d::m_d3d_textures.size() - 1;

	IDirect3DDevice9* pD3dDevice = cls_gl->m_pDrawListener?cls_gl->m_pDrawListener->m_D3DDev:NULL;
	if(!pD3dDevice)
		return ;
	IDirect3DTexture*  tex;
	HRESULT result = pD3dDevice->CreateTexture(
		w, h, 1, 0,      // Usage
		render_handler_d3d::m_FormatA,  // Format
		D3DPOOL_MANAGED, &tex
#if DIRECT3D_VERSION >= 0x0900
		, NULL
#endif
		);

	if (result != S_OK)
	{
		gameswf::log_error("error: can't create texture\n");
		return;
	}
	assert(tex);
	render_handler_d3d::m_d3d_textures.back() = tex;

	IDirect3DSurface*  surf = NULL;

	result = tex->GetSurfaceLevel(0, &surf);
	if (result != S_OK)
	{
		gameswf::log_error("error: can't get surface\n");
		return;
	}
	assert(surf);

	RECT	source_rect;
	source_rect.left    = 0;
	source_rect.top     = 0;
	source_rect.right   = width;
	source_rect.bottom  = height;
	result = D3DXLoadSurfaceFromMemory( surf, NULL, NULL, data,
		D3DFMT_A8, width, NULL, &source_rect, D3DX_DEFAULT, 0);

	if (result != S_OK)
		gameswf::log_error("error: can't load surface from memory, result = %d\n", result);

	surf->Release();

	m_suspended_image = new image::image_base(NULL,width,height,width,image::image_base::ALPHA);

	//  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);

	//  // Build mips.
	//  int level = 1;
	//  while (width > 1 || height > 1)
	//  {
	//    render_handler_d3d::make_next_miplevel(&width, &height, data);
	//    glTexImage2D(GL_TEXTURE_2D, level, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	//    level++;
	//  }
}

D3DFORMAT         render_handler_d3d::m_FormatRGB;
D3DFORMAT         render_handler_d3d::m_FormatRGBA;
D3DFORMAT         render_handler_d3d::m_FormatA;
D3DXMATRIX        render_handler_d3d::m_ModelViewMatrix;
D3DXMATRIX        render_handler_d3d::m_ProjMatrix;
array<IDirect3DBaseTexture*> render_handler_d3d::m_d3d_textures;

gameswf::render_handler*  gameswf::create_render_handler_d3d(IDirect3DDevice* device)
// Factory.
{
	render_handler_d3d *hndlr = new render_handler_d3d( device );
	return hndlr;
}

//TODO: These two functions need to be exported so that the program running direct3d
//			can call these functions whenever the device is lost and regained
//			there can possibly be a better way of handling this, the way
//			I have found was just exporting it in the gameswf.h
//										~Ed Pereira
void  gameswf::reset_render_handler_d3d(IDirect3DDevice* device,gameswf::render_handler* rend_hndlr)
{
	render_handler_d3d* hndlr = dynamic_cast<render_handler_d3d*>(rend_hndlr);

	hndlr->on_reset_device(device);
}

void  gameswf::release_render_handler_d3d(gameswf::render_handler* rend_hndlr)
{
	render_handler_d3d* hndlr = dynamic_cast<render_handler_d3d*>(rend_hndlr);
	hndlr->on_release_device();
}

// Local Variables:
// mode: C++
// c-basic-offset: 8
// tab-width: 8
// indent-tabs-mode: t
// End:
