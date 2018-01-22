#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED
#include <iostream>
//#include <winsock2.h>
#include <sys/socket.h>
//#include <winbase.h>
#include <vector>
#include <map>
#include <string>
//#include <process.h>
#include <pthread.h>
#include "sclient.h"

using namespace std;
#pragma comment(lib, "ws2_32.lib")			

#define SERVER_SETUP_FAIL			1	 
#define START_SERVER                1     
#define INPUT_DATA                  2     

#define SERVERPORT			6666
#define CONN_NUM            10              
#define TIMEFOR_THREAD_SLEEP		500	
#define TIMEFOR_THREAD_HELP			1500	
#define TIMEFOR_THREAD_EXIT			5000	
#define WRITE_ALL   "all"                  
#define WRITE       "write"                
#define READ        "read"                 
#define READ_ALL    "read all"             
typedef vector<CClient*> ClIENTVECTOR;
typedef vector<string> SVECTOR;         

extern char	dataBuf[MAX_NUM_BUF];
extern BOOL	bConning;	    
extern BOOL    bSend;                
extern BOOL    clientConn;           
extern SOCKET	sServer;	     
extern CRITICAL_SECTION  cs;		
extern HANDLE	hAcceptThread;		
extern HANDLE	hCleanThread;		

BOOL initSever(void);                   
void initMember(void);
bool initSocket(void);		
void exitServer(void);			
bool startService(void);		
void inputAndOutput(void);              
void showServerStartMsg(BOOL bSuc);     
void showServerExitMsg(void);           
void handleData(char* str);             
void showTipMsg(BOOL bFirstInput);      
BOOL createCleanAndAcceptThread(void);      
DWORD __stdcall acceptThread(void* pParam); 
DWORD __stdcall cleanThread(void* pParam);


#endif // SERVER_H_INCLUDED
