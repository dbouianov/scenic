#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "common.h"
#include "object.h"

#ifndef _WINDOWS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

NAMESPACE_BEGIN

//compatibility macros
#define DllExport
#define closesocket close

//socket type definition
typedef int SocketType;
#else
#include <winsock2.h>
#include <ws2tcpip.h>

//socket library is a dll
#define DllExport   __declspec(dllexport)

//socket type definition
typedef SOCKET SocketType;
#endif

#define MAX_HOST_NAME_SIZE        NI_MAXHOST
#define MAX_SERVICE_NAME_SIZE     NI_MAXSERV
#define MAX_NUM_CONNECTIONS       SOMAXCONN
#define AUTO_PORT_NUMBER          0
#define BAD_SOCKET_NUMBER         -1
#define BAD_PORT_NUMBER           0xffffffff

//generic socket
class DllExport Socket : public Object
{
public:
  virtual ~Socket();

  bool Open(uint localPortNumber);
  void Close();

  virtual bool Listen(uint queueSize) = 0;
  virtual Socket *Accept() = 0;

  virtual bool Connect(cchar *szRemoteHostName, uint remotePortNumber) = 0;

  virtual ulong Send   (void *pBuffer, ulong size) = 0;
  virtual ulong Receive(OUT void *pBuffer, ulong size) = 0;

  cchar *GetLocalHostName();
  uint GetLocalPortNumber();

  cchar *GetRemoteHostName();
  uint GetRemotePortNumber();

protected:
  //socket id
  SocketType m_socket;
  //local/remote addresses
  sockaddr_in m_localHostAddress;
  sockaddr_in m_remoteHostAddress;
  //local/remote host names
  char m_szLocalHostName  [MAX_HOST_NAME_SIZE + 1];
  char m_szRemoteHostName [MAX_HOST_NAME_SIZE + 1];
  //local/remote port numbers
  uint m_localPortNumber;
  uint m_remotePortNumber;

  //protectect constructor for use only by derived classes
  Socket();

  void Close(bool reset);

private:
  void Reset();
  void GetHostInfo(sockaddr_in *pHostAddress, char *szHostName,
                   ulong hostNameSize, OUT uint *pPortNumber);

  virtual SocketType CreateSocket() = 0;
};

NAMESPACE_END

//include tcp/udp sockets
#include "sockettcp.h"
#include "socketudp.h"

#endif //_SOCKET_H_
