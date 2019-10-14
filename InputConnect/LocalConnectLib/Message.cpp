#include "stdafx.h"
#include "Message.h"
#include "atlalloc.h"
namespace LocalConn
{
	CMessage::CMessage(unsigned int nLen)
	{
		m_msg.pData = 0;
		SetLen(nLen);
	}
	CMessage::CMessage(CMessage &copy)
	{
		*this = copy;
	}
	CMessage& CMessage::operator=(CMessage &copy)
	{
		Clear();
		if (!copy.GetLen() == 0)
		{
			SetLen(copy.GetLen());
			AppendData(copy.GetData(), copy.GetLen());
		}

		return *this;
	}
	CMessage::~CMessage()
	{
	}
	unsigned char*	CMessage::GetData()
	{
		return m_msg.pData;
	}
	unsigned int	CMessage::GetLen()
	{
		return m_msg.uLen;
	}
	void			CMessage::SetLen(unsigned int len)
	{
		if (m_msg.uLen != len)
		{
			Clear();
			m_msg.pData = new unsigned char[len];
		}
	}
	unsigned int	CMessage::AppendData(void *pData, unsigned int len)
	{
		SetLen(len);
		memcpy(pData, m_msg.pData, len);
		return len;
	}
	bool			CMessage::CheckData()
	{
		return true;
	}
	void			CMessage::Clear()
	{
		if (m_msg.pData)
		{
			delete[]m_msg.pData;
		}
		m_msg.uLen = 0;
		m_msg.pData = 0;
	}
}