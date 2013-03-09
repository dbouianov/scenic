#ifndef _SOCKETTCP_H_
#define _SOCKETTCP_H_

#include "socket.h"

NAMESPACE_BEGIN

class DllExport SocketTcp : public Socket
{
public:
  SocketTcp();
  virtual ~SocketTcp();

  virtual bool Listen(uint queueSize);
  virtual SocketTcp *Accept();

  virtual bool Connect(cchar *szRemoteHostName, uint remotePortNumber);

  virtual ulong Send   (void *pBuffer, ulong size);
  virtual ulong Receive(OUT void *pBuffer, ulong size);

private:
  virtual SocketType CreateSocket();
};

NAMESPACE_END

#endif //_SOCKETTCP_H_
