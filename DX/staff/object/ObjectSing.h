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

	//���𣬳�ʼ������
	virtual bool awake(void* info);
	//���õ�ǰʱ��ֵ��
	virtual void setCurMs(unsigned int ms);
	//���²�����������
	//@param ms:��ȥ��ʱ�䣬��λ�����룩
	virtual bool update(unsigned int ms);
	//��Ⱦ����
	virtual bool render();
	//��������Դ�Ȼ��ղ���
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
	void eventAvRealtimeGrade(unsigned int elasped_ms/*����*/ ,GLRealtimeGrade  grade ,bool is_callback=true);
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
	IEval*											m_pIEval;//����Ҫ�ͷţ��ͷ���SceneMgr��
	bool												m_bAccompany;
	float												m_fpeak;//�ֱ�ֵ
	//¼���ļ���������ʱ�䣬�����������������ܷ֣�����ֵ����
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
	int								m_iCurSentence;//��ǰ�ݳ��ľ�

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
	//����
	CGLFont*				m_pGLFont;

	_tSongInfo				m_songInfo;
	_tSentenceInfo		mPreSentence_info;
	_tSentenceInfo		mCurSentence_info;
	_tSentenceInfo		mNextSentence_info;

	GLRealtimeGrade	mCallbackRealtimeGrade[16];
	int							mnCallbackRealtimeGradeCount;

	GLSentenceGrade	mCallbackSentenceGrade[8];
	int						    mnCallbackSentenceGradeCount;
	//�α��趨
	int  							cursor_start_pos;//��ǰ�䣬�α���ʼλ��
	int							cursor_start_pos_next;//��һ�䣬�α���ʼλ��
	int							cursor_end_pos;//��ǰ�䣬�α����λ��
	int							cursor_end_pos_next;//��һ�䣬�α����λ��

	unsigned int			cur_sentence_start_tm;
	unsigned int			cur_sentence_end_tm;
	unsigned int			cur_sentence_ahead_tm;

	int			m_pre_left_pitch_count;	
	// ���ߴ��ڳ�������

	float			POS_WAVE_HEIGHT;
public:
	int			m_ave_dest_score;
};


#endif//OBJECTSING__H__
