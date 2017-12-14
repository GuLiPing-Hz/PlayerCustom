#include "StdAfx.h"
#include "ThreadMgr.h"

CThreadMgr::CThreadMgr(void)
: m_hThread( NULL )
, m_bSuspended( false )
, m_bStop( false )
,m_hEvent(NULL)
{
	InitializeCriticalSection( &m_lstcs );//初始化临界区

	m_hSendEvent = ::CreateEvent( NULL, FALSE, FALSE, _T("{597A8999-F0FE-4481-9CCC-1CBB3E898E4A}") );
}

CThreadMgr::~CThreadMgr(void)
{
	CloseHandle( m_hSendEvent );

	DeleteCriticalSection( &m_lstcs );//资源回收
}

bool CThreadMgr::CreateThread( int iPriority/* = THREAD_PRIORITY_NORMAL */ )
{
	DWORD dwThreadID;
	m_hThread = ::CreateThread(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, &dwThreadID);//创建一个暂停的线程
	if ( NULL == m_hThread ) return false;
	if (THREAD_PRIORITY_NORMAL != iPriority)
	{
		if (!::SetThreadPriority(m_hThread, iPriority))
			WLOGH_WARN << L"SetThreadPriority error";
	}
	::ResumeThread(m_hThread);//唤起线程
	return true;
}

bool CThreadMgr::ReleaseThread()
{
	if ( NULL == m_hThread ) return true;

	m_bStop = true;
	if ( !WaitForThread(500) )
	{
		Terminate();
	}
	if ( FALSE == ::CloseHandle(m_hThread) ) return false;
	m_hThread = NULL;
	return true;
}

bool CThreadMgr::SetPriority(int iPriority) const
{
	return ( TRUE == ::SetThreadPriority(m_hThread, iPriority) );
}

int CThreadMgr::GetPriority() const
{
	return ::GetThreadPriority(m_hThread);
}

bool CThreadMgr::Suspend()
{
	if (m_bSuspended) return true;
	if (::SuspendThread(m_hThread) == (DWORD) -1) return false;
	m_bSuspended = true;
	return true;
}

bool CThreadMgr::Resume()
{
	if (!m_bSuspended) return true;
	if (::ResumeThread(m_hThread) == (DWORD) -1) return false;
	m_bSuspended = false;
	return true;
}

bool CThreadMgr::IsSuspended() const
{
	return m_bSuspended;
}

bool CThreadMgr::IsRunning() const
{
	if (m_hThread == NULL) return false;
	DWORD dwCode = 0;
	::GetExitCodeThread(m_hThread, &dwCode);
	return dwCode == STILL_ACTIVE;
}

bool CThreadMgr::GetExitCode(DWORD * pExitCode) const
{
	return ( TRUE == ::GetExitCodeThread(m_hThread, pExitCode) );
}

#if(WINVER >= 0x0500)
bool CThreadMgr::GetThreadTimes(LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime) const
{
	return ( TRUE == ::GetThreadTimes(m_hThread, lpCreationTime, lpExitTime, lpKernelTime, lpUserTime) );
}
#endif

void CThreadMgr::PostMessageOS( int id, void* pData )
{
	_tMsg msg = { false, id, pData };
	
	EnterCriticalSection( &m_lstcs );
	m_lstMsg.push_back( msg );
	LeaveCriticalSection( &m_lstcs );
}

bool CThreadMgr::SendMessageOS( int id, void* pData )
{
	_tMsg msg = { true, id, pData };

	EnterCriticalSection( &m_lstcs );
	m_lstMsg.push_front( msg );
	LeaveCriticalSection( &m_lstcs );

	if ( WAIT_OBJECT_0 == ::WaitForSingleObject( m_hSendEvent, INFINITE ) )
	{
		return true;
	}

	return false;
}

void CThreadMgr::ProcessIdle()
{
	Sleep(50);
}

bool CThreadMgr::WaitForThread(DWORD dwTimeout/* = INFINITE*/) const
{
	return ( WAIT_OBJECT_0 == ::WaitForSingleObject(m_hThread, dwTimeout) );
}

bool CThreadMgr::Terminate(DWORD dwExitCode/* = 0*/) const
{
	return ( TRUE == ::TerminateThread(m_hThread, dwExitCode) );
}

DWORD WINAPI CThreadMgr::ThreadProc(LPVOID pData)
{
	CThreadMgr * pThis = static_cast<CThreadMgr *>(pData);
	DWORD dwRet = 0;
	(dwRet = pThis->Run());
	return dwRet;
}

DWORD CThreadMgr::Run()
{
	while ( !m_bStop )
	{
		DWORD r = WaitForSingleObject(m_hEvent,INFINITE);

		bool bIsEmpty = false;
		//_tMsg msg = { false, MSG_INVALID, 0, 0 };
		_tMsg msg = { false, MSG_INVALID, 0 };
		
		EnterCriticalSection( &m_lstcs );
		bIsEmpty = m_lstMsg.empty();
		if ( !bIsEmpty )
		{
			msg = m_lstMsg.front();
			m_lstMsg.pop_front();
		}
		LeaveCriticalSection( &m_lstcs );
		
		if ( !bIsEmpty && msg.id != MSG_INVALID )//如果不为空，并且消息不为MSG_INVALID,
		{
			//ProcessMessage( msg.id, msg.wParam, msg.lParam );//处理消息后，写入共享内存
			ProcessMessage(msg.id,msg.pData);
			if ( msg.isSend )
				::SetEvent( m_hSendEvent );//发送事件，使SendMessage继续执行
		}
		else//读取共享内存
		{
			ProcessIdle();
		}
	}

	return 0;
}