/*
	2014-8-11
	copyright @glp
*/

#ifndef FILEUTIL__H__
#define FILEUTIL__H__

#include "../GLListDef.h"

class CFileUtil
{
public:
	CFileUtil();
	virtual ~CFileUtil();

	/*
	获取模块名字
	@param pName：名字存放地址
	@param nSize：pName空间的大小

	@return :成功，true；失败，false
	*/
	static bool GetModuleName(char* pName,int& nSize);
	static bool GetModuleNameW(wchar_t* pName,int& nSize);
	/*
	获取模块路径
	@param pDir：目录存放地址
	@param nSize：pDir空间的大小

	@return :成功，true；失败，false
	*/
	static bool GetModuleDir(char* pDir,int& nSize);
	static bool GetModuleDirW(wchar_t* pDir,int& nSize);
	/*
	寻找后缀名相同的文件
	@param dir[in] : 需要寻找的目录
	@param pattern[in] : 比对模型，一般用于寻找含有相同字符串的文件，或者后缀名相同的文件
	@files[out] : 返回寻找到的文件
	*/
	static void GetFileListFromDir(std::string dir, const char* pattern, LISTSTRING &files);
	//处理CSV文件的方法接口，用于随机点歌，噱头而已，最后的选歌画面还没开发完，但是已经不需要了。
	/*
	@param csvFile
	@param mc

	@return :成功，true；失败，false
	*/
	static bool ReadCSV(const char* csvFile,MAPCATEGORY& mc);
};

#endif//FILEUTIL__H__

