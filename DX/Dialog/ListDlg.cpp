// DXDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../DX.h"
#include "../OpenGLWindow.h"
#include "../Florid/FloridMgr.h"
#include "ListDlg.h"


class ITimerListener;
const char* KTVDATADIR = "F:\\data";//数据目录

HANDLE g_hThread = NULL;

extern HWND g_hWnd;
extern CListDialog* g_listDlg;

//压力测试定时器
#define GTIMER_TIMES 100
unsigned int g_nNextTimer = 0;

// CDXDlg 对话框

CListDialog::CListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CListDialog::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

 void CListDialog::OnOK()
 {
	 DestroyWindow();
	 delete this;
	 g_listDlg = NULL;
 }

void CListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SONGLIST, m_wndList);
}

BEGIN_MESSAGE_MAP(CListDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCLOSE, &CListDialog::OnBnClickedClose)
	ON_NOTIFY(NM_DBLCLK, IDC_SONGLIST, &CListDialog::OnNMDblclkSonglist)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDXDlg 消息处理程序

BOOL CListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitListCtrl();
	ShowList();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CListDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CListDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CListDialog::OnBnClickedClose()
{
	OnOK();
	// TODO: Add your control notification handler code here
}

void CListDialog::OnNMDblclkSonglist(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW pNMList = (LPNMLISTVIEW) pNMHDR;
	if(-1 == pNMList->iItem)
	{
		return;
	}
	CString strText = m_wndList.GetItemText(pNMList->iItem,0);
	//AfxMessageBox(strText);
	//950052=●=950052=●=突然的自我=●=伍佰
	//963931=●=963931=●=小情歌=●=苏打绿乐团
	//1304=●=022951=●=老男孩=●=筷子兄弟
	char buf_curdir[260] = {0};
	GetCurrentDirectory(259,buf_curdir);
	char *fileName = new char[260];
	ZeroMemory(fileName,260);
	sprintf(fileName,"%s\\%s",buf_curdir,strText);
	//memcpy(fileName,strText,strlen(strText)/*-4*/);
	int *pInt = new int;
	*pInt = 300;
	//int* p_start  = new int;
	//int* p_end = new int;
	//*p_start = 60;
	//*p_end = 100;
	//::PostMessage(g_hWnd,WM_CLOSE,0,0);
	//::PostMessage(g_hWnd,WM_PLAY_SONGSTARTEND,(WPARAM)p_start,(LPARAM)p_end);
	::PostMessage(g_hWnd,WM_SWITCHSONG,(WPARAM)fileName,(LPARAM)pInt);

	//////////////////////////////////////////////////////////////////////////压力测试
	if (g_nNextTimer == 0)
	{	
		//g_nNextTimer = SetTimer(GTIMER_TIMES,1500,NULL);
	}

}

void CListDialog::InitListCtrl()
{

}

void CListDialog::ShowList()
{
	COpenGLWindow*		popenGLWindow = cls_gl;
	CString strPath = "";
	strPath.Format("%s\\*.*",popenGLWindow->getAppDir());
	CFileFind find;
	BOOL bFind = find.FindFile(strPath);
	while (bFind)
	{
		bFind = find.FindNextFile();
		if(find.IsDirectory())
		{
			//插入项
			//m_wndList.InsertItem(0,find.GetFileName(),0);
			//m_wndList.SetItemText(nItem,1,"100K");//第一项信息
		}
		else
		{
			int nItem = m_wndList.InsertItem(1,find.GetFileName(),1);//下个
		}
	}
	find.Close();
}

void CListDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == GTIMER_TIMES)
	{
		static bool nSwitch = true;
		_bstr_t fileName;
		static bool bSwitchName = true;//只能先播放视频小的。否则会导致不能播放视频(VMR9的问题)
		if (nSwitch)
		{
			fileName= "950052=●=950052=●=突然的自我=●=伍佰";
			nSwitch = false;
		}
		else
		{
			fileName = "1304=●=022951=●=老男孩=●=筷子兄弟";
			nSwitch = true;
		}
		FILE* fp = fopen("F:\\press.txt","w");
		if (fp == NULL)
		{
			return ;
		}
		static int COUNT = 0;
		COUNT++;
		fprintf(fp,"切歌次数 【%d】",COUNT);
		fclose(fp);
		//switchSong(fileName);
	}
	CDialog::OnTimer(nIDEvent);
}
