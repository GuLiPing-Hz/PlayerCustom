/*
	注释时间:2014-4-25
	author: glp
	ImgsetMgr 提供对图片精灵集的管理
*/
#pragma once

#include "../Opengl.h"
#include "ImgsetEx.h"
#include <map>

#ifndef SPECIAL_VERSION
typedef std::map<std::wstring,Imageset *>	MAPMUTABLEIMAGESET;

struct CResourceList
{
	char				filename[_MAX_PATH];
	char				password[64];
	CResourceList*		next;
};

class COpenGLWindow;


class ImgsetMgr
{
public:
	ImgsetMgr();
	~ImgsetMgr();

	//添加多个纹理图集通过一个Gif文件的目录
	MAPMUTABLEIMAGESET addImageSetsGifDir(const std::string imgse_dir); 
	//添加一张纹理图集通过Gif文件
	Imageset * addImageSetsGif(const std::string imgse_dir,const std::string imgset_name);
	//添加融合纹理图集
	Imageset *	addImageSets(const std::wstring imgset_name,const std::wstring img_name,const std::wstring imgset_file1
		,const std::wstring imgset_file2,int dest_bend,int src_blend);
	//添加融合纹理图集
	Imageset *	addImageSets(const std::wstring& imgset_dir,const std::wstring imgset_name,const std::wstring img_name
		,const std::wstring& imgset_file1,const std::wstring& imgset_file2,int dest_bend,int src_blend);
	//添加普通纹理图集
	Imageset *	addImageSets(const std::wstring& imgset_dir,const std::wstring& imgset_name);
	Imageset*	addImageSetSingle(const std::wstring& imgset_dir,const std::wstring& imgset_name,const std::wstring strExt);
	//添加粒子系统纹理图集，一般一个纹理就一个图片,默认纹理路径,默认PNG图片
	Imageset* addImageSetSingle(const std::string imgset_file);
	//添加单个图片的纹理，路径外面指定
	//Imageset* addImageSetSingle(const std::string imgset_file,const std::string name);
	Imageset* addImageSetSingle(const std::wstring imgset_file,const std::wstring name);
	//从内存添加一张纹理
	Imageset *	addImageSetsMem(uint w,uint h,const std::wstring& imgset_name,const std::wstring imgName);
	//添加ImgsetEx纹理图集
	Imageset *	addImageSetsEx(const std::wstring& imgsetFile,const std::wstring& imgset_name,const std::wstring imgName);
	//添加一个图集，提供字库使用
	Imageset *	addImageSet(const std::wstring imgset_name);

	Imageset* getImageset(const std::string& imgset_name);
	Imageset* getImageset(const std::wstring& imgset_name);

	RenderEImage*		getImage(const std::string& imgset_name ,const std::string& img_name);
	RenderEImage*		getImage(const std::wstring& imgset_name ,const std::wstring& img_name);

	void			clearAllQuad();
	void			clearMapImageset();
	bool			initAllImagesets(uint nBytesPerPixel);
protected:
	void			addImgsetToMap(const std::wstring& imgset_name,Imageset* imgset);
	//文件相关
	void getFileListFromDir(std::string dir, std::string pattern, std::vector<std::string> &files,bool findsubdir=true,bool absolute=true);

private:
	MAPMUTABLEIMAGESET		m_Imagesets;
	GSize										m_windowsize;
public:
	std::wstring								m_imgset_dir;
};
#else
#include <../Player/DX/staff/image/ImgsetMgr.h>
#endif
