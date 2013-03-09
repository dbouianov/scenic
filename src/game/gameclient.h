#ifndef _GAMECLIENT_H_
#define _GAMECLIENT_H_

#include "socket.h"
#include "factory.h"

NAMESPACE_BEGIN

//game client
class GameClient
{
public:
  GameClient();
  virtual ~GameClient();

  bool Create(OUT Factory *pFactory);

private:
  //factory for object creation
  Factory *m_pFactory;
  //communication socket
  Socket *m_pSocket;
};

NAMESPACE_END

#endif //_GAMECLIENT_H_
