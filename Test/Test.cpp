// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../LogSystem/filelog/loghelp.h"
#include <Windows.h>

DWORD WINAPI ThreadProc(LPVOID pParam)
{
	int nId = *((int*)pParam);

	while(1)
	{
		LOG_IFS2("Thread run id",nId);
		printf("Thread run id=%d\n",nId);
		Sleep(2);
	}
}

class hgeVector2
{
public:
	float	x,y;

	hgeVector2(float _x, float _y)	{ x=_x; y=_y; }
	hgeVector2()						{ x=0; y=0; }

	hgeVector2& operator=(const hgeVector2& v){this->x = v.x;this->y = v.y;return *this;}
	hgeVector2	operator-  () const { return hgeVector2(-x, -y);		}
	hgeVector2	operator-  (const hgeVector2 &v) const { return hgeVector2(x-v.x, y-v.y); }
	hgeVector2	operator+  (const hgeVector2 &v) const { return hgeVector2(x+v.x, y+v.y); }
	hgeVector2&	operator-= (const hgeVector2 &v)		  { x-=v.x; y-=v.y; return *this;	}
	hgeVector2&	operator+= (const hgeVector2 &v)		  { x+=v.x; y+=v.y; return *this;	}
	bool		operator== (const hgeVector2 &v)	const { return (x==v.x && y==v.y);		}
	bool		operator!= (const hgeVector2 &v)	const { return (x!=v.x || y!=v.y);		}

	hgeVector2	operator/  (const float scalar)	const { return hgeVector2(x/scalar, y/scalar); }
	hgeVector2	operator*  (const float scalar) const { return hgeVector2(x*scalar, y*scalar); }
	hgeVector2&	operator*= (const float scalar)		  { x*=scalar; y*=scalar; return *this;   }
};

class hgeColorRGB
{
public:
	float		r,g,b,a;

	hgeColorRGB(int _r,int _g,int _b,int _a){r=(_r&0xff)/255.0f;g=(_g&0xff)/255.0f;b=(_b&0xff)/255.0f;a=(_a&0xff)/255.0f;}
	hgeColorRGB(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }
	hgeColorRGB() { r=g=b=a=0; }

	hgeColorRGB&	operator=(const hgeColorRGB &c){r=c.r;g=c.g;b=c.b;a=c.a;return *this;}
	hgeColorRGB		operator-  (const hgeColorRGB &c) const { return hgeColorRGB(r-c.r, g-c.g, b-c.b, a-c.a); }
	hgeColorRGB		operator+  (const hgeColorRGB &c) const { return hgeColorRGB(r+c.r, g+c.g, b+c.b, a+c.a); }
	hgeColorRGB		operator*  (const hgeColorRGB &c) const { return hgeColorRGB(r*c.r, g*c.g, b*c.b, a*c.a); }
	hgeColorRGB&	operator-= (const hgeColorRGB &c)		{ r-=c.r; g-=c.g; b-=c.b; a-=c.a; return *this;   }
	hgeColorRGB&	operator+= (const hgeColorRGB &c)		{ r+=c.r; g+=c.g; b+=c.b; a+=c.a; return *this;   }
	bool			operator== (const hgeColorRGB &c) const { return (r==c.r && g==c.g && b==c.b && a==c.a);  }
	bool			operator!= (const hgeColorRGB &c) const { return (r!=c.r || g!=c.g || b!=c.b || a!=c.a);  }

	hgeColorRGB		operator/  (const float scalar) const { return hgeColorRGB(r/scalar, g/scalar, b/scalar, a/scalar); }
	hgeColorRGB		operator*  (const float scalar) const { return hgeColorRGB(r*scalar, g*scalar, b*scalar, a*scalar); }
	hgeColorRGB&	operator*= (const float scalar)		  { r*=scalar; g*=scalar; b*=scalar; a*=scalar; return *this;   }
};

typedef hgeColorRGB hgeColor;

typedef struct _CGPoint
{
	float x;
	float y;
	float z;
	_CGPoint& operator+=(const _CGPoint& a)
	{
		this->x += a.x;
		this->y += a.y;
		this->z += a.z;
		return *this;
	}
	_CGPoint& operator=(const _CGPoint& a)
	{
		this->x = a.x;
		this->y = a.y;
		this->z = a.z;
		return *this;
	}
	bool operator==(const _CGPoint& a)
	{
		if ((this->x-a.x<0.01)&&(this->y-a.y)<0.01&&(this->z-a.z)<0.01)
		{
			return true;
		}
		return false;
	}
	_CGPoint(const float a=0,const float b=0,const float c=0)
	{
		this->x = a;
		this->y = b;
		this->z = c;
	}
	_CGPoint(const _CGPoint& point)
	{
		this->x = point.x;
		this->y = point.y;
		this->z = point.z;
	}
}CGPoint;

typedef struct _CGSize 
{
	int width;
	int height;
	_CGSize& operator+=(const _CGSize& a)
	{
		this->width += a.width;
		this->height += a.height;
		return *this;
	}
	_CGSize& operator=(const _CGSize& a)
	{
		if ( this == &a )
			return *this;
		this->width = a.width;
		this->height = a.height;
		return *this;
	}
	bool operator==(const _CGSize& a)
	{
		if (this->width==a.width&&this->height==a.height)
		{
			return true;
		}
		return false;
	}
	_CGSize(const int w=0,const int h=0)
	{
		this->width = w;
		this->height = h;
	}
	_CGSize(const _CGSize& size)
	{
		this->width = size.width;
		this->height = size.height;
	}
}CGSize;
typedef struct _CGRect
{
	CGPoint origin;
	CGSize size;
	_CGRect& operator=(const _CGRect& a)
	{
		this->origin = a.origin;
		this->size = a.size;
		return *this;
	}
	bool operator==(const _CGRect& a)
	{
		if (this->origin==a.origin&&this->size==a.size)
		{
			return true;
		}
		return false;
	}
	_CGRect(const CGPoint& a=CGPoint(0.0f,0.0f),const CGSize& b=CGSize(0,0))
	{
		this->origin = a;
		this->size = b;
	}
	_CGRect(const float x,const float y,const int w,const int h)
	{
		this->origin = CGPoint(x,y);
		this->size = CGSize(w,h);
	}
	_CGRect(const _CGRect& rect)
	{
		this->origin = rect.origin;
		this->size = rect.size;
	}
}CGRect;

struct structParticleSystemInfo
{
	unsigned int		sprite;    // texture + blend mode
	bool					bOneShot;
	bool					bSpread;
	//////////////////////////////////////////////////////////////////////////glp
	int					nEmitter;
	CGRect			pointCreateRect;//粒子生成框
	float					circleInsideA;
	float					circleOutsideA;
	float					circleInsideB;
	float					circleOutsideB;

	float					fOriginX;//原点坐标
	float					fOriginY;
	//////////////////////////////////////////////////////////////////////////
	int		nEmission; // particles per sec
	float		fLifetime;

	float		fParticleLifeMin;
	float		fParticleLifeMax;

	float		fDirection;
	float		fSpread;
	bool		bRelative;

	bool		btwoEnds;
	float		fSpeedMin;
	float		fSpeedMax;

	float		fGravityMin;
	float		fGravityMax;

	float		fRadialAccelMin;
	float		fRadialAccelMax;

	float		fTangentialAccelMin;
	float		fTangentialAccelMax;

	float		fSizeStart;
	float		fSizeEnd;
	float		fSizeVar;

	float		fSpinStart;
	float		fSpinEnd;
	float		fSpinVar;

	hgeColor	 colColorStart; // + alpha
	hgeColor	 colColorEnd;
	float			fColorVar;
	float			fAlphaVar;
};

struct structParticleSystemInfo2
{
	unsigned int		sprite;    // texture + blend mode
	bool					bOneShot;
	bool					bSpread;
	//////////////////////////////////////////////////////////////////////////glp
	int					nEmitter;
	CGRect			pointCreateRect;//粒子生成框
	float					circleInsideA;
	float					circleOutsideA;
	float					circleInsideB;
	float					circleOutsideB;

	float					fOriginX;//原点坐标
	float					fOriginY;
	//////////////////////////////////////////////////////////////////////////
	int		nEmission; // particles per sec
	float		fLifetime;

	float		fParticleLifeMin;
	float		fParticleLifeMax;

	float		fDirection;
	float		fSpread;
	bool		bRelative;

	bool		btwoEnds;
	float		fSpeedMin;
	float		fSpeedMax;
	//替换重力值
	hgeVector2 fForceMin;
	hgeVector2 fForceMax;

	float		fRadialAccelMin;
	float		fRadialAccelMax;

	float		fTangentialAccelMin;
	float		fTangentialAccelMax;

	float		fSizeStart;
	float		fSizeEnd;
	float		fSizeVar;

	float		fSpinStart;
	float		fSpinEnd;
	float		fSpinVar;

	hgeColor	colColorStart; // + alpha
	hgeColor	colColorEnd;
	float			fColorVar;
	float			fAlphaVar;
};

int _tmain(int argc, _TCHAR* argv[])
{

	for(int i = 1;i<40;i++)
	{
		char buf1[260] = {0};
		char buf2[260] = {0};
		sprintf(buf1,"psi/particle%d.psi",i);
		sprintf(buf2,"particle%d.psi",i);
		FILE* p1 = fopen(buf1,"rb");
		FILE* p2 = fopen(buf2,"wb");
		if(p1 && p2)
		{
			structParticleSystemInfo oldI;
			structParticleSystemInfo2 newI;
			fread(&oldI,1,sizeof(oldI),p1);
			newI.bOneShot = oldI.bOneShot;
			newI.bRelative = oldI.bRelative;
			newI.bSpread = oldI.bSpread;
			newI.btwoEnds = oldI.btwoEnds;
			newI.circleInsideA = oldI.circleInsideA;
			newI.circleInsideB = oldI.circleInsideB;
			newI.circleOutsideA = oldI.circleOutsideA;
			newI.circleOutsideB = oldI.circleOutsideB;
			newI.colColorEnd = oldI.colColorEnd;
			newI.colColorStart = oldI.colColorStart;
			newI.fAlphaVar = oldI.fAlphaVar;
			newI.fColorVar = oldI.fColorVar;
			newI.fDirection = oldI.fDirection;
			newI.fForceMax = hgeVector2(0,oldI.fGravityMax);
			newI.fForceMin = hgeVector2(0,oldI.fGravityMin);
			newI.fLifetime = oldI.fLifetime;
			newI.fOriginX = oldI.fOriginX;
			newI.fOriginY = oldI.fOriginY;
			newI.fParticleLifeMax = oldI.fParticleLifeMax;
			newI.fParticleLifeMin = oldI.fParticleLifeMin;
			newI.fRadialAccelMax = oldI.fRadialAccelMax;
			newI.fRadialAccelMin = oldI.fRadialAccelMin;
			newI.fSizeEnd = oldI.fSizeEnd;
			newI.fSizeStart = oldI.fSizeStart;
			newI.fSizeVar = oldI.fSizeVar;
			newI.fSpeedMax = oldI.fSpeedMax;
			newI.fSpeedMin = oldI.fSpeedMin;
			newI.fSpinEnd = oldI.fSpinEnd;
			newI.fSpinStart = oldI.fSpinStart;
			newI.fSpinVar = oldI.fSpinVar;
			newI.fSpread = oldI.fSpread;
			newI.fTangentialAccelMax = oldI.fTangentialAccelMax;
			newI.fTangentialAccelMin = oldI.fTangentialAccelMin;
			newI.nEmission = oldI.nEmission;
			newI.nEmitter = oldI.nEmitter;
			newI.pointCreateRect = oldI.pointCreateRect;
			newI.sprite = oldI.sprite;

			fwrite(&newI,sizeof(newI),1,p2);
			fclose(p2);
			fclose(p1);
		}

	}
// 	LogHelp::InitLogHelp("test.log");
// 
// 	int nId = 1;
// 	DWORD hID,hID2;
// 	HANDLE hT = CreateThread(NULL,0,ThreadProc,&nId,0,&hID);
// 	CloseHandle(hT);
// 	int nId2 = 2;
// 	HANDLE hT2 = CreateThread(NULL,0,ThreadProc,&nId2,0,&hID2);
// 	CloseHandle(hT2);
// 	while(1)
// 	{
// 		LOG_IFN0("main thread");
// 		printf("main thread\n");
// 		Sleep(9);
// 	}

	return 0;
}

