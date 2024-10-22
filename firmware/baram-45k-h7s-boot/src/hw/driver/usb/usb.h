#ifndef USB_H_
#define USB_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"
#include "tusb.h"


bool usbInit(void);
void usbDeInit(void);
void usbUpdate(void);


#ifdef __cplusplus
}
#endif

#endif