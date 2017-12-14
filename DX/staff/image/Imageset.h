/*
	注释时间:2014-4-25
	author: glp
	图片精灵集
*/
#pragma once

//#include <windo ws.h>		// Header File For Windows
/*
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include "../glext.h"*/
#include <string>
#include "RenderEImage.h"

#include "../xml/tinystr.h"
#include "../xml/tinyxml.h"

#include "OS_LineDraw.h"
#include <map>

class CxImage;

#ifndef SPECIAL_VERSION

enum two_pic_blend
{
	GL_TPBLEND_DESTALPHA,
	GL_TPBLEND_SRCALPHA,
	GL_TPBLEND_DESTCOLOUR,
	GL_TPBLEND_SRCCOLOUR,
};

struct _tImageInfo
{
	HBITMAP hBitmap;
	int nX;
	int nY;
	bool alphaChannel;
	DWORD dwMask;
	LPBYTE bitmapData;
};

struct _tWFastLessCompare
{
	bool operator() (const std::wstring& a, const std::wstring& b) const
	{
		const size_t la = a.length();
		const size_t lb = b.length();
		if (la == lb)
			return (wmemcmp(a.c_str(), b.c_str(), la) < 0);
		return (la < lb);
	}
};

typedef std::map<std::wstring,RenderEImage*,_tWFastLessCompare>	MAPMUTABLEIMAGE;
typedef std::map<std::wstring,float,_tWFastLessCompare>		MAPXMLITEM;
typedef std::vector<RenderEImage*> VECTIMAGE;

typedef enum _eImageSet
{
	Set_Common,
	Set_Particle,
	Set_Gif
}eImageSet;

class Imageset : public COS_LineDraw
{
public:
	Imageset(const GSize& winsize);
	virtual ~Imageset();

protected:
	//xml 相关函数
	std::string getStrAttr( TiXmlElement * element, const char * attr_name );
	int getIntAttr( TiXmlElement * element, const char * attr_name );
	bool loadXML(const std::string & strFile);

	void freeImage(_tImageInfo *_image);
	_tImageInfo* loadImage(const char *pngFile, bool onlyout = false, DWORD mask = 0);//读取png图片数据
	//BOOL LoadGLTextures(BYTE* pglTextureData,int w,int h);//根据传入数据，真正生成纹理
	GSize	loadTextureImage(const wchar_t* imgfile_name);//,const std::string imgset_name);//加载数据，生成纹理
	IDirect3DTexture9 *createTexture(uint& w,uint& h,D3DFORMAT fmt,IDirect3DTexture9* &pTex);
	IDirect3DTexture9 *loadTexture(const wchar_t* fileName,int &w,int &h);
	bool combinTexture(GSize& buffer_size,D3DFORMAT pixel_format,IDirect3DTexture9* &pTexture
		,IDirect3DTexture9* pTexture1,IDirect3DTexture9* pTexture2,int dest_blend,int src_blend);
	bool loadTexture(const wchar_t* fileName,int &w,int &h,IDirect3DTexture9*&ptexture);
public:
	//清除map数据
	void clearMapImage();
	//	提供对纹理的加载
	bool combinTexture(const std::wstring imgname,const wchar_t* file1,int dest_blend,const wchar_t* file2,int src_blend);
	bool loadFromDir(const std::wstring imgset_dir,const std::wstring imgset_name);
	bool loadSingle(const std::wstring& imgset_name,const std::wstring& imgFile);
	bool loadGif(const std::wstring imgset_name,CxImage* m_xImage);

	virtual bool updateTexture(const byte* pBuf,int w,int h,bool isRgb=false);
	//Image* genImage(MAPXMLITEM plist_item);
	RenderEImage* getImage(const std::string& img_name);
	RenderEImage* getImage( const std::wstring& img_name);
	//gif interface
	RenderEImage* getImage(uint& n);

	bool isDefined(const std::string& img_name);
	bool isDefined(const std::wstring& img_name);

	bool addImage( const std::string & img_name, const CGRect & img_rect , const GPoint & img_offset);
	bool addImage( const std::wstring & img_name, const CGRect & img_rect , const GPoint & img_offset);

	void setTexture(IDirect3DTexture9* &texid ,GSize& tex_size);
	void setTextureSize(GSize& tex_size){m_TextureSize = tex_size;}

	//void setBlend(int blend){m_blend = blend;}
	void releaseD3D(IDirect3DDevice9* d3dDev = NULL);

	bool render(const CUSTOMVERTEX& v1,const CUSTOMVERTEX& v2,const CUSTOMVERTEX& v3,const CUSTOMVERTEX& v4,int blend);
	void render(const CGRect& source_rect ,const CGRect& dest_rect,bool bdest_pclip,CGRect* pclip_rect ,const GColorRect& color_rect,bool bXFlip,bool bYFilp,int blend);
	void render(const GPoint& v1,const GPoint& v2,const GPoint& v3,const GPoint& v4,const CGRect& source_rect,const GColorRect& color_rect,bool bXFlip,bool bYFilp,int blend);

	void render(const GPoint& hot_point,const GPoint& dest_point,const CGRect& source_rect,float rot,float hscale, float vscale,const GColorRect& colour_rect,int blend);
	//void resizeGeometryBuffer (int count);
	IDirect3DTexture9*		    getTexture()const {return m_TextureID;}
	IDirect3DTexture9*		 &getTexture() {
		return m_TextureID;
	}
	GSize									getTextureSize() const {return m_TextureSize;}
	IDirect3DVertexBuffer9* &getD3DVertexBatchBuffer(){return m_vertexBatchBuffer;}
	CUSTOMVERTEX*			 &getCustomVertex(){return m_batchVertices;}

	IDirect3DVertexBuffer9* &getD3DVertexBatchBufferSec(){return m_vertexBatchBufferSec;}
	CUSTOMVERTEX*			 &getCustomVertexSec(){return m_batchVerticesSec;}

	void clear() {m_nQuadFirst=0;m_nQuadSecond=0;}

public:
	std::wstring					m_ImagesetName;
protected:
	IDirect3DTexture9*	m_TextureID;
	IDirect3DTexture9*	m_pRenderTex;
	GSize							m_TextureSize;
	// 	CGSize							m_windowsize;
	D3DFORMAT							m_fmt;
	MAPMUTABLEIMAGE			m_mapImages;//release this but not m_vectImages
	VECTIMAGE							m_vectImages;//used for gif,don't release this.

	bool											m_bD3DThingsInit;
	IDirect3DVertexBuffer9*		m_vertexBatchBuffer;
	CUSTOMVERTEX*					m_batchVertices;
	int											m_nQuadFirst;//四边形个数
	int											m_nQuadCurFirst;
	//用于粒子系统，普通纹理一般不用下面5个参数
	bool											m_bdouble;
	uint											m_nVetexBufferSize;
	uint											m_nVetexBufferSize2;
	IDirect3DVertexBuffer9*		m_vertexBatchBufferSec;
	CUSTOMVERTEX*					m_batchVerticesSec;
	int											m_nQuadSecond;//四边形个数
	int											m_nQuadCurSecond;
	//////////////////////////////////////////////////////////////////////////
	//BatchVerticesBlend*				m_batchVerticesBlend;
	//IDirect3DDevice9*					m_d3dDev;


	int								geometry_count;
	D3DPRIMITIVETYPE	renderStyle;
	_eQuadSplitMode		quad_split_mode;

public:
	eImageSet					m_type;
};
#else
#include <../Player/Dx/staff/image/imageset.h>
#endif//SPECIAL_VERSION
