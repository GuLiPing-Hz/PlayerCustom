#include <stdafx.h>
#include "ImgsetEx.h"

#define SINGERPIC_WIDTH 170
#define SINGERPIC_HEIGHT 170

CImgsetEx::CImgsetEx(const GSize& winsize)
:Imageset(winsize)
{
	
}

CImgsetEx::~CImgsetEx()
{
	
}

bool CImgsetEx::loadFromMemory(const std::wstring& imgset_name,const std::wstring& img_name,uint w,uint h,D3DFORMAT fmt)
{
	m_type = Set_Common;
	m_ImagesetName = imgset_name;
	m_fmt = fmt;
	uint tmp_w = w;
	uint tmp_h = h;
	if (/*fmt!=D3DFMT_A8R8G8B8 || */!createTexture(w,h,fmt,m_TextureID))
	{
		WLOGH_ERRO << L"texture create failed from memory";
		return false;
	}

	GSize imgSize = CGSizeMake(tmp_w,tmp_h);
	CGRect rect = CGRectMake(GPoint(0.0f,0.0f),imgSize);
	RenderEImage* img = new RenderEImage(rect,this);

	std::pair<MAPMUTABLEIMAGE::iterator,bool> pair_re;
	pair_re = m_mapImages.insert( std::pair<std::wstring,RenderEImage*>( img_name, img ) );
	if (!(pair_re.second))
	{
		WLOGH_ERRO << L"two images have the same name";
		SAFE_DELETE(img);
		return false;
	}
	m_TextureSize = CGSizeMake(w,h);
	return true;
}

bool CImgsetEx::load(const std::wstring imgsetFile,const std::wstring imgset_name,const std::wstring imgName)
{
	m_type = Set_Common;
	// 保存Imageset名称
	m_ImagesetName =  imgset_name;

	clearMapImage();

	//加载图册
	//std::string fileName = imgset_name + ".png";
	m_TextureSize = loadTextureImage(imgsetFile.c_str());
	if (m_TextureSize == CGSIZEZERO)
	{
		WLOGH_ERRO << L"m_TextureSize == zero";
		return false;
	}
	
	CGRect rect = CGRectMake(0.0f,0.0f,0.0f,m_TextureSize.width,m_TextureSize.height);
	RenderEImage * img = new RenderEImage(rect,this);

	std::pair<MAPMUTABLEIMAGE::iterator,bool> pair_re;
	pair_re = m_mapImages.insert( std::pair<std::wstring,RenderEImage*>( imgName, img ) );
	if (!(pair_re.second))
	{
		WLOGH_ERRO << L"two images have the same name";
		SAFE_DELETE(img);
		return false;
	}
	MAKETEXTURESIZE(m_TextureSize.width,m_TextureSize.height);
	return true;
}

