#include "socketudp.h"

NAMESPACE_BEGIN

SocketUdp::SocketUdp()
{}

SocketUdp::~SocketUdp()
{}

bool SocketUdp::Listen(uint)
{
  return true;
}

SocketUdp *SocketUdp::Accept()
{
  SocketUdp *pNewSocket = new SocketUdp();

  memcpy(&pNewSocket->m_localHostAddress, &m_localHostAddress,
    sizeof(m_localHostAddress));
  memcpy(&pNewSocket->m_remoteHostAddress, &m_remoteHostAddress,
    sizeof(m_remoteHostAddress));
  pNewSocket->m_socket = m_socket;

  return pNewSocket;
}

bool SocketUdp::Connect(cchar *szRemoteHostName, uint remotePortNumber)
{
  hostent *pHostEntity = gethostbyname(szRemoteHostName);
  if (!pHostEntity)
  {
    return false;
  }

  m_remoteHostAddress.sin_family = AF_INET;
  m_remoteHostAddress.sin_port = htons(remotePortNumber);
  memcpy(&m_remoteHostAddress.sin_addr,
    pHostEntity->h_addr, pHostEntity->h_length);

  //strncpy(m_szRemoteHostName, szRemoteHostName, MAX_HOST_NAME_SIZE);
  //m_szRemoteHostName[MAX_HOST_NAME_SIZE] = NULL_CHAR;
  //m_remotePortNumber = remotePortNumber;

  return true;
}

ulong SocketUdp::Send(void *pBuffer, ulong size)
{
  return sendto(m_socket, (char *)pBuffer, size, 0,
    (sockaddr *)&m_remoteHostAddress, sizeof(m_remoteHostAddress));
}

ulong SocketUdp::Receive(void *pBuffer, ulong size)
{
  ulong addressSize = sizeof(m_remoteHostAddress);
  return recvfrom(m_socket, (char *)pBuffer, size, 0,
    (sockaddr *)&m_remoteHostAddress, (socklen_t *)&addressSize);
}

SocketType SocketUdp::CreateSocket()
{
  return socket(AF_INET, SOCK_DGRAM, 0);
}

NAMESPACE_END
