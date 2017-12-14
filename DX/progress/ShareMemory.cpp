#include <windows.h>
#include "ShareMemory.h"
#include <assert.h>

#ifndef BUILD_PLAYER
#define  LOG_ERN0(x) 
//MessageBoxA(NULL,x,"ERROR",MB_ICONERROR|MB_OK)
#define  LOG_ERS2(x) 
//MessageBoxA(NULL,x,"ERROR",MB_ICONERROR|MB_OK)
#endif

#ifndef SAFE_CLOSEHANDLE
#define SAFE_CLOSEHANDLE(h)         \
	if ((h) != NULL)                  \
{                               \
	CloseHandle((h));             \
	(h) = NULL;                   \
}
#endif

//11M = 11*1024*1024
#define BUFFER_SIZE			11534336
//1280*720*3+4
#define SEPERATOR_SIZE   2764800

CShareMemory::CShareMemory()
:m_hShareMem(NULL)
,m_pBuffer(NULL)
,m_isOwner(false)
,m_hMetux(NULL)
{
	// 	SYSTEM_INFO info;
	// 	GetSystemInfo(&info);
 
	m_nBufferSize = BUFFER_SIZE;
	m_nRealBufferSize = m_nBufferSize - BUFFER_HEAD_SIZE;

	assert(m_nRealBufferSize >= SEPERATOR_SIZE*4);
}

CShareMemory::~CShareMemory()
{
	if (m_pBuffer)
	{
		UnmapViewOfFile(m_pBuffer);
		m_pBuffer = NULL;
	}
	SAFE_CLOSEHANDLE(m_hShareMem);
/*
	if(m_isOwner)
		SAFE_CLOSEHANDLE(m_hMetux);
*/
}

/*
 * 创建共享内存
 * param[in] : szName 共享内存名称
 * return : 操作是否成功
 */
bool CShareMemory::Create( const char * szName )
{
	CHAR szFullName[MAX_PATH] = { 0 };
	strcpy( szFullName, "share_memory_" );
	strcat( szFullName, szName );

/*
	if(m_hMetux)
		return false;

	char mutexName[260] = {0};
	sprintf(mutexName,"%s_mutex",szFullName);
	m_hMetux = ::CreateMutexA(NULL,FALSE,mutexName);
	if(m_hMetux == NULL || ::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		SAFE_CLOSEHANDLE(m_hMetux);
		OutputDebugStringA("Share Memory Metux Create Error\n");
		return false;
	}
	m_isOwner = true;
*/

	m_hShareMem = ::CreateFileMappingA( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_nBufferSize, szFullName );
	if ( NULL == m_hShareMem )
	{// 创建共享内存失败
		//WLOGH_ERRO << L"create share memory failed";
		return false;
	}
	if (m_pBuffer)
	{
		UnmapViewOfFile(m_pBuffer);
		m_pBuffer = NULL;
	}
	m_pBuffer = (char*)MapViewOfFile( m_hShareMem, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
	if (!m_pBuffer)
	{
		//WLOGH_ERRO << L"map vide of file failed,Errorcode";
		return false;
	}
	*((int*)m_pBuffer) = 0;
	*((int*)m_pBuffer+1) = 0;

	return true;
}

/*
 * 打开共享内存
 * param[in] : szName 共享内存名称
 * return : 操作是否成功
 */
bool CShareMemory::Open( const char * szName )
{
	char szFullName[MAX_PATH] = { 0 };
	strcpy( szFullName, "share_memory_" );
	strcat( szFullName, szName );

/*
	if(m_hMetux)
		return false;

	char mutexName[260] = {0};
	sprintf(mutexName,"%s_mutex",szFullName);
	m_hMetux = ::CreateMutexA(NULL,FALSE,mutexName);
	if(m_hMetux == NULL || ::GetLastError() != ERROR_ALREADY_EXISTS)
	{
		SAFE_CLOSEHANDLE(m_hMetux);
		OutputDebugStringA("Share Memory Metux Open Error\n");
		return false;
	}
	m_isOwner = false;
*/

	m_hShareMem = OpenFileMappingA( FILE_MAP_ALL_ACCESS, FALSE, szFullName );
	if ( NULL == m_hShareMem )
	{// 打开共享内存失败
		//WLOGH_ERRO << L"open share memory failed,ErrorCode";
		return false;
	}
	if (m_pBuffer)
	{
		UnmapViewOfFile(m_pBuffer);
		m_pBuffer = NULL;
	}
	m_pBuffer = (char*) MapViewOfFile(m_hShareMem,FILE_MAP_ALL_ACCESS,0,0,0);
	if (!m_pBuffer)
	{
		//WLOGH_ERRO << L"map view of file failed,ErrorCode";
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
bool CShareMemory::ReadInt( int & v,bool bPeek )
{
	return Read( &v, sizeof(int),bPeek );
}

//----------------------------------------------------------------------------
bool CShareMemory::WriteInt( int v )
{
	return Write( &v, sizeof(int) );
}

bool CShareMemory::WriteFloat(float v)
{
	return Write(&v,sizeof(float));
}
//----------------------------------------------------------------------------
bool CShareMemory::WriteString( const std::string & v )
{
	int l = (int)v.length();
	if ( (l > (int)m_nRealBufferSize)||(l==0)/*MAX_STRING_LENGTH*/ )
	{
		//WLOGH_ERRO << L"WriteString error, Invalid length value = " << l;
	}
	if ( Write( &l, sizeof(int) ) )
	{
		if ( Write( (void*)(v.c_str()), l ) )
		{
			return true;
		}
	}
	return false;
}

/*
 * 从共享内存中读取指定长度的数据
 * param[in out] : szReadBuffer 读取的buffer
 * param[in] : nNumberOfBytesToRead 读取的buffer长度
 * param[in out] : lpNumberOfBytesToRead 已读取的buffer长度
 * return : 操作是否成功
 */
bool CShareMemory::Read( void * pReadBuffer, DWORD nNumberOfBytesToRead,bool bPeek )
{
	if (!pReadBuffer)
	{
		//WLOGH_ERRO << L"the ReadBuffer is NULL";
		return false;
	}

	if (!m_pBuffer)
	{
		//WLOGH_ERRO << L"the address of the sharedmemory is NULL";
		return false;
	}

// 	::WaitForSingleObject(m_hMetux,INFINITE);

	bool ret = false;
	unsigned int nReadOffset = *((unsigned int*)m_pBuffer);
	unsigned int nWriteOffset = *(((unsigned int*)m_pBuffer)+1);

	if ( nReadOffset != nWriteOffset )//认为不相等的时候就是非空
	{// buffer不为空
		int nCanReadBufferSize = 0;

		int nLast_Read_Size = m_nRealBufferSize - nReadOffset;//后半可读部分
		if ( nReadOffset > nWriteOffset )
		{//情况1
			nCanReadBufferSize = nLast_Read_Size + nWriteOffset;
		}
		else
		{//情况2
			nCanReadBufferSize = nWriteOffset - nReadOffset;
			nLast_Read_Size = nCanReadBufferSize;//修改后半可读部分的值
		}

		char * pRingBuffer = m_pBuffer+BUFFER_HEAD_SIZE;
		if ( nCanReadBufferSize >= nNumberOfBytesToRead )
		{
			if ( nLast_Read_Size >= nNumberOfBytesToRead )
			{//情况2 都会在这里进行操作
				memcpy( pReadBuffer, pRingBuffer + nReadOffset, nNumberOfBytesToRead );
			}
			else
			{//情况1 可能会在这里操作
				memcpy( pReadBuffer, pRingBuffer + nReadOffset, nLast_Read_Size );
				char* pReadBufferChar = (char*)pReadBuffer;
				memcpy( pReadBufferChar+nLast_Read_Size, pRingBuffer, nNumberOfBytesToRead -  nLast_Read_Size  );
			}
			if(!bPeek)
			{
				nReadOffset = ( nReadOffset + nNumberOfBytesToRead ) % m_nRealBufferSize;
				*((unsigned int*)m_pBuffer) = nReadOffset;//保存下次读的位置
			}
			ret = true;
		}
		else
		{
			//WLOGH_ERRO << L"the ring buffer doesn't have enough buffer to read";
		}
	}

// 	ReleaseMutex(m_hMetux);

	return ret;
}

/*
 * 向共享内存写入指定长度的数据
 * param[in] : szWriteBuffer 写入的buffer
 * param[in] : nNumberOfBytesToWrite 写入的buffer长度
 * param[in out] : lpNumberOfBytesWritten 已写入的buffer长度
 * return : 操作是否成功
 */
bool CShareMemory::Write( void * pWriteBuffer, DWORD nNumberOfBytesToWrite )
{
	if (!pWriteBuffer)
	{
		return false;
	}

	if (!m_pBuffer)
	{
		//WLOGH_ERRO << L"the address of the sharedmemory is NULL";
		return false;
	}

// 	::WaitForSingleObject(m_hMetux,INFINITE);

	bool ret = false;
	unsigned int nReadOffset = *((unsigned int*)m_pBuffer);
	unsigned int nWriteOffset = *(((unsigned int*)m_pBuffer)+1);

	unsigned int nFutureWriteOffset = nWriteOffset+nNumberOfBytesToWrite;//新的Write Offset

	if (((nWriteOffset < nReadOffset) && (nFutureWriteOffset >=nReadOffset))//如果读写相等，会引起读操作的时候认为是空的异常，所以不能相等。
		||((nWriteOffset > nReadOffset) 
			&& ((nFutureWriteOffset/m_nRealBufferSize >=2)//防止超过整个buffer的写入操作
				|| (((nFutureWriteOffset/m_nRealBufferSize) == 1 )&& ((nFutureWriteOffset%m_nRealBufferSize) >= nReadOffset)) )))
	{
		//WLOGH_ERRO << L"the ring buffer doesn't have enough buffer to write";
		return ret;
	}

	{// buffer可用
		int nCanWriteBufferSize = 0;
		int nLast_Write_Size = m_nRealBufferSize - nWriteOffset;//后半可写部分
		if ( nReadOffset > nWriteOffset )
		{//情况1
			nCanWriteBufferSize = nReadOffset - nWriteOffset;
			nLast_Write_Size = nCanWriteBufferSize;//修改后半可写部分
		}
		else
		{//情况2
			nCanWriteBufferSize = nLast_Write_Size + nReadOffset;
		}

		char * pRingBuffer = m_pBuffer+BUFFER_HEAD_SIZE;
		if ( nCanWriteBufferSize >= nNumberOfBytesToWrite )
		{
			if ( nLast_Write_Size >= nNumberOfBytesToWrite )
			{//情况1 都会在这里操作
				memcpy( pRingBuffer + nWriteOffset, pWriteBuffer, nNumberOfBytesToWrite );
			}
			else
			{//情况2 可能会在这里操作
				memcpy( pRingBuffer + nWriteOffset, pWriteBuffer, nLast_Write_Size );
				char* pWriteBufferChar = (char*)pWriteBuffer;
				memcpy( pRingBuffer, pWriteBufferChar+nLast_Write_Size, nNumberOfBytesToWrite - nLast_Write_Size );//modified by glp
			}
			nWriteOffset = ( nWriteOffset + nNumberOfBytesToWrite ) % m_nRealBufferSize;
			*((unsigned int*)m_pBuffer+1) = nWriteOffset;//保存下次写的位置
			ret = true;
		}
		else
		{
			//WLOGH_ERRO << L"the ring buffer is too small!";
		}
	}

// 	ReleaseMutex(m_hMetux);

	return ret;
}

#define ONEC_MAX_MEMORY 1048576
inline void CopyVideoMemory(void* dest,void* src,unsigned long size)
{
 
}

bool CShareMemory::readVideo(int type,void* pReadBuffer, DWORD len)
{
	if (!pReadBuffer)
		return false;

	if (!m_pBuffer)
	{
		//WLOGH_ERRO << L"the address of the sharedmemory is NULL";
		return false;
	}

// 	::WaitForSingleObject(m_hMetux,INFINITE);

	int nWriteFlag;
	if(type == 0)
	{
		nWriteFlag = *((int*)m_pBuffer);

		if(nWriteFlag == 0)
			memcpy(pReadBuffer,m_pBuffer+BUFFER_HEAD_SIZE,len);
		else
			memcpy(pReadBuffer,m_pBuffer+BUFFER_HEAD_SIZE+SEPERATOR_SIZE,len);
	}
	else
	{
		nWriteFlag = *((int*)m_pBuffer+1);
		static int nOffset = BUFFER_HEAD_SIZE+SEPERATOR_SIZE*2;
		if(nWriteFlag == 0)
			memcpy(pReadBuffer,m_pBuffer+nOffset,len);
		else
			memcpy(pReadBuffer,m_pBuffer+nOffset+SEPERATOR_SIZE,len);
	}
// 	ReleaseMutex(m_hMetux);

	return true;
}

bool CShareMemory::writeVideo(int type,void* pWriteBuffer,DWORD len)
{
	assert (SEPERATOR_SIZE >= len);
	if (!pWriteBuffer)
		return false;

	if (!m_pBuffer)
	{
		//WLOGH_ERRO << L"the address of the sharedmemory is NULL";
		return false;
	}

// 	::WaitForSingleObject(m_hMetux,INFINITE);

	int nWriteFlag;
	if (type == 0)
	{
		nWriteFlag = *((int*)m_pBuffer);
		if(nWriteFlag == 0)
		{
			memcpy(m_pBuffer+BUFFER_HEAD_SIZE+SEPERATOR_SIZE,pWriteBuffer,len);
			*((int*)m_pBuffer) = 1;
		}
		else
		{
			memcpy(m_pBuffer+BUFFER_HEAD_SIZE,pWriteBuffer,len);
			*((int*)m_pBuffer) = 0;
		}
	}
	else
	{
		nWriteFlag = *((int*)m_pBuffer+1);
		static int nOffset = BUFFER_HEAD_SIZE+SEPERATOR_SIZE*2;
		if(nWriteFlag == 0)
		{
			memcpy(m_pBuffer+nOffset+SEPERATOR_SIZE,pWriteBuffer,len);
			*((int*)m_pBuffer+1) = 1;
		}
		else
		{
			memcpy(m_pBuffer+nOffset,pWriteBuffer,len);
			*((int*)m_pBuffer+1) = 0;
		}
	}
	Sleep(5);

// 	ReleaseMutex(m_hMetux);

	return true;
}

