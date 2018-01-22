#include "server.h"
#include "sclient.h"


char	dataBuf[MAX_NUM_BUF];				
BOOL	bConning;				
BOOL    bSend;                             
BOOL    clientConn;                         
SOCKET	sServer;			
CRITICAL_SECTION  cs;			       
HANDLE	hAcceptThread;			
HANDLE	hCleanThread;		
ClIENTVECTOR clientvector;     

BOOL initSever(void)
{
	initMember();

	if (!initSocket())
		return FALSE;

	return TRUE;
}

void	initMember(void)
{
	InitializeCriticalSection(&cs);				            
	memset(dataBuf, 0, MAX_NUM_BUF);
	bSend = FALSE;
	clientConn = FALSE;
	bConning = FALSE;			
	hAcceptThread = NULL;		
	hCleanThread = NULL;
	sServer = INVALID_SOCKET;	
	clientvector.clear();	
}

bool initSocket(void)
{
	int reVal;

	WSADATA  wsData;
	reVal = WSAStartup(MAKEWORD(2,2),&wsData);

	sServer = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET== sServer)
		return FALSE;

	unsigned long ul = 1;
	reVal = ioctlsocket(sServer, FIONBIO, (unsigned long*)&ul);
	if (SOCKET_ERROR == reVal)
		return FALSE;

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(SERVERPORT);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	reVal = bind(sServer, (struct sockaddr*)&serAddr, sizeof(serAddr));
	if(SOCKET_ERROR == reVal )
		return FALSE;

	reVal = listen(sServer, CONN_NUM);
	if(SOCKET_ERROR == reVal)
		return FALSE;

	return TRUE;
}

bool startService(void)
{
    BOOL reVal = TRUE;	

	showTipMsg(START_SERVER);	

	char cInput;		
	do
	{
		cin >> cInput;
		if ('s' == cInput || 'S' == cInput)
		{
			if (createCleanAndAcceptThread())	
			{
				showServerStartMsg(TRUE);	
			}else{
				reVal = FALSE;
			}
			break;
		}else{
			showTipMsg(START_SERVER);
		}

	} while(cInput != 's' && cInput != 'S'); 

    cin.sync();                    

	return reVal;

}

BOOL createCleanAndAcceptThread(void)
{
    bConning = TRUE;

	unsigned long ulThreadId;
	hAcceptThread = CreateThread(NULL, 0, acceptThread, NULL, 0, &ulThreadId);
	if( NULL == hAcceptThread)
	{
		bConning = FALSE;
		return FALSE;
	}
	else
    {
		CloseHandle(hAcceptThread);
	}
	hCleanThread = CreateThread(NULL, 0, cleanThread, NULL, 0, &ulThreadId);
	if( NULL == hCleanThread)
	{
		return FALSE;
	}
	else
    {
		CloseHandle(hCleanThread);
	}
	return TRUE;
}

DWORD __stdcall acceptThread(void* pParam)
{
    SOCKET  sAccept;	
	sockaddr_in addrClient;	

	while(bConning)	
	{
		memset(&addrClient, 0, sizeof(sockaddr_in));	
		int	lenClient = sizeof(sockaddr_in);
		sAccept = accept(sServer, (sockaddr*)&addrClient, &lenClient);
		if(INVALID_SOCKET == sAccept )
		{
			int nErrCode = WSAGetLastError();
			if(nErrCode == WSAEWOULDBLOCK)	                     
			{
				Sleep(TIMEFOR_THREAD_SLEEP);
				continue;                                   
			}
			else
            {
				return 0;                                  
			}

		}
		else
		{
		    clientConn = TRUE;
		    CClient *pClient = new CClient(sAccept, addrClient);
		    EnterCriticalSection(&cs);
            char *pClientIP = inet_ntoa(addrClient.sin_addr);
            u_short  clientPort = ntohs(addrClient.sin_port);
            cout<<"Accept a client IP: "<<pClientIP<<"\tPort: "<<clientPort<<endl;
			clientvector.push_back(pClient); 
            LeaveCriticalSection(&cs);

            pClient->StartRuning();
		}
	}
	return 0;
}

DWORD __stdcall cleanThread(void* pParam)
 {
    while (bConning) 
	{
		EnterCriticalSection(&cs);

		ClIENTVECTOR::iterator iter = clientvector.begin();
		for (iter; iter != clientvector.end();)
		{
			CClient *pClient = (CClient*)*iter;
			if (!pClient->IsConning())	
			{
				clientvector.erase(iter);
				delete pClient;		
				pClient = NULL;
			}else{
				iter++;		
			}
		}
		if(clientvector.size() == 0)
        {
            clientConn = FALSE;
        }

		LeaveCriticalSection(&cs);     

		Sleep(TIMEFOR_THREAD_HELP);
	}


	if (!bConning)
	{
		EnterCriticalSection(&cs);
		ClIENTVECTOR::iterator iter = clientvector.begin();
		for (iter; iter != clientvector.end();)
		{
			CClient *pClient = (CClient*)*iter;
			if (pClient->IsConning())
			{
				pClient->DisConning();
			}
			++iter;
		}
		LeaveCriticalSection(&cs);

		Sleep(TIMEFOR_THREAD_HELP);
	}

	clientvector.clear();	
	clientConn = FALSE;

	return 0;
 }

 void inputAndOutput(void)
 {
    char sendBuf[MAX_NUM_BUF];

    showTipMsg(INPUT_DATA);

    while(bConning)
    {
        memset(sendBuf, 0, MAX_NUM_BUF);
        cin.getline(sendBuf,MAX_NUM_BUF);
        handleData(sendBuf);
    }
 }


 void handleData(char* str)
 {
    CClient *sClient ;
    string recvsting;
    char cnum;
    int num;

    if(str != NULL)
    {
       if(!strncmp(WRITE, str, strlen(WRITE))) 
        {
            EnterCriticalSection(&cs);
            str += strlen(WRITE);
            cnum = *str++;
            num = cnum - '1';
            if(num<clientvector.size())
            {
                sClient = clientvector.at(num); 
                strcpy(dataBuf, str);
                sClient->IsSend();
                LeaveCriticalSection(&cs);
            }
            else                               
            {
                cout<<"The client isn't in scope!"<<endl;
                LeaveCriticalSection(&cs);
            }

        }
        else if(!strncmp(WRITE_ALL, str, strlen(WRITE_ALL)))
        {
            EnterCriticalSection(&cs);
            str += strlen(WRITE_ALL);
            strcpy(dataBuf, str);
            bSend = TRUE;
            LeaveCriticalSection(&cs);
        }
        else if('e'==str[0] || 'E'== str[0])  
        {
            bConning = FALSE;
            showServerExitMsg();
            Sleep(TIMEFOR_THREAD_EXIT);
            exitServer();
        }
        else
        {
            cout <<"Input error!!"<<endl;
        }

    }
 }

void  exitServer(void)
{
	closesocket(sServer);
	WSACleanup();	
}

void showTipMsg(int input)
{
    EnterCriticalSection(&cs);
	if (START_SERVER == input) 
	{
		cout << "**********************" << endl;
		cout << "* s(S): Start server *" << endl;
		cout << "* e(E): Exit  server *" << endl;
		cout << "**********************" << endl;
		cout << "Please input:" ;

	}
	else if(INPUT_DATA == input)
    {
        cout << "*******************************************" << endl;
        cout << "* please connect clients,then send data   *" << endl;
		cout << "* write+num+data:Send data to client-num  *" << endl;
		cout << "*   all+data:Send data to all clients     *" << endl;
		cout << "*          e(E): Exit  server             *" << endl;
		cout << "*******************************************" << endl;
		cout << "Please input:" <<endl;
    }
	 LeaveCriticalSection(&cs);
}

void  showServerStartMsg(BOOL bSuc)
{
	if (bSuc)
	{
		cout << "**********************" << endl;
		cout << "* Server succeeded!  *" << endl;
		cout << "**********************" << endl;
	}else{
		cout << "**********************" << endl;
		cout << "* Server failed   !  *" << endl;
		cout << "**********************" << endl;
	}

}

void  showServerExitMsg(void)
{

	cout << "**********************" << endl;
	cout << "* Server exit...     *" << endl;
	cout << "**********************" << endl;
}

