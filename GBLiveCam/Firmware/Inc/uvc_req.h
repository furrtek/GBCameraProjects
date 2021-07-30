#ifndef __UVC_REQ_H
#define __UVC_REQ_H

#include "uvc.h"
#include "usbd_def.h"

typedef struct {
	uint8_t data[USB_MAX_EP0_SIZE];
	uint8_t len;
	uint8_t unit;	// Interface
	uint8_t cs;		// Control
} UVCClassData_t;

void UVC_REQ_CT(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
void UVC_REQ_PU(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
void UVC_REQ_STREAM(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

#endif /* __UVC_REQ_H */
