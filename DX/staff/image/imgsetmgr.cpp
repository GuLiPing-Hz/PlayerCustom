
#include <stdafx.h>
#include "ImgsetMgr.h"

//////////////////////////////////////////////////////////////////////////
#define NOCRYPT
//#define NOUNCRYPT
#include "../OpenGLWindow.h"
#include "../ximage/ximage.h"

ImgsetMgr::ImgsetMgr()
:m_windowsize(cls_gl->m_winSize)
{
	std::string tmpStr;
	tmpStr = Ini_GetString(cls_gl->getszIniFile(),"DIR","ImgsetDir","");
	if (tmpStr == "")
	{
		WLOGH_ERRO << L"ini file read failed,ImgsetDir is NULL";
	}
	std::string dir = cls_gl->getAppDir() + tmpStr;
	Str2Wstr(dir,m_imgset_dir);

	m_Imagesets.clear();
}

ImgsetMgr::~ImgsetMgr()
{
	clearMapImageset();
}

Imageset * ImgsetMgr::getImageset(const std::string& imgset_name)
{
	if(imgset_name.empty())
		return NULL;
	wchar_t unicodeName[256] = {0};
	MultiByteToWideChar(CP_ACP,0,imgset_name.c_str(),-1,unicodeName,255);
	return getImageset(unicodeName);
}

Imageset * ImgsetMgr::getImageset(const std::wstring& imgset_name)
{
	if (m_Imagesets.empty() || imgset_name.empty())
		return NULL;
	MAPMUTABLEIMAGESET::const_iterator i;
	i=m_Imagesets.find(imgset_name);
	if (i!=m_Imagesets.end())
	{
		return (i->second);
	}
	return NULL;
}

RenderEImage * ImgsetMgr::getImage(const std::string& imgset_name ,const std::string& img_name)
{
	wchar_t unicodeSetName[256] = {0};
	wchar_t unicodeName[256] = {0};
	MultiByteToWideChar(CP_ACP,0,imgset_name.c_str(),-1,unicodeSetName,255);
	MultiByteToWideChar(CP_ACP,0,img_name.c_str(),-1,unicodeName,255);
	return getImage(unicodeSetName,unicodeName);
}

RenderEImage * ImgsetMgr::getImage(const std::wstring& imgset_name ,const std::wstring& img_name)
{
    if (m_Imagesets.empty())
    {
		return NULL;
    }
	MAPMUTABLEIMAGESET::const_iterator i;
	i=m_Imagesets.find(imgset_name);
	if (i!=m_Imagesets.end())
	{
		Imageset * timgset = i->second;
		return timgset->getImage(img_name);
	}
	return NULL;
}

void ImgsetMgr::addImgsetToMap(const std::wstring& imgset_name,Imageset* imgset)
{
	MAPMUTABLEIMAGESET::iterator i;
	i = m_Imagesets.find(imgset_name);
	if (i!=m_Imagesets.end())
	{
		SAFE_DELETE (i->second);
		m_Imagesets.erase(i);//去掉原来的。
	}
	m_Imagesets.insert(std::pair<std::wstring,Imageset*>(imgset_name,imgset));
}

void ImgsetMgr::getFileListFromDir(std::string dir, std::string pattern, std::vector<std::string> &files,bool findsubdir,bool absolute)
{
	WIN32_FIND_DATAA FindFileData;

	HANDLE hFind = FindFirstFileA((dir+"\\*.*").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do 
	{
		if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if(FindFileData.cFileName[0] != '.' && findsubdir)
			{
				char pszTemp[MAX_PATH]={0};
				sprintf(pszTemp,"%s\\%s",dir.c_str(),FindFileData.cFileName);
				std::string tmpDir = pszTemp;
				getFileListFromDir(tmpDir,pattern,files,true,absolute);
			}
		}
		else
		{
			if (strstr(FindFileData.cFileName,pattern.c_str()))
			{
				std::string str_file = absolute?(dir + "\\" + FindFileData.cFileName):FindFileData.cFileName;
				files.push_back(str_file);
			}
		}

	}while (FindNextFileA(hFind, &FindFileData) != 0);

	FindClose(hFind);
}
//添加多个纹理图集通过一个Gif文件的目录
MAPMUTABLEIMAGESET ImgsetMgr::addImageSetsGifDir(const std::string imgse_dir)
{
	MAPMUTABLEIMAGESET mapset;
	std::vector<std::string> files;
	getFileListFromDir(imgse_dir,".gif",files,false,false);

	std::vector<std::string>::iterator i= files.begin();
	for(i;i!=files.end();i++)
	{
		std::size_t pos = i->rfind(".gif");
		std::string imgsetname = i->substr(0,pos);
		Imageset* imgset = addImageSetsGif(imgse_dir,imgsetname);
		std::wstring wimgsetname;
		Str2Wstr(imgsetname,wimgsetname);
		if(imgset)
			mapset[wimgsetname] = imgset;
	}

	return mapset;
}
//添加一张纹理图集通过Gif文件
Imageset * ImgsetMgr::addImageSetsGif(const std::string imgse_dir,const std::string imgset_name)
{
	Imageset* imageset = NULL;//		imageset = new Imageset(winsize);
	if(imgset_name != "")
	{
		CxImage xImage;
		xImage.SetRetreiveAllFrames(true);

		std::string file = imgse_dir+"\\"+imgset_name + ".gif";
		if(!xImage.Load(file.c_str(),CXIMAGE_FORMAT_GIF) || xImage.GetType()!=CXIMAGE_FORMAT_GIF)
		{
			return imageset;
		}

		imageset = new Imageset(cls_gl->getWinSize());

		std::wstring wStr;
		Str2Wstr(imgset_name,wStr);
		if(!imageset || !imageset->loadGif(wStr,&xImage))
		{
			SAFE_DELETE(imageset);
			return imageset;
		}
		addImgsetToMap(wStr,imageset);
	}

	return imageset;
}

Imageset * ImgsetMgr::addImageSets(const std::wstring imgset_name,const std::wstring img_name
								   ,const std::wstring imgset_file1,const std::wstring imgset_file2,int dest_bend,int src_blend)
{
	if( (imgset_file1.size()!=0) && (imgset_file2.size()!=0) )
	{
		Imageset * imgset = new Imageset(cls_gl->getWinSize());
		if(!imgset)
			return NULL;
		if(!imgset->combinTexture(img_name,imgset_file1.c_str(),dest_bend,imgset_file2.c_str(),src_blend))
		{
			WLOGH_ERRO << L"combinTexture error";
			delete imgset;
			return NULL;
		}
		addImgsetToMap(imgset_name,imgset);
		return imgset;
	}
	return NULL;
}

Imageset * ImgsetMgr::addImageSets(const std::wstring& imgset_dir,const std::wstring imgset_name,const std::wstring img_name
								   ,const std::wstring& imgset_file1,const std::wstring& imgset_file2,int dest_bend,int src_blend)
{
	if( (imgset_file1.size()!=0) && (imgset_file2.size()!=0) )
	{
		std::wstring file1 = imgset_dir+L"\\"+imgset_file1;
		std::wstring file2 = imgset_dir+L"\\"+imgset_file2;
		return addImageSets(imgset_name,img_name,file1,file2,dest_bend,src_blend);
	}
	return NULL;
}


Imageset * ImgsetMgr::addImageSet(const std::wstring imgset_name)
{//为Font
	if( imgset_name.size() != 0 )
	{
		MAPMUTABLEIMAGESET::iterator i;
		i = m_Imagesets.find(imgset_name);
		if (i!=m_Imagesets.end())
		{
			int cc = 0;
			//rrcount 16
			delete i->second;
			m_Imagesets.erase(i);//去掉原来的。
			//rcount 13
		}
		Imageset * imgset = new Imageset(cls_gl->getWinSize());
		if(!imgset)
			return NULL;
		imgset->m_ImagesetName = imgset_name;
		//imgset->mImagesetName = "zfont";
		m_Imagesets.insert(std::pair<std::wstring,Imageset*>(imgset_name,imgset));//add by glp
		return imgset;
	}
	return NULL;
}

Imageset*  ImgsetMgr::addImageSetSingle(const std::wstring& imgset_dir,const std::wstring& imgset_name,const std::wstring strExt)
{
	if( imgset_name.size() != 0 )
	{
		 Imageset * imgset = new Imageset(cls_gl->getWinSize());
		 if(!imgset)
			 return NULL;
		 std::wstring fileName = imgset_dir+L"\\"+imgset_name+strExt;
		if(!imgset->loadSingle(imgset_name,fileName))
		{
			delete imgset;
			return NULL;
		}
		addImgsetToMap(imgset_name,imgset);
		return imgset;
	}
	return NULL;
}
Imageset* ImgsetMgr::addImageSetSingle(const std::string imgset_file)
{
	if (imgset_file.size() == 0)
		return NULL;

	std::string imgset_name = imgset_file.substr(0,imgset_file.rfind(".png"));
	if(imgset_name == imgset_file)
		return NULL;

	std::wstring wimgset_name;
	Str2Wstr(imgset_name,wimgset_name);
	MAPMUTABLEIMAGESET::iterator it = m_Imagesets.find(wimgset_name);
	if(it != m_Imagesets.end())
		return it->second;
	else
		return addImageSetSingle(m_imgset_dir,wimgset_name,L".png");
}

Imageset* ImgsetMgr::addImageSetSingle(const std::wstring imgset_file,const std::wstring imgset_name)
{
	if (imgset_name.size() == 0 || imgset_file.size() == 0)
		return NULL;

	MAPMUTABLEIMAGESET::iterator it = m_Imagesets.find(imgset_name);
	if(it != m_Imagesets.end())
		return it->second;
	else
	{
		Imageset* imgset = new Imageset(cls_gl->getWinSize());
		if(!imgset)
			return NULL;

		if(!imgset->loadSingle(imgset_name,imgset_file))
		{
			delete imgset;
			return NULL;
		}
		addImgsetToMap(imgset_name,imgset);
		return imgset;
	}
}

Imageset * ImgsetMgr::addImageSets(const std::wstring& imgset_dir,const std::wstring& imgset_name)
{
    if( imgset_name.size() != 0 )
    {
        Imageset * imgset = new Imageset(cls_gl->getWinSize());
		if(!imgset)
			return NULL;
        if(!imgset->loadFromDir(imgset_dir,imgset_name))
		{
			delete imgset;
			return NULL;
		}
		addImgsetToMap(imgset_name,imgset);
		return imgset;
    }
    return NULL;
}

Imageset * ImgsetMgr::addImageSetsMem(uint w,uint h,const std::wstring& imgset_name,const std::wstring imgName)
{
	if( imgset_name.size() != 0 )
	{
		CImgsetEx * imgset = new CImgsetEx(cls_gl->getWinSize());
		if(!imgset->loadFromMemory(imgset_name,imgName,w,h))
		{
			delete imgset;
			return NULL;
		}
		addImgsetToMap(imgset_name,(Imageset*)imgset);
		return imgset;
	}
	return NULL;
}

Imageset * ImgsetMgr::addImageSetsEx(const std::wstring& imgsetFile,const std::wstring& imgset_name,const std::wstring imgName)
{
	if( imgset_name.size() != 0 )
	{
		CImgsetEx * imgset = new CImgsetEx(cls_gl->getWinSize());
		if(!imgset->load(imgsetFile,imgset_name,imgName))
		{
			delete imgset;
			return NULL;
		}
		addImgsetToMap(imgset_name,(Imageset*)imgset);
		return imgset;
	}
	return NULL;

}

void	ImgsetMgr::clearAllQuad()
{
	MAPMUTABLEIMAGESET::iterator i = m_Imagesets.begin();;
	for(;i!=m_Imagesets.end();i++)
	{
		if (i->second)
			(i->second)->clear();
	}
}

void ImgsetMgr::clearMapImageset()
{
	if (!m_Imagesets.empty())
	{
		MAPMUTABLEIMAGESET::iterator i;
		for (i = m_Imagesets.begin();i!=m_Imagesets.end();i++)
		{
			SAFE_DELETE(i->second);
		}
	}
	m_Imagesets.clear();
}

bool	ImgsetMgr::initAllImagesets(uint nBytesPerPixel)
{
	//纹理图片宽高是2的倍数。
	Imageset* p = addImageSets(m_imgset_dir,L"renderer");
	if (!p)
	{
		WLOGH_ERRO << L"miss renderer imageset,,please check the picture directory";
		return false;
	}
#ifdef SPECIAL_VERSION
	p->initVetexBuffer(false,4000);
#endif
	p = addImageSets(m_imgset_dir,L"!renderer2");
	if(!p)
		WLOGH_ERRO << L"miss renderer2 imageset,please check the data directory";
#ifdef SPECIAL_VERSION
	else
		p->initVetexBuffer(false,400);
#endif
	p = addImageSets(m_imgset_dir,L"zsbag");
	if(!p)
		WLOGH_ERRO << L"miss sbag picture directory";
#ifdef SPECIAL_VERSION
	else
		p->initVetexBuffer(false,40);
#endif

	p = addImageSets(m_imgset_dir,L"zzmask");
	if (!p)
	{
		WLOGH_ERRO << L"add second imageset zzmask failed";
		return false;
	}
#ifdef SPECIAL_VERSION
	p->initVetexBuffer(false,8);
#endif

	p = addImageSets(m_imgset_dir,L"zzanima");
	if (!p)
	{
		WLOGH_ERRO << L"add third imageset zzanima failed";
		return false;
	}
#ifdef SPECIAL_VERSION
	p->initVetexBuffer(false,12);
#endif

	p = addImageSetSingle(m_imgset_dir,L"zzzhuojing",L".jpg");
	if (!p)
	{
		WLOGH_ERRO << L"fire warning image not exists.please add it in the dir";
		return false;
	}
#ifdef SPECIAL_VERSION
	p->initVetexBuffer(false,4);
#endif

	p = addImageSetsMem(cls_gl->m_nVideoSize[vtHost].width,cls_gl->m_nVideoSize[vtHost].height,L"vHost",L"host");
	if (!p)
	{
		WLOGH_ERRO << L"add host picture  failed";
		return false;
	}
#ifdef SPECIAL_VERSION
	p->initVetexBuffer(false,4);
#endif

	p= addImageSetsMem(cls_gl->m_nVideoSize[vtSing].width,cls_gl->m_nVideoSize[vtSing].height,L"vSing",L"sing");
	if (!p)
	{
		WLOGH_ERRO << L"add sing picture  failed";
		return false;
	}
#ifdef SPECIAL_VERSION
	p->initVetexBuffer(false,4);
#endif

	p = addImageSetSingle(cls_gl->m_gCurPlayInfo.m_cur_singerpic,L"singerpic2");
	if(!p)
		WLOGH_WARN << L"load singer picture failed";
	else
	{
#ifdef SPECIAL_VERSION
		p->initVetexBuffer(false,4);
#endif
		std::wstring file1 = cls_gl->getAppDirW();
		file1 = file1+L"\\data\\images\\face_alpha.png";
		p = addImageSets(L"singerpic",L"Singer",file1.c_str(),cls_gl->m_gCurPlayInfo.m_cur_singerpic,GL_TPBLEND_DESTALPHA,GL_TPBLEND_SRCCOLOUR);
		if (!p)
			WLOGH_WARN << L"mix singer picture alpha failed,singer pic path[" << cls_gl->m_gCurPlayInfo.m_cur_singerpic << L"]";
#ifdef SPECIAL_VERSION
		else
			p->initVetexBuffer(false,4);
#endif

	}

	if(!cls_gl->m_downloadUrl.empty())
	{
		p = addImageSetSingle(cls_gl->m_downloadUrl,L"DownloadApp");
		if(!p)
			WLOGH_WARN << L"App Download Scan Code read error";
#ifdef SPECIAL_VERSION
		else
			p->initVetexBuffer(false,4);
#endif
	}

	if(!cls_gl->m_enterRoomUrl.empty())
	{
		p = addImageSetSingle(cls_gl->m_enterRoomUrl,L"EnterRoom");
		if(!p)
			WLOGH_WARN << L"Enter Room Scan Code read error";
#ifdef SPECIAL_VERSION
		else
			p->initVetexBuffer(false,4);
#endif
	}

	return true;
}
