#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_uvc.h"
#include "usbd_audio_if.h"

USBD_HandleTypeDef hUsbDeviceFS;

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void) {
	/* Init Device Library, add supported class and start the library. */
	if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK) {
		Error_Handler();
	}
	if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_VIDEO) != USBD_OK) {
		Error_Handler();
	}
	/*if (USBD_AUDIO_RegisterInterface(&hUsbDeviceFS, &USBD_VIDEO_fops_FS) != USBD_OK) {
		Error_Handler();
	}*/
	if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
		Error_Handler();
	}
}
