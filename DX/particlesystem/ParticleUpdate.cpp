/*
	注释时间:2014-4-25
	author: glp
	粒子系统更新的具体实现，单独拎出来，比较容易修改
*/
#include "stdafx.h"
#include "hgeparticlesys.h"

#define COLORCLAMP1_0(x) \
(x) = ((x)<0.0f)?0.0f:((x)>1.0f)?1.0f:(x)

#define COLORCLAMP255(x) \
(x) = ((x)<0)?0:((x)>255)?255:(x)

#define COLORCHANGE(x) \
if(par->bAdd##x) \
{ \
	par->colColor.x += par->colColorDelta.x*fDeltaTime; \
	if(par->colColor.x>par->colColorMax.x) \
	{ \
		par->colColor.x = par->colColorMax.x; \
		par->colColorDelta.x = -par->colColorDelta.x;\
		par->bAdd##x = false; \
	} \
} \
else \
{ \
	par->colColor.x += par->colColorDelta.x*fDeltaTime; \
	if(par->colColor.x<par->colColorMin.x) \
	{ \
		par->colColor.x = par->colColorMin.x; \
		par->colColorDelta.x = -par->colColorDelta.x;\
		par->bAdd##x = true; \
	} \
}

void hgeParticleSystem::Update(float fDeltaTime)
{
	int i;
	float ang;
	structParticle *par;
	hgeVector2 vecAccel, vecAccel2;

	if(m_fAge >= 0)
	{
		m_fAge += fDeltaTime;
		if(m_fAge >= m_info.fLifetime) m_fAge = -2.0f;
	}

	// update all alive particles
	if(m_bUpdateBoundingBox) m_rectBoundingBox.Clear();
	par=m_arrparticles;

	for(i=0; i<m_nParticlesAlive; i++)
	{
		par->fAge += fDeltaTime;
		if(par->fAge >= par->fTerminalAge)
		{
			m_nParticlesAlive--;
			if (m_info.bOneShot&&m_nParticlesAlive==0)
			{
				m_bOneShotCreate = true;
			}
			memcpy(par, &m_arrparticles[m_nParticlesAlive], sizeof(structParticle));
			i--;
			continue;
		}

		if(par->bRadiusMode)
		{
			par->fAngle += par->fRotatePerSecond*fDeltaTime;
			par->fRadius += par->fRadiusDelta*fDeltaTime;

			par->vecLocation.x = m_info.fOriginX-cosf(par->fAngle)*par->fRadius;
			par->vecLocation.y = m_info.fOriginY-sinf(par->fAngle)*par->fRadius;
		}
		else
		{
			vecAccel = par->vecLocation-m_vecLocation;
			vecAccel.Normalize();
			vecAccel2 = vecAccel;//(cos(a),sin(a));
			vecAccel *= par->fRadialAccel;//径向加速度

			// vecAccel2.Rotate(M_PI_2);
			// the following is faster
			ang = vecAccel2.x;
			vecAccel2.x = -vecAccel2.y;
			vecAccel2.y = ang;//(-sin(a),cos(a));

			vecAccel2 *= par->fTangentialAccel;//切向加速度
			//径向，切向，力方向的加速度叠加
			par->vecVelocity += (vecAccel+vecAccel2+par->fForce)*fDeltaTime;
			par->vecLocation += par->vecVelocity*fDeltaTime;
		}
		//自身旋转
		par->fSpin += par->fSpinDelta*fDeltaTime;
		//大小变化
		par->fSize += par->fSizeDelta*fDeltaTime;
		//颜色变化
		if(par->bColorOscillate)
		{//震荡模式
			COLORCHANGE(r);
			COLORCHANGE(g);
			COLORCHANGE(b);
			COLORCHANGE(a);
		}
		else//渐变模式
			par->colColor += par->colColorDelta*fDeltaTime;

		if(m_bUpdateBoundingBox) m_rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

		par++;
	}

	// 生成新的粒子
	if(m_fAge != -2.0f)
	{
		if ((m_info.bOneShot&&m_bOneShotCreate) || !m_info.bOneShot)
		{
			int nParticlesCreated;
			if (m_info.bOneShot)
			{
				nParticlesCreated = m_info.nEmission;
				m_bOneShotCreate = false;
			}
			else
			{
				float fParticlesNeeded = m_info.nEmission*fDeltaTime + m_fEmissionResidue;
				nParticlesCreated = (unsigned int)fParticlesNeeded;
				m_fEmissionResidue=fParticlesNeeded-nParticlesCreated;
			}

			if (m_nParticlesAlive >= MAX_PARTICLES)
			{
				goto backout;
			}
			par=&m_arrparticles[m_nParticlesAlive];

			for(i=0; i<nParticlesCreated; i++)
			{
				if(m_nParticlesAlive>=MAX_PARTICLES) break;
				par->fAlpha = 1.0f;
				par->fAge = 0.0f;
				//随机寿命
				par->fTerminalAge = cls_gl->Random_Float(m_info.fParticleLifeMin, m_info.fParticleLifeMax);

				//随机方向 这个要大改psi 可能
				ang=m_info.fDirection+cls_gl->Random_Float(-m_info.fSpread,m_info.fSpread);
				//随机位置
				par->bRadiusMode = m_info.bRadiusMode;
				if(par->bRadiusMode)
				{
					par->fAngle = ang;
					par->fRotatePerSecond = m_info.fRotatePerSecond+cls_gl->Random_Float(-m_info.fRotatePerSecondVar,m_info.fRotatePerSecondVar);
					par->fRadius = m_info.fRadiusStart+cls_gl->Random_Float(-m_info.fRadiusStartVar,m_info.fRadiusStartVar);
					float fRadiusEnd = m_info.fRadiusEnd+cls_gl->Random_Float(-m_info.fRadiusEndVar,m_info.fRadiusEndVar);
					par->fRadiusDelta = (fRadiusEnd-par->fRadius)/par->fTerminalAge;

					par->vecLocation.x = m_info.fOriginX-cosf(par->fAngle)*par->fRadius;
					par->vecLocation.y = m_info.fOriginY-sinf(par->fAngle)*par->fRadius;
				}
				else
				{
					if (m_info.nEmitter & EMITTER_RECT)//矩形发射器
					{	
						par->vecLocation.x = cls_gl->Random_Float(m_info.pointCreateRect.origin.x,m_info.pointCreateRect.origin.x+m_info.pointCreateRect.size.width);
						par->vecLocation.y = cls_gl->Random_Float(m_info.pointCreateRect.origin.y,m_info.pointCreateRect.origin.y+m_info.pointCreateRect.size.height);
					}
					else if (m_info.nEmitter & EMITTER_POINT)//点发射器
					{
						par->vecLocation = m_vecPrevLocation;//+(m_vecLocation-m_vecPrevLocation)*cls_gl->Random_Float(-1.0f, 1.0f);
						if (m_info.bSpread)
						{
							par->vecLocation.x += cls_gl->Random_Float(-2.0f, 2.0f);
							par->vecLocation.y += cls_gl->Random_Float(-2.0f, 2.0f);
						}
					}
					else if (m_info.nEmitter & EMITTER_CIRCLE)//圈圈发射器
					{
						m_fInside_2_A = m_info.circleInsideA*m_info.circleInsideA;
						m_fInside_2_B = m_info.circleInsideB*m_info.circleInsideB;

						float tmpX = cls_gl->Random_Float(-m_info.circleInsideA,m_info.circleInsideA);
						float tmpY = (float) sqrt(m_fInside_2_B-m_fInside_2_B*tmpX*tmpX/m_fInside_2_A);
						tmpY = cls_gl->Random_Float(-tmpY,tmpY);
						par->vecLocation.x = tmpX+m_info.fOriginX;
						par->vecLocation.y = tmpY+m_info.fOriginY;
					}
					else if (m_info.nEmitter & EMITTER_RING)//圆环发射器
					{
						if (m_info.circleInsideA<=m_info.circleOutsideA || m_info.circleInsideB<=m_info.circleOutsideB)//不在要求内
						{
							goto backout;
						}

						m_fInside_2_A = m_info.circleInsideA*m_info.circleInsideA;
						m_fInside_2_B = m_info.circleInsideB*m_info.circleInsideB;

						float tmpX = cls_gl->Random_Float(-m_info.circleInsideA,m_info.circleInsideA);
						float tmpY1,tmpY2;
						tmpY1 = (float) sqrt(m_fInside_2_B - m_fInside_2_B*tmpX*tmpX/m_fInside_2_A);
						if ((tmpX>-m_info.circleInsideA&&tmpX<-m_info.circleOutsideA) || (tmpX>m_info.circleOutsideA&&tmpX<m_info.circleInsideA))
						{
							tmpY2 = -tmpY1;
						}
						else
						{
							static bool bControl = true;

							m_fOutside_2_A = m_info.circleOutsideA*m_info.circleOutsideA;
							m_fOutside_2_B = m_info.circleOutsideB*m_info.circleOutsideB;

							tmpY2 = (float) sqrt(m_fOutside_2_B - m_fOutside_2_B*tmpX*tmpX/m_fOutside_2_A);

							if (bControl)
							{
								bControl = false;
							}
							else
							{
								float tmp = tmpY1;
								tmpY1 = -tmpY2;
								tmpY2 = -tmp;
								bControl =true;
							}

						}
						float tmpY = cls_gl->Random_Float(tmpY2,tmpY1);
						par->vecLocation.x = tmpX+m_info.fOriginX;
						par->vecLocation.y = tmpY+m_info.fOriginY;
					}
					else if(m_info.nEmitter & EMITTER_MASK)//遮罩发射器
					{
						if (m_bClearMask)
						{
							m_vectMask.clear();
							m_bClearMask = false;
						}

						if (m_vectMask.empty())
						{
							goto backout;
						}

						int index = cls_gl->Random_Int(0,(int)(m_vectMask.size()-1));
						par->fAlpha = m_vectMask[index].alpha/255.0f;
						par->vecLocation.x = m_info.pointCreateRect.origin.x+m_vectMask[index].point.x;
						par->vecLocation.y = m_info.pointCreateRect.origin.y+m_vectMask[index].point.y; 
					}
					else 
					{
						goto backout;
					}

					if(m_info.bRelative) ang += (m_vecPrevLocation-m_vecLocation).Angle()+M_PI_2;
					par->vecVelocity.x = cosf(ang);
					par->vecVelocity.y = sinf(ang);

					//分成两半
					if (m_info.btwoEnds)
					{
						static bool bControl = true;
						if (bControl)
						{
							par->vecVelocity *=m_info.fSpeedMin;
							bControl = false;
						}
						else
						{
							par->vecVelocity *=m_info.fSpeedMax;
							bControl = true;
						}
					}
					else
					{
						par->vecVelocity *= cls_gl->Random_Float(m_info.fSpeedMin, m_info.fSpeedMax);
					}
					//随机力量
					par->fForce.x = cls_gl->Random_Float(m_info.fForceMin.x, m_info.fForceMax.x);
					par->fForce.y = cls_gl->Random_Float(m_info.fForceMin.y, m_info.fForceMax.y);
					//随机径向加速度
					par->fRadialAccel = cls_gl->Random_Float(m_info.fRadialAccelMin, m_info.fRadialAccelMax);
					//随机切向加速度
					par->fTangentialAccel = cls_gl->Random_Float(m_info.fTangentialAccelMin, m_info.fTangentialAccelMax);
				}

				//随机大小
				par->fSize = m_info.fSizeStart  + cls_gl->Random_Float(-m_info.fSizeVarStart, m_info.fSizeVarStart);
				float fSizeEnd = m_info.fSizeEnd + cls_gl->Random_Float(-m_info.fSizeVarEnd,m_info.fSizeVarEnd);
				par->fSizeDelta = (fSizeEnd-par->fSize) / par->fTerminalAge;
				//随机旋转
				par->fSpin = m_info.fSpinStart + cls_gl->Random_Float(-m_info.fSpinVarStart,m_info.fSpinVarStart);
				float fSpinEnd = m_info.fSpinEnd + cls_gl->Random_Float(-m_info.fSpinVarEnd,m_info.fSpinVarEnd);
				par->fSpinDelta = (fSpinEnd-par->fSpin) / par->fTerminalAge;
				//随机颜色
				par->bColorOscillate = m_info.bColorOscillate;
				if(par->bColorOscillate)
				{
					//最小最大颜色震荡变化
					par->colColorMin.r = m_info.colColorMin.r + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					COLORCLAMP1_0(par->colColorMin.r);
					par->colColorMax.r = m_info.colColorMax.r + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					COLORCLAMP1_0(par->colColorMax.r);

					par->colColorMin.g = m_info.colColorMin.g + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					COLORCLAMP1_0(par->colColorMin.g);
					par->colColorMax.g = m_info.colColorMax.g + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					COLORCLAMP1_0(par->colColorMax.g);

					par->colColorMin.b = m_info.colColorMin.b + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					COLORCLAMP1_0(par->colColorMin.b);
					par->colColorMax.b = m_info.colColorMax.b + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					COLORCLAMP1_0(par->colColorMax.b);

					par->colColorMin.a = m_info.colColorMin.a + cls_gl->Random_Float(-m_info.fAlphaVar,m_info.fAlphaVar);
					COLORCLAMP1_0(par->colColorMin.a);
					par->colColorMax.a = m_info.colColorMax.a + cls_gl->Random_Float(-m_info.fAlphaVar,m_info.fAlphaVar);
					COLORCLAMP1_0(par->colColorMax.a);
					
					par->colColor.r = cls_gl->Random_Float(par->colColorMin.r, par->colColorMax.r);
					par->colColor.g = cls_gl->Random_Float(par->colColorMin.g, par->colColorMax.g);
					par->colColor.b = cls_gl->Random_Float(par->colColorMin.b, par->colColorMax.b);
					par->colColor.a = cls_gl->Random_Float(par->colColorMin.a, par->colColorMax.a);

					if(par->colColorMin.r == par->colColorMax.r)
						par->colColorDelta.r = 0.0f;
					else
						par->colColorDelta.r = m_info.colColorStep.r + cls_gl->Random_Float(-m_info.fStepColorVar, m_info.fStepColorVar);
					//COLORCLAMP1_0(par->colColorDelta.r);
					if(par->colColorMin.g == par->colColorMax.g)
						par->colColorDelta.g = 0.0f;
					else
						par->colColorDelta.g = m_info.colColorStep.g + cls_gl->Random_Float(-m_info.fStepColorVar, m_info.fStepColorVar);
					//COLORCLAMP1_0(par->colColorDelta.g);
					if(par->colColorMin.b == par->colColorMax.b)
						par->colColorDelta.b = 0.0f;
					else
						par->colColorDelta.b = m_info.colColorStep.b + cls_gl->Random_Float(-m_info.fStepColorVar, m_info.fStepColorVar);
					//COLORCLAMP1_0(par->colColorDelta.b);
					if(par->colColorMin.a == par->colColorMax.a)
						par->colColorDelta.a = 0.0f;
					else
						par->colColorDelta.a = m_info.colColorStep.a + cls_gl->Random_Float(-m_info.fStepAlphaVar, m_info.fStepAlphaVar);
					//COLORCLAMP1_0(par->colColorDelta.a);

					par->bAdda = par->colColorDelta.a >= 0;
					par->bAddr = par->colColorDelta.r >= 0;
					par->bAddg = par->colColorDelta.g >= 0;
					par->bAddb = par->colColorDelta.b >= 0;;
				}
				else
				{
					//起始颜色到结束颜色的渐变
					par->colColor.r = m_info.colColorStart.r + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					par->colColor.g = m_info.colColorStart.g + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					par->colColor.b = m_info.colColorStart.b + cls_gl->Random_Float(-m_info.fColorVar,m_info.fColorVar);
					par->colColor.a = m_info.colColorStart.a + cls_gl->Random_Float(-m_info.fAlphaVar,m_info.fAlphaVar);
					//颜色变化的量
					par->colColorDelta.r = (m_info.colColorEnd.r-par->colColor.r) / par->fTerminalAge;
					par->colColorDelta.g = (m_info.colColorEnd.g-par->colColor.g) / par->fTerminalAge;
					par->colColorDelta.b = (m_info.colColorEnd.b-par->colColor.b) / par->fTerminalAge;
					par->colColorDelta.a = par->fAlpha*(m_info.colColorEnd.a-par->colColor.a) / par->fTerminalAge;
				}
				//demo中使用，是否更新边框
				if(m_bUpdateBoundingBox) m_rectBoundingBox.Encapsulate(par->vecLocation.x, par->vecLocation.y);

				m_nParticlesAlive++;
				par++;
			}
		}
// 		else
// 			m_bOneShotCreate = true;
	}

backout:
	m_vecPrevLocation=m_vecLocation;
}
