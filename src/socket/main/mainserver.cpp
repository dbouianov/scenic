#include "socket.h"

#define PORT_NUMBER     2002
#define BUFFER_SIZE     128

int main(int argc, char *argv[])
{
  SocketTcp socket;
  SocketTcp *pSocket;
  char pBuffer [BUFFER_SIZE + 1];

  if (!socket.Open(PORT_NUMBER))
  {
    return 0;
  }

  printf("Server started on %s:%d\n",
    socket.GetLocalHostName(), socket.GetLocalPortNumber());

  if (!socket.Listen(MAX_NUM_CONNECTIONS))
  {
    return 0;
  }

  pSocket = socket.Accept();
  if (!pSocket)
  {
    return 0;
  }

  if (BUFFER_SIZE != pSocket->Receive(pBuffer, BUFFER_SIZE))
  {
    return 0;
  }

  printf("Connected to client on %s:%d\n",
    pSocket->GetRemoteHostName(), pSocket->GetRemotePortNumber());

  printf("Data received:\t%s\n", pBuffer);

  sprintf(pBuffer, "hello client!");

  if (BUFFER_SIZE != pSocket->Send(pBuffer, BUFFER_SIZE))
  {
    return 0;
  }

  printf("Data sent:\t%s\n", pBuffer);

  pSocket->Close();
  delete pSocket;

  socket.Close();

  return 0;
}
