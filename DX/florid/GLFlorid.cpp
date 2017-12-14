#include "StdAfx.h"
#include "GLFlorid.h"
#include "../staff/image/ImgsetMgr.h"
#include "../particlesystem/hgeparticlesys.h"
#include "../ximage/ximage.h"

CGLFlorid::CGLFlorid(const std::string szInFile,bool bFromOther)
:m_particleSystem(NULL)
,m_szIniFile(szInFile)
,m_imgset(NULL)
,m_nCount(0)
,m_bHalf(false)
,m_bClockwise (true)
,m_fCircle_a(0)
,m_fCircle_b(0)
,m_destX(0/*mx*/)
,m_destY(0/*my*/)
,m_liveTime(0)
,m_lastTime(0)
,m_x1(0)
,m_y1(0)
,m_x2(0)
,m_y2(0)
,m_speedLine(0)
,m_speedCircle(0)
,m_bFloridLineInit(false)
,m_bCircleInit(false)
,m_bFromOther(bFromOther)
{
}

CGLFlorid::~CGLFlorid(void)
{
	donePS();
}


void CGLFlorid::setImageset(Imageset* imgset,const std::string name)
{
	m_imgset = imgset;
	m_imgName = name;
}
static bool s_bAlphaBlend = false;
void CGLFlorid::loadPSI(const wchar_t* appDir,int n)
{
	HANDLE hF;
	DWORD size;//, col1, col2;
	wchar_t filename[_MAX_PATH] = {0};

	if(!m_particleSystem) return;

	std::string strPsiDir = Ini_GetString(m_szIniFile.c_str(),"DIR","PsiDir","");
	if (strPsiDir == "")
	{
		WLOGH_ERRO << L"Psi dir is Null";
		return ;
	}
	std::wstring wstrPsiDir;
	Str2Wstr(strPsiDir,wstrPsiDir);
	swprintf(filename,L"%s%s/particle%d.psi",appDir,wstrPsiDir.c_str(),n);
	hF = CreateFileW( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if(hF == INVALID_HANDLE_VALUE)
	{
		WLOGH_ERRO << L"psi open error or no such file :" << filename;
		return;
	}
	ReadFile(hF, &m_particleSystem->m_info, sizeof(structParticleSystemInfo), &size, NULL );
	SAFE_CLOSEHANDLE(hF);
	
	if(!m_bFromOther)
	{
		//cls_gl->m_pImgsetMgr->m_floridFileImgset.push_back(std::string(m_particleSystem->m_info.textureFile));
		m_imgset = cls_gl->m_pImgsetMgr->addImageSetSingle(m_particleSystem->m_info.textureFile);
		if(!m_imgset)
		{
			WLOGH_ERRO << L"imageset load failed : ",m_particleSystem->m_info.textureFile;
			return ;
		}
#ifdef SPECIAL_VERSION
		m_imgset->initVetexBuffer(true,1000);
#endif//SPECIAL_VERSION
	}

	setOriginPoint();

	unsigned long blend = (unsigned long)(((DWORD_PTR)(m_particleSystem->m_info.sprite) )>> 16);
#ifdef SPECIAL_VERSION
	s_bAlphaBlend = !! (blend&BLEND_ALPHAMASK ) & BLEND_ALPHABLEND;
#endif
	if(!m_bFromOther)//暂时用这个替代
	{
		std::string img_name = m_particleSystem->m_info.textureFile;
		m_imgName = img_name.substr(0,img_name.rfind(".png"));
	}

	m_particleSystem->m_info.sprite=(CGLSprite*)(m_imgset?(m_imgset->getImage(m_imgName)):NULL);
	if(m_particleSystem->m_info.sprite)
	{
		m_particleSystem->m_info.sprite->setHotSpot();
		m_particleSystem->m_info.sprite->setBlendMode(blend);
	}
	else
	{
		WLOGH_ERRO << L"image get failed";
		return ;
	}
	// System parameters
	if(m_particleSystem->m_info.fLifetime==-1.0f)
	{
		//m_particleSystem->Fire();
	}
}

void CGLFlorid::initPS(const wchar_t* appDir,int npsi,float z)
{
	if(m_particleSystem)
		delete m_particleSystem;
	structParticleSystemInfo psi;
	
	memset(&psi, 0, sizeof(structParticleSystemInfo));
	m_particleSystem = new hgeParticleSystem(&psi,z);
	if (!m_particleSystem)
	{
		WLOGH_ERRO << L"m_particleSystem new failed";
		return ;
	}
	loadPSI(appDir,npsi);//load *.psi 文件
}
void CGLFlorid::donePS()
{
	SAFE_DELETE(m_particleSystem);
}

void CGLFlorid::renderPS()
{
	if(!m_particleSystem)
		return;
#ifdef SPECIAL_VERSION
	if (s_bAlphaBlend)
	{
		m_particleSystem->drawRender(true);//画在第一个buf中
	}
	else
	{
		m_particleSystem->drawRender(false);//画在第二个buf中
	}
#else
	m_particleSystem->drawRender();
#endif//SPECIAL_VERSION
}

void CGLFlorid::updatePS(const float delta_second)
{
	if (m_particleSystem)
	{
		if (m_liveTime > 0)
		{
			m_lastTime += delta_second;
			if (m_lastTime >= m_liveTime)
			{
				m_particleSystem->Stop();//停止生成新的粒子
			}
		}
		m_particleSystem->Update(delta_second);
	}
}

void CGLFlorid::setOriginPoint()
{
	setPosition(m_particleSystem->m_info.fOriginX,m_particleSystem->m_info.fOriginY);
}

void CGLFlorid::setOriginPoint(float x,float y)
{
	if(!m_particleSystem)
		return;
	m_particleSystem->m_info.fOriginX=x;
	m_particleSystem->m_info.fOriginY=y;
	setOriginPoint();
}

void CGLFlorid::setPosition(float x,float y)//设置位置
{
	if(!m_particleSystem)
		return;
	m_particleSystem->SetPosition(x,y);
}


void CGLFlorid::moveTo(float x,float y,bool bMoveAll)
{
	if (m_particleSystem)
		m_particleSystem->MoveTo(x,y,bMoveAll);
}

void CGLFlorid::moveLineToY(float x,float& y,float delta_tm,int speed,bool bAll)
{
	float preX;
	float preY;
	if (m_particleSystem)
	{
		m_particleSystem->GetPosition(&preX,&preY);
		float fChange = delta_tm*speed;
		if (fChange >= 1)
		{
			fChange = 1;
		}
		y = preY + (y-preY)*fChange;
		m_particleSystem->MoveTo(x,y,bAll);
	}
}

void CGLFlorid::moveLineTo(float x,float y,float deltaTime,int speed)
{
	float preX;
	float preY;
	if (m_particleSystem)
	{
		m_particleSystem->GetPosition(&preX,&preY);
		float fChange = deltaTime*speed;
		if (fChange >= 1)
		{
			fChange = 1;
		}
		m_particleSystem->MoveTo(preX+(x-preX)*deltaTime*speed,preY+(y-preY)*deltaTime*speed,true);
	}
}

void CGLFlorid::moveCircle(const float deltaTime)
{
	if (!m_bCircleInit || !m_particleSystem)
	{
		return;
	}
	float preX;
	float preY;
	m_particleSystem->GetPosition(&preX,&preY);
	bool bneedCal = false;
	if (m_bHalf)
	{
		if (preX<=(m_destX+m_particleSystem->m_info.fOriginX+0.001))
		{
			m_nCount --;
			bneedCal = true;
			if (m_nCount == 0)
			{
				m_bHalf = false;
			}
		}
	}
	else
	{
		if (preX>=(m_destX+m_particleSystem->m_info.fOriginX-0.001))
		{
			m_nCount ++;
			bneedCal = true;
			if (m_nCount == 25)
			{
				m_bHalf = true;
			}
		}
	}
	if (bneedCal)
	{
		m_destX = -m_fCircle_a+2*m_fCircle_a*m_nCount/25.0f;
		if (m_bHalf)
		{
			if (m_bClockwise )
			{
				m_destY = (float) sqrt(m_fCircle_b*m_fCircle_b - m_destX*m_destX*m_fCircle_b*m_fCircle_b/m_fCircle_a/m_fCircle_a);
			}
			else
			{
				m_destY = (float)-sqrt(m_fCircle_b*m_fCircle_b - m_destX*m_destX*m_fCircle_b*m_fCircle_b/m_fCircle_a/m_fCircle_a);
			}
		}
		else
		{
			if (m_bClockwise )
			{
				m_destY = (float)-sqrt(m_fCircle_b*m_fCircle_b - m_destX*m_destX*m_fCircle_b*m_fCircle_b/m_fCircle_a/m_fCircle_a);
			}
			else
			{
				m_destY = (float)sqrt(m_fCircle_b*m_fCircle_b - m_destX*m_destX*m_fCircle_b*m_fCircle_b/m_fCircle_a/m_fCircle_a);
			}
		}
	}
	
	moveLineTo(m_destX+m_particleSystem->m_info.fOriginX,m_destY+m_particleSystem->m_info.fOriginY,deltaTime,m_speedCircle);
}

void CGLFlorid::move(const float deltaTime)
{
	if (m_bCircleInit)
	{
		moveCircle(deltaTime);
	}
	if (m_bFloridLineInit)
	{
		moveFloridLine(deltaTime);
	}
}

void CGLFlorid::setMoveFloridLine(float x1,float y1,float x2,float y2,int speed)
{
	m_x1 = x1;
	m_y1 = y1;
	m_x2 = x2;
	m_y2 = y2;
	m_speedLine = speed;
	m_bFloridLineInit = true;
}

void CGLFlorid::moveFloridLine(const float deltaTime)
{
	
	if (!m_bFloridLineInit || !m_particleSystem)
	{
		return ;
	}

	static bool bFire = true;
	if (bFire)
	{
		m_particleSystem->SetPosition(m_x1,m_y1);
		m_particleSystem->Fire();
	}
	moveLineTo(m_x2,m_y2,deltaTime,m_speedLine);
	bFire = false;
	float preX;
	float preY;
	m_particleSystem->GetPosition(&preX,&preY);
	if (preX>=m_x2-0.001)
	{
		bFire = true;
		m_particleSystem->Stop();
	}
	
}

void CGLFlorid::setRectXY(const float rect_x,const float rect_y)
{
	if (m_particleSystem)
	{
		m_particleSystem->m_info.pointCreateRect.origin.x = rect_x;
		m_particleSystem->m_info.pointCreateRect.origin.y = rect_y;
	}
}

void CGLFlorid::setRect(const float rect_x,const float rect_y,const int rect_width,const int rect_height)
{
	if (m_particleSystem)
	{
		m_particleSystem->m_info.pointCreateRect.origin.x = rect_x;
		m_particleSystem->m_info.pointCreateRect.origin.y = rect_y;
		m_particleSystem->m_info.pointCreateRect.size.width = rect_width;
		m_particleSystem->m_info.pointCreateRect.size.height = rect_height;
	}
}

void CGLFlorid::setColor(const hgeColorRGB start_color,const hgeColorRGB end_color)
{
	if (m_particleSystem)
	{
		m_particleSystem->m_info.colColorStart = start_color;
		m_particleSystem->m_info.colColorEnd = end_color;
	}
}

void CGLFlorid::setSpeed(const float minSpeed,const float maxSpeed)
{
	if (m_particleSystem)
	{
		m_particleSystem->m_info.fSpeedMin = minSpeed;
		m_particleSystem->m_info.fSpeedMax = maxSpeed;
	}
}

void CGLFlorid::setSize(const float start_size,const float end_size)
{
	if (m_particleSystem)
	{
		if (start_size >= 0)
		{
			m_particleSystem->m_info.fSizeStart = start_size;
		}
		if (end_size >= 0)
		{
			m_particleSystem->m_info.fSizeEnd = end_size;
		}
	}
}

void CGLFlorid::setSpread(const float fspread)
{
	if (m_particleSystem)
	{
		m_particleSystem->m_info.fSpread  = fspread;
	}
}

void CGLFlorid::setEmission(const int nNumber)
{
	if (m_particleSystem)
	{
		m_particleSystem->m_info.nEmission  = nNumber;
	}
}

bool	CGLFlorid::setMaskPng(const char* file)
{
	CxImage ximg;
	bool b = ximg.Load(file,CXIMAGE_FORMAT_PNG);
	if(b && m_particleSystem)
	{
		ximg.Flip(false,true);
		m_particleSystem->setMaskAlpha(ximg.AlphaGetPointer(),ximg.GetWidth(),ximg.GetHeight());
		return true;
	}
	return false;
}


void CGLFlorid::setCircleStart(int nCount/* =0 */)
{
	if (!m_particleSystem)
	{
		return;
	}
	m_nCount = nCount;
	float startX = -m_fCircle_a+2*m_fCircle_a*m_nCount/25.0f;
	float startY = (float) sqrt(m_fCircle_b*m_fCircle_b - startX*startX*m_fCircle_b*m_fCircle_b/m_fCircle_a/m_fCircle_a);
	float preX;
	float preY;
	m_particleSystem->GetPosition(&preX,&preY);
	m_particleSystem->m_info.fOriginX = preX;//设置原点坐标
	m_particleSystem->m_info.fOriginY = preY;
	m_destX = startX;
	m_destY = startY;
	m_particleSystem->SetPosition(startX+m_particleSystem->m_info.fOriginX,startY+m_particleSystem->m_info.fOriginY);
	m_particleSystem->Fire();
}

void CGLFlorid::setCircleArgument(bool bclockwise,int speed,float a,float b,int nCount)
{
	setClockwise(bclockwise);
	setCircleSpeed(speed);
	setCircleAB(a,b);
	setCircleStart(nCount);
	
	m_bCircleInit = true;
}

bool CGLFlorid::getRunning()
{
	return (-2.0f != m_particleSystem->GetAge());
}
void CGLFlorid::startPS()
{
	if (!m_particleSystem)
		return;
	m_particleSystem->Fire();
	m_lastTime=0;
}
void CGLFlorid::startPS(float fliveTime)
{
	startPS();
	setKeepTime(fliveTime);
}

void CGLFlorid::stopPS(bool killAll)
{
	if (!m_particleSystem)
		return;
	if(m_particleSystem->isStop())
		return;
	m_particleSystem->Stop(killAll);
	m_lastTime=0;
}
