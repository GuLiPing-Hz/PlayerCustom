/*
	注释时间:2014-4-25
	author: glp
	测试debug 使用，一般情况不需要用到。
*/
#pragma once
#include "afxcmn.h"

class CDrawListener;
// CDXDlg 对话框
//
class CListDialog : public CDialog
{
// 构造
public:
	CListDialog(CWnd* pParent = NULL);	// 标准构造函数
	virtual void OnOK();
// 对话框数据
	enum { IDD = IDD_DX_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnNMDblclkSonglist(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	//void switchSong(const char* fileName);
	//bool CreateVideoThread();
	void InitListCtrl();
	void ShowList();
public:
	// 歌曲列表
	CListCtrl m_wndList;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	CCritSec						m_ObjectLock;
};
