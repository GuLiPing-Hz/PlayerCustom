// DXdemoTestSMDlg.h : 头文件
//

#pragma once

#include <list>
#include <processcommunicate.h>
#include <SMHelp.h>
#include <Snmp.h>
#include <WinSock2.h>
#include <Iphlpapi.h>
#include "afxwin.h"
#include <cstdio>
#include "afxcmn.h"
#include <string>
#include <map>

#define VIDEO_TIMER 1001
// CDXdemoTestSMDlg 对话框

typedef struct _SongListInfo{
	std::string songname;
	std::string singername;
	std::string filename;
	int			  endsecond;
}SongListInfo;
typedef std::map<std::wstring,SongListInfo> MAPSONGLIST;
typedef std::map<std::string,MAPSONGLIST> MAPCATEGORY;

class CDXdemoTestSMDlg : public CDialog
{
// 构造
public:
	CDXdemoTestSMDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CDXdemoTestSMDlg();
// 对话框数据
	enum { IDD = IDD_DXDEMOTESTSM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	static void DealMessage(int cmd, int w, int l,void* s);
// 实现
protected:
	MAPCATEGORY	m_mapCategory;
	int						m_TimerId;
	CSMHelp*			m_psmHelp;
	HICON m_hIcon;
	CStringW				   m_curSongName;
	std::list<CStringW> m_songlist;
	unsigned int		   m_uCur;
	unsigned int		   m_uTotal;
	int						   m_ncurValue;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void getMac(char * mac);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSwitchsong();
	void OnErrorLog(wchar_t* msg);
	void OnRankingNum(int wParam);
	void OnEndSong(int wParam,int lParam);
	void OnScoreStarNum(int wParam,int lParam);
	afx_msg void OnBnClickedResing();
	afx_msg void OnBnClickedGrade();
	afx_msg void OnBnClickedAccompany();
	afx_msg void OnBnClickedClosePlayer();
	afx_msg void OnBnClickedMute();
	afx_msg void OnStnClickedFinalScore();
	afx_msg void OnBnClickedMaxVolume();
	afx_msg void OnBnClickedAddVolume();
	afx_msg void OnBnClickedSubVolume();
	afx_msg void OnBnClickedPublicNotify();
	CEdit m_ctrlMessage;
	afx_msg void OnBnClickedFirewarning();
	afx_msg void OnBnClickedResing2();
	afx_msg void OnBnClickedBapplaud();
	afx_msg void OnBnClickedBcheer();
	CEdit m_ctrlNextSongName;
	afx_msg void OnBnClickedBnextsong();
	afx_msg void OnBnClickedBplayPause();
	afx_msg void OnBnClickedPlayResume();
	afx_msg void OnBnClickedBonline();
	BOOL m_bDebugResing;

	afx_msg void OnBnClickedCheckas();
	CButton m_ctrlIsAS;
	afx_msg void OnBnClickedBbarragemsg();
	afx_msg void OnBnClickedVideoimitate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_ctrlBarrageMsg;
	// 显示歌曲文件
	CImageList m_imgNormal;
	CTreeCtrl m_ctrlFileList;
//	afx_msg void OnTvnEndlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	// 播放进度控制
	CSliderCtrl m_ctrlProgress;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void findNeedFile(CString CurDir,CString strPath,CString& songList,HTREEITEM hRoot=NULL);
	// 歌曲分类
	CComboBox m_ctrlCategory;
	afx_msg void OnBnClickedSelectcategory();
	afx_msg void OnBnClickedStartroll();
	afx_msg void OnBnClickedStoproll();
	CButton m_ctrlMV;
	CButton m_ctrlAd;
	afx_msg void OnCbnSelchangeEffect();
	CComboBox m_ctrlEffect;
	// 是否需要星光视频
	BOOL m_bVideo;
	CButton m_ctrlXgVideo;
	afx_msg void OnBnClickedXgvideo();
	afx_msg void OnBnClickedBspecial();
	afx_msg void OnBnClickedBupdagemoney();
	afx_msg void OnBnClickedBgiftexchange();
	afx_msg void OnBnClickedBgiftsend();
	afx_msg void OnBnClickedBstupdate();
	afx_msg void OnBnClickedBshoptop3();
	afx_msg void OnBnClickedNotmv();
	afx_msg void OnBnClickedAdvertisment();
	afx_msg void OnBnClickedCheckDebug();
	CButton m_ctrlDebugResing;
	CButton m_ctrlJump;
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
};

extern CDXdemoTestSMDlg* pdlg;

