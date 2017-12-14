/*
	注释时间:2014-4-25
	author: glp
	封装一个CGLFont类，用于管理需要显示的字体
	提供两种方法一种是FreeType，一种是Gdi+，提供描边其他需要再实现。
*/
#pragma  once

#include "../Opengl.h"
#include "../staff/image/Imageset.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "freetype/ftoutln.h"
#include "freetype/fttrigon.h"
#include "freetype/ftimage.h"
#include "freetype/fttypes.h"


#include <GdiPlus.h>
#include <math.h>

enum font_type
{
	FONT_NONE,
	FONT_BANDING,
	FONT_SHADE
};

const int THICKNESS = 6;
const int GAP_PIXELS = 1;
const Gdiplus::Color GLOW_COLOR(255, 0, 0, 0);//黑色
const Gdiplus::Color TEXT_COLOR(255,255,255,255);//白色
const Gdiplus::Color SONGNAME_TEXTCOLOR(255,255,255,255);//白色
const Gdiplus::Color SONGNAME_BANDCOLOR(50,0,0,0);//透明
const Gdiplus::Color SINGERNAME_TEXTCOLOR(255,177,162,148);//黄色
const Gdiplus::Color SINGERNAME_BANDCOLOR(10,0,0,0);//透明

typedef std::vector<Imageset*> VECTIMAGESET;

interface IDirect3DDevice9;

class CGLFont
{
public:
	CGLFont(int config_font_size,const std::wstring font_imgsetname=L"afont",bool bCLear = false);
	~CGLFont();
	//获取相应的纹理元素
	RenderEImage* getImage(const std::wstring & ch);
	//获取这一串字的最大高度
	int getTextHeight(const std::string& str,float fScale=1.0f);
	int getTextHeight(const std::wstring & strLyric,float fScale=1.0f);
	//获取这一串字的宽度
	int getTextWidth(const std::string& str,float fSpace=1.0f,float fScale=1.0f);
	int getTextWidth( const std::wstring & strLyric,float fSpace=1.0f,float fScale=1.0f);
	//获取配置的文字大小
	int getConfigFontSize(){return CONG_FONT_SIZE;}
	//描绘文字
	int drawText(const std::string strText,const GPoint& pt, CGRect* pclip_rect=NULL,const  GColorRect& color_rect=WHITE_RECT,float fSpace=1.0f,float fScale=1.0f,bool isAlignCenter=false);
	/*
	描绘文字 unicode
	@param strText:指定需要描绘的字符串
	@param pt:指定需要描绘的位置
	@param pclip_rect:指定需要裁剪的区域
	@param color_rect:指定文字颜色的4个角
	@param fSpace:指定文字描绘的空隙
	@param fScale:指定文字描绘的大小缩放
	@param isAlignCenter:指定是否需要水平居中 居中点就是pt指定的y值

	@return : 已经描绘字符串的宽度
	*/
	int drawText(const std::wstring strText,const GPoint& pt, CGRect* pclip_rect=NULL,const  GColorRect& color_rect=WHITE_RECT,float fSpace=1.0f,float fScale=1.0f,bool isAlignCenter=false);
	//需要左上角与左下角alpha一致，右上角与右下角alpha一致,rgb颜色一致
	int drawTextAlpha( const std::wstring & strText,const GPoint& pt, CGRect* pclip_rect=NULL,bool bRMaxAlpha=true,const GColorRect& color_rect=WHITE_RECT,float fSpace=1.0f,float fScale=1.0f);
	//获取纹理集
	Imageset * getImgset(int index=0){return m_vectImageset[0];}
	//增加文字
	bool addFont(const std::wstring& wStr);
	//重置文字
	void resetFont();
	//设置字体风格 针对gdi
	void setFontExtra(bool bIsBold=false,bool bIsItalic=false,bool bIsUnderLine=false ){m_bIsBold=bIsBold;m_bIsItalic=bIsItalic;m_bIsUnderLine=bIsUnderLine;}
	//加载文字
	void setBufferSize(int nSize);
 	bool loadFont(const GSize& windowsize,bool bUseGdi,const font_type ft=FONT_NONE
		,Gdiplus::Color band_color=GLOW_COLOR, Gdiplus::Color text_color=TEXT_COLOR,int nTHICKNESS=THICKNESS);
protected:
	//gdi+ 加载字体
	/*
	本函数的机理是通过生成两张对应的图片，一个是纹理图片，一个是PNG图片，一一对应，通过共用一个x坐标，和不同的y坐标，
	达到不断通过PNG图片往纹理上添加字库。每次添加都会通过m_ulDstSkip记录相对位置，以便下次继续再后面添加。
	如果发现纹理不够了，就会new 一张新的纹理，然后继续重复上面的过程。

	@nSize：纹理的顶点buffer大小，
	@win_size：需要渲染的窗口大小
	@pwszFaceName：字体库名称
	@pwszText：需要加载的字体库
	@nFontSize：字体大小
	@ft：字体的特效类型，目前使用描边（FONT_BANDING）
	@band_color：字体描边使用的颜色
	@text_color：字体主题颜色
	@nTHICKNESS：描边的次数，次数越多，边线越明显
	@bIsBold：是否粗体
	@bIsItalic：是否斜体
	@bIsUnderLine：是否有下划线

	@return：true，成功；false，失败
	*/
	bool loadGIDPlusFont(const GSize& win_size,const wchar_t * pwszFaceName, const std::wstring& pwszText,const int nFontSize
		,const font_type ft,Gdiplus::Color band_color, Gdiplus::Color text_color,int nTHICKNESS
		, bool bIsBold = false, bool bIsItalic = false, bool bIsUnderLine = false);
	//freetype 加载字体，可能还有些问题，以后再修改
	bool loadFreeTypeFont(const GSize& windowsize, const std::wstring & strLyric);
	//从内存中加载
	inline int loadFromMemory(const void* buffer,const GSize& buffer_size,IDirect3DTexture9* &texture,D3DFORMAT pixel_format=D3DFMT_A8R8G8B8);
	
	void drawGlyphToBuffer( uint * buffer, uint buf_width, FT_Bitmap * glyph_bitmap, int xoff = 0, int yoff = 0, bool bkg = true );
	//转化到像素
	bool _ConvertToPixels( Gdiplus::Graphics* pGraphics, float fSrcWidth, float fSrcHeight, float* pfDestWidth, float* pfDestHeight );
	//处理图像
	bool _MeasureGraphicsPath( Gdiplus::Graphics* pGraphics, Gdiplus::GraphicsPath* pGraphicsPath, float* pfPixelsWidth, float* pfPixelsHeight );
	//处理String
	bool _MeasureString( Gdiplus::Graphics* pGraphics, Gdiplus::FontFamily* pFontFamily, Gdiplus::FontStyle fontStyle, int nfontSize, int nThickness, 
		const wchar_t*pszText, Gdiplus::Point ptDraw, Gdiplus::StringFormat* pStrFormat, float* pfDestWidth, float* pfDestHeight );
	//gdi+ 导入到内存
	bool flushToDib(const GSize& tex_size,IN Gdiplus::Bitmap* pMemBitmap, IN OUT void** bmpData);
	//把当前字先加到纹理中
	bool flushToTexture(Gdiplus::Bitmap * pPngImage,int tmp_y,int tex_h);
public:
	void onLostDevice();
private:
	int												m_nVertexBufferSize;
	bool												m_bClear;
	bool												m_bIsBold;
	bool												m_bIsItalic;
	bool												m_bIsUnderLine;

	uint												m_nCurImageset;
	VECTIMAGESET							m_vectImageset;
    //Imageset *								m_Imageset;
    //unsigned int       						TEXSIZE;
	GSize											m_texSize;
	HBITMAP									m_hBitmap;

	// Gdiplus 初始化
	Gdiplus::GdiplusStartupInput	m_gdiplusStartupInput;
	ULONG_PTR								m_gdiplusToken;
	int												CONG_FONT_SIZE;

	ulong											m_ulDstSkip;
	int												m_nX;
	int												m_nY;
	int												m_nH;
	int												m_nLastH;
	int												m_nLen;
	
	std::wstring									m_strName;
	std::wstring									m_strPreFont;
	std::wstring									m_strCurrentFont;
};

