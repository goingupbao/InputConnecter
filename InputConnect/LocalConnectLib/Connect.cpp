#include "stdafx.h"
#include "Connect.h"

namespace LocalConn
{

	CConnect::CConnect(IConnectListener *plistener)
	{
	}


	CConnect::~CConnect()
	{
	}
	void CConnect::Init(_CONN &conn)
	{
		m_conn = conn;

	}
	void CConnect::DisConnect()
	{

	}
	void CConnect::Send(unsigned char* pData, unsigned int ulen)
	{

	}
}

