#ifndef _GAMESERVER_H_
#define _GAMESERVER_H_

#include "socket.h"
#include "factory.h"

NAMESPACE_BEGIN

//game server
class GameServer
{
public:
  GameServer();
  virtual ~GameServer();

  bool Create(OUT Factory *pFactory);

private:
  //factory for object creation
  Factory *m_pFactory;
  //communication socket
  Socket *m_pSocket;
};

NAMESPACE_END

#endif //_GAMESERVER_H_
