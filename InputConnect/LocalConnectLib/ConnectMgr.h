#pragma once
#include "Connect.h"
#include "normal.h"
namespace LocalConn
{
	class ILocalConnNotify
	{
	public:

	};

	class ConnectMgr :public IConnectListener
	{
	private:
		ConnectMgr();
		~ConnectMgr();
	public:
		ConnectMgr*GetInstance()
		{
			static ConnectMgr conn;
			return &conn;
		}

		void Init();
		void UnInit();
		int ListenAt(int nPort);
		void ConnectTo(char*IP, int nPort);

		void OnGetNewConnectAsServer(SOCKET soc ,sockaddr saddr);

		void ListenerThread(SOCKET soc);
		void ConnectThread(char*IP, int nPort);
	public: ////////IConnectListener
		virtual void OnConnect(bool bConnected);
		virtual void OnDisConnect();
		virtual void OnRetry(int count);
		virtual void OnReceive(CMessage msg);
	////////IConnectListener

	private:
		std::map<ConnID, CConnect*> m_mapConnects;
		static int					m_nIDCount;
		CRITICAL_SECTION			m_mapCS;
	};

	int ConnectMgr::m_nIDCount = 1000;
}