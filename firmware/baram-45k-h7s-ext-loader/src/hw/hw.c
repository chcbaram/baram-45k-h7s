#include "hw.h"




bool hwInit(void)
{  
  bool ret = true;

  bspInit();

  ledInit();

  ret = qspiInit(); 
  
  return ret;
}