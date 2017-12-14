/*
	注释时间:2014-4-25
	author: glp
	hgeParticleSystem 粒子系统的真正实现类
*/
#ifndef HGEPARTICLE_H
#define HGEPARTICLE_H

#include <vector>
#include "../OpenGLWindow.h"
#include "ParticleInfo.h"
#include "hgerect.h"


//#include "../hge/hge.h"
#ifdef HGE_H
#define COMMNEPTR HGE*
#else
#define COMMNEPTR COpenGLWindow*
#endif

typedef struct _maskPoint
{
	GPoint point;
	unsigned char alpha;
}MaskPoint;

typedef std::vector<MaskPoint> VECTPOINT;

class hgeParticleSystem
{
public:	
	hgeParticleSystem(structParticleSystemInfo *psi,float z=GL3D_Z_PARTICLE);
	hgeParticleSystem(const hgeParticleSystem &ps);
	~hgeParticleSystem() {  }

	hgeParticleSystem&	operator= (const hgeParticleSystem &ps);

#ifdef SPECIAL_VERSION
	void				drawRender(bool bAlphaBlend=true);
#else
	void				drawRender();
#endif
	void				FireAt(float x, float y);
	void				Fire();
	void				Stop(bool bKillParticles=false);
	void				Update(float fDeltaTime);
	void				MoveTo(float x, float y, bool bMoveParticles=false);
	void				Transpose(float x, float y) { m_fTx=x; m_fTy=y; }
	void				SetScale(float scale) { m_fScale = scale; }
	void				TrackBoundingBox(bool bTrack) { m_bUpdateBoundingBox=bTrack; }

	int				GetParticlesAlive() const { return m_nParticlesAlive; }
	float				GetAge() const { return m_fAge; }
	void				SetPosition(float x,float y) {m_vecLocation.x=x;m_vecLocation.y=y;m_vecPrevLocation.x=x;m_vecPrevLocation.y=y;}
	void				GetPosition(float *x, float *y) const { *x=m_vecLocation.x; *y=m_vecLocation.y; }
	void				GetTransposition(float *x, float *y) const { *x=m_fTx; *y=m_fTy; }
	float				GetScale() { return m_fScale; }
	hgeRect*		GetBoundingBox(hgeRect *rect) const;
	void				SetPointCreateRect(const CGRect& rect){m_info.pointCreateRect=rect;}
	bool				isStop();

	void				setMaskRGB(const unsigned char* pARGB,int width,int height);
	void				setMaskAlpha(const unsigned char* pALpha,int width,int height);
private:
	hgeParticleSystem();

	static COMMNEPTR s_gl;
protected:
	float					m_fAge;
	float					m_fEmissionResidue;
	bool					m_bOneShotCreate;

	hgeVector2		m_vecPrevLocation;
	hgeVector2		m_vecLocation;
	float					m_fTx, m_fTy;
	float					m_fScale;

	int					m_nParticlesAlive;
	hgeRect			m_rectBoundingBox;
	bool					m_bUpdateBoundingBox;

	structParticle	m_arrparticles[MAX_PARTICLES];
	

	float 				m_fInside_2_A;
	float 				m_fInside_2_B;
	float					m_fOutside_2_A;
	float					m_fOutside_2_B;

	VECTPOINT		m_vectMask;
	bool					m_bClearMask;
	float					m_fZ;
public:
	structParticleSystemInfo m_info;
};


#endif
