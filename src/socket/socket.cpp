#include "socket.h"

NAMESPACE_BEGIN

Socket::Socket()
{
  Reset();
}

Socket::~Socket()
{}

bool Socket::Open(uint localPortNumber)
{
  hostent *pHostEntity;

  m_socket = CreateSocket();
  if (BAD_SOCKET_NUMBER == m_socket)
  {
    return false;
  }

  if (0 != gethostname(m_szLocalHostName, MAX_HOST_NAME_SIZE))
  {
    return false;
  }

  pHostEntity = gethostbyname(m_szLocalHostName);
  if (!pHostEntity)
  {
    return false;
  }

  m_localHostAddress.sin_family = AF_INET;
  m_localHostAddress.sin_port = htons(localPortNumber);
  MemCopy(&m_localHostAddress.sin_addr, pHostEntity->h_addr, pHostEntity->h_length);

  if (0 != bind(m_socket, (sockaddr *)&m_localHostAddress, sizeof(m_localHostAddress)))
    
  {
    return false;
  }

  m_localPortNumber = localPortNumber;

  return true;
}

void Socket::Close()
{
  Close(true);
}

cchar *Socket::GetLocalHostName()
{
  if (BAD_PORT_NUMBER == m_localPortNumber)
  {
    GetHostInfo(&m_localHostAddress, m_szLocalHostName,
      MAX_HOST_NAME_SIZE, &m_localPortNumber);
  }
  return m_szLocalHostName;
}

uint Socket::GetLocalPortNumber()
{
  if (BAD_PORT_NUMBER == m_localPortNumber)
  {
    GetHostInfo(&m_localHostAddress, m_szLocalHostName,
      MAX_HOST_NAME_SIZE, &m_localPortNumber);
  }
  return m_localPortNumber;
}

cchar *Socket::GetRemoteHostName()
{
  if (BAD_PORT_NUMBER == m_remotePortNumber)
  {
    GetHostInfo(&m_remoteHostAddress, m_szRemoteHostName,
      MAX_HOST_NAME_SIZE, &m_remotePortNumber);
  }
  return m_szRemoteHostName;
}

uint Socket::GetRemotePortNumber()
{
  if (BAD_PORT_NUMBER == m_remotePortNumber)
  {
    GetHostInfo(&m_remoteHostAddress, m_szRemoteHostName,
      MAX_HOST_NAME_SIZE, &m_remotePortNumber);
  }
  return m_remotePortNumber;
}

void Socket::Close(bool reset)
{
  closesocket(m_socket);
  if (reset)
  {
    Reset();
  }
}

void Socket::Reset()
{
  //clear socket data
  MemZero(&m_localHostAddress,  sizeof(m_localHostAddress));
  MemZero(&m_remoteHostAddress, sizeof(m_remoteHostAddress));
  m_socket = BAD_SOCKET_NUMBER;
  strcpy(m_szLocalHostName,  EMPTY_STRING);
  strcpy(m_szRemoteHostName, EMPTY_STRING);
  m_localPortNumber  = BAD_PORT_NUMBER;
  m_remotePortNumber = BAD_PORT_NUMBER;
}

void Socket::GetHostInfo(sockaddr_in *pHostAddress, char *szHostName,
                         ulong hostNameSize, uint *pPortNumber)
{
  char szServiceName[MAX_SERVICE_NAME_SIZE + 1];

  if (0 == getnameinfo((sockaddr *)pHostAddress,
    sizeof(sockaddr_in), szHostName, hostNameSize,
    szServiceName, MAX_SERVICE_NAME_SIZE, NI_NUMERICSERV))
  {
    *pPortNumber = atoi(szServiceName);
  }
}

NAMESPACE_END
