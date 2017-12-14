#ifndef OBJECTSING__H__
#define OBJECTSING__H__
#include "ISceneObject.h"
#include "../Opengl.h"

#include "../fto/ftotype.h"
#include "../fto/eval.h"

class LyricRender;
class WaveRender;
class SentenceRender;
class CSingObject : public ISceneObject
{
public:
	CSingObject(bool& bInit,SingInfo* pSingInfo,PlayInfo* pPlayInfo,bool bFirst=true);
	virtual ~CSingObject();

	//唤起，初始化操作
	virtual bool awake(void* info);
	//设置当前时间值，
	virtual void setCurMs(unsigned int ms);
	//更新操作场景对象
	//@param ms:逝去的时间，单位（毫秒）
	virtual bool update(unsigned int ms);
	//渲染对象
	virtual bool render();
	//结束，资源等回收操作
	virtual void end();

	void setFont(CGLFont* pFont){assert(pFont!=NULL);m_pGLFont = pFont;}

	void setIEval(IEval* pIEval){m_pIEval = pIEval;}

	int getFinalScoreStar(int &nFinalScore,int &nFinalStarNum);
	void clearGrade();
	void realtime(ulong ns_cur_ms,eShowWave eType);
private:
	void createWaveLyric(const _tSongInfo& song_info);
	void dealRealtimeGrade( const GLRealtimeGrade &  realtime_grade);
	void dealSentenceGrade(const GLSentenceGrade &  sentence_grade);
	void eventMainUpdate(unsigned int elapsed_ms);
	float checkSentenceCurTM();
	float checkSentenceNextTM();
	void eventAvRealtimeGrade(unsigned int elasped_ms/*毫秒*/ ,GLRealtimeGrade  grade ,bool is_callback=true);
	void eventAvSentenceGrade (GLSentenceGrade & grade);
	int calculateSentence(const uint start_ms,const uint end_ms);
	std::wstring & getLyric(const _tSongInfo& song_info);
	bool sampleAwake(uint start_ms,uint end_ms);
private:
	bool									m_bFirst;
	Imageset*						m_imgset;//renderer

	LyricRender*					lyric_render;
	WaveRender*					wave_render;
	SentenceRender*			sentence_render;

	SingInfo										m_gSingInfo;
	PlayInfo										m_gPlayInfo;

	bool												m_bLoadFromDat;
	IEval*											m_pIEval;//不需要释放，释放在SceneMgr中
	bool												m_bAccompany;
	float												m_fpeak;//分贝值
	//录音文件名，根据时间，歌曲名，歌手名，总分，星星值命名
	bool												m_bAS;
	std::wstring									m_wcur_singer_name;
	std::wstring									m_cur_singer_part;
	std::wstring									m_wcur_song_name;
	std::wstring									m_next_song_name;
	int												m_star_number;
	int												m_nsentence;

	LISTREALTIMEPOINT					m_realtime_point;
	VECTLINEVERTEX						m_vect_drawpoint; 
	//game mode
	LISTGUILYRICVECT						m_GuiLyricVect;	
	LISTGUIPITCHVECT					m_GuiPitchVect;
	LISTSENTENCELINEVECT			m_GuiSentencelineVect;
	VECTPARAGRAPHVECT 			m_ParagraphVect;

	VECTGUILYRICVECT					m_tmpgui_lyric_list;
	VECTGUIPITCHVECT					m_tmpgui_pitch_list;
	_tGuiCursorInfo							m_tmpcursor;

	bool							mbInterpolation;
	bool							m_bIsCallback;
	bool							mbChangeMode;
	bool							m_bShowEndScore;

	GLRealtimeGrade		m_CurGrade;
	int								m_iCurSentence;//当前演唱的句

	float                   mfElapsedCount;
	uint                    miElapsedCount;
	uint                    miLastElapsedCount;
	uint                    mErrCorrect;
	uint                    mPreTime;
	uint                    mCurTime;
	uint                    mCurSingTime;

	float                   mfKtvErr;
	float                   mfGameErr;

	float							m_pre_cur_pitch;
	//字体
	CGLFont*				m_pGLFont;

	_tSongInfo				m_songInfo;
	_tSentenceInfo		mPreSentence_info;
	_tSentenceInfo		mCurSentence_info;
	_tSentenceInfo		mNextSentence_info;

	GLRealtimeGrade	mCallbackRealtimeGrade[16];
	int							mnCallbackRealtimeGradeCount;

	GLSentenceGrade	mCallbackSentenceGrade[8];
	int						    mnCallbackSentenceGradeCount;
	//游标设定
	int  							cursor_start_pos;//当前句，游标起始位置
	int							cursor_start_pos_next;//下一句，游标起始位置
	int							cursor_end_pos;//当前句，游标结束位置
	int							cursor_end_pos_next;//下一句，游标结束位置

	unsigned int			cur_sentence_start_tm;
	unsigned int			cur_sentence_end_tm;
	unsigned int			cur_sentence_ahead_tm;

	int			m_pre_left_pitch_count;	
	// 谱线窗口常量定义

	float			POS_WAVE_HEIGHT;
public:
	int			m_ave_dest_score;
};


#endif//OBJECTSING__H__
