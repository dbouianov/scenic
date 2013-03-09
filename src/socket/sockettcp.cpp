#include "sockettcp.h"

NAMESPACE_BEGIN

SocketTcp::SocketTcp()
{}

SocketTcp::~SocketTcp()
{
  Socket::Close(false);
}

bool SocketTcp::Listen(uint queueSize)
{
  if (0 != listen(m_socket, queueSize))
  {
    return false;
  }
  return true;
}

SocketTcp *SocketTcp::Accept()
{
  SocketTcp *pNewSocket = new SocketTcp();
  ulong size;

  size = sizeof(pNewSocket->m_remoteHostAddress);
  pNewSocket->m_socket = accept(m_socket,
    (sockaddr *)&pNewSocket->m_remoteHostAddress, (socklen_t *)&size);
  if (BAD_SOCKET_NUMBER == pNewSocket->m_socket)
  {
    delete pNewSocket;
    return NULL;
  }

  size = sizeof(pNewSocket->m_localHostAddress);
  if (0 != getsockname(pNewSocket->m_socket,
    (sockaddr *)&pNewSocket->m_localHostAddress, (socklen_t *)&size))
  {
    delete pNewSocket;
    return NULL;
  }

  return pNewSocket;
}

bool SocketTcp::Connect(cchar *szRemoteHostName, uint remotePortNumber)
{
  hostent *pHostEntity;

  pHostEntity = gethostbyname(szRemoteHostName);
  if (!pHostEntity)
  {
    return false;
  }

  m_remoteHostAddress.sin_family = AF_INET;
  m_remoteHostAddress.sin_port = htons(remotePortNumber);
  memcpy(&m_remoteHostAddress.sin_addr,
    pHostEntity->h_addr, pHostEntity->h_length);

  if (0 != connect(m_socket, (sockaddr *)&m_remoteHostAddress,
    sizeof(m_remoteHostAddress)))
  {
    return false;
  }

  //strncpy(m_szRemoteHostName, szRemoteHostName, MAX_HOST_NAME_SIZE);
  //m_szRemoteHostName[MAX_HOST_NAME_SIZE] = NULL_CHAR;
  //m_remotePortNumber = remotePortNumber;

  return true;
}

ulong SocketTcp::Send(void *pBuffer, ulong size)
{
  return send(m_socket, (char *)pBuffer, size, 0);
}

ulong SocketTcp::Receive(void *pBuffer, ulong size)
{
  return recv(m_socket, (char *)pBuffer, size, 0);
}

SocketType SocketTcp::CreateSocket()
{
  return socket(AF_INET, SOCK_STREAM, 0);
}

NAMESPACE_END
