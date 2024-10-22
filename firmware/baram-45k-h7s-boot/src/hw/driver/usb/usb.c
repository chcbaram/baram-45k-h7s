#include "usb.h"

#include "tusb.h"
#include "usb_desc.h"


static void usbInitPhy(void);




bool usbInit(void)
{
  usbInitPhy();

  tusb_init();

  return true;
}

void usbDeInit(void)
{
  __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
  __HAL_RCC_USBPHYC_CLK_DISABLE();

  /* USB_OTG_HS interrupt Deinit */
  HAL_NVIC_DisableIRQ(OTG_HS_IRQn);  
}

void usbUpdate(void)
{
  tud_task();
}

void tud_mount_cb(void)
{
  logPrintf("tud_mount_cb()\n");
}

void tud_umount_cb(void)
{
  logPrintf("tud_umount_cb()\n");
}

void usbInitPhy(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};


  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USBPHYC;
  PeriphClkInit.UsbPhycClockSelection = RCC_USBPHYCCLKSOURCE_HSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_PWREx_EnableUSBVoltageDetector();

  /* USB_OTG_HS clock enable */
  __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
  __HAL_RCC_USBPHYC_CLK_ENABLE();

  /*Configuring the SYSCFG registers OTG_HS PHY*/
  /*OTG_HS PHY enable*/
  // HAL_SYSCFG_EnableOTGPHY(SYSCFG_OTG_HS_PHY_ENABLE);

  // Disable VBUS sense (B device)
  USB_OTG_HS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

  // B-peripheral session valid override enable
  USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBVALEXTOEN;
  USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBVALOVAL;  
}

void OTG_HS_IRQHandler(void)
{
  tud_int_handler(BOARD_TUD_RHPORT);
}