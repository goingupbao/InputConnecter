#pragma once

namespace LocalConn
{
	struct _MSG
	{
		unsigned int	uLen;
		unsigned char*	pData;
	};

	class CMessage
	{
	public:
		CMessage(unsigned int nLen);
		CMessage(CMessage &copy);
		CMessage& operator=(CMessage &copy);
		~CMessage();
		unsigned char*	GetData();
		unsigned int	GetLen();
		void			SetLen(unsigned int len);
		unsigned int	AppendData(void *pData, unsigned int len);
		bool			CheckData();
		void			Clear();
	private:
		_MSG			m_msg;
	};
}