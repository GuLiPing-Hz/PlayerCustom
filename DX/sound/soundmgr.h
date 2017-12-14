/*
	注释时间:2014-4-25
	author: glp
	封装对CGLSound类的管理，
*/
#pragma once

#include "sound.h"
#include <map>
#include <string>

typedef std::map<std::string,CGLSound*> MAPSTRGLSOUND;

class CGLSoundMgr
{

public:
	CGLSoundMgr();
	~CGLSoundMgr();

	//设置声音文件的参数属性，目录位置
	void setEnvArg(HWND hwnd,const std::string iniFile,const std::string current_dir);
	//增加音效
	bool addGLSound(const std::string name,bool bLoop=true);
	//获取音效
	CGLSound* getGLSound(const std::string name);
	//清除所有声音
	void clearSound(){releaseSound();}
	//停止所有播放的声音
	void stopAll();
	//初始化所有声音文件
	bool initAllSound();
	//调节stream volume global
	bool	soundSetGStreamVol(DWORD dwV);
protected:
	bool	soundInit(HWND hWnd);
	void	soundUninit();
	void releaseSound();
private:
	bool addGLSound(const std::string name,CGLSound* psound);
private:
	bool									m_bInit;
	HWND								m_hwnd;
	std::string						m_szIniFile;
	std::string						m_sound_dir;
	MAPSTRGLSOUND		m_mapSound;
};
