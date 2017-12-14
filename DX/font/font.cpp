#include "../stdafx.h"

#include "font.h"
#include "../staff/image/ImgsetMgr.h"

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <D3dx9tex.h>
// Pixels to put between glyphs
#define INTER_GLYPH_PAD_SPACE 2
// A multiplication coefficient to convert FT_Pos values into normal floats
#define FT_POS_COEF  (1.0/64.0)
#define CONFIG_FONT_NUMBER 20
//512
#define FONT_TEXTURE_SIZEW 512
//1024
#define FONT_TEXTURE_SIZEH 512

#define ADDNEWTEXTURE \
{\
	wchar_t tmpName[260] = {0};\
	swprintf_s(tmpName,259,L"%s%d",m_strName.c_str(),++m_nCurImageset);\
	Imageset *imgset = cls_gl->m_pImgsetMgr->addImageSet( tmpName );\
	m_vectImageset.push_back(imgset);\
	m_nY = 0;\
	tmp_y = 0;\
	m_nLastH = m_nH;\
	m_nH = 0;\
	m_ulDstSkip = 0;\
	graphics.Clear(Gdiplus::Color(0,0,0,0));\
}

CGLFont::CGLFont(int config_font_size,const std::wstring font_imgsetname,bool bCLear)
:m_hBitmap(NULL)
,m_texSize(FONT_TEXTURE_SIZEW,FONT_TEXTURE_SIZEH)
,m_strName(font_imgsetname)
,CONG_FONT_SIZE(config_font_size)
,m_ulDstSkip(0)
,m_nX(0)
,m_nY(0)
,m_nH(0)
,m_nLen(0)
,m_bClear(bCLear)
,m_nCurImageset(0)
,m_nLastH(0)
,m_nVertexBufferSize(100)
{
	//MAKETEXTURESIZE(m_texSize.width,m_texSize.height);
	Imageset *imgset =cls_gl->m_pImgsetMgr->addImageSet( m_strName );
	m_vectImageset.push_back(imgset);
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

	setFontExtra();

#ifdef SPECIAL_VERSION
	cls_gl->getImgsetMgr()->m_setFont.insert(this);
#endif
	
}

CGLFont::~CGLFont()
{
#ifdef SPECIAL_VERSION
	cls_gl->getImgsetMgr()->m_setFont.erase(this);
#endif
	
	if (m_hBitmap)
	{
		::DeleteObject( m_hBitmap );
		m_hBitmap = NULL;
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
}

void CGLFont::drawGlyphToBuffer( uint * buffer, uint buf_width, FT_Bitmap * glyph_bitmap, int xoff/* = 0*/, int yoff/* = 0*/, bool bkg/* = true */ )
{
	switch (glyph_bitmap->pixel_mode)
	{
        case FT_PIXEL_MODE_GRAY:
		{
			buffer += buf_width * yoff;
			for (int i = 0; i < glyph_bitmap->rows; ++i)
			{
				u_char *src = glyph_bitmap->buffer + ( i * glyph_bitmap->pitch );
				u_char *dst = (u_char*)( buffer + xoff );
				for (int j = 0; j < glyph_bitmap->width; ++j)
				{
					// RGBA
					if ( bkg )
					{
                        *dst++ = 0xFF;
                        *dst++ = 0xFF;
                        *dst++ = 0xFF;

						*dst = ( *src ) >> 1;
					}
					else
					{
						if ( *src != 0 )
						{
                            *dst++ = 0xFF;
                            *dst++ = 0xFF;
                            *dst++ = 0xFF;

							if ( *dst == 0 )
							{
								*dst = *src;
							}
							else if ( *dst + *src > glyph_bitmap->num_grays - 1 )
							{
								*dst = glyph_bitmap->num_grays - 1;
							}
							else
							{
								*dst = ( u_char )( *dst + *src );
							}
                            
                            ++dst;
						}
						else
						{
                            dst += 4;
                            // if current pixel alpha value is 0,ignore.
						}
					}
                    
					++src;
				}
				buffer += buf_width;
			}
		}
        break;
            
        case FT_PIXEL_MODE_MONO:
		{
			for (int i = 0; i < glyph_bitmap->rows; ++i)
			{
				u_char *src = glyph_bitmap->buffer + ( ( i + yoff ) * glyph_bitmap->pitch) + xoff;
                
				for (int j = 0; j < glyph_bitmap->width; ++j)
					buffer [j] = (src [j / 8] & (0x80 >> (j & 7))) ? 0xFFFFFFFF : 0x00000000;
                
				buffer += buf_width;
			}
		}
        break;
            
        default:
        break;
	}
}

void CGLFont::resetFont()
{
	m_strCurrentFont = L"";
}

bool CGLFont::addFont(const std::wstring& wStr)
{
	if (wStr == L"")
	{
		return false;
	}

	m_strPreFont = m_strCurrentFont;
	m_strCurrentFont += wStr;
	GetOnlyDifferentFont(m_strCurrentFont);

	if (m_strCurrentFont.size() == m_strPreFont.size())
	{
		return false;
	}

	return true;
}
void CGLFont::setBufferSize(int nSize)
{
	m_nVertexBufferSize = nSize;
}
bool CGLFont::loadFont(const GSize& windowsize,bool bUseGdi 
			  ,const font_type ft ,Gdiplus::Color band_color, Gdiplus::Color text_color,int nTHICKNESS)
{
	if (bUseGdi)
	{
		return loadGIDPlusFont(windowsize,L"微软雅黑",m_strCurrentFont,CONG_FONT_SIZE,ft
			,band_color,text_color,nTHICKNESS,m_bIsBold,m_bIsItalic,m_bIsUnderLine);
	}
	else
	{
		return loadFreeTypeFont(windowsize,m_strCurrentFont);
	}
}

bool CGLFont::flushToDib(const GSize& tex_size,IN Gdiplus::Bitmap* pMemBitmap, IN OUT void** bmpData)
{
	BITMAPINFO info = {0};
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth = tex_size.width;
	info.bmiHeader.biHeight = -tex_size.height;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = tex_size.width * tex_size.height * 32 / 8;

	HDC hdc = ::GetDC(NULL);
	if (m_hBitmap)
	{
		::DeleteObject( m_hBitmap );
		m_hBitmap = NULL;
	}
	m_hBitmap = ::CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)(bmpData), NULL, 0);
	if (!bmpData)
	{
		WLOGH_ERRO <<"CreateDIBSection bmpData is NULL";
		return false;
	}
	::ReleaseDC(NULL, hdc);

	Gdiplus::BitmapData data;
	data.Height = pMemBitmap->GetHeight();
	data.PixelFormat = pMemBitmap->GetPixelFormat();
	data.Scan0 = *bmpData;
	data.Stride = (pMemBitmap->GetWidth() * 32 / 8);
	data.Width = pMemBitmap->GetWidth();
	Gdiplus::Rect rct(0, 0, pMemBitmap->GetWidth(), pMemBitmap->GetHeight());
	pMemBitmap->LockBits(&rct, Gdiplus::ImageLockModeRead|Gdiplus::ImageLockModeUserInputBuf, pMemBitmap->GetPixelFormat(), &data);
	pMemBitmap->UnlockBits(&data);

	return true;
}

bool CGLFont::flushToTexture(Gdiplus::Bitmap * pPngImage,int tmp_y,int tex_h)
{
	uint * memBuffer = NULL;

	if (!flushToDib(m_texSize,pPngImage,(void**)&memBuffer))
	{
		WLOGH_ERRO <<"flush to Dib failed";
		return false;
	}
	int ttmp_h = tmp_y;
	ASSERT( ttmp_h >= 0 && ttmp_h < m_texSize.height);
	GSize buffer_size = CGSizeMake( m_texSize.width, ttmp_h);
	int r = loadFromMemory((void*)memBuffer,buffer_size,m_vectImageset[m_nCurImageset]->getTexture());
	if (r == -1)
	{
		WLOGH_ERRO <<"loadFromMemory failed";
		return false;
	}
	m_vectImageset[m_nCurImageset]->setTextureSize(m_texSize);

	return true;
}

void CGLFont::onLostDevice()
{
#ifdef SPECIAL_VERSION
	for(int i=0;i<m_nCurImageset;i++)
		m_vectImageset[m_nCurImageset]->initVetexBuffer(false,m_nVertexBufferSize);
#endif
}

bool CGLFont::loadGIDPlusFont(const GSize& win_size,const wchar_t * pwszFaceName, const std::wstring& pwszText,const int nFontSize
					 ,const font_type ft,Gdiplus::Color band_color, Gdiplus::Color text_color,int nTHICKNESS
					 , bool bIsBold, bool bIsItalic, bool bIsUnderLine)
{
	//GLuint texID = 0;
//////////////////////////////////////////////////////////////////////////
	bool result = true;
	int i = 0, j = 0;

	if(ft == FONT_NONE)
		nTHICKNESS = 0;
// 	int row = 0, col = 0;
// 	float single_font_pixel = (float)nFontSize * 118 / 72;

	if (m_bClear)
	{
		if (m_vectImageset[m_nCurImageset])
			m_vectImageset[m_nCurImageset]->clearMapImage();
		m_nH = 0;
		m_nX = 0;
		m_nY = 0;
		m_nLen= 0;
	}
	int len = (int)pwszText.size();

	int tex_width = m_texSize.width;//(int)(col * single_font_pixel);
	int tex_height = m_texSize.height;//(int)(row * single_font_pixel);

	Gdiplus::Bitmap * pPngImage = new Gdiplus::Bitmap(tex_width,tex_height,PixelFormat32bppARGB);
	Gdiplus::Graphics graphics((Gdiplus::Image*)(pPngImage));
	graphics.Clear(Gdiplus::Color(0,0,0,0));

	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::FontFamily * pFontFamily = new Gdiplus::FontFamily(pwszFaceName);
	if ( Gdiplus::Ok != pFontFamily->GetLastStatus() )
	{
		delete pFontFamily;
		pFontFamily = new Gdiplus::FontFamily(L"黑体");
	}
	
	Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
	if ( bIsBold && bIsItalic )
		fontStyle = Gdiplus::FontStyleBoldItalic;
	else if ( bIsBold )//粗体
		fontStyle = Gdiplus::FontStyleBold;
	else if ( bIsItalic )//斜体
		fontStyle = Gdiplus::FontStyleItalic;

	Gdiplus::StringFormat strformat;

	m_nX = 0;
	m_nY += m_nH;

	int nCountText = 0;//统计当前添加到内存的字体数量，还没导入到纹理中
	int tmp_y=0;//记录新增加的高度,pPngImage的高度当前Y值使用
	for ( i = m_nLen; i < len; ++i )
	{
		std::wstring wstrTemp;
		wstrTemp.append( 1, pwszText[i] );//当前字符串所保存的字符
		float fDestWidth = 0.0f, fDestHeight = 0.0f;
		//处理该字的一些信息，字体宽度，高度等信息
		if ( !_MeasureString( &graphics, pFontFamily, fontStyle, nFontSize, nTHICKNESS, wstrTemp.c_str(), Gdiplus::Point(0,0), &strformat, &fDestWidth, &fDestHeight ) )
			continue;

		int single_font_w = int(ceil(fDestWidth));
		int single_font_h = int(ceil(fDestHeight));
		int _w = single_font_w + GAP_PIXELS;//字与字之间保持一个空隙，必须！
		int _h = single_font_h + GAP_PIXELS;

		if ( _h > m_nH )
		{
			int oldH = m_nH;
			m_nH = _h;
			//FLUSHTOTEXTURE
			//检测新增加的字的高度是否越界了
			if ((m_nY+m_nH) > tex_height)//如果某个字的高度加上之前已经使用的高度超出了纹理的最大高度，
			{
				if (!flushToTexture(pPngImage,tmp_y+oldH,single_font_h))
					goto failed;

				m_nLen += nCountText;
				nCountText = 0;

				ADDNEWTEXTURE
			}
		}
		if ( m_nX + single_font_w > tex_width )
		{
			m_nX = 0;
			m_nY += m_nH;//纹理中的高度
			tmp_y += m_nH;//Png中的高度
		}

		//检测新换一行的高度是否越界
		if (m_nY+m_nH > m_texSize.height)
		{
			if (i != m_nLen)//如果有新增加的字符，需要先flush
			{
				if(!flushToTexture(pPngImage,tmp_y,single_font_h))
				{
					goto failed;
				}
				m_nLen += nCountText;
				nCountText = 0;
			}

			ADDNEWTEXTURE
		}

		switch(ft)
		{
		case FONT_BANDING:
			{
				Gdiplus::GraphicsPath path;
				if ( Gdiplus::Ok != path.AddString(wstrTemp.c_str(), (int)wstrTemp.length(), pFontFamily, fontStyle, (Gdiplus::REAL)nFontSize, Gdiplus::Point(m_nX,tmp_y), &strformat ) )
					continue;

				for( j = 1; j < nTHICKNESS; ++j)//描边
				{
					Gdiplus::Pen pen( band_color, (Gdiplus::REAL)j);
					pen.SetLineJoin(Gdiplus::LineJoinRound);
					graphics.DrawPath(&pen, &path);
				}

				Gdiplus::SolidBrush brush(text_color);//填充字体中间部分
				graphics.FillPath(&brush, &path);
				break;
			}
		case FONT_SHADE://待完工
			{
				Gdiplus::SolidBrush textBrush(TEXT_COLOR), shadowBrush(Gdiplus::Color::Gray);
				Gdiplus::HatchBrush hatchBrush(Gdiplus::HatchStyleForwardDiagonal, Gdiplus::Color::Black, Gdiplus::Color::White);
				Gdiplus::Font font(pFontFamily,(Gdiplus::REAL)nFontSize,fontStyle);
				graphics.DrawString(wstrTemp.c_str(), (int)wstrTemp.size(), &font, Gdiplus::PointF((Gdiplus::REAL)(m_nX+nTHICKNESS), (Gdiplus::REAL)(m_nY+nTHICKNESS)), &shadowBrush);
				graphics.DrawString(wstrTemp.c_str(), (int)wstrTemp.size(), &font, Gdiplus::PointF((Gdiplus::REAL)m_nX, (Gdiplus::REAL)m_nY), &textBrush);
				for (int i = 1; i < nTHICKNESS; i++)
					graphics.DrawString(wstrTemp.c_str(), (int)wstrTemp.size(), &font, Gdiplus::PointF((Gdiplus::REAL)m_nX + i, (Gdiplus::REAL)(/*150 + */m_nY + i + 2)),&strformat,&hatchBrush);
				graphics.DrawString(wstrTemp.c_str(), (int)wstrTemp.size(), &font,Gdiplus::PointF((Gdiplus::REAL)m_nX, (Gdiplus::REAL)(/*150 + */m_nY)),&strformat,&textBrush);
				break;
			}
		case FONT_NONE:
			{
				Gdiplus::GraphicsPath path;
				if ( Gdiplus::Ok != path.AddString(wstrTemp.c_str(), (int)wstrTemp.length(), pFontFamily, fontStyle, (Gdiplus::REAL)nFontSize, Gdiplus::Point(m_nX,tmp_y), &strformat ) )
					continue;

				Gdiplus::SolidBrush brush(text_color);//填充字体中间部分
				graphics.FillPath(&brush, &path);
				break;
			}
		}
		
		CGRect rect = CGRectMake((float)m_nX,(float)m_nY,0.0f,single_font_w,single_font_h);
		if ( !m_vectImageset[m_nCurImageset] )
			return false;

		bool b = !!(m_vectImageset[m_nCurImageset]->addImage(wstrTemp,rect,GPoint((float)m_nX,(float)m_nY)));
		if (b)
			m_nX += _w;
		//m_nLen ++;
		nCountText ++;
	}

	//if (m_nH)
	{
		uint * memBuffer = NULL;
		if (!flushToDib(m_texSize,pPngImage,(void**)&memBuffer))
		{
			WLOGH_ERRO <<"flush to Dib failed";
			result = false;
			goto failed;
		}

		//if (m_nPreLen != m_nLen)
		{
			ASSERT(tmp_y + m_nH/*-GAP_PIXELS*/ <= m_texSize.height);
			int ttmp_h = tmp_y + (m_nH?m_nH/*-GAP_PIXELS*/:m_nLastH/*-GAP_PIXELS*/);
			GSize buffer_size = CGSizeMake( m_texSize.width, ttmp_h);
			int r = loadFromMemory((void*)memBuffer,buffer_size,m_vectImageset[m_nCurImageset]->getTexture());
			if (r == -1)
			{
				WLOGH_ERRO <<"loadFromMemory failed";
				result = false;
			}
			m_nLen += nCountText;
			m_vectImageset[m_nCurImageset]->setTextureSize( m_texSize );
		}
	}

failed:
	SAFE_DELETE(pPngImage);
	SAFE_DELETE(pFontFamily);

	return result;
}

bool CGLFont::loadFreeTypeFont(const GSize& windowsize, const std::wstring & strLyric/*,const std::wstring font_imgsetname*/ )
{
	bool result = true;
	uint tex_x = 0;
    uint tex_y = 0;
    int tex_yb = 0;
    uint * memBuffer = (uint *)( malloc(sizeof(uint) * m_texSize.width * m_texSize.height) );
    GSize tex_size;
	IDirect3DTexture9* texture=NULL;

    // 初始化字体库
    FT_Library library = NULL;
    FT_Face face = NULL;
    
    FT_Error error = FT_Init_FreeType( &library );
	if (error!=0)
	{
		WLOGH_ERRO <<"FT_Init_FreeType error";
		result = false;
		goto failed;
	}
	error = FT_New_Face( library, "C:\\Windows\\Fonts\\msyhbd.ttf", 0, &face );//微软雅黑MSYH.TTF

	if (error!=0)
	{
		WLOGH_ERRO <<"FT_New_Face error";
		error = FT_New_Face(library,"C:\\Windows\\Fonts\\simsun.ttc", 0, &face );//宋体
		if (error!=0)
		{
			WLOGH_ERRO <<"FT_New_Face error 2";
			result = false;
			goto failed;
		}
	}
	
	
    error = FT_Set_Char_Size(face, // handle to face object
                             6*64, // char_width in 1/64th of points
                             6*64, // char_height in 1/64th of points
                             326, // horizontal device resolution
                             326 ); // vertical device resolution

	if (error!=0)
	{
		WLOGH_ERRO <<"FT_Set_Char_Size error";
		result = false;
		goto failed;
	}
    /*
    error = FT_Set_Pixel_Sizes(face, // handle to face object
                               0, // pixel_width
                               18 ); // pixel_height
    */
    //w riteLog("FT_Set_Char_Size");
    int l = (int)strLyric.length();
    for ( int i = 0; i < l; ++i )
    {
        const wchar_t & c = strLyric[i];

        uint glyph_w = 0;
        uint glyph_h = 0;
        
        // Render the glyph
        bool is_load = (0 == FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_NORMAL));
        
        if ( is_load )
        {
			//w riteLog("is_load is true");
            glyph_w = face->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE;
            glyph_h = face->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE;
        }
        
        // Check if glyph right margin does not exceed texture size
        int x_next = tex_x + glyph_w;
        if (x_next > m_texSize.width)
        {
            tex_x = INTER_GLYPH_PAD_SPACE;
            x_next = tex_x + glyph_w;
            tex_y = tex_yb;
        }
        
        // Check if glyph bottom margine does not exceed texture size
        int y_bot = tex_y + glyph_h;
        if (y_bot > m_texSize.height)
        {
			WLOGH_ERRO <<"FreeType Font Texsize out of size";
        }
        
        if ( NULL == m_vectImageset[m_nCurImageset] )
        {
            memset(memBuffer, 0, m_texSize.width * m_texSize.height * sizeof(uint));
            
			if (!m_vectImageset[m_nCurImageset])
			{
				result = false;
				goto failed;
			}
            tex_x = INTER_GLYPH_PAD_SPACE;
            tex_y = INTER_GLYPH_PAD_SPACE;
            tex_yb = INTER_GLYPH_PAD_SPACE;
            
            x_next = tex_x + glyph_w;
            y_bot = tex_y + glyph_h;
        }
        
        if ( !is_load )
        {            
            // Create a 'null' image for this glyph so we do not seg later
            CGRect area;
            GPoint offset;
			std::wstring ws;
			ws.append(1,c);
            m_vectImageset[m_nCurImageset]->addImage( ws, area, offset );
        }
        else
        {
            uint * buf = memBuffer + (tex_y * m_texSize.width) + tex_x;
            FT_Bitmap * glyphBitmap = &face->glyph->bitmap;
            
            drawGlyphToBuffer( buf, m_texSize.width, glyphBitmap, 0, 0, false );
            
            // Create a new image in the imageset
            CGRect area = CGRectMake( (CGFloat)tex_x,(CGFloat)tex_y,.0f,face->glyph->bitmap.width,face->glyph->bitmap.rows);
            
            GPoint offset = CGPointMake( face->glyph->metrics.horiBearingX * (CGFloat)(FT_POS_COEF),
                         -face->glyph->metrics.horiBearingY * (CGFloat)(FT_POS_COEF));
			std::wstring wsg;
			wsg.append(1, c);
            m_vectImageset[m_nCurImageset]->addImage( wsg, area, offset );//c
            
            // Advance to next position
            tex_x = x_next;
            if (y_bot > tex_yb)
            {
                tex_yb = y_bot;
            }
        }
    }
    
	tex_size = CGSizeMake( m_texSize.width, m_texSize.height );

	int r = loadFromMemory((void*)memBuffer,tex_size,m_vectImageset[m_nCurImageset]->getTexture());
	if (r == -1)
		result = false;
	
	m_vectImageset[m_nCurImageset]->setTextureSize(tex_size);
    // 启用混合
    //glEnable(GL_BLEND);
    
failed:
	FT_Done_Face(face);
	FT_Done_FreeType(library);
    SAFE_FREE(memBuffer);
    return result;
}

inline int CGLFont::loadFromMemory(const void* buffer,const GSize& buffer_size,IDirect3DTexture9* &texture,D3DFORMAT pixel_format)
{
	// create a texture
	// TODO: Check resulting pixel format and react appropriately.
	D3DFORMAT pixfmt = pixel_format;
	switch (pixel_format)
	{
	case D3DFMT_R8G8B8:
		break;
	case /*D3DFMT_A8B8G8R8*/D3DFMT_A8R8G8B8:
		break;
	default:
		WLOGH_ERRO <<"Invalid PixelFormat value specified";
		return -1;
	}
	HRESULT hr;
	if (!texture)
	{
		//Size tex_sz(d_owner.getAdjustedSize(buffer_size));
		IDirect3DDevice9* d3dDev = cls_gl->m_pDrawListener->m_D3DDev;

		hr = D3DXCreateTexture(d3dDev,static_cast<UINT>(m_texSize.width),static_cast<UINT>(m_texSize.height),1, 0
			, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture);
		if (FAILED(hr))
		{
			WLOGH_ERRO <<"Direct3D9 texture creation failed";
			return -1;
		}
#ifdef SPECIAL_VERSION
		m_vectImageset[m_nCurImageset]->initVetexBuffer(false,m_nVertexBufferSize);
#endif//SPECIAL_VERSION
	}
	
	// lock the D3D texture
	D3DLOCKED_RECT rect;
	hr = texture->LockRect(0, &rect, 0, 0);
	if (FAILED(hr))
	{
		texture->Release();
		texture = NULL;

		WLOGH_ERRO <<"IDirect3DTexture9::LockRect failed";
		return -1;
	}

	// copy data from buffer into texture
	ulong* dst = (static_cast<ulong*>(rect.pBits)) + m_ulDstSkip;
	const ulong* src = static_cast<const ulong*>(buffer);

	int nSkip = rect.Pitch / sizeof(ulong);
	// RGBA
	if (pixel_format == D3DFMT_A8R8G8B8)
	{
		for (int i = 0; i < buffer_size.height; ++i)
		{
			for (int j = 0; j < buffer_size.width; ++j)
			{
// 				// we don't need endian safety on microsoft
// 				uchar r = static_cast<uchar>(src[j] & 0xFF);
// 				uchar g = static_cast<uchar>((src[j] >> 8) & 0xFF);
// 				uchar b = static_cast<uchar>((src[j] >> 16)  & 0xFF);
// 				uchar a = static_cast<uchar>((src[j] >> 24) & 0xFF);			
// 				dst[j] = D3DCOLOR_ARGB(a, r, g, b);
				dst[j] = src[j];
			}
			//memcpy(dst,src,buffer_size.width);
			dst += nSkip;
			m_ulDstSkip += nSkip;
			src += static_cast<ulong>(buffer_size.width);
		}
	}
	// RGB
	else
	{
		for (int i = 0; i < buffer_size.height; ++i)
		{
			for (int j = 0; j < buffer_size.width; ++j)
			{
				uchar b = reinterpret_cast<const uchar*>(src)[j * 3];
				uchar g = reinterpret_cast<const uchar*>(src)[j * 3 + 1];
				uchar r = reinterpret_cast<const uchar*>(src)[j * 3 + 2];
				uchar a = 0xFF;

				dst[j] = D3DCOLOR_ARGB(a, r, g, b);
			}
			dst += nSkip;
			m_ulDstSkip += nSkip;
			src = reinterpret_cast<const ulong*>(reinterpret_cast<const uchar*>(src) +static_cast<int>(buffer_size.width) * 3);
		}
	}

	texture->UnlockRect(0);

	return 0;
}

RenderEImage* CGLFont::getImage(const std::wstring & ch)
{
	RenderEImage* img = NULL;
	VECTIMAGESET::iterator i = m_vectImageset.begin();
	for (i;i!=m_vectImageset.end();i++)
	{
		img = (*i)->getImage(ch);
		if (img)
		{
			break;
		}
	}

	return img;
}
int CGLFont::getTextHeight(const std::string& str,float fScale)
{
	std::wstring wstr;
	Str2Wstr(str,wstr);
	return getTextHeight(wstr,fScale);
}
int CGLFont::getTextHeight(const std::wstring & strLyric,float fScale)
{
	int h = 0;
	int max_h = 0;
	int l =  (int)strLyric.length();
	for ( int i = 0; i < l; ++i )
	{
		const wchar_t & c = strLyric[i];
		std::wstring wsg;
		wsg.append(1,c);
		RenderEImage * img = getImage( wsg );
		if (img)
		{
			h = (int)(img->m_rect.size.height*fScale);
			if (h>max_h)
				max_h = h;
		}	

	}
	return max_h;
}

int CGLFont::getTextWidth(const std::string& str,float fSpace,float fScale)
{
	std::wstring wstr;
	Str2Wstr(str,wstr);
	return getTextWidth(wstr,fSpace,fScale);
}

int CGLFont::getTextWidth(const std::wstring & strLyric,float fSpace,float fScale)
{
	if(fScale <= 0.0f)
		return 0;
	if(fSpace < 0.0f)
		fSpace = 0.0f;

    int w = 0;
    int l =  (int)strLyric.length();
    for ( int i = 0; i < l; ++i )
	{
        const wchar_t & c = strLyric[i];
		std::wstring wsg;
		wsg.append(1,c);
        RenderEImage * img = getImage( wsg );
		if (img)
			  w += (int)(img->m_rect.size.width*fScale*fSpace);
    }
    return w;
}
int CGLFont::drawText(const std::string strText,const GPoint& pt, CGRect* pclip_rect,const  GColorRect& color_rect,float fSpace,float fScale,bool isAlignCenter)
{
	std::wstring wStr;
	Str2Wstr(strText,wStr);
	return drawText(wStr,pt,pclip_rect,color_rect,fSpace,fScale,isAlignCenter);
}
int CGLFont::drawText( const std::wstring strText,const GPoint& pt, CGRect* pclip_rect,const GColorRect& color_rect,float fSpace,float fScale,bool isAlignCenter)
{
	if(fScale <= 0.0f)
		return 0;
	if(fSpace < 0.0f)
		fSpace = 0.0f;

    int l = (int)strText.length();
    float x = pt.x;
    float y = pt.y;
    int width = 0;
    for ( int i = 0; i < l; ++i )
	{
        const wchar_t & c = strText[i];
		std::wstring wsg;
		wsg.append(1,c);
        RenderEImage * img = getImage( wsg );//c
        if ( img )
        {
			float drawY = y;
			if(isAlignCenter)
				drawY = drawY-img->m_rect.size.height*fScale/2.0f;
			int cur_width = (int)(img->m_rect.size.width*fScale);
			int cur_height = (int)(img->m_rect.size.height*fScale);
            CGRect dest_rect = CGRectMake( x, drawY,pt.z,cur_width ,cur_height  );
			img->draw( dest_rect, pclip_rect, color_rect );

			width += (int)(cur_width*fSpace);
            x += cur_width*fSpace;
        }
    }
	return width;
}

int CGLFont::drawTextAlpha( const std::wstring & strText,const GPoint& pt, CGRect* pclip_rect,bool bRMaxAlpha,const GColorRect& color_rect,float fSpace,float fScale)
{

	if (!pclip_rect)
	{
		return 0;
	}

	GColorRect tmp_rect = color_rect;
	int l = (int)strText.length();
	float x = pt.x;
	float y = pt.y;
	int width = 0;
	int text_width = pclip_rect->size.width;
	float alpha_min;
	float alpha_max;
	if (bRMaxAlpha)
	{
		alpha_min = float((color_rect.left_top_color.c>>24)&0xff);
		alpha_max = float((color_rect.right_top_color.c>>24)&0xff);
	}
	else
	{
		alpha_min = float((color_rect.right_top_color.c>>24)&0xff);
		alpha_max = float((color_rect.left_top_color.c>>24)&0xff);
	}
	float alpha_per = (alpha_max - alpha_min)/text_width;
	float alpha_x1 = x;
	float alpha_x2 = x;
	int color_rgb = (tmp_rect.left_top_color.c&0xffffff);
	for ( int i = 0; i < l; ++i )
	{
		const wchar_t & c = strText[i];
		std::wstring wsg;
		wsg.append(1,c);
		RenderEImage * img = getImage( wsg );
		if ( img )
		{
			int cur_width = int(img->m_rect.size.width*fScale);
			int cur_height = int(img->m_rect.size.height*fScale);
			float cur_space = img->m_rect.size.width*fSpace;
			alpha_x2 = alpha_x1+cur_space;

			if (alpha_x2<=(pclip_rect->origin.x))
			{//判断是否已经出去
				x += cur_space;
				alpha_x1 += cur_space;
				continue;
			}
			if (alpha_x1>= (pclip_rect->origin.x+pclip_rect->size.width))
			{//判断是否还没进来
				break;
			}

			int alpha_l = int((alpha_x1-pclip_rect->origin.x)*alpha_per);
			int alpha_r = int((alpha_x2-pclip_rect->origin.x)*alpha_per);
			alpha_l = alpha_l<0?0:(alpha_l>0xFF?0xFF:alpha_l);
			alpha_r = alpha_r<0?0:(alpha_r>0xFF?0xFF:alpha_r);
			if (!bRMaxAlpha)
			{
				alpha_l = 0xFF-alpha_l;
				alpha_r = 0xFF-alpha_r;
			}

			tmp_rect.left_top_color.c = ((alpha_l)<<24)|color_rgb;
			tmp_rect.left_bottom_color = tmp_rect.left_top_color;
			tmp_rect.right_top_color.c = ((alpha_r)<<24)|color_rgb;
			tmp_rect.right_bottom_color = tmp_rect.right_top_color;
			CGRect dest_rect = CGRectMake( x, y,pt.z, cur_width, cur_height );
			img->draw( dest_rect, pclip_rect, tmp_rect );
			x += cur_space;
			alpha_x1 = x;
			width += (int)cur_space;
		}
		else
		{
			//OutputDebugStringA("CGLFont::drawTextAlpha font image not found\n");
		}
	}
	return width;
}

bool CGLFont::_ConvertToPixels( Gdiplus::Graphics* pGraphics, float fSrcWidth, float fSrcHeight, float* pfDestWidth, float* pfDestHeight )
{
	if(!pGraphics)
		return false;

	Gdiplus::Unit unit = pGraphics->GetPageUnit();
	float fDpiX = pGraphics->GetDpiX();
	float fDpiY = pGraphics->GetDpiY();

	if(unit==Gdiplus::UnitWorld)
		return false; // dunno how to convert

	if(unit==Gdiplus::UnitDisplay||unit==Gdiplus::UnitPixel)
	{
		if(pfDestWidth)
			*pfDestWidth = fSrcWidth;
		if(pfDestHeight)
			*pfDestHeight = fSrcHeight;
		return true;
	}

	if(unit==Gdiplus::UnitPoint)
	{
		if(pfDestWidth)
			*pfDestWidth = 1.0f/72.0f * fDpiX * fSrcWidth;
		if(pfDestHeight)
			*pfDestHeight = 1.0f/72.0f * fDpiY * fSrcHeight;
		return true;
	}

	if(unit==Gdiplus::UnitInch)
	{
		if(pfDestWidth)
			*pfDestWidth = fDpiX * fSrcWidth;
		if(pfDestHeight)
			*pfDestHeight = fDpiY * fSrcHeight;
		return true;
	}

	if(unit==Gdiplus::UnitDocument)
	{
		if(pfDestWidth)
			*pfDestWidth = 1.0f/300.0f * fDpiX * fSrcWidth;
		if(pfDestHeight)
			*pfDestHeight = 1.0f/300.0f * fDpiY * fSrcHeight;
		return true;
	}

	if(unit==Gdiplus::UnitMillimeter)
	{
		if(pfDestWidth)
			*pfDestWidth = 1.0f/25.4f * fDpiX * fSrcWidth;
		if(pfDestHeight)
			*pfDestHeight = 1.0f/25.4f * fDpiY * fSrcHeight;
		return true;
	}

	return false;
}

bool CGLFont::_MeasureGraphicsPath( Gdiplus::Graphics* pGraphics, Gdiplus::GraphicsPath* pGraphicsPath, float* pfPixelsWidth, float* pfPixelsHeight )
{
	if(!pGraphics||!pGraphicsPath||!pfPixelsWidth||!pfPixelsHeight)
		return false;

	Gdiplus::PathData pathData;
	Gdiplus::Status status = pGraphicsPath->GetPathData(&pathData);

	if(status != Gdiplus::Ok)
		return false;

	if(pathData.Count<=0)
		return false;

	float fHighestX = pathData.Points[0].X;
	float fHighestY = pathData.Points[0].Y;
	float fLowestX = pathData.Points[0].X;
	float fLowestY = pathData.Points[0].Y;
	pathData.Points[0];
	for(int i=1; i<pathData.Count; ++i)
	{
		if(pathData.Points[i].X < fLowestX)
			fLowestX = pathData.Points[i].X;
		if(pathData.Points[i].Y < fLowestY)
			fLowestY = pathData.Points[i].Y;
		if(pathData.Points[i].X > fHighestX)
			fHighestX = pathData.Points[i].X;
		if(pathData.Points[i].Y > fHighestY)
			fHighestY = pathData.Points[i].Y;
	}

	// Hack!
	if(fLowestX<0.0f)
		fLowestX = -fLowestX;
	else
		fLowestX = 0.0f;

	if(fLowestY<0.0f)
		fLowestY = -fLowestY;
	else
		fLowestY = 0.0f;
	//转换成像素值
	bool b = _ConvertToPixels(pGraphics,fHighestX -fLowestX,fHighestY-fLowestY,pfPixelsWidth,pfPixelsHeight );

	return b;
}

bool CGLFont::_MeasureString( Gdiplus::Graphics* pGraphics, Gdiplus::FontFamily* pFontFamily, Gdiplus::FontStyle fontStyle, int nfontSize, int nThickness, 
				   const wchar_t*pszText, Gdiplus::Point ptDraw, Gdiplus::StringFormat* pStrFormat, float* pfDestWidth, float* pfDestHeight )
{
	Gdiplus::GraphicsPath path;
	Gdiplus::Status status = path.AddString(pszText,(int)wcslen(pszText),pFontFamily,fontStyle,(Gdiplus::REAL)nfontSize,ptDraw,pStrFormat);
	if(status!=Gdiplus::Ok)
		return false;

	*pfDestWidth= float(ptDraw.X);
	*pfDestHeight= float(ptDraw.Y);
	//获取字的最终宽度，高度
	bool b = _MeasureGraphicsPath(pGraphics, &path, pfDestWidth, pfDestHeight);

	if(!b)
		return false;

	//描边转换成像素值
	if(nThickness > 0)
	{
		float pixelThick = 0.0f;

		b = _ConvertToPixels(pGraphics,(float)nThickness,0.0f,&pixelThick,NULL);
		if(!b)
			return false;

		//加上用于描边的像素值
		*pfDestWidth += pixelThick;
		*pfDestHeight += pixelThick;
	}

	return true;
}
