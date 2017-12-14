#ifndef SWFMGR__H__
#define SWFMGR__H__

#include "../GLListDef.h"
#include "Swf.h"

typedef std::map<int,CSwf*> MAPIDSWF;

class CSwfMgr
{
public:
	CSwfMgr();
	~CSwfMgr();

	bool initSwfRender();
	void uninitSwfRender();

	//����һ��Swf�ļ�����Ԥ����
	bool addSwf(int id,const char* file);

	//�������Swf
	void clearSwf();
	
	//������Ҫʹ�õ���swf�ļ�����ȡ����Ŀ¼
	bool initAllSwf();

	
	//��ȡָ��id��Swf
	CSwf* getSwf(int id);
	std::string getSwfFile(int id);
private:
	bool initAllSwfTexture();
	void uninitAllSwfTexture();
	bool addSwfToMap(int id,CSwf* pSwf);
	bool addSwfToMap(int id,const std::string file);
private:
	//gameswf::player*	m_pPlayer;
	MAPIDSWF				m_mapSwf;
	MAPIDSTRING		m_mapSwfFile;
	LISTSTRING				m_listFiles;
};

#endif//SWFMGR__H__
