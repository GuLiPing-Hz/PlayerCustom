// DXDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../DX.h"
#include "ListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char* KTVDATADIR = "D:\\work\\video\\Data";//����Ŀ¼

extern BOOL	g_done;
extern HWND g_hWnd;
extern bool g_bGLThread;
// CDXDlg �Ի���

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


// CDXDlg ��Ϣ�������

BOOL CListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	InitListCtrl();
	ShowList();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CListDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
			//������
			//m_wndList.InsertItem(0,find.GetFileName(),0);
			//m_wndList.SetItemText(nItem,1,"100K");//��һ����Ϣ
		}
		else
		{
			int nItem = m_wndList.InsertItem(1,find.GetFileName(),1);//�¸�
		}
	}
	find.Close();
}
