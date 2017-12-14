/*
	注释时间:2014-4-25
	author: glp
	MV
*/
#pragma once

#include "GLDef.h"


interface IDirect3DDevice9;
interface IDirect3DTexture9;
interface IDirect3DVertexBuffer9;
class CRenderEngine;
class CGLVideo
{	
public:
	CGLVideo();
	virtual ~CGLVideo();

	virtual void		init(IDirect3DDevice9* d3ddev);
	int		getTextureSwitch();
	IDirect3DTexture9* getTextureId();
	bool		getTextureIDInit();
	//BOOL Init(char *fileName);//
	BOOL	loadGLTexturesVideo();
	BOOL	updateGLTextures(IDirect3DTexture9* d3dTexture);//更新纹理
	
	void		setSrcRect( float fTU, float fTV );
	void		setTexture(IDirect3DTexture9* d3dTexture1,IDirect3DTexture9* d3dTexture2);
	void		setOwnListener(CRenderEngine* pListener);

	void		render();
protected:
	//视频数据转为纹理格式的数据
	BOOL	 video2TextureMem(BYTE* pVideo,BYTE* pTexture,int format=3);

protected:
	CCritSec	m_cs;
	BYTE*	m_pvideoData;//视频数据
	BYTE * m_pvideoTextureData;//纹理数据

	/*GLuint	m_texture[2];			//  Texture id
	GLuint m_pboIds[2];			//PBO id*/
	IDirect3DTexture9*			m_texture1;
	IDirect3DTexture9*			m_texture2;
	IDirect3DVertexBuffer9*	m_vertexBuffer;
	reQuad								m_gQuad;
	int										m_switchTextureID;
	CRenderEngine*					m_owner;
#ifdef SPECIAL_VERSION
	CUSTOMVERTEX				m_vertices[4];
#endif//SPECIAL_VERSION
public:
	int				m_videoWidth;//视频宽
	int				m_videoHeight;//视频高
	int				m_DATA_SIZE;
	int				m_textureX;
	int				m_textureY;
	bool				m_bInit;
	bool				m_bUseTexture1;
};
