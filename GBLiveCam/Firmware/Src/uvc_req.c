#include "main.h"
#include "uvc_req.h"
#include "uvc_desc.h"
#include "usbd_ioreq.h"

UVCClassData_t UVCClassData;

void UVC_REQ_CT(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	uint8_t temp;
	uint32_t tempdw;

	switch (req->wValue >> 8) {
		case CT_AE_MODE_CONTROL:
			switch (req->bRequest) {
				case GET_CUR:
					temp = settings.auto_exposure;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_RES:	// bAutoExposureMode: Only auto-exposure and manual mode are supported
					temp = 0b00000011;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_INFO:	// Both GET and SET requests are supported
					temp = 0b00000011;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_DEF:
					temp = 1;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case SET_CUR:
					USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
					UVCClassData.len = req->wLength;
					UVCClassData.unit = 1;	// UVC_REQ_CT = 1
					UVCClassData.cs = CT_AE_MODE_CONTROL;
					break;
				default:
					USBD_CtlError(pdev, req);
					break;
			}
			break;

		case CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
			switch (req->bRequest) {
				case GET_CUR:
					tempdw = settings.exposure;
					USBD_CtlSendData(pdev, (uint8_t*)&tempdw, req->wLength);
					break;
				case GET_MIN:
					tempdw = 1;
					USBD_CtlSendData(pdev, (uint8_t*)&tempdw, req->wLength);
					break;
				case GET_MAX:
					tempdw = 100;
					USBD_CtlSendData(pdev, (uint8_t*)&tempdw, req->wLength);
					break;
				case GET_RES:
					tempdw = 1;
					USBD_CtlSendData(pdev, (uint8_t*)&tempdw, req->wLength);
					break;
				case GET_INFO:	// Both GET and SET requests are supported, auto modes not supported
					tempdw = 0b00000011;
					USBD_CtlSendData(pdev, (uint8_t*)&tempdw, req->wLength);
					break;
				case GET_DEF:
					tempdw = 50;
					USBD_CtlSendData(pdev, (uint8_t*)&tempdw, req->wLength);
					break;
				case SET_CUR:
					USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
					UVCClassData.len = req->wLength;
					UVCClassData.unit = 1;	// UVC_REQ_CT = 1
					UVCClassData.cs = CT_EXPOSURE_TIME_ABSOLUTE_CONTROL;
					break;
				default:
					USBD_CtlError(pdev, req);
					break;
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
	}
}

// Request to Control interface for Processing Unit
void UVC_REQ_PU(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	uint16_t temp;

	switch (req->wValue >> 8) {

		case PU_SHARPNESS_CONTROL:
		// This setting is used to trigger special functions such as DFU and DEBUG
		switch (req->bRequest) {
			case GET_CUR:
				temp = 0;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_MIN:
				temp = 0;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_MAX:
				temp = 2000;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_RES:
				temp = 1;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_INFO:	// Both GET and SET requests are supported, auto modes not supported
				temp = 0b00000011;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_DEF:
				temp = 0;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case SET_CUR:
				USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
				UVCClassData.len = req->wLength;
				UVCClassData.unit = 2;	// UVC_REQ_PU = 2
				UVCClassData.cs = PU_SHARPNESS_CONTROL;
				break;
			default:
				USBD_CtlError(pdev, req);
				break;
		}
		break;

		case PU_BRIGHTNESS_CONTROL:
		switch (req->bRequest) {
			case GET_CUR:
				USBD_CtlSendData(pdev, (uint8_t*)&settings.brightness, req->wLength);
				break;
			case GET_MIN:
				temp = 0;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_MAX:
				temp = 200;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_RES:
				temp = 1;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_INFO:	// Both GET and SET requests are supported, auto modes not supported
				temp = 0b00000011;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_DEF:
				temp = 70;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case SET_CUR:
				USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
				UVCClassData.len = req->wLength;
				UVCClassData.unit = 2;	// UVC_REQ_PU = 2
				UVCClassData.cs = PU_BRIGHTNESS_CONTROL;
				break;
			default:
				USBD_CtlError(pdev, req);
				break;
		}
		break;

		case PU_CONTRAST_CONTROL:
		switch (req->bRequest) {
			case GET_CUR:
				USBD_CtlSendData(pdev, (uint8_t*)&settings.contrast, req->wLength);
				break;
			case GET_MIN:
				temp = 1;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_MAX:
				temp = 20;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_RES:
				temp = 1;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_INFO:	// Both GET and SET requests are supported, auto modes not supported
				temp = 0b00000011;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_DEF:
				temp = 10;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case SET_CUR:
				USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
				UVCClassData.len = req->wLength;
				UVCClassData.unit = 2;	// UVC_REQ_PU = 2
				UVCClassData.cs = PU_CONTRAST_CONTROL;
				break;
			default:
				USBD_CtlError(pdev, req);
				break;
		}
		break;

		case PU_GAIN_CONTROL:
		switch (req->bRequest) {
			case GET_CUR:
				temp = settings.gain;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_MIN:
				temp = 0;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_MAX:
				temp = 15;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_RES:
				temp = 1;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_INFO:	// Both GET and SET requests are supported, auto modes not supported
				temp = 0b00000011;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case GET_DEF:
				temp = 5;
				USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
				break;
			case SET_CUR:
				USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
				UVCClassData.len = req->wLength;
				UVCClassData.unit = 2;	// UVC_REQ_PU = 2
				UVCClassData.cs = PU_GAIN_CONTROL;
				break;
			default:
				USBD_CtlError(pdev, req);
				break;
		}
		break;

		case PU_HUE_CONTROL:
			switch (req->bRequest) {
				case GET_CUR:
					USBD_CtlSendData(pdev, (uint8_t*)&settings.hue, req->wLength);
					break;
				case GET_MIN:
					// The UVC spec says this should be -180°*10
					temp = 0;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_MAX:
					// The UVC spec says this should be +180°*10
					temp = 7;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_RES:
					temp = 1;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_INFO:	// Both GET and SET requests are supported, auto modes not supported
					temp = 0b00000011;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case GET_DEF:
					temp = 0;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);
					break;
				case SET_CUR:
					USBD_CtlPrepareRx(pdev, UVCClassData.data, req->wLength);
					UVCClassData.len = req->wLength;
					UVCClassData.unit = 2;	// UVC_REQ_PU = 2
					UVCClassData.cs = PU_HUE_CONTROL;
					break;
				default:
					USBD_CtlError(pdev, req);
					break;
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
	}
}

// Request to Streaming interface
void UVC_REQ_STREAM(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	uint16_t temp;

	switch (req->wValue >> 8) {
		case VS_PROBE_CONTROL:
			switch (req->bRequest) {
				case GET_INFO:	// Both GET and SET requests are supported
					temp = 0x0003;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);	// req->wLength should be == 2
					break;
				case GET_LEN:	// Length of PROBE data
					temp = 34;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);	// req->wLength should be == 1
					break;
				case GET_CUR:
				case GET_MIN:
				case GET_MAX:
				case GET_DEF:
					USBD_CtlSendData(pdev, (uint8_t*)&videoProbeControl, req->wLength);
					break;
				case SET_CUR:
					USBD_CtlPrepareRx(pdev, (uint8_t*)&videoProbeControl, req->wLength);
					break;
				default:
					USBD_CtlError(pdev, req);
					break;
			}
			break;
		case VS_COMMIT_CONTROL:
			switch (req->bRequest) {
				case GET_INFO:	// Both GET and SET requests are supported
					temp = 0x0003;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);	// req->wLength should be == 2
					break;
				case GET_LEN:	// Length of PROBE data
					temp = 34;
					USBD_CtlSendData(pdev, (uint8_t*)&temp, req->wLength);	// req->wLength should be == 1
					break;
				case GET_CUR:
					USBD_CtlSendData(pdev, (uint8_t*)&videoCommitControl, req->wLength);
					break;
				case SET_CUR:
					USBD_CtlPrepareRx(pdev, (uint8_t*)&videoCommitControl, req->wLength);
					break;
				default:
					USBD_CtlError(pdev, req);
					break;
			}
			break;

		default:
			USBD_CtlError(pdev, req);
			break;
	}
}
