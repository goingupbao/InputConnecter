#include "stdafx.h"
#include "ConnectMgr.h"

namespace LocalConn
{
	ConnectMgr::ConnectMgr()
	{
		InitializeCriticalSection(&m_mapCS);
	}
	ConnectMgr::~ConnectMgr()
	{
		DeleteCriticalSection(&m_mapCS);
	}
	void ConnectMgr::Init()
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	void ConnectMgr::UnInit()
	{
		WSACleanup();
	}
	int ConnectMgr::ListenAt(int nPort)
	{
		SOCKET soc = socket(AF_INET, SOCK_STREAM, 0);
		if (soc == -1)
		{
			return -1;
		}
		sockaddr_in my_addr;
		my_addr.sin_family = AF_INET; /* host byte order */
		my_addr.sin_port = htons(nPort); /* short, network byte order */
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(soc, (struct sockaddr *)&my_addr, sizeof(struct sockaddr));
		auto func = std::bind(&ConnectMgr::ListenerThread, this, std::placeholders::_1);

		auto thread = std::thread(func, soc);
		thread.detach();
		thread.join();

		return 0;
	}
	void ConnectMgr::ConnectTo(char * IP, int nPort)
	{

	}
	void ConnectMgr::OnGetNewConnectAsServer(SOCKET soc, sockaddr saddr)
	{
		_CONN conn;
		ZeroMemory(&conn, sizeof(_CONN));
		conn._uConnID = m_nIDCount++;
		conn._uConnHandle = soc;
		conn._socAddr = saddr;
		CConnect * pNewConnect = new CConnect(this);
		pNewConnect->Init(conn);

		EnterCriticalSection(&m_mapCS);
		m_mapConnects[conn._uConnID] = pNewConnect;
		LeaveCriticalSection(&m_mapCS);
	}
	void ConnectMgr::ListenerThread(SOCKET soc)
	{
		while (listen(soc, 10))
		{
			sockaddr their_addr; /* connector's address information */
			int sin_size = sizeof(struct sockaddr);
			SOCKET new_fd = accept(soc, &their_addr, &sin_size);

			if (new_fd!=SOCKET_ERROR)
			{
				this->OnGetNewConnectAsServer(new_fd, their_addr);
			}
		}
	}
	void ConnectMgr::ConnectThread(char * IP, int nPort)
	{

	}
	void ConnectMgr::OnConnect(bool bConnected)
	{

	}
	void ConnectMgr::OnDisConnect()
	{

	}
	void ConnectMgr::OnRetry(int count)
	{

	}
	void ConnectMgr::OnReceive(CMessage msg)
	{

	}
}