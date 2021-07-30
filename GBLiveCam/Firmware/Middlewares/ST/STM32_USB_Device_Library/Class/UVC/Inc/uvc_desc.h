#ifndef __UVC_DESC_H
#define __UVC_DESC_H

#include "uvc.h"
#include "usbd_uvc.h"

extern VideoControl videoCommitControl;
extern VideoControl videoProbeControl;

extern const uint8_t USBD_VIDEO_CfgDesc[USB_VIDEO_DESC_SIZ];
extern const uint8_t USBD_VIDEO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC];

#endif  /* __UVC_DESC_H */
