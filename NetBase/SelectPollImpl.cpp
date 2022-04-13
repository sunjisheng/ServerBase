#include "CommonBase.h"
#include "SelectPollImpl.h"
namespace Minicat
{
#ifdef _WINDOWS
	SelectPollImpl::SelectPollImpl() : m_fdMax(INVALID_SOCKET)
	{
		m_timeout.tv_sec = 0;
		m_timeout.tv_usec = 1000000;  //1s
		for (int i = 0; i < FDSet_Count; i++)
		{
			FD_ZERO(&m_ReadSet[i]);
			FD_ZERO(&m_WriteSet[i]);
			FD_ZERO(&m_ExceptSet[i]);
		}
	}

	SelectPollImpl::~SelectPollImpl()
	{

	}

	void SelectPollImpl::AddSocket(SocketID fd, int nMask)
	{
		if (nMask & Socket_Read)
		{
			FD_SET(fd, &m_ReadSet[FDSet_Bak]);
		}

		if (nMask & Socket_Write)
		{
			FD_SET(fd, &m_WriteSet[FDSet_Bak]);
		}
		FD_SET(fd, &m_ExceptSet[FDSet_Bak]);
	}

	void SelectPollImpl::DelSocket(SocketID fd, int nMask)
	{
		if (nMask & Socket_Read)
		{
			FD_CLR(fd, &m_ReadSet[FDSet_Bak]);
		}
		if (nMask & Socket_Write)
		{
			FD_CLR(fd, &m_WriteSet[FDSet_Bak]);
		}
		FD_CLR(fd, &m_ExceptSet[FDSet_Bak]);
	}

	void SelectPollImpl::ModSocket(SocketID fd, int nMask)
	{
		if (nMask & Socket_Read)
		{
			FD_SET(fd, &m_ReadSet[FDSet_Bak]);
		}
		else
		{
			FD_CLR(fd, &m_ReadSet[FDSet_Bak]);
		}

		if (nMask & Socket_Write)
		{
			FD_SET(fd, &m_WriteSet[FDSet_Bak]);
		}
		else
		{
			FD_CLR(fd, &m_WriteSet[FDSet_Bak]);
		}
	}

	void SelectPollImpl::Poll()
	{
		m_ReadSet[FDSet_Cur] = m_ReadSet[FDSet_Bak];
		m_WriteSet[FDSet_Cur] = m_WriteSet[FDSet_Bak];
		m_ExceptSet[FDSet_Cur] = m_ExceptSet[FDSet_Bak];
		int nRet = select(m_fdMax + 1, &m_ReadSet[FDSet_Cur], &m_WriteSet[FDSet_Cur], &m_ExceptSet[FDSet_Cur], &m_timeout);
		if (nRet <= 0) //error or timeout
		{
			return;
		}
		else
		{
			//Windows下采用遍历的方式
			if (m_ReadHandler)
			{
				for (uint i = 0; i < m_ReadSet[FDSet_Cur].fd_count; i++)
				{
					(*m_ReadHandler)(m_ReadSet[FDSet_Cur].fd_array[i]);
				}

				for (uint i = 0; i < m_ExceptSet[FDSet_Cur].fd_count; i++)
				{
					(*m_ReadHandler)(m_ExceptSet[FDSet_Cur].fd_array[i]);
				}
			}
			if (m_WriteHandler)
			{
				for (int i = 0; i < m_WriteSet[FDSet_Cur].fd_count; i++)
				{
					(*m_WriteHandler)(m_WriteSet[FDSet_Cur].fd_array[i]);
				}
			}
		}
	}
#endif
}