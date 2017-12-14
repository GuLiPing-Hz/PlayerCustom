/*
	注释时间:2014-4-25
	author: glp
	共享内存的读写类
*/
#ifndef __SHAREMEMORY_H__
#define __SHAREMEMORY_H__

#ifdef BUILD_PLAYER
#include "../Opengl.h"
#endif
#include <string>

const int BUFFER_HEAD_SIZE = 8;//预留8个字节，前面4个字节保存读的偏移，后面4个字节保存写的偏移
//int MAX_STRING_LENGTH /*= 1048576*/;

class CSafePointer
{
public:
	CSafePointer(void* p=NULL):m_p(p){}
	~CSafePointer(){if(m_p)delete m_p;}

	operator void*()const{return m_p;}

private:
	void* m_p;
};

class CShareMemory
{
public:
	CShareMemory();
	~CShareMemory();

public:
	// 创建共享内存
	bool Create( const char * szName);

	// 打开共享内存
	bool Open( const char * szName );

	//这里读取Int的时候最好是Peek一下，int放在结构了，使用ReadStruct读取数据
	bool ReadInt( int & v,bool bPeek=true );
	//这里使用Int只适合写入一个单独的命令，不带其他参数
	bool WriteInt( int v );
private:
	bool WriteFloat(float v); 

	bool WriteString( const std::string & v );

	// 从共享内存中读取指定长度的数据
	bool Read( void * pReadBuffer, DWORD nNumberOfBytesToRead,bool bPeek=false );

	// 向共享内存写入指定长度的数据
	bool Write( void * pWriteBuffer, DWORD nNumberOfBytesToWrite );
public:
	//使用这个函读取数据，
	template<typename T>
	bool ReadStruct( T *pData ) { return Read(pData,sizeof(T)); }

	//只能使用这个函数写入数据
	template<typename T>
	bool WriteStruct( T *pData ) { return Write(pData,sizeof(T)); }
	
	//视频数据读取
	bool readVideo(int type,void* pReadBuffer, DWORD len);
	//视频数据写入
	bool writeVideo(int type,void* pWriteBuffer,DWORD len);
private:
	//指向共享内存的指针
	char*			m_pBuffer;
	HANDLE		m_hShareMem;
	DWORD		m_nBufferSize;
	DWORD		m_nRealBufferSize;

	HANDLE		m_hMetux;//读写全局锁，一般由Owner 释放
	bool				m_isOwner;//读写全局锁，是否是自己的。
};

#endif	//__SHAREMEMORY_H__