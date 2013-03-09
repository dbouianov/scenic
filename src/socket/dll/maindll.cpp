#include "socket.h"

BOOL InitDll();
void FreeDll();

BOOL WINAPI DllMain(HINSTANCE hDllInstance,
                    DWORD reason, LPVOID reserved)
{
  switch (dwReason)
  {
  case DLL_PROCESS_ATTACH:
    return InitDll();

  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;

  case DLL_PROCESS_DETACH:
    FreeDll();
    break;
  }
  return TRUE;
}

BOOL InitDll()
{
  WORD version = MAKEWORD(2, 0);
  WSADATA wsaData;
  if (0 != WSAStartup(version, &wsaData))
  {
    WSACleanup();
    return FALSE;
  }
  return TRUE;
}

void FreeDll()
{
  WSACleanup();
}
