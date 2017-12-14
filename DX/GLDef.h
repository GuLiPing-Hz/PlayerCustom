/*
	注释时间:2014-4-25
	author: glp
*/
#ifndef GLDEF__H__
#define GLDEF__H__

#include <objbase.h>
#include "progress/processcommunicate.h"
#include <string>
//glp

static char LOGPATH[260] = {0}; //日志path

#define WSTR_MONEYEXCHANGE_NOTIFY L"请您到点歌界面兑换礼物!"

enum eVideoType
{
	vtHost,
	vtSing,
};

enum eShowWave
{
	SHOW_NONE,//不评分
	SHOW_ONE,//单人模式
	SHOW_TWO,//PK模式
	SHOW_THREE,//对唱模式
};

enum eNotify
{
	NOTIFY_NONE=-1,
	//NOTIFY_MESSAGE
	NOTIFY_MUTE_OPEN,//Notify0
	NOTIFY_MUTE_CLOSE,//Notify1
	NOTIFY_PLAY_STOPPING,//Notify2
	NOTIFY_PLAY_RUNNING,//Notify3
	NOTIFY_ORIGIN_OPEN,//Notify4
	NOTIFY_ORIGIN_CLOSE,//Notify5
	//评分模式
	NOTIFY_CLOSE_GRADE,//Notify6
	NOTIFY_SINGLE_GRADE,//Notify7
	NOTIFY_DOUBLE_GRADE,//Notify8
	NOTIFY_THREE_GRADE,//Notify9

	//音效
	NOTIFY_MAGICAL_FAST,//Notify 10
	NOTIFY_MAGICAL_TOP,//Notify 11
	NOTIFY_MAGICAL_SINGER,//Notify 12
	NOTIFY_MAGICAL_KTV,//Notify 13
	NOTIFY_PROFESSINAL_FAST,//Notify 14
	NOTIFY_PROFESSINAL_TOP,//Notify 15
	NOTIFY_PROFESSINAL_SINGER,//Notify 16
	NOTIFY_PROFESSINAL_KTV,//Notify 17

	NOTIFY_APPLAUD,//png序列 applaud
	NOTIFY_CHEER,//png序列 cheer

	NOTIFY_MICVOLUME_SET,//组合图
	NOTIFY_MUSICVOLUME_SET,//组合图
	NOTIFY_TONE_SET,//组合图
	//HEAD_MESSAGE
	MESSAGE_NOTIFY,
	MESSAGE_NEXTSONG,
};

typedef struct _SingInfo{
	wchar_t mdmFile[260];
	wchar_t songName[260];
	wchar_t singerName[260];
	wchar_t singerSingle[260];//如果为对唱歌曲，则这里保存某一位歌手名
	wchar_t nextSongName[260];
}SingInfo;

typedef struct _StructSound{
	int cur_mic;
	int min_mic;
	int max_mic;
	int cur_music;
	int min_music;
	int max_music;
	int cur_tone;
	int min_tone;
	int max_tone;
}StructSound;

typedef struct _WaveConfig{
	float			SING_START_B;//歌词演唱框
	int 			WAVEWND_WIDTH_TIMEMS;// = 10000;//2222;//5000;		// 一屏5000毫秒
	int 			WAVEWND_WIDTH_PIXELS;// = 1280;//320;//720;		// 一屏宽度720像素
	int 			WAVEWND_HEIGHT_PIXELS;// = 87;//79;		// 曲谱的高度40像素
	int 			WAVEWND_HEIGHT_PIXELS_KTV;// = 87;//105;	// 曲谱的高度40像素
	int 			WAVEWND_LEFT_PIXELS;// = 200;//170;		// 左边的像素
	//int CURSOR_WIDTH_PIXELS;// = 36;			// 光标宽度
	int			WAVEWND_LEFT_TIMEMS;// = 1210;		// WAVEWND_WIDTH_TIMEMS * WAVEWND_LEFT_PIXELS / WAVEWND_WIDTH_PIXELS + 30;

	//int WAVEWND_WIDTH_PIXELS_KTV;// = 320;//460;	// 一屏宽度632像素(KTV模式)
	float			WAVEWND_WIDTH_PIXELS_KTV_TM_CUR;//指定谱线 像素/时间（毫秒）
	float			WAVEWND_WIDTH_PIXELS_KTV_TM_NEXT;

	int			SHOW_TIP_IMAGE_MSEC;// = 4000;		// 4000毫秒~2000毫秒	显示提示图标（图标闪烁）
	int			SHOW_MOVE_IMAGE_MSEC;// = 2000;		// 2000毫秒~0毫秒		显示光标滑动动画
	int			NEED_SHOW_MOVE_IMAGE_MSEC;// = 500;	// >= 1000毫秒		需要显示光标滑动动画

	// 估算16号字体的中文英文字符宽度
	int			CHAR_WIDTH_CHS;// = 21;				// 中文字体宽度高度
	int			CHAR_WIDTH_EN;// = 12;				// 英文字体宽度高度

	int			MAX_LYRIC_WIDTH;//  700// 歌词最大宽度
	float			VERTICAL_LINE_HEIGHT;//竖线位置的高度
	int			CONFIG_SPEED_PARTICLE;
	int			CONFIG_BARRAGEF_SIZE;//弹幕字体大小
}WaveConfig;

typedef struct _PlayInfo{
	float			POS_WAVE_HEIGHTSINGLE;//数据库中取，曲谱高度
	float			POS_WAVE_HEIGHTDOUBLE;
	//float height_second;//曲谱2高度
	eShowWave eType;//播放模式
	int			start_ms;//支持简版歌曲的开始毫秒数
	int			end_ms;//支持简版歌曲的结束毫秒数
	bool			ktv_mode;//是否为KTV 模式
}PlayInfo;

#define SECONDWAVEHEIGHT 280
//下面数据精心调配，在当前的透视投影下可以时屏幕坐标从LEFTUPPER（0,0）到RIGHTBOTTOM（1280,720）
//透视投影使用值 支持最多三位小数，多了就没用了
#define GL3D_Z_VIDEO 10.0f
//10.0f
//base z
//【唱歌SceneObject】:包含评分渲染框架，含一切东西,打算把它整合到一起做成一个SceneObject，不再单独提供某个render了
//【评分结算SceneObject】:从原来的object中的render分离出来。
#define GL3D_Z_BASE 9.999f
//唱歌歌词->【唱歌SceneObject】
#define GL3D_Z_LYRIC 9.998f
//来自星光的视频，提供【星光视频SceneObject】
#define GL3D_Z_XGVIDEO 9.997f
//弹幕隶属于提醒SceneObject
#define GL3D_Z_BARRAGE 9.996f
//提醒，公告，气氛等，作为一个单独【提醒SceneObject】来使用管理提醒公告
#define GL3D_Z_NOTIFY 9.995f
//9.995f
#define GL3D_Z_SONGINFO 9.994f
//默认粒子系统z坐标，隶属于需要粒子系统展示的Object，也可提供【粒子系统SceneObject】加载一个单独的粒子系统
#define GL3D_Z_PARTICLE 9.993f
//需要展示在粒子系统之前的物件
#define GL3D_Z_BEFOREPARTICLE 9.992f
//火警，黑幕，如果粒子系统不使用默认值，则需要停止生成新的粒子，并Stop.提供【火警SceneObject】
#define GL3D_Z_FIRALARM 9.5f
//其他z值也可酌情考虑使用，
#define CAN_BEFORE_FIRALARM(z) \
{ \
	(-0.0000001f<((z)-9.0f)&&((z)-9.0f)<0.0000001f) ? true:false; \
}


#define FLASHWND_WIDTH 623
#define FLASHWND_HEIGHT 365

typedef struct _FBControl{
	float alpha;
	bool control;
}FBControl;

typedef struct _SongListInfo{
	std::string songname;
	std::string singername;
	std::string filename;
	int			  endsecond;
}SongListInfo;

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
	_CGPoint(const float a=0,const float b=0,const float c=GL3D_Z_BASE)
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
}GPoint;

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
}GSize;

static GSize CGSIZEZERO(0,0);

typedef struct _CGRect
{
	GPoint origin;
	GSize size;
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
	_CGRect(const GPoint& a=GPoint(0.0f,0.0f),const GSize& b=GSize(0,0))
	{
		this->origin = a;
		this->size = b;
	}
	_CGRect(const float x,const float y,const int w,const int h)
	{
		this->origin = GPoint(x,y);
		this->size = GSize(w,h);
	}
	_CGRect(const _CGRect& rect)
	{
		this->origin = rect.origin;
		this->size = rect.size;
	}
}CGRect;

typedef struct _LINEVERTEX
{
	GPoint point;
	unsigned long colour;
}LINEVERTEX;


typedef struct _CUSTOMVERTEX
{
	GPoint point;
	unsigned long colour;
	float u;
	float v;
}CUSTOMVERTEX;

static CUSTOMVERTEX s_initVertex = { GPoint(0.0f,0.0f,GL3D_Z_BASE),
																0xFFFFFFFF,
																0.0f,0.0f};

typedef struct _hgeInputEvent 
{
	int		type;			// event type
	int		key;			// key code
	int		flags;			// event flags
	int		chr;			// character code
	int		wheel;			// wheel shift
	float		x;				// mouse cursor x-coordinate
	float		y;				// mouse cursor y-coordinate
}hgeInputEvent;

typedef struct _CInputEventList
{
	hgeInputEvent		event;
	_CInputEventList*	next;
}CInputEventList;

typedef struct _BarrageInfo
{
	int type;//0： 普通，1：弹幕
	unsigned int mode;//对弹幕有效，字幕模式: 0:普通弹幕;1:顶上弹幕;2:底下弹幕
	/*
	针对产品需求制定的字符串格式
	<txt></txt>标签中： 属性space指定文字间距，属性size指定文字大小,属性color指定文字颜色，属性txt指定需要展示的文字内容
	<space></space>标签中：属性width指定空格像素大小
	<img></img>标签中：属性src指定单个的纹理图片，属性space指定图片前后的间距，前间距space/2
	*/
	char str[1024];//包含'\0'   不要超过CONFIG_MAX_LEN个字符 如果是弹幕的话则是说话者内容
	/*
	@saytype 指定显示的开始对象，
		0：代表显示用户名字+id的方式
		1：图片-天天抢榜 当指定为这个的时候sayname指定图片名称 DailyRankingPic
	*/
	int saytype;
	int sayid;//对弹幕有效 说话者ID
	wchar_t sayname[100];//对弹幕有效 说话者名字
	wchar_t location[260];//说话者位置
}BarrageInfo;

typedef struct _SpecialInfo
{
	/*
	1 : 通用的消息提醒
	2 : 门店三甲
	*/
	int type;
	/*
	<line indentation yoffset></line> 指定一行文字 intendation是缩进 yoffset指定y偏移
	*/
	char str[1024];//注释同上BarrageInfo
}SpecialInfo;

typedef unsigned char uchar;
typedef float CGFloat;
typedef unsigned int  uint;
typedef unsigned long ulong;

static CGRect CGRECTZERO(0.0f,0.0f,0,0);

typedef struct _reVertex
{
	CUSTOMVERTEX* v;
	unsigned int count;
	void* tex;
	int blend;
}reVertex;
/*
** Triple structure
*/
typedef struct _reTriangle
{
	CUSTOMVERTEX		v[3];
	void*			tex;
	int				blend;
}reTriangle;
/*
** Quad structure
*/
typedef struct _reQuad
{
	CUSTOMVERTEX		v[4];
	void*		tex;
	int			blend;
}reQuad;

typedef enum _eDrawType
{
	REPRIM_LINESTRIP,
	REPRIM_TRIANGLESTRIP,
	REPRIM_LINES=2,
	REPRIM_TRIANGLES,
	REPRIM_QUADS,
}eDrawType;
/*
** Common math constants
*/
#ifndef M_PI
#define M_PI	3.14159265358979323846f
#endif//M_PI
#define M_PI_2	1.57079632679489661923f
#define M_PI_4	0.785398163397448309616f
#define M_1_PI	0.318309886183790671538f
#define M_2_PI	0.636619772367581343076f
#define M_PI_180 0.01745329252f
#define M_180_PI 57.29577951f


#define	BLEND_COLORMUL		0
#define	BLEND_COLORADD		1
#define	BLEND_COLORSELECT2 2
#define	BLEND_COLORMASK		0xF

#define	BLEND_ALPHAADD		0x00
#define	BLEND_ALPHABLEND	0x10
#define	BLEND_ALPHAMASK		0x10

#define   BLEND_NOZWRITE 0x00
#define   BLEND_ZWRITE		0x20
#define   BLEND_ZMASK		0x20

#define	BLEND_DEFAULT		(BLEND_ALPHABLEND|BLEND_COLORMUL)


#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_LINEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

#define BATCH_BUFFER_SIZE 6000
#define BATCH_LINE_BUFFERSIZE 2000
#define WM_PLAYER_MESSAGE (WM_USER+200)
#define WM_GRAPHNOTIFY				(WM_PLAYER_MESSAGE+1)
#define WM_SWITCHSONG				(WM_PLAYER_MESSAGE+2)
#define WM_PLAY_CONTROL			(WM_PLAYER_MESSAGE+3)
#define WM_PLAY_GRADE				(WM_PLAYER_MESSAGE+4)
#define WM_PLAY_ACOMMPANY	(WM_PLAYER_MESSAGE+5)
#define WM_PLAY_END					    (WM_PLAYER_MESSAGE+6)
#define WM_DEBUG							(WM_PLAYER_MESSAGE+7)
#define WM_PLAY_SONGINFO			(WM_PLAYER_MESSAGE+8)
#define WM_PLAY_SINGERPIC			(WM_PLAYER_MESSAGE+9)
#define WM_PLAY_SONGSTARTEND	(WM_PLAYER_MESSAGE+10)
#define WM_PLAY_FINALSCORE (WM_PLAYER_MESSAGE+11)

#define WM_PLAY_VOLUME_PUT		(WM_PLAYER_MESSAGE+11)
#define WM_PLAY_VOLUME_GET		(WM_PLAYER_MESSAGE+12)
//#define MSE_PITCHTRACK_ALGTYPE_YinACF	2

#define SAFEPOINTER_CALL(p)		!(p)?(void)0:(p)->
#endif//GLDEF__H__

