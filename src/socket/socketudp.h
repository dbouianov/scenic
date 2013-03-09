#ifndef _SOCKETUDP_H_
#define _SOCKETUDP_H_

#include "socket.h"

NAMESPACE_BEGIN

class DllExport SocketUdp : public Socket
{
public:
  SocketUdp();
  virtual ~SocketUdp();

  virtual bool Listen(uint queueSize);
  virtual SocketUdp *Accept();

  virtual bool Connect(cchar *szRemoteHostName, uint remotePortNumber);

  virtual ulong Send   (void *pBuffer, ulong size);
  virtual ulong Receive(OUT void *pBuffer, ulong size);

private:
  virtual SocketType CreateSocket();
};

NAMESPACE_END

#endif //_SOCKETUDP_H_
