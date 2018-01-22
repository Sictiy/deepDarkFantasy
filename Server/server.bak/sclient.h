#ifndef SCLIENT_H_INCLUDED
#define SCLIENT_H_INCLUDED
//#include <winsock2.h>
#include <sys/socket.h>

#include <iostream>
using namespace std;


#define TIMEFOR_THREAD_CLIENT		500		

#define	MAX_NUM_CLIENT		10		
#define	MAX_NUM_BUF			64
#define INVALID_OPERATOR	1	
#define INVALID_NUM			2	
#define ZERO				0


typedef struct _head
{
	char			type;	
	unsigned short	len;	
}hdr, *phdr;

typedef struct _data
{
	char	buf[MAX_NUM_BUF];
}DATABUF, *pDataBuf;


class CClient
{
public:
	CClient(const SOCKET sClient,const sockaddr_in &addrClient);
	virtual ~CClient();

public:
	BOOL		StartRuning(void);		
	void		HandleData(const char* pExpr);	
	BOOL		IsConning(void){		
				return m_bConning;
				}
	void		DisConning(void){		
				m_bConning = FALSE;
				}
	BOOL		IsExit(void){			
				return m_bExit;
				}
    BOOL		IsSend(void){
				m_bSend = TRUE;
				}

public:
	static DWORD __stdcall	 RecvDataThread(void* pParam);	
	static DWORD __stdcall	 SendDataThread(void* pParam);	

private:
	CClient();
private:
	SOCKET		m_socket;	
	sockaddr_in	m_addr;		
	DATABUF		m_data;		
	HANDLE		m_hEvent;	
	HANDLE		m_hThreadSend;	
	HANDLE		m_hThreadRecv;	
	CRITICAL_SECTION m_cs;		
	BOOL		m_bConning;
	BOOL        m_bSend;            
	BOOL		m_bExit;	
};


#endif // SCLIENT_H_INCLUDED
