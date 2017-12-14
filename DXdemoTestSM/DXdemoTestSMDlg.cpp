// DXdemoTestSMDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DXdemoTestSM.h"
#include "DXdemoTestSMDlg.h"
#include <Nb30.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND g_hWnd = NULL;

#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_SIZE (VIDEO_WIDTH*VIDEO_HEIGHT*3)

void getMac1(char* mac);
// CDXdemoTestSMDlg �Ի���

CStringW Gbk2Unicode(CString str)
{
	wchar_t wstr[1024] = {0};
	MultiByteToWideChar(CP_ACP,0,str,str.GetLength(),wstr,1023);
	return CStringW(wstr);
}

CString Unicode2Utf8(CStringW wstr)
{
	char str[1024] = {0};
	WideCharToMultiByte(CP_UTF8,0,wstr,wstr.GetLength(),str,1023,NULL,NULL);
	return CString(str);
}


void CDXdemoTestSMDlg::DealMessage(int cmd, int w, int l,void* s)
{
	switch(cmd)
	{
	case CMD_CENTER_ERROR_LOG:
		{
			pdlg->OnErrorLog((wchar_t*)s);
			break;
		}
	case CMD_CENTER_RANKING_NUM:
		{
			pdlg->OnRankingNum(w);
			break;
		}
	case CMD_CENTER_ENDSONG:
		{
			pdlg->OnEndSong(w,l);
			break;
		}
		//wparam�ܷ�1(int),lparam�ܷ�2(int)
	case CMD_CENTER_SCORE_STARNUM:
		{
			pdlg->OnScoreStarNum(w,l);
			break;
		}
		//wparam��������(int) 
	case CMD_CENTER_MUSICVOLUME_CURRENT:
		{
			break;
		}
		//wparam��˷�����(int)
	case CMD_CENTER_MICVOLUME_CURRENT:
		{
			break;
		}
		//wparam����(int)
	case CMD_CENTER_TONE_CURRENT:
		{
			break;
		}
	case CMD_CENTER_ENABLE_BUTTON:
		{
			break;
		}
		//����һЩ��ť��ֱ���и���س�
	case CMD_CENTER_DISABLE_BUTTON:
		{
			break;
		}
	}
}

CDXdemoTestSMDlg::CDXdemoTestSMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDXdemoTestSMDlg::IDD, pParent)
	, m_ncurValue(0)
	, m_TimerId(0)
	, m_bVideo(FALSE)
	, m_bDebugResing(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CSMHelp::InitSMHelp(DealMessage);

	char buf [260] = {0};
	GetCurrentDirectory(259,buf);
	m_psmHelp = new CSMHelp(buf,"");
}

CDXdemoTestSMDlg::~CDXdemoTestSMDlg()
{
	OnBnClickedClosePlayer();
	SAFE_DELETE(m_psmHelp);
}

void CDXdemoTestSMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDC_ENEXTSONG, m_ctrlNextSongName);
	DDX_Control(pDX, IDC_CHECKAS, m_ctrlIsAS);
	DDX_Control(pDX, IDC_EBMSG, m_ctrlBarrageMsg);
	DDX_Control(pDX, IDC_TREE1, m_ctrlFileList);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_COMBCATEGORY, m_ctrlCategory);
	DDX_Control(pDX, IDC_NOTMV, m_ctrlMV);
	DDX_Control(pDX, IDC_ADVERTISMENT, m_ctrlAd);
	DDX_Control(pDX, IDC_EFFECT, m_ctrlEffect);
	DDX_Control(pDX, IDC_XGVIDEO, m_ctrlXgVideo);
	DDX_Control(pDX, IDC_CHECK_DEBUG, m_ctrlDebugResing);
	DDX_Control(pDX, IDC_CHECK_JUMP, m_ctrlJump);
}

BEGIN_MESSAGE_MAP(CDXdemoTestSMDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SWITCHSONG, &CDXdemoTestSMDlg::OnBnClickedSwitchsong)
	//ON_MESSAGE(WM_DXPLAYER_ENDSONG,&CDXdemoTestSMDlg::OnEndSong)
	//ON_MESSAGE(WM_DXPLAYER_SCORE_STARNUM,&CDXdemoTestSMDlg::OnScoreStarNum)
	ON_BN_CLICKED(IDC_RESING, &CDXdemoTestSMDlg::OnBnClickedResing)
	ON_BN_CLICKED(IDC_GRADE, &CDXdemoTestSMDlg::OnBnClickedGrade)
	ON_BN_CLICKED(IDC_ACCOMPANY, &CDXdemoTestSMDlg::OnBnClickedAccompany)
	ON_BN_CLICKED(IDC_CLOSE_PLAYER, &CDXdemoTestSMDlg::OnBnClickedClosePlayer)
	ON_BN_CLICKED(IDC_MUTE, &CDXdemoTestSMDlg::OnBnClickedMute)
	ON_STN_CLICKED(STATIC_FINAL_SCORE, &CDXdemoTestSMDlg::OnStnClickedFinalScore)
	ON_BN_CLICKED(IDC_MAX_VOLUME, &CDXdemoTestSMDlg::OnBnClickedMaxVolume)
	ON_BN_CLICKED(IDC_ADD_VOLUME, &CDXdemoTestSMDlg::OnBnClickedAddVolume)
	ON_BN_CLICKED(IDC_SUB_VOLUME, &CDXdemoTestSMDlg::OnBnClickedSubVolume)
	ON_BN_CLICKED(IDC_PUBLIC_NOTIFY, &CDXdemoTestSMDlg::OnBnClickedPublicNotify)
	ON_BN_CLICKED(IDC_FIREWARNING, &CDXdemoTestSMDlg::OnBnClickedFirewarning)
	ON_BN_CLICKED(IDC_RESING2, &CDXdemoTestSMDlg::OnBnClickedResing2)
	ON_BN_CLICKED(IDC_BAPPLAUD, &CDXdemoTestSMDlg::OnBnClickedBapplaud)
	ON_BN_CLICKED(IDC_BCHEER, &CDXdemoTestSMDlg::OnBnClickedBcheer)
	ON_BN_CLICKED(IDC_BNEXTSONG, &CDXdemoTestSMDlg::OnBnClickedBnextsong)
	ON_BN_CLICKED(IDC_BPLAY_PAUSE, &CDXdemoTestSMDlg::OnBnClickedBplayPause)
	ON_BN_CLICKED(IDC_PLAY_RESUME, &CDXdemoTestSMDlg::OnBnClickedPlayResume)
	ON_BN_CLICKED(IDC_BONLINE, &CDXdemoTestSMDlg::OnBnClickedBonline)
	ON_BN_CLICKED(IDC_CHECKAS, &CDXdemoTestSMDlg::OnBnClickedCheckas)
	ON_BN_CLICKED(IDC_BBARRAGEMSG, &CDXdemoTestSMDlg::OnBnClickedBbarragemsg)
	ON_BN_CLICKED(IDC_VIDEOIMITATE, &CDXdemoTestSMDlg::OnBnClickedVideoimitate)
	ON_WM_TIMER()
//ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CDXdemoTestSMDlg::OnNMDblclkTree1)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CDXdemoTestSMDlg::OnTvnSelchangedTree1)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_SELECTCATEGORY, &CDXdemoTestSMDlg::OnBnClickedSelectcategory)
ON_BN_CLICKED(IDC_STARTROLL, &CDXdemoTestSMDlg::OnBnClickedStartroll)
ON_BN_CLICKED(IDC_STOPROLL, &CDXdemoTestSMDlg::OnBnClickedStoproll)
ON_CBN_SELCHANGE(IDC_EFFECT, &CDXdemoTestSMDlg::OnCbnSelchangeEffect)
ON_BN_CLICKED(IDC_XGVIDEO, &CDXdemoTestSMDlg::OnBnClickedXgvideo)
ON_BN_CLICKED(IDC_BSPECIAL, &CDXdemoTestSMDlg::OnBnClickedBspecial)
ON_BN_CLICKED(IDC_BUPDAGEMONEY, &CDXdemoTestSMDlg::OnBnClickedBupdagemoney)
ON_BN_CLICKED(IDC_BGIFTEXCHANGE, &CDXdemoTestSMDlg::OnBnClickedBgiftexchange)
ON_BN_CLICKED(IDC_BGIFTSEND, &CDXdemoTestSMDlg::OnBnClickedBgiftsend)
ON_BN_CLICKED(IDC_BSTUPDATE, &CDXdemoTestSMDlg::OnBnClickedBstupdate)
ON_BN_CLICKED(IDC_BSHOPTOP3, &CDXdemoTestSMDlg::OnBnClickedBshoptop3)
ON_BN_CLICKED(IDC_NOTMV, &CDXdemoTestSMDlg::OnBnClickedNotmv)
ON_BN_CLICKED(IDC_ADVERTISMENT, &CDXdemoTestSMDlg::OnBnClickedAdvertisment)
ON_BN_CLICKED(IDC_CHECK_DEBUG, &CDXdemoTestSMDlg::OnBnClickedCheckDebug)
ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CDXdemoTestSMDlg::OnNMDblclkTree1)
END_MESSAGE_MAP()


// CDXdemoTestSMDlg ��Ϣ�������

void CDXdemoTestSMDlg::findNeedFile(CString CurDir,CString strPath,CString& songList,HTREEITEM hRoot)
{
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

			CString str = CurDir+"\\"+find.GetFileName();
			wchar_t wstr[260] = {0};
			MultiByteToWideChar(CP_ACP,0,str,str.GetLength(),wstr,259);
			m_songlist.push_back(wstr);//�¸�
			songList += find.GetFileName()+"\r\n";
			if(hRoot)
				m_ctrlFileList.InsertItem(find.GetFileName(),0,1,hRoot);
		}
	}
	find.Close();
}

inline bool parseLine(char* buf,SongListInfo& sli,std::string& category)
{
	if(!buf)
		return false;

	if(buf[0]=='\n' || buf[0]==',')
		return false;

	char* pOld = buf;
	char* pBuffer = strchr(pOld,',');
	if(pBuffer)
	{
		*pBuffer = '\0';
		sli.songname = pOld;
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if (pBuffer)
	{
		*pBuffer = '\0';
		sli.singername = pOld;
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if(pBuffer)
	{
		*pBuffer = '\0';
		sli.filename = pOld;
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if(pBuffer)
	{
		*pBuffer = '\0';
		sli.endsecond = atoi(pOld);
		pOld = pBuffer+1;
	}
	pBuffer = strchr(pOld,',');
	if (pBuffer)
	{
		pBuffer = '\0';
		category = pOld;
		pOld = pBuffer+1;
	}
	else
	{
		//ȥ���س���
		int len = strlen(pOld);
		pOld[len-1] = 0;
		category = pOld;
	}

	return true;
}

inline bool readCSV(const char* csvFile,MAPCATEGORY& mc)
{
	char buf[1024] = {0};

	if(!csvFile)
		goto failed;

	FILE* fp = fopen(csvFile,"r");
	if(!fp)
		goto failed;

	while(fgets(buf,1024,fp))
	{
		SongListInfo sli;
		std::string c;
		if(parseLine(buf,sli,c))
		{
			wchar_t w_str[260] = {0};
			MultiByteToWideChar(CP_ACP,0,sli.songname.c_str(),sli.songname.length(),w_str,259);
			std::wstring wSongName = w_str;

			MAPCATEGORY::iterator it = mc.find(c);
			if(it == mc.end())
			{
				MAPSONGLIST songlist;
				songlist.insert(std::make_pair(wSongName,sli));
				mc.insert(std::make_pair(c,songlist));
			}
			else
				it->second.insert(std::make_pair(wSongName,sli));
		}
	}
	return true;

failed: 
	if(fp)
		fclose(fp);
	return false;
}

BOOL CDXdemoTestSMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);			// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	readCSV("randomlist.csv",m_mapCategory);
	g_hWnd = m_hWnd;

	m_imgNormal.Create(IDB_NORMAL,18,2,0);
	m_ctrlFileList.SetImageList(&m_imgNormal,TVSIL_NORMAL);
	m_ctrlFileList.SetItemHeight(20);


	CString songList;
	CString strPath = "";
	char buf[260] = {0};
	GetCurrentDirectory(259,buf);
	CString CurDir = buf;
	HTREEITEM hRoot = m_ctrlFileList.InsertItem(CurDir,0,1);
	strPath.Format("%s\\*.mp4",CurDir);
	findNeedFile(CurDir,strPath,songList,hRoot);
	strPath.Format("%s\\*.st4",CurDir);
	findNeedFile(CurDir,strPath,songList);
	strPath.Format("%s\\*.mpg",CurDir);
	findNeedFile(CurDir,strPath,songList,hRoot);


	m_uCur = 0;
	m_uTotal = m_songlist.size();
	
	int cur_display_w = ::GetSystemMetrics(SM_CXSCREEN);
	int cur_display_h = ::GetSystemMetrics(SM_CYSCREEN);
	RECT rect;
	::GetWindowRect(m_hWnd,&rect);
	//rect.left += cur_display_w+100;
	//rect.right += cur_display_w+100; 
	//MoveWindow(&rect);

	SetDlgItemText(IDC_STATIC_SONGLIST,songList);
	char strMac[260] = {0};
	getMac(strMac);
	//getMac(strMac);
	SetDlgItemText(IDC_STATIC_TMAC,strMac);
	m_ctrlFileList.Expand(hRoot,TVE_EXPAND);

	m_ctrlProgress.SetRange(0,100);
	m_ctrlProgress.SetPos(0);

	m_ctrlCategory.AddString("�����ȸ�");
	m_ctrlCategory.AddString("����ʱ��");
	m_ctrlCategory.AddString("Ӱ�ӽ���");
	m_ctrlCategory.AddString("�������");
	m_ctrlCategory.AddString("�˸辢��");
	m_ctrlCategory.SetCurSel(0);

	m_ctrlEffect.AddString("ħ�����");
	m_ctrlEffect.AddString("ħ������");
	m_ctrlEffect.AddString("ħ������");
	m_ctrlEffect.AddString("ħ��K��");
	m_ctrlEffect.AddString("רҵ���");
	m_ctrlEffect.AddString("רҵ����");
	m_ctrlEffect.AddString("רҵ����");
	m_ctrlEffect.AddString("רҵK��");
	m_ctrlEffect.SetCurSel(3);

	m_ctrlMV.SetCheck(1);

	m_ctrlBarrageMsg.SetWindowText("<txt size=\"18\" color=\"#ff0000\" txt=\"abcd\"/><img src=\"images\\a.gif\"/>\n");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDXdemoTestSMDlg::OnPaint()
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
HCURSOR CDXdemoTestSMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
Dev C++ -> include libnetapi32.a
BCC 5.5 or VC++ -> #pragma comment(lib,"netapi32.lib")
*/
typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff [30];
} ASTAT, *PASTAT;

///��������,���������Ϣ
void output(PIP_ADAPTER_INFO pIpAdapterInfo)
{
	//�����ж�����,���ͨ��ѭ��ȥ�ж�
	while (pIpAdapterInfo)
	{
		printf("�������ƣ�%s\n",pIpAdapterInfo->AdapterName);
		printf("����������%s\n",pIpAdapterInfo->Description);
		//printf("����MAC��ַ��%sd",pIpAdapterInfo->Address);
		for (UINT i = 0; i < pIpAdapterInfo->AddressLength; i++)
			if (i==pIpAdapterInfo->AddressLength-1)
			{
				printf("%02x\n", pIpAdapterInfo->Address[i]);
			}
			else
			{
				printf("%02x-", pIpAdapterInfo->Address[i]);
			}
			printf("����IP��ַ���£�\n");
			//���������ж�IP,���ͨ��ѭ��ȥ�ж�
			IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);
			do 
			{
				printf("%s\n",pIpAddrString->IpAddress.String);
				pIpAddrString=pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
			printf("*****************************************************\n");
	}
	return;
}

void getMac1(char* mac)
{
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//�õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
	if (ERROR_BUFFER_OVERFLOW==nRel)
	{
		//����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		//�ͷ�ԭ�����ڴ�ռ�
		delete pIpAdapterInfo;
		//���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
	}
	if (ERROR_SUCCESS==nRel)
	{
		//���������Ϣ
		output(pIpAdapterInfo);
	}
	//�ͷ��ڴ�ռ�
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
}

void CDXdemoTestSMDlg::getMac(char * mac)
{
	ASTAT Adapter;
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM lenum;
	int i = 0;

	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);

	uRetCode = Netbios( &Ncb );
	//printf( "The NCBENUM return adapter number is: %d \n ", lenum.length);
	for(i=0; i < lenum.length ; i++)
	{
		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];
		uRetCode = Netbios( &Ncb );

		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];
		strcpy((char *)Ncb.ncb_callname, "* ");
		Ncb.ncb_buffer = (unsigned char *) &Adapter;
		Ncb.ncb_length = sizeof(Adapter);
		uRetCode = Netbios( &Ncb );
		
		if (uRetCode == NRC_GOODRET)
		{
			//sprintf(mac, "%02x-%02x-%02x-%02x-%02x-%02x ",
			sprintf(mac, "����Mac:%02X-%02X-%02X-%02X-%02X-%02X ",
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
			//printf( "The Ethernet Number on LANA %d is: %s\n ", lenum.lana[i], mac);
		}
	}
}

void CDXdemoTestSMDlg::OnBnClickedSwitchsong()
{
	if (!m_songlist.empty())
	{
		m_curSongName = m_songlist.front();
		m_songlist.pop_front();
		//m_curSongName = "\\\\192.168.2.131\\f\\193\\960139=��=����=��=������.mpg";
		//m_curSongName = "file://192.168.2.131/f/193/960139=��=����=��=������.mpg";

		int nAS = m_ctrlIsAS.GetCheck()?1:0;//�Ƿ�Գ�
		int nAd = m_ctrlAd.GetCheck()?1:0;//�Ƿ���
		int nMV = m_ctrlMV.GetCheck()?1:0;//�Ƿ�MV
		int nJump = m_ctrlJump.GetCheck()?1:0;//�Ƿ�������
		int info = ((30<<24)|(1<<23)|(2<<19)|(3<<15)|(nMV<<14)|(nAd<<13)|(nJump<<12)|(0<<10)|(nAS<<9)|(16<<1)|1);
		//*pstaff_pos_height = 300;
		//m_psmHelp->PostSingerPic("C:\\Users\\Administrator\\Documents\\Visual Studio 2005\\Projects\\SMDLLEx\\debug\\singer.jpg");
		//m_psmHelp->PostPlayGrade(2);
		//��Ƶ��Ϣ
		m_psmHelp->PostVideoInfo(1,0,VIDEO_WIDTH,VIDEO_HEIGHT,3,0,1,"aaa");
		m_psmHelp->PostVideoInfo(1,1,VIDEO_WIDTH,VIDEO_HEIGHT,3,0,1,"bbb");

		std::wstring strMdm = m_curSongName;
		std::wstring::size_type b = strMdm.rfind(L".");
		strMdm = strMdm.substr(0,b);
		std::wstring::size_type f = strMdm.rfind(L"=��=");
		std::wstring singer = strMdm.substr(f+3);
		strMdm = strMdm.substr(0,f);
		f = strMdm.rfind(L"=��=");
		std::wstring song = strMdm.substr(f);
		ThreeWString gThreeStrs = {0};
		wcscpy(gThreeStrs.str1,singer.c_str());
		wcscpy(gThreeStrs.str2,song.c_str());
		//���͸�����Ϣ
		m_psmHelp->PostSongInfo(&gThreeStrs);
		//��ʼ����ʱ��
		m_psmHelp->PostSongStartEnd(100,160);
		//m_psmHelp->PostNextSong("ktv2");
		//m_psmHelp->PostFromOther(true);
		//"D:\\glp\\work\\SVN\\ktv play\\src\\DXDPdemo\\debug\\mv\\000006=��=Ϊʲô�㱳���Ұ�����=��=��־��.dat"
		m_psmHelp->PostSwitchSong(m_curSongName,info,NULL);
		//m_psmHelp->PostNextSong("3002��һ��ѩ");
		SetDlgItemText(IDC_SONG_STATE,"�������ڲ��Ż���ͣ��");
	}
	//���Ƶƹ⣬������
}

void CDXdemoTestSMDlg::OnScoreStarNum(int wParam,int lParam)
{

	float aveScore = wParam/10.0f;
	float aveScore1 = lParam/10.0f;
	char strScore[260] = {0};
	sprintf(strScore,"�ܷ�1:%.1f\n\r�ܷ�2:%.1f",aveScore,aveScore1);
	SetDlgItemText(STATIC_FINAL_SCORE,strScore);
}

void CDXdemoTestSMDlg::OnErrorLog(wchar_t* msg)
{
	if(msg)
		SetDlgItemTextW(GetSafeHwnd(),STATIC_FINAL_SCORE,msg);
}

void CDXdemoTestSMDlg::OnRankingNum(int wParam)
{
	PlayerScore ps[2];
	for (int i=0;i<wParam;i++)
	{
		ps[i].valid = true;
		ps[i].noInCountry = i+100;
		ps[i].noInHistory = i+10;
		ps[i].noInSong = i+1;
		ps[i].percentCountry = 40;
		ps[i].percentHistory = 60;
		m_psmHelp->PostRankingList(ps,2);
	}
}

void CDXdemoTestSMDlg::OnEndSong(int wParam,int lParam)
{
	SetDlgItemText(IDC_SONG_STATE,"�����Ѳ������");//�������ֽ��㻭��
}

void CDXdemoTestSMDlg::OnBnClickedResing()
{
	if (m_curSongName != "")
	{
		int n= 20000;
		do 
		{

			int nAS = m_ctrlIsAS.GetCheck()?1:0;//�Ƿ�Գ�
			int nAd = m_ctrlAd.GetCheck()?1:0;//�Ƿ���
			int nMV = m_ctrlMV.GetCheck()?1:0;//�Ƿ�MV
			int nJump = m_ctrlJump.GetCheck()?1:0;//�Ƿ�������
			int info = ((30<<24)|(0<<23)|(2<<19)|(3<<15)|(nMV<<14)|(nAd<<13)|(nJump<<12)|(0<<10)|(nAS<<9)|(16<<1)|1);
			m_psmHelp->PostSwitchSong(m_curSongName,info,NULL);

			n--;
			//���÷�������Ϣ
			if(n%100 == 0)
				Sleep(1000);
		} while (m_bDebugResing && n);
	}
}

void CDXdemoTestSMDlg::OnBnClickedGrade()
{
	static int nStart = 0;
	if (m_ctrlIsAS.GetCheck())
	{
		nStart = nStart == 3 ? 0 : 3;
	}
	else
	{
		nStart++;
		nStart = nStart >= 3 ? 0 : nStart;
	}
	m_psmHelp->PostPlayGrade(nStart);
}

void CDXdemoTestSMDlg::OnBnClickedAccompany()
{
	m_psmHelp->PostPlayAccompany();
}

void CDXdemoTestSMDlg::OnBnClickedClosePlayer()
{
	m_psmHelp->PostClosePlayer();
}

void CDXdemoTestSMDlg::OnBnClickedMute()
{
	m_psmHelp->PostMuteOpen();
}

void CDXdemoTestSMDlg::OnStnClickedFinalScore()
{
	// TODO: Add your control notification handler code here
}

void CDXdemoTestSMDlg::OnBnClickedMaxVolume()
{
	m_psmHelp->PostMuteClose();
}

static int vVolume = 20;
void CDXdemoTestSMDlg::OnBnClickedAddVolume()
{
	m_psmHelp->PostAddTone();
	vVolume+= 10;
	if(vVolume > 100)
		vVolume = 100;
	m_psmHelp->PostSetVolume(vVolume);
}

void CDXdemoTestSMDlg::OnBnClickedSubVolume()
{
	vVolume -= 10;
	if(vVolume < 0)
		vVolume = 0;
	m_psmHelp->PostSubTone();
}

void CDXdemoTestSMDlg::OnBnClickedPublicNotify()
{
	CString str;
	m_ctrlMessage.GetWindowText(str);
	int nLen = str.GetLength();
	PMessage pm = {0};
	strcpy(pm.str,str);
	m_psmHelp->PostMessage(&pm);
}

void CDXdemoTestSMDlg::OnBnClickedFirewarning()
{
	static bool b = false;
	b = !b;
	if (b)
	{
		m_psmHelp->PostFireWarning();
	}
	else
	{
		m_psmHelp->PostNoFireWarning();

	}
}


void CDXdemoTestSMDlg::OnBnClickedResing2()
{
	if (m_curSongName != "")
	{
		//for ( int i=0;i<20000;i++ )
		{
			//m_psmHelp->PostVideoInfo(1,0,320,240,3,0);
			//m_psmHelp->PostVideoInfo(1,1,320,240,3,0);
			m_psmHelp->PostMusicRang(0,100);

			std::wstring strMdm = m_curSongName;
			std::wstring::size_type b = strMdm.rfind(L".");
			strMdm = strMdm.substr(0,b);
			std::wstring::size_type f = strMdm.rfind(L"=��=");
			std::wstring singer = strMdm.substr(f+3);
			strMdm = strMdm.substr(0,f);
			f = strMdm.rfind(L"=��=");
			std::wstring song = strMdm.substr(f+3);
			//���͸�����Ϣ
			ThreeWString gThreeStr = {0};
			wcscpy(gThreeStr.str1,singer.c_str());
			wcscpy(gThreeStr.str2,song.c_str());
			m_psmHelp->PostSongInfo(&gThreeStr);
			m_psmHelp->PostNextSong(L"ktv2");
			bool isFind = false;
			MAPCATEGORY::iterator it = m_mapCategory.begin();
			int nEndMs = 0;
			for(it;it!=m_mapCategory.end();it++)
			{
				MAPSONGLIST::iterator it2 = it->second.find(song);
				if(it2!= it->second.end())
				{
					isFind = true;
					nEndMs = it2->second.endsecond;
					break;
				}
			}
			if(isFind)
			{
				//��ʼ����ʱ��
				m_psmHelp->PostSongStartEnd(0,nEndMs);
			}
			else
				m_psmHelp->PostSongStartEnd(0,0);

			int nAS = m_ctrlIsAS.GetCheck()?1:0;
			int nAd = m_ctrlAd.GetCheck()?1:0;
			int nMV = m_ctrlMV.GetCheck()?1:0;
			int nJump = m_ctrlJump.GetCheck()?1:0;//�Ƿ�������
			int info = ((30<<24)|(1<<23)|(2<<19)|(3<<15)|(nMV<<14)|(nAd<<13)|(nJump<<12)|(0<<10)|(nAS<<9)|(16<<1)|1);
			//*pint = 300;
			m_psmHelp->PostSingerPic(L"D:\\glp\\VSProject\\2005\\PlayerS\\Debug\\A ����.jpg");
			m_psmHelp->PostSwitchSong(m_curSongName,info,NULL);
			//���÷�������Ϣ
			//Sleep(2500);
		}
	}
}

void CDXdemoTestSMDlg::OnBnClickedBapplaud()
{
	m_psmHelp->PostAtomosphere(0);
}

void CDXdemoTestSMDlg::OnBnClickedBcheer()
{
	m_psmHelp->PostAtomosphere(1);
}

void CDXdemoTestSMDlg::OnBnClickedBnextsong()
{
	wchar_t wstr[260] = {0};
	::GetWindowTextW(m_ctrlNextSongName.GetSafeHwnd(),wstr,259);
	m_psmHelp->PostNextSong(wstr);
}

void CDXdemoTestSMDlg::OnBnClickedPlayResume()
{
	m_psmHelp->PostControlResume();
}

void CDXdemoTestSMDlg::OnBnClickedBplayPause()
{
	m_psmHelp->PostControlPause();
}

void CDXdemoTestSMDlg::OnBnClickedBonline()
{
	static int a = 1;
	a++;
	m_psmHelp->PostOnline(a);
}

void CDXdemoTestSMDlg::OnBnClickedCheckas()
{
}

void CDXdemoTestSMDlg::OnBnClickedBbarragemsg()
{
	CString str;
	m_ctrlBarrageMsg.GetWindowText(str);
	PMessage pm = {0};
	pm.type = 1;
	pm.argb = 0x7FFF0000;
	pm.fsize = 5;
	pm.mode = 0;
	pm.sayid = 12345678;
	wcscpy(pm.sayname,L"��Ļ��");//�Ե�Ļ��Ч ˵��������
	
	CString utf8str = Unicode2Utf8(Gbk2Unicode(str));

	strcpy(pm.str,utf8str);
	m_psmHelp->PostMessage(&pm);
}

void CDXdemoTestSMDlg::OnBnClickedVideoimitate()
{
	if (m_TimerId)
	{
		KillTimer(VIDEO_TIMER);
		m_TimerId = 0;
		m_psmHelp->PostVideoInfo(0,0,VIDEO_WIDTH,VIDEO_HEIGHT,3,0,false,NULL);
		m_psmHelp->PostVideoInfo(0,1,VIDEO_WIDTH,VIDEO_HEIGHT,3,0,false,NULL);
	}
	else
	{
		m_psmHelp->PostMusicRang(0,100);

		m_psmHelp->PostVideoInfo(1,0,VIDEO_WIDTH,VIDEO_HEIGHT,3,0,false,NULL);
		m_psmHelp->PostVideoInfo(0,1,VIDEO_WIDTH,VIDEO_HEIGHT,3,0,false,NULL);

		//���͸�����Ϣ
		m_psmHelp->PostNextSong(L"ktv2");

		int nAS = m_ctrlIsAS.GetCheck()?1:0;//�Ƿ�Գ�
		int nAd = m_ctrlAd.GetCheck()?1:0;//�Ƿ���
		int nMV = m_ctrlMV.GetCheck()?1:0;//�Ƿ�MV
		int nJump = m_ctrlJump.GetCheck()?1:0;//�Ƿ�������
		int info = ((30<<24)|(1<<23)|(2<<19)|(3<<15)|(nMV<<14)|(nAd<<13)|(nJump<<12)|(0<<10)|(nAS<<9)|(16<<1)|1);
		//*pint = 300;
		m_psmHelp->PostSingerPic(L"D:\\glp\\VSProject\\2005\\PlayerS\\Debug\\A ����.jpg");
		m_psmHelp->PostSwitchSong(L"",info,NULL);

		m_TimerId = SetTimer(VIDEO_TIMER,30,NULL);
		if(m_TimerId == 0)
			OutputDebugStringA("Timer Create Failed");
	}
}

void CDXdemoTestSMDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == VIDEO_TIMER)
	{
		unsigned char* pBuffer = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT*3];//640*480*3
		if(pBuffer)
		{
			static unsigned char r = 0xff;
			static unsigned char g = 0x00;
			static unsigned char b = 0x00;

			//r --;
			unsigned char* pBuf = pBuffer;
			for (int i=0;i<VIDEO_HEIGHT;i++)
			{
				for(int j=0;j<VIDEO_WIDTH;j++)
				{
					memset(pBuf++,r,1);
					memset(pBuf++,g,1);
					memset(pBuf++,b,1);
				}
			}

			m_psmHelp->PostVideoInfo(1,0,VIDEO_WIDTH,VIDEO_HEIGHT,3,VIDEO_SIZE,false,NULL);
			//m_psmHelp->PostVideoInfo(1,1,VIDEO_WIDTH,VIDEO_HEIGHT,3,VIDEO_SIZE,false,NULL);

			delete pBuffer;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDXdemoTestSMDlg::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	HTREEITEM item = m_ctrlFileList.GetSelectedItem();
	HTREEITEM parenti = m_ctrlFileList.GetParentItem(item);
	m_curSongName = m_ctrlFileList.GetItemText(parenti) +"\\"+ m_ctrlFileList.GetItemText(item);
	//m_curSongName.Replace(".mdm",".mpg");
	OnBnClickedResing();
	//OnBnClickedAccompany();Ĭ��ԭ������������趨���ܻᵼ�½����������������Ƿ���Audioswitch filter���洦��Ĭ��ʲô�������ע���ˡ�����Ĭ�ϰ鳪��
	*pResult = 0;
}

void CDXdemoTestSMDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CString szText = m_ctrlFileList.GetItemText(pNMTreeView->itemNew.hItem);

	*pResult = 0;
}

void CDXdemoTestSMDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	if(pScrollBar->m_hWnd == m_ctrlProgress.GetSafeHwnd())
	{
		m_psmHelp->PostControlPause();
		if(nSBCode == SB_ENDSCROLL)
		{
			int pos = m_ctrlProgress.GetPos();
			m_psmHelp->PostProgress(pos);
			m_psmHelp->PostControlResume();
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDXdemoTestSMDlg::OnBnClickedSelectcategory()
{
	// TODO: Add your control notification handler code here
	CString category;
	int index = m_ctrlCategory.GetCurSel();
	if(index == -1)
		AfxMessageBox("��ѡ�����");
	m_ctrlCategory.GetLBText(index,category);

	m_psmHelp->PostCategory(category);
}

void CDXdemoTestSMDlg::OnBnClickedStartroll()
{
	m_psmHelp->PostSelectState(false);
}

void CDXdemoTestSMDlg::OnBnClickedStoproll()
{
	m_psmHelp->PostSelectState(true);
}

void CDXdemoTestSMDlg::OnCbnSelchangeEffect()
{
	int nCur = m_ctrlEffect.GetCurSel();
	if (nCur >= 0)
	{
		m_psmHelp->PostEffectNotify(nCur);
	}
}

void CDXdemoTestSMDlg::OnBnClickedXgvideo()
{
	m_bVideo = m_ctrlXgVideo.GetCheck();
}

void CDXdemoTestSMDlg::OnBnClickedBspecial()
{
	PlayerScore ps;
	ps.valid = true;
	ps.noInSong = 32;
	ps.noInHistory = 12;
	ps.percentHistory = 50;
	ps.noInCountry = 326;
	ps.percentCountry = 40;
	m_psmHelp->PostSpecialMsg(L"��Ϫӡ���",L"A3-6",L"���糱ˮ",958,&ps);
}

void CDXdemoTestSMDlg::OnBnClickedBupdagemoney()
{
	static bool b = true;
	b= !b;
	if(b)
		m_psmHelp->PostUpdateMoney(6000);
	else
		m_psmHelp->PostUpdateMoney(100);
}

void CDXdemoTestSMDlg::OnBnClickedBgiftexchange()
{
	GiftExchange ge = {0};
	ge.count = 1;
	wcscpy(ge.giftunit,L"ƿ");
	wcscpy(ge.giftname,L"ѩ��ơ��");
	wcscpy(ge.shop,L"��Ϫӡ��ǵ�");
	wcscpy(ge.room,L"A3-6");
	ge.money = 5000;

	m_psmHelp->PostGiftExchange(&ge);
}

void CDXdemoTestSMDlg::OnBnClickedBgiftsend()
{
	GiftSend gs = {0};
	gs.giftId = 100001;
	gs.count = 11;
	wcscpy(gs.giftfile,L"D:\\work\\vs\\ktv_player_src\\PlayerS\\Debug\\data\\gift\\gift_100001.png");
	wcscpy(gs.giftname,L"�ʻ�");
	gs.count = 10;
	wcscpy(gs.giftunit,L"��");
	gs.recvid = 12345678;
	wcscpy(gs.recvlocation,L"���˺�����С��ɽ��A01");
	wcscpy(gs.recvname,L"�����");
	gs.sendid = 87654321;
	wcscpy(gs.sendname,L"�ɵ���ɽ���ϻ���дʫ");
	wcscpy(gs.sendlocation,L"���Ŵ�S01");

	m_psmHelp->PostGiftSend(&gs);
}

void CDXdemoTestSMDlg::OnBnClickedBstupdate()
{
	// TODO: Add your control notification handler code here
	static bool overed = true;
	overed = !overed;

	STUpdate data = {0};
	data.overed = overed;
	data.rank = 2;
	wcscpy(data.room,L"��ʥA32");
	wcscpy(data.song ,L"�����찲��");
	data.score = 596;
	m_psmHelp->PostSTUpdate(&data);
}

void CDXdemoTestSMDlg::OnBnClickedBshoptop3()
{
	// TODO: Add your control notification handler code here
	ShopTopTip data = {0};
	wcscpy(data.gift , L"������Ѽ");
	wcscpy(data.time , L"22��30��");
	TopInfo top[3] = {{true,L"A11",900},{true,L"B11",800},{true,L"C11",700}};
	memcpy(data.top,top,sizeof(top));
	m_psmHelp->PostShopTop(&data);
}

void CDXdemoTestSMDlg::OnBnClickedNotmv()
{
}

void CDXdemoTestSMDlg::OnBnClickedAdvertisment()
{
}

void CDXdemoTestSMDlg::OnBnClickedCheckDebug()
{
	m_bDebugResing = m_ctrlDebugResing.GetCheck();
}
