cmake_minimum_required(VERSION 3.13)
set(USB_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/hw/driver/usb/tinyusb")


# 지정한 폴더에 있는 파일만 포함한다.
#
file(GLOB USB_SRC_FILES CONFIGURE_DEPENDS
  # common
  ${USB_ROOT_PATH}/tusb.c
  ${USB_ROOT_PATH}/common/tusb_fifo.c
  # device
  ${USB_ROOT_PATH}/device/usbd.c
  ${USB_ROOT_PATH}/device/usbd_control.c
  ${USB_ROOT_PATH}/class/audio/audio_device.c
  ${USB_ROOT_PATH}/class/cdc/cdc_device.c
  ${USB_ROOT_PATH}/class/dfu/dfu_device.c
  ${USB_ROOT_PATH}/class/dfu/dfu_rt_device.c
  ${USB_ROOT_PATH}/class/hid/hid_device.c
  ${USB_ROOT_PATH}/class/midi/midi_device.c
  ${USB_ROOT_PATH}/class/msc/msc_device.c
  ${USB_ROOT_PATH}/class/net/ecm_rndis_device.c
  ${USB_ROOT_PATH}/class/net/ncm_device.c
  ${USB_ROOT_PATH}/class/usbtmc/usbtmc_device.c
  ${USB_ROOT_PATH}/class/vendor/vendor_device.c
  ${USB_ROOT_PATH}/class/video/video_device.c

  ${USB_ROOT_PATH}/portable/synopsys/dwc2/dcd_dwc2.c
)

# 하위폴더에 있는 파일까지 포함한다.
#
# file(GLOB_RECURSE SRC_FILES_RECURSE CONFIGURE_DEPENDS
# )


set (USB_INC_DIR 
  ${USB_ROOT_PATH}
  ${CMAKE_CURRENT_LIST_DIR}
)


