#include "FileUtil.h"

#ifdef WIN32
#include <Windows.h>
#else
#endif

bool CFileUtil::GetModuleName(char* pName,int& nSize)
{
	if(!pName || nSize==0)
		return false;
	GetModuleFileNameA(NULL,pName,nSize);
	nSize = (int)strlen(pName);
	return true;
}

bool CFileUtil::GetModuleNameW(wchar_t* pName,int& nSize)
{
	if(!pName || nSize==0)
		return false;
	GetModuleFileNameW(NULL,pName,nSize);
	nSize = (int)wcslen(pName);
	return true;
}

bool CFileUtil::GetModuleDir(char* pDir,int& nSize)
{
	if(!pDir || nSize == 0)
		return false;

	static char moduleName[260] = {0};
	static int nModuleNameSize = sizeof(moduleName);
	GetModuleName(moduleName,nModuleNameSize);
	
	nSize = (int)(strrchr(moduleName,'\\')-moduleName);
	memcpy(pDir,moduleName,nSize);
	pDir[nSize] = 0;
	return true;
}

bool CFileUtil::GetModuleDirW(wchar_t* pDir,int& nSize)
{
	if(!pDir || nSize == 0)
		return false;

	static wchar_t moduleName[260] = {0};
	static int nModuleNameSize = sizeof(moduleName);
	GetModuleNameW(moduleName,nModuleNameSize);

	nSize = (int)(wcsrchr(moduleName,L'\\')-moduleName);
	wmemcpy(pDir,moduleName,nSize);
	pDir[nSize] = 0;
	return true;
}

void CFileUtil::GetFileListFromDir(std::string dir, const char* pattern, LISTSTRING &files)
{
	if(!pattern)
	{
		files.clear();
		return;
	}

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = FindFirstFileA((dir+"/*.*").c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do 
	{
		if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if(FindFileData.cFileName[0] != '.')
			{
				char pszTemp[MAX_PATH]={0};
				sprintf(pszTemp,"%s/%s",dir.c_str(),FindFileData.cFileName);
				std::string tmpDir = pszTemp;
				GetFileListFromDir(tmpDir,pattern,files);
			}
		}
		else
		{
			if (strstr(FindFileData.cFileName,pattern))
			{
				std::string str_file = dir + "/" + FindFileData.cFileName;
				files.push_back(str_file);
			}
		}

	}
	while (FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);
}

inline bool parseLine(char* buf,SongListInfo& sli,std::string& category)
{
	if(!buf)
		return false;

	if(buf[0]=='\n' || buf[0]==',')
		return false;

	char* pOld = buf;
	char* pBuffer = strchr(pOld,',');
	if(pBuffer)
	{
		*pBuffer = '\0';
		sli.songname = pOld;
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if (pBuffer)
	{
		*pBuffer = '\0';
		sli.singername = pOld;
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if(pBuffer)
	{
		*pBuffer = '\0';
		sli.filename = pOld;
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if(pBuffer)
	{
		*pBuffer = '\0';
		sli.endsecond = atoi(pOld);
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if (pBuffer)
	{
		pBuffer = '\0';
		category = pOld;
		pOld = pBuffer+1;
	}
	else
	{
		//È¥µô»Ø³µ·û
		int len = (int)strlen(pOld);
		pOld[len-1] = 0;
		category = pOld;
	}

	return true;
}

bool CFileUtil::ReadCSV(const char* csvFile,MAPCATEGORY& mc)
{
	char buf[1024] = {0};

	if(!csvFile)
		goto failed;

	FILE* fp = fopen(csvFile,"r");
	if(!fp)
		goto failed;

	while(fgets(buf,1024,fp))
	{
		SongListInfo sli;
		std::string c;
		if(parseLine(buf,sli,c))
		{
			MAPCATEGORY::iterator it = mc.find(c);
			if(it == mc.end())
			{
				MAPSONGLIST songlist;
				songlist.insert(std::make_pair(sli.songname,sli));
				mc.insert(std::make_pair(c,songlist));
			}
			else
			{
				it->second.insert(std::make_pair(sli.songname,sli));
			}
		}
	}
	return true;

failed: 
	if(fp)
		fclose(fp);
	return false;
}
