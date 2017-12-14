/*
	注释时间:2014-4-25
	author: glp
	粒子系统的属性
*/
#ifndef PARTICLEINFO__H__
#define PARTICLEINFO__H__

#include "hgesprite.h"
#include "hgevector.h"
#include "hgecolor.h"
#include "../GLDef.h"

#define MAX_PARTICLES	700
#define MAX_PSYSTEMS	50
//运行时保存的粒子信息
struct structParticle
{
	//位置
	hgeVector2		vecLocation;

	//运动模式------------------------begin
	bool bRadiusMode;
	//力模式
	//速度
	hgeVector2		vecVelocity;
	//力量
	hgeVector2		fForce;
	float		fRadialAccel;
	float		fTangentialAccel;
	//半径模式
	float fAngle;//单位弧度
	float fRadius;
	float fRadiusDelta;//每秒变化弧度
	float fRotatePerSecond;
	//运动模式------------------------end

	//自身旋转控制
	float		fSpin;
	float		fSpinDelta;
	//大小控制
	float		fSize;
	float		fSizeDelta;

	//颜色控制
	bool bColorOscillate;//true 颜色震荡模式，false 颜色渐变模式
	bool bAddr;//用来区分是加还是减
	bool bAddg;
	bool bAddb;
	bool bAdda;
	hgeColor	colColor;		// + alpha
	hgeColor	colColorDelta;
	hgeColor colColorMin;
	hgeColor colColorMax;

	//寿命
	float		fAge;//当前寿命
	float		fTerminalAge;//结束寿命

	float fAlpha;//used for mask，most mode is 1.0f
	float z;
};
//文件中需要保存的粒子信息 
struct structParticleSystemInfo
{
	CGLSprite*		sprite;    // blend mode //前面16位以后可能更多
	//是否一次
	bool					bOneShot;
	//是否扩散
	bool					bSpread;
	//////////////////////////////////////////////////////////////////////////glp
	//发射器类型
	int					nEmitter;//plist modeA默认使用矩形框 modeB使用点，并且不扩散
	//粒子生成框
	CGRect			pointCreateRect;//(_sourcePosition.x- _posVar.x * CCRANDOM_MINUS1_1(),_sourcePosition.y- _posVar.y * CCRANDOM_MINUS1_1())
	float					circleInsideA;
	float					circleOutsideA;
	float					circleInsideB;
	float					circleOutsideB;

	//原点坐标
	float					fOriginX;//plist 默认中心位置（640,320）可能根据坐标系需要转换一下。
	float					fOriginY;//0
	//////////////////////////////////////////////////////////////////////////
	//一秒发射多少粒子
	int		nEmission;
	//粒子系统的寿命
	float		fLifetime;
	//粒子系统的寿命最小 最大值
	float		fParticleLifeMin;//plist中 MAX（0，_life-|_lifeVar|） ~ _life+|_lifeVar| 最小为0
	float		fParticleLifeMax;//
	//方向
	float		fDirection;//弧度
	float		fSpread;//弧度 == fDirectionVar
	//是否相对
	bool		bRelative;
	//是否是分为两边
	bool		btwoEnds;

	//**************************************力模式 BEGIN********************
	//速度最小值 最大值
	//plist文件是speed-|speedVar| ~ speed+|speedVar|
	float		fSpeedMin;
	float		fSpeedMax;
	//替换重力值 方向力量  /1.0f
	//plist文件是 
	hgeVector2 fForceMin;//
	hgeVector2 fForceMax;
	//径向，切向加速度 /1.0f
	//plist文件是radialAccel-|radialAccelVar| ~ radialAccel+|radialAccelVar|
	//tangentialAccel-|tangentialAccelVar| ~ tangentialAccel+|tangentialAccelVar|
	float		fRadialAccelMin;
	float		fRadialAccelMax;
	float		fTangentialAccelMin;
	float		fTangentialAccelMax;
	//**************************************力模式 END********************

	//大小的控制  /1.0f
	float		fSizeStart;//_startSize-|_startSizeVar| ~ _startSize+|_startSizeVar| 确保大于0
	float		fSizeEnd;//-1的话就是跟fSizeStart一致 _endSize-|_endSizeVar| ~ _endSize+|_endSizeVar| 确保大于0
	float		fSizeVarStart;//确保>=0  
	float		fSizeVarEnd;//确保>=0
	//自身旋转 /300.0f
	float		fSpinStart;//_startSpin-|_startSpinVar| ~ _startSpin+|_startSpinVar|
	float		fSpinEnd;//_endSpin-|_endSpinVar| ~ _endSpin+|_endSpinVar|
	float		fSpinVarStart;//确保>=0 
	float		fSpinVarEnd;//确保>=0
	//颜色变化
	hgeColor	colColorStart; // + alpha clampf(_startColor.r + _startColorVar.r * CCRANDOM_MINUS1_1(), 0, 1);
	hgeColor	colColorEnd;
	float			fColorVar;//确保>=0
	//透明度可变量
	float			fAlphaVar;//确保>=0

	//新增信息
	char textureFile[260];
	char maskFile[260];
	bool bColorOscillate;

	hgeColor colColorMin;
	hgeColor colColorMax;
	hgeColor colColorStep;
	float fStepColorVar;//确保>=0
	float fStepAlphaVar;//确保>=0

	bool bRadiusMode;//是否是半径模式
	//**************************************半径模式 BEGIN********************
	float fRadiusStart;
	float fRadiusStartVar;//确保>=0
	float fRadiusEnd;
	float fRadiusEndVar;//确保>=0
	float fRotatePerSecond;
	float fRotatePerSecondVar;//确保>=0
	//**************************************半径模式 END********************
};

#endif//PARTICLEINFO__H__