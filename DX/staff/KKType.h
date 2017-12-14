/*
	注释时间:2014-4-25
	author: glp
	曲谱相关数据结构定义
*/
#ifndef __KKTYPE_H__
#define __KKTYPE_H__

#include "../Opengl.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
	const float TIP_IMAGE_CX = 0;//39;
/*	const float TIP_IMAGE_Y1 = 0;*/
/*	const float TIP_IMAGE_Y2 = 42;*/

	const float MOVE_IMAGE_SPACE = 100;
	const float MOVE_IMAGE_Y1 = 3;
	const float MOVE_IMAGE_Y2 = 36;

	//glp
//	const float STAFF_HEIGHT = 400;//曲谱高度
//	const float SING_START_B = 200/1280.0f;//歌词演唱框

	//-----------------------------------------------------------------------------
	typedef struct _GColor
	{
		ulong c;

		_GColor()
		{
			c=0xFFFFFFFF;
		}
		_GColor(const ulong uargb)
		{
			this->c = uargb;
		}
		_GColor& operator=(const _GColor a)
		{
			if (&a != this)
			{
				this->c = a.c;
			}
			return *this;
		}
		_GColor(uchar r,uchar g,uchar b,uchar a)
		{
			this->c = (((int)a<<24)|((int)r<<16)|((int)g<<8)|b);
		}
		_GColor(float r,float g,float b,float a)
		{
			this->c = (((int(a*255.0f))&0xff)<<24) | (((int(r*255.0f))&0xff)<<16) | (((int(g*255.0f))&0xff)<<8) | ((int(b*255.0f))&0xff);
		}
		_GColor(float a,unsigned int rgb)
		{
			this->c = (((int(a*255))&0xff)<<24)|(rgb&0x00ffffff);
		}
	}GColor;

	//-----------------------------------------------------------------------------
	typedef struct _GColorRect
	{
		GColor left_top_color;
		GColor left_bottom_color;
		GColor right_top_color;
		GColor right_bottom_color;

		_GColorRect( ulong ltc, ulong lbc, ulong rtc, ulong rbc )
		{
			left_top_color.c = ltc;

			left_bottom_color.c = lbc;

			right_top_color.c = rtc;

			right_bottom_color.c = rbc;
		}

		_GColorRect( const GColor & c )
		{
			left_top_color = c;
			left_bottom_color = c;
			right_top_color = c;
			right_bottom_color = c;
		}
	}GColorRect;

	static GColorRect WHITE_RECT( 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF );

	//-----------------------------------------------------------------------------
	// 变换类型
	enum _eTansType
	{
		TRANS_NONE  = 0x00,
		TRANS_ALPHA = 0x01,
		TRANS_SCALE = 0x02,
		TRANS_ANGLE = 0x04
	};

	enum _eRank
	{
		RANK_NONE = 0,                              // 无
		RANK_RED,										 // 红阶
		RANK_YELLOW,                               // 黄阶
		RANK_GREEN                                  // 绿阶
	};

	enum _eQuadSplitMode
	{
		TopLeftToBottomRight = 0,
		BottomLeftToTopRight = 1
	};

	// 句评分等级
	enum GLSentenceLevel
	{
		GLSENTENCEGRADELEVEL_NONE = 0,
		GLSENTENCEGRADELEVEL_GOOD,
		GLSENTENCEGRADELEVEL_COOL,
		GLSENTENCEGRADELEVEL_GREAT,
		GLSENTENCEGRADELEVEL_PERFECT
	};

	// 实时评分信息
	typedef struct _GLRealtimeGrade
	{
		unsigned int		cur_tm;						// 当前时间
		float						cur_pitch;					// 当前演唱者唱的音高
		unsigned int		sentence_index;				// 句序号（第一句序号为0）
		float						realtime_score;				// 当前句的实时得分
		float						accumulate_score;			// 当前句的累加得分
		double					cur_db;
	}GLRealtimeGrade;

	// 句评分信息
	typedef struct _GLSentenceGrade
	{
		bool							sentence_switch;
		int							sentence_index;				// 句序号（第一句序号为0）
		float							sentence_pitch_score;				// 句音高得分
		float							sentence_lyric_score;		//句歌词得分
		GLSentenceLevel	sentence_level;				// 句评分等级
		float							combo_value;				// combo值
	}GLSentenceGrade;
	//-----------------------------------------------------------------------------
	// 歌词信息
	struct _tGuiLyricInfo
	{
		std::wstring	lyric;
		int				width;
		int				height;
		float				pos;
		float				y;//歌词位置高度
		float				z;
		float				npass;//当前唱过歌词的百分比 0-1,起始为0
		_tGuiLyricInfo& operator=(const _tGuiLyricInfo& a)
		{
			if (this != &a)
			{
				this->lyric = a.lyric;
				this->width = a.width;
				this->pos = a.pos;
				this->y = a.y;
				this->npass = a.npass;
			}
			return *this;
		}
	};

	struct _tGuiParagraphInfo//段落信息
	{
		int             sentence_index;
		int             duration;
	};
	//-----------------------------------------------------------------------------
	// 谱线位置信息
	struct _tGuiWaveInfo
	{
		float l, r, y;		// 左 右 y轴全局像素位置
		unsigned int end_tm;
	};

	//-----------------------------------------------------------------------------
	//句评分返回的点
	typedef struct _RealTimePoint
	{
		float  pos;//返回这个点的对应时间,计算出的位置
		float   y;//最后算出的y位置
	}RealTimePoint;
	typedef struct _RealTimeDrawPoint//
	{
		float x;
		float y;
	}RealTimeDrawPoint;

	// 光标信息
	struct _tGuiCursorInfo
	{
		float 	x;
		float 	y;
		bool b;											// 是否落在标准谱线Y位置的某个区间内
		_eRank r;

		_tGuiCursorInfo() : x( 0.0f ), y( 0.0f ), b( false ), r( RANK_NONE ) {}

		_tGuiCursorInfo( float _x, float _y, bool _b, _eRank _r )
			: x( _x ), y( _y ), b( _b ), r( _r ) {}

		_tGuiCursorInfo& operator = (_tGuiCursorInfo tmp)
		{
			if (this != &tmp)
			{
				this->x = tmp.x;
				this->y = tmp.y;
				this->b = tmp.b;
				this->r = tmp.r;
			}
			return *this;
		}
	};
#ifdef __cplusplus
}
#endif

#endif//__KKTYPE_H__