/*
	注释时间:2014-4-25
	author: glp
	图片精灵集的另一种实现方式
*/
#pragma  once

#include "Imageset.h"

#ifndef SPECIAL_VERSION

class CImgsetEx : public Imageset
{
public:
	CImgsetEx(const GSize& winsize);
	virtual ~CImgsetEx();
	
public:
	virtual bool load(const std::wstring imgset_dir,const std::wstring imgset_name,const std::wstring imgName);
	virtual bool loadFromMemory(const std::wstring& imgset_name,const std::wstring& img_name,uint w,uint h,D3DFORMAT fmt=D3DFMT_R8G8B8);
protected:
};
#else
#include <../Player/DX/staff/image/ImgsetEx.h>
#endif//
