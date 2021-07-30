#include "usbd_uvc.h"
#include "uvc.h"
#include "uvc_req.h"
#include "uvc_desc.h"
#include "usbd_ctlreq.h"
#include "palettes.h"

static uint8_t USBD_VIDEO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_VIDEO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_VIDEO_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t* USBD_VIDEO_GetCfgDesc(uint16_t *length);
static uint8_t* USBD_VIDEO_GetDeviceQualifierDesc(uint16_t *length);
static uint8_t USBD_VIDEO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_VIDEO_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_VIDEO_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_VIDEO_EP0_TxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_VIDEO_SOF(USBD_HandleTypeDef *pdev);
static uint8_t USBD_VIDEO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_VIDEO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
//static void VIDEO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
//static void VIDEO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint32_t usbd_video_AltSet = 0;	// Current interface alternative setting

extern uint8_t linear_buffer_a[128*112/4];
extern uint8_t linear_buffer_b[128*112/4];
extern uint32_t buffer_flip;
extern UVCClassData_t UVCClassData;

USBD_ClassTypeDef USBD_VIDEO = {
	USBD_VIDEO_Init,
	USBD_VIDEO_DeInit,
	USBD_VIDEO_Setup,
	USBD_VIDEO_EP0_TxReady,	// Unused
	USBD_VIDEO_EP0_RxReady,
	USBD_VIDEO_DataIn,
	USBD_VIDEO_DataOut,
	USBD_VIDEO_SOF,
	USBD_VIDEO_IsoINIncomplete,		// Unused
	USBD_VIDEO_IsoOutIncomplete,	// Unused
	USBD_VIDEO_GetCfgDesc,
	USBD_VIDEO_GetCfgDesc,
	USBD_VIDEO_GetCfgDesc,
	USBD_VIDEO_GetDeviceQualifierDesc,
};

static uint8_t USBD_VIDEO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	// Open EP IN
	USBD_LL_OpenEP(pdev, USB_ENDPOINT_IN(1), USBD_EP_TYPE_ISOC, VIDEO_PACKET_SIZE);
	// Attach pClassData struct
	pdev->pClassData = (void*)&UVCClassData;
	return USBD_OK;
}

static uint8_t USBD_VIDEO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	USBD_LL_CloseEP(pdev, USB_ENDPOINT_IN(1));
	return USBD_OK;
}

static uint8_t USBD_VIDEO_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	uint16_t len;
	uint8_t *pbuf;
	uint16_t status_info = 0U;
	uint8_t ret = USBD_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
		case USB_REQ_TYPE_CLASS:
			if ((req->wIndex & 0xFF) == 0) {
				// Control interface
				switch (req->wIndex >> 8) {
					case 1:
						// Camera Terminal
						UVC_REQ_CT(pdev, req);
						break;

					case 2:
						// Processing Unit
						UVC_REQ_PU(pdev, req);
						break;

					default:
						USBD_CtlError(pdev, req);
						ret = USBD_FAIL;
						break;
				}
			} else if ((req->wIndex & 0xFF) == 1) {
				// Streaming interface
				UVC_REQ_STREAM(pdev, req);
			} else {
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest) {
				case USB_REQ_GET_STATUS:
					if (pdev->dev_state == USBD_STATE_CONFIGURED) {
						USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
					} else {
						USBD_CtlError(pdev, req);
						ret = USBD_FAIL;
					}
					break;

				case USB_REQ_GET_DESCRIPTOR:
					if( (req->wValue >> 8) == CS_DEVICE) {
						pbuf = (uint8_t*)USBD_VIDEO_CfgDesc + 26;
						len = MIN(USB_VIDEO_DESC_SIZ, req->wLength);

						USBD_CtlSendData(pdev, pbuf, len);
					}
					break;

				case USB_REQ_GET_INTERFACE :
					if (pdev->dev_state == USBD_STATE_CONFIGURED) {
						USBD_CtlSendData(pdev, (uint8_t*)&usbd_video_AltSet, 1);
					} else {
						USBD_CtlError (pdev, req);
						ret = USBD_FAIL;
					}
					break;

				case USB_REQ_SET_INTERFACE :
					if (pdev->dev_state == USBD_STATE_CONFIGURED) {
						if ((uint8_t)(req->wValue) < VIDEO_TOTAL_IF_NUM) {
							usbd_video_AltSet = (uint8_t)(req->wValue);

							if (usbd_video_AltSet == 1) {
								cam_status = STATUS_READY;

								LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_14);	// Green LED
								LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
							} else {
								USBD_LL_FlushEP(pdev, USB_ENDPOINT_IN(1));
								cam_status = STATUS_STOPPED;

								LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_14);	// Red LED
								LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
							}
						} else {
							USBD_CtlError (pdev, req);
							ret = USBD_FAIL;
						}
					} else {
						USBD_CtlError (pdev, req);
						ret = USBD_FAIL;
					}
					break;

				default:
					USBD_CtlError (pdev, req);
					ret = USBD_FAIL;
					break;
			}
			break;
		default:
			USBD_CtlError (pdev, req);
			ret = USBD_FAIL;
			break;
	}

	return ret;
}

static uint8_t *USBD_VIDEO_GetCfgDesc(uint16_t *length) {
	*length = sizeof(USBD_VIDEO_CfgDesc);
	return (uint8_t*)USBD_VIDEO_CfgDesc;
}

static uint8_t USBD_VIDEO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	static uint16_t packets_cnt = 0xffff;
	static uint8_t header[2] = { 2, 0 };	// Header length + data
	static uint32_t picture_pos, x, y;
	static uint8_t sr;
	static const palette_t * palette_ptr;
	uint8_t packet[VIDEO_PACKET_SIZE];

	USBD_LL_FlushEP(pdev, USB_ENDPOINT_IN(1));

	packets_cnt++;

	// Scale 2x
	// 128*2*112*2 = 57344 pixels
	// 1.5 bytes / pixel -> 57344*1.5 = 86016 bytes/frame
	// 86016 * 8fps = 688128 bytes/s

	// 672 bytes/packet -> 86016/672 = 128 packets/frame

	// We have to send 256*224*3/2*8fps = 688128 bytes per second
	// USB FS is 12Mbps max so we're already using 44% !

	if (packets_cnt > (PACKETS_IN_FRAME-1)) {
		// Start sending new frame
		packets_cnt = 0;
		header[1] ^= 1;		// Bit 0 is toggled every new frame
		picture_pos = 0;
		x = 0;
		y = 0;
		if (!new_frame_trigger) {
			buffer_flip ^= 1;
			new_frame_trigger = 1;
		}
		palette_ptr = &palettes[settings.hue & 7];
	}

	packet[0] = header[0];
	packet[1] = header[1];

	uint8_t * linear_buffer_rd = buffer_flip ? linear_buffer_a : linear_buffer_b;

	// The following optimization attempt would only work if the entire packet group was generated at once
	/*uint8_t * packet_ptr = packet + 2;	// Start filling after header
	for (x = 0; x < 128*112; x++) {
		// Y plane, scale pixels x2
		// 1 sample for 2x2 final pixels
		// 128*2*112*2 = 57344 bytes out
		if (!(x & 3))
			sr = linear_buffer_rd[x >> 2];		// Reload every 4 pixels

		// Draw scaled pixel 1px -> 2x2px
		uint8_t Y = palettes[palette_index].color[sr >> 6].Y;
		*(packet_ptr++) = Y;
		*(packet_ptr++) = Y;
		*(packet_ptr+256-2) = Y;
		*(packet_ptr+256-1) = Y;

		if (!(x & 127))
			packet_ptr += 256;	// Skip 1 line out of 2

		sr <<= 2;	// Next pixel
	}
	// Y plane done, reset pixel positions
	x = 0;
	y = 0;
	for (x = 0; x < 128*112; x++) {
		// U and V planes, scale pixels x2
		// 2 samples for 4x2 final pixels
		// 0 0 1 1
		// 0 0 1 1
		// 256/4*224/2*2*2 = 28672 bytes out
		if (!(x & 3))
			sr = linear_buffer_rd[x >> 2];		// Reload every 4 pixels

		L = sr >> 6;
		*(packet_ptr++) = palettes[palette_index].color[L].V;
		*(packet_ptr++) = palettes[palette_index].color[L].U;

		sr <<= 2;	// Next pixel
	}*/

	// This loop has to be fast
	for (uint32_t i = 2; i < VIDEO_PACKET_SIZE; i++) {
		if (picture_pos == 57344) {
			// 128*2*112*2 = 57344
			// Y plane done, reset pixel positions
			x = 0;
			y = 0;
		}

		if (picture_pos < 57344) {
			// Y plane, scale pixels x2
			// 1 sample for 2x2 final pixels
			// 128*2*112*2 = 57344 bytes out
			if (!((x >> 1) & 3))
				sr = linear_buffer_rd[((x>>1) + ((y>>1)<<7)) >> 2];	// Reload 4 pixels

			packet[i] = palette_ptr->color[sr >> 6].Y;
		} else {
			// U and V planes, scale pixels x2
			// 2 samples for 4x2 final pixels
			// 128*112*2 = 28672 bytes out
			if (!((x >> 1) & 3))
				sr = linear_buffer_rd[((x>>1) + (y<<7)) >> 2];	// Reload 4 pixels

			// Interleave U and V
			packet[i] = (picture_pos & 1) ? palette_ptr->color[sr >> 6].V : palette_ptr->color[sr >> 6].U;
		}

		if (x & 1)
			sr <<= 2;

		if (x < 255) {
			x++;
		} else {
			x = 0;
			y++;
		}

		picture_pos++;
	}

	if (cam_status == STATUS_RUNNING) {
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(1), (uint8_t*)&packet, (uint32_t)VIDEO_PACKET_SIZE);
	} else {
		packets_cnt = 0xffff;
	}

	return USBD_OK;
}

static uint8_t USBD_VIDEO_EP0_RxReady (USBD_HandleTypeDef *pdev) {
	// Data transfer from EP0 just finished
	if (UVCClassData.unit == 1)	{	// UVC_REQ_CT = 1
		switch (UVCClassData.cs) {
			case CT_AE_MODE_CONTROL:
				settings.auto_exposure = UVCClassData.data[0];
				// Force brightness update if auto-exposure was disabled
				if (!settings.auto_exposure)
					flag_update_exposure = 1;
				break;
			case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
				settings.exposure = UVCClassData.data[0];
				flag_update_exposure = 1;
				break;
		}
		UVCClassData.unit = 0;	// Clear
	} else if (UVCClassData.unit == 2) {	// UVC_REQ_PU = 2
		switch (UVCClassData.cs) {
			case PU_SHARPNESS_CONTROL:
				settings.debug = UVCClassData.data[0] + (UVCClassData.data[1] << 8);
				break;
			case PU_BRIGHTNESS_CONTROL:
				settings.brightness = UVCClassData.data[0];
				flag_update_matrix = 1;
				break;
			case PU_CONTRAST_CONTROL:
				settings.contrast = UVCClassData.data[0];
				flag_update_matrix = 1;
				break;
			case PU_GAIN_CONTROL:
				settings.gain = UVCClassData.data[0];
				flag_update_matrix = 1;
				break;
			case PU_HUE_CONTROL:
				settings.hue = UVCClassData.data[0];
				OSD_timer = 20;	// Display palette name during 20 frames
				break;
		}
		UVCClassData.unit = 0;	// Clear
	}
	return USBD_OK;
}

static uint8_t USBD_VIDEO_EP0_TxReady (USBD_HandleTypeDef *pdev) {
	return USBD_OK;
}

static uint8_t USBD_VIDEO_SOF (USBD_HandleTypeDef *pdev) {
	if (cam_status == STATUS_READY) {
		USBD_LL_FlushEP(pdev, USB_ENDPOINT_IN(1));
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(1), (uint8_t*)0x0002, 2);//header
		cam_status = STATUS_RUNNING;
	}
	return USBD_OK;
}

static uint8_t USBD_VIDEO_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum) {
	return USBD_OK;
}

static uint8_t USBD_VIDEO_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum) {
	return USBD_OK;
}

static uint8_t USBD_VIDEO_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum) {
	return USBD_OK;
}

/*static void VIDEO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	USBD_LL_FlushEP(pdev, USB_ENDPOINT_OUT(0));

	if (req->wValue == 256) {
		//Probe Request
		USBD_CtlSendData(pdev, (uint8_t*)&videoProbeControl, req->wLength);
	} else if (req->wValue == 512) {
		//Commit Request
		USBD_CtlSendData(pdev, (uint8_t*)&videoCommitControl, req->wLength);
	}
}*/

/*static void VIDEO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	USBD_AUDIO_HandleTypeDef *haudio;
	haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;

	if (req->wLength) {
		USBD_CtlPrepareRx(pdev, haudio->if1_control.data, req->wLength);

		haudio->if1_control.cmd = AUDIO_REQ_SET_CUR;
		haudio->if1_control.len = req->wLength;
		haudio->if1_control.unit = HIBYTE(req->wIndex);
	}
}*/

static uint8_t *USBD_VIDEO_GetDeviceQualifierDesc(uint16_t *length) {
	*length = sizeof(USBD_VIDEO_DeviceQualifierDesc);
	return (uint8_t*)USBD_VIDEO_DeviceQualifierDesc;
}
