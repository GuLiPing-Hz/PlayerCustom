// DXDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../DX.h"
#include "ListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* KTVDATADIR = "D:\\work\\video\\Data";//数据目录

extern BOOL	g_done;
extern HWND g_hWnd;
extern bool g_bGLThread;
// CDXDlg 对话框

CListDialog::CListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CListDialog::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	// TODO: Add your control notification handler code here
	*pResult = 0;
	LPNMLISTVIEW pNMList = (LPNMLISTVIEW) pNMHDR;
	if(-1 == pNMList->iItem)
	{
		return;
	}
	CString strText = m_wndList.GetItemText(pNMList->iItem,0);
	AfxMessageBox(strText);
}

void CListDialog::InitListCtrl()
{

}

void CListDialog::ShowList()
{
	CString strPath = "";
	strPath.Format("%s\\mpg\\*.*",KTVDATADIR);
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
