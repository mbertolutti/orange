1 WinMain; MessageBox; windows.h; user32.lib; CRT;  
2 MainWndProc; WNDCLASS; 'MessageLoop'; PAINTSTRUCT; HDC; PatBlt;  
3 CreateDIBSection; StretchDIBits; BITMAPINFO;  
4 StrechDIBits - pointer to the bits; BitBlt ~ dc to dc;  
  Allignment;  
  HeapAlloc: Like malloc, but specific Windows;  
  VirtualAllos: We use it here; HeapAlloc is an alternative;  
  VirtualAlloc returns pages;  
  VirtualFree with MEM_RELEASE because we still want to use it. Otherwise we could use MEM_DECOMMIT;  
   We could change VirtualProtect from PAGE_READWRITE to PAGE_NOACCESS, which means  
   no execution, no writing, no reading, so nobody has a stale pointer to the memory.  
   We can catch 'read after free bug', when someone tries to touch the address.  
  MEM_COMMIT: asking for memory, OS needs to track it because we use it directly;  
  MEM_RESERVE: asking for memory, but OS does not need to track yet, because we don't use it yet;  
  PAGE_READWRITE;  
  Switch GetMessage loop to PeekMessageLoop;  
  PeekMessageLoop allows us to also keep on running when there are no more messages left;  
  PeekMessage PM_REMOVE: removes message after peak;  
5 Refactor; Page size; pointer aliasing; volatile; pixel memory order; const;  
  r62;  
 