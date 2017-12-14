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
	��ȡģ������
	@param pName�����ִ�ŵ�ַ
	@param nSize��pName�ռ�Ĵ�С

	@return :�ɹ���true��ʧ�ܣ�false
	*/
	static bool GetModuleName(char* pName,int& nSize);
	static bool GetModuleNameW(wchar_t* pName,int& nSize);
	/*
	��ȡģ��·��
	@param pDir��Ŀ¼��ŵ�ַ
	@param nSize��pDir�ռ�Ĵ�С

	@return :�ɹ���true��ʧ�ܣ�false
	*/
	static bool GetModuleDir(char* pDir,int& nSize);
	static bool GetModuleDirW(wchar_t* pDir,int& nSize);
	/*
	Ѱ�Һ�׺����ͬ���ļ�
	@param dir[in] : ��ҪѰ�ҵ�Ŀ¼
	@param pattern[in] : �ȶ�ģ�ͣ�һ������Ѱ�Һ�����ͬ�ַ������ļ������ߺ�׺����ͬ���ļ�
	@files[out] : ����Ѱ�ҵ����ļ�
	*/
	static void GetFileListFromDir(std::string dir, const char* pattern, LISTSTRING &files);
	//����CSV�ļ��ķ����ӿڣ����������裬��ͷ���ѣ�����ѡ�軭�滹û�����꣬�����Ѿ�����Ҫ�ˡ�
	/*
	@param csvFile
	@param mc

	@return :�ɹ���true��ʧ�ܣ�false
	*/
	static bool ReadCSV(const char* csvFile,MAPCATEGORY& mc);
};

#endif//FILEUTIL__H__

