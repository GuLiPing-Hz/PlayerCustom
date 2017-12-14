/*
	注释时间:2014-4-25
	author: glp
	封装一个CGLFlorid类，用于管理粒子系统的加载，播放，属性设定等等，
	对于一个粒子系统，每一帧都需要调用它的Update函数，才能不断更新，获得想要的效果。
*/
#pragma once

#include "GLDef.h"
#include <string>
class hgeColorRGB;
class hgeParticleSystem;
class ImgsetMgr;
class Imageset;
class CGLFlorid
{
	void loadPSI(const wchar_t* appDir,int n);
public:
	CGLFlorid(const std::string szInFile,bool bFromOther=false);
	virtual ~CGLFlorid(void);

	void setImageset(Imageset* imgset,const std::string name);
	//粒子系统初始化
	void initPS(const wchar_t* appDir,int npsi,float z=GL3D_Z_PARTICLE);
	//粒子系统的回收资源
	void donePS();
	//更新粒子系统
	void updatePS(const float delta_second);
	//渲染粒子
	void renderPS();
	hgeParticleSystem* getParticleSystem() const {return m_particleSystem;}
	//移动
	//根据加载的粒子系统属性，设置原点坐标
	void setOriginPoint();
	//设置原点坐标
	void setOriginPoint(float x,float y);
	//设置粒子系统位置
	void setPosition(float x,float y);

	void moveTo(float x,float y,bool bMoveAll=true);
	//当前位置移动y 
	//@param delta_tm: 单位：秒
	void moveLineToY(float x,float& y,float delta_tm,int speed,bool bAll);
	//当前位置向目标点移动
	//@param delta_tm: 单位：秒
	void moveLineTo(float x,float y,float deltaTime,int speed);
	
	//重复从(x1,y1)移动到(x2,y2)
	void setMoveFloridLine(float x1,float y1,float x2,float y2,int speed);
	void moveFloridLine(const float deltaTime);
	
	//圆相应方法
	void setCircleAB(float a,float b = 0){if(b==0)b=a;m_fCircle_a=a;m_fCircle_b=b;}
	void setClockwise(bool bclockwise){m_bClockwise = bclockwise;}
	void setCircleStart(int nCount=0);
	void setCircleSpeed(int speed){m_speedCircle=speed;}
	void setCircleArgument(bool bclockwise,int speed,float a,float b=0,int nCount=0);//引到所有参数
	void moveCircle(const float deltaTime);//默认起点是(-a,0),沿着椭圆移动

	//单位：秒
	void move(const float deltaTime);
	//控制
	bool getRunning();
	//0 代表永远持续
	void startPS();
	void startPS(float fliveTime);
	//彻底停止
	void stopPS(bool killAll=true);
	void setKeepTime(float t=0){m_liveTime=t;}
	//粒子属性控制
	void setRectXY(const  float rect_x,const  float rect_y);
	//生成粒子的区域
	void setRect(const  float rect_x,const  float rect_y,const  int rect_width,const  int rect_height);
	//颜色变化
	void setColor(const hgeColorRGB start_color,const hgeColorRGB end_color);
	//速度最大最小控制
	void setSpeed(const float minSpeed,const float maxSpeed);
	//大小控制在0-1之间
	void setSize(const float start_size,const float end_size);
	//扩散角度
	void setSpread(const float fspread);
	//粒子每秒发射数量
	void setEmission(const int nNumber);
	//设置alpha遮罩
	bool	setMaskPng(const char* file);
private:
	std::string					m_szIniFile;
	hgeParticleSystem*	m_particleSystem;
	std::string					m_imgName;
	Imageset*					m_imgset;//don't release

	int								m_nCount;//处理椭圆
	bool								m_bHalf;
	bool								m_bClockwise ;
	float								m_fCircle_a;//长轴
	float								m_fCircle_b;//短轴
	float								m_destX;
	float								m_destY;
// 	float								m_originX;//原点坐标
// 	float								m_originY;
	int								m_speedCircle;
	bool								m_bCircleInit;

	float								m_liveTime;//指定存在时间 单位(秒)
	float								m_lastTime;//已经持续时间 


	//Florid Line
	float								m_x1;
	float								m_y1;
	float								m_x2;
	float								m_y2;
	int								m_speedLine;
	bool								m_bFloridLineInit;
public:
	bool								m_bFromOther;
};
