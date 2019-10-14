#pragma once
#include "Message.h"
#include "normal.h"
namespace LocalConn
{
	class IConnectListener
	{
	public:
		virtual void OnConnect(bool bConnected) = 0;
		virtual void OnDisConnect() = 0;
		virtual void OnRetry(int count) = 0;
		virtual void OnReceive(CMessage msg) = 0;
	};

	struct _CONN
	{
		unsigned int	_uConnID;
		unsigned int	_uConnHandle;
		sockaddr		_socAddr;
	};
	class CConnect
	{
	public:
		CConnect(IConnectListener *pListener);
		~CConnect();
		void Init(_CONN &conn);
		void DisConnect();
		void Send(unsigned char* pData, unsigned int ulen);
		
		void ReceiveThread();
	private:
		IConnectListener *	m_pListener;
		_CONN				m_conn;
	};
}

