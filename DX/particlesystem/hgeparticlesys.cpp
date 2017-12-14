#include "stdafx.h"
#include "hgeparticlesys.h"
#include <assert.h>


//HGE	*hgeParticleSystem::cls_gl=0;
COMMNEPTR hgeParticleSystem::s_gl = NULL;

hgeParticleSystem::hgeParticleSystem(structParticleSystemInfo *psi,float z)
:m_bOneShotCreate(true)
,m_bClearMask(false)
,m_fZ(z)
{
#ifdef HGE_H
	s_gl = hgeCreate(HGE_VERSION);
#else
	s_gl = cls_gl;
#endif
	
	memcpy(&m_info, psi, sizeof(structParticleSystemInfo));
	m_fInside_2_A = m_info.circleInsideA*m_info.circleInsideA;
	m_fInside_2_B = m_info.circleInsideB*m_info.circleInsideB;
	m_fOutside_2_A = m_info.circleOutsideA*m_info.circleOutsideA;
	m_fOutside_2_B = m_info.circleOutsideB*m_info.circleOutsideB;

	m_vecLocation.x=m_vecPrevLocation.x=m_info.fOriginX;
	m_vecLocation.y=m_vecPrevLocation.y=m_info.fOriginY;
	m_fTx=m_fTy=0;
	m_fScale = 1.0f;

	m_fEmissionResidue=0.0f;
	m_nParticlesAlive=0;
	m_fAge=-2.0;

	m_rectBoundingBox.Clear();
	m_bUpdateBoundingBox=false;
}

hgeParticleSystem::hgeParticleSystem(const hgeParticleSystem &ps)
{
	memcpy(this, &ps, sizeof(hgeParticleSystem));
#ifdef HGE_H
	s_gl = hgeCreate(HGE_VERSION);
#else
	s_gl = cls_gl;
#endif
}

void hgeParticleSystem::MoveTo(float x, float y, bool bMoveParticles)
{
	int i;
	float dx,dy;
	
	if(bMoveParticles)
	{
		dx=x-m_vecLocation.x;
		dy=y-m_vecLocation.y;

		for(i=0;i<m_nParticlesAlive;i++)
		{
			m_arrparticles[i].vecLocation.x += dx;
			m_arrparticles[i].vecLocation.y += dy;
		}

		m_vecPrevLocation.x=m_vecPrevLocation.x + dx;
		m_vecPrevLocation.y=m_vecPrevLocation.y + dy;
	}
	else
	{
		if(m_fAge==-2.0) { m_vecPrevLocation.x=x; m_vecPrevLocation.y=y; }
		else { m_vecPrevLocation.x=m_vecLocation.x;	m_vecPrevLocation.y=m_vecLocation.y; }
	}

	m_vecLocation.x=x;
	m_vecLocation.y=y;
}

void hgeParticleSystem::FireAt(float x, float y)
{
	Stop();
	MoveTo(x,y);
	Fire();
}

void hgeParticleSystem::Fire()
{
	if(m_info.fLifetime+1.0f <= 0.001f) m_fAge=-1.0f;
	else m_fAge=0.0f;
}

bool	hgeParticleSystem::isStop()
{
	return (m_fAge+2.0f < 0.001f);
}

void	hgeParticleSystem::setMaskRGB(const unsigned char* pARGB,int width,int height)
{
	if (pARGB)
	{
		const unsigned char* pA = pARGB;
		for(int i=0;i<height;i++)
		{
			for(int j=0;j<width;j++)
			{
				MaskPoint point;
				point.alpha = *pA;
				pA += 4;
				if(point.alpha > 0)
				{
					point.point = GPoint((float)j+1,(float)i+1);
					m_vectMask.push_back(point);
				}
			}
		}
	}
	else
	{
		m_bClearMask = true;
	}
}

void	hgeParticleSystem::setMaskAlpha(const unsigned char* pALpha,int width,int height)
{
	if(pALpha)
	{
		const unsigned char* pA = pALpha;
		for(int i=0;i<height;i++)
		{
			for(int j=0;j<width;j++)
			{
				MaskPoint point;
				point.alpha = *pA;
				pA ++;
				if(point.alpha > 0)
				{
					point.point = GPoint((float)j+1,(float)i+1);
					m_vectMask.push_back(point);
				}
			}
		}
	}
	else
	{
		m_bClearMask = true;
	}
}

void hgeParticleSystem::Stop(bool bKillParticles)
{
	m_fAge=-2.0f;
	if(bKillParticles) 
	{
		m_nParticlesAlive=0;
		m_rectBoundingBox.Clear();
	}
}

#ifdef SPECIAL_VERSION
void hgeParticleSystem::drawRender(bool bAlphaBlend)
{
	int i;
	DWORD col;
	structParticle *par=m_arrparticles;

	assert(m_info.sprite != NULL);

	col=m_info.sprite->GetColor();

	for(i=0; i<m_nParticlesAlive; i++)
	{
		if(m_info.colColorStart.r < 0)
			m_info.sprite->SetColor(SETA(m_info.sprite->GetColor(),par->colColor.a*255));
		else
			m_info.sprite->SetColor(par->colColor.GetHWColor());

#ifdef HGE_H
		m_info.sprite->RenderEx(par->vecLocation.x*m_fScale+m_fTx, par->vecLocation.y*m_fScale+m_fTy, par->fSpin*par->fAge, par->fSize*m_fScale);
#else
		m_info.sprite->drawRenderEx(par->vecLocation.x*m_fScale, par->vecLocation.y*m_fScale, par->fSpin/**par->fAge*/,bAlphaBlend, par->fSize*m_fScale);
#endif
		par++;
	}

	m_info.sprite->SetColor(col);
}
#else
void hgeParticleSystem::drawRender()
{
	int i;
	DWORD col;
	structParticle *par=m_arrparticles;

	if(m_info.sprite == NULL)
		return ;
	
	col=m_info.sprite->GetColor();
	
	for(i=0; i<m_nParticlesAlive; i++)
	{
		if(m_info.colColorStart.r < 0)
			m_info.sprite->SetColor(SETA(m_info.sprite->GetColor(),par->colColor.a*255));
		else
			m_info.sprite->SetColor(par->colColor.GetHWColor());

#ifdef HGE_H
		m_info.sprite->RenderEx(par->vecLocation.x*m_fScale+m_fTx, par->vecLocation.y*m_fScale+m_fTy, par->fSpin*par->fAge, par->fSize*m_fScale);
#else
		m_info.sprite->drawRenderEx(par->vecLocation.x*m_fScale, par->vecLocation.y*m_fScale,m_fZ, par->fSpin, par->fSize*m_fScale);
#endif
		par++;
	}
	//m_info.sprite->SetColor(col);
}
#endif

hgeRect *hgeParticleSystem::GetBoundingBox(hgeRect *rect) const
{
	*rect = m_rectBoundingBox;

	rect->x1 *= m_fScale;
	rect->y1 *= m_fScale;
	rect->x2 *= m_fScale;
	rect->y2 *= m_fScale;

	return rect;
}
