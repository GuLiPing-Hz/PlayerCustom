#ifndef SWF__H__
#define SWF__H__

#include "../GLDef.h"
#include "ISwfListener.h"

interface IDirect3DDevice9;
interface IDirect3DTexture9;
interface IDirect3DVertexBuffer9;

namespace gameswf
{
	struct player;
	struct root;
}

class CSwf
{
public:
	CSwf();
	virtual ~CSwf();

	static void InitSwfRender();
	static void UninitSwfRender();
	bool initTexture();
	void uninitTexture();
	bool loadFile(gameswf::player* pPlayer,const char* swf);

	void update(unsigned int delta_ms);
	void render();

	void setLoop(bool bLoop){m_bLoop=bLoop;}
	//用于自动暂停后，可以重头开始播放
	void resumePlay();

	void setSwfListener(ISwfListener* pListener){m_pSwfListener=pListener;}
private:
	ISwfListener*			m_pSwfListener;

	//gameswf::player*	m_pPlayer;
	gameswf::root*		m_pRoot;
	bool							m_bPlay;
	bool							m_bLoop;

	reQuad					m_gQuad;
	IDirect3DTexture9* m_pTex;
};

#endif//SWF__H__
