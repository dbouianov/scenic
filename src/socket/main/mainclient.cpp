#include "socket.h"

#define PORT_NUMBER     2002
#define BUFFER_SIZE     128

int main(int argc, char *argv[])
{
  SocketTcp socket;
  char pBuffer [BUFFER_SIZE + 1];

  if (!socket.Open(AUTO_PORT_NUMBER))
  {
    return 0;
  }

  printf("Client started on %s:%d\n",
    socket.GetLocalHostName(), socket.GetLocalPortNumber());

  if (!socket.Connect(socket.GetLocalHostName(), PORT_NUMBER))
  {
    return 0;
  }

  printf("Connected to server on %s:%d\n",
  socket.GetRemoteHostName(), socket.GetRemotePortNumber());

  sprintf(pBuffer, "hello server!");

  if (BUFFER_SIZE != socket.Send(pBuffer, BUFFER_SIZE))
  {
    return 0;
  }

  printf("Data sent:\t%s\n", pBuffer);

  if (BUFFER_SIZE != socket.Receive(pBuffer, BUFFER_SIZE))
  {
    return 0;
  }

  printf("Data received:\t%s\n", pBuffer);

  socket.Close();

  return 0;
}
