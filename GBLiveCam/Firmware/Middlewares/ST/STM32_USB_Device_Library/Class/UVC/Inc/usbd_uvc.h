#ifndef __USB_AUDIO_H
#define __USB_AUDIO_H

#include "usbd_ioreq.h"

#define WBVAL(x) (x & 0xFF),((x >> 8) & 0xFF)
#define DBVAL(x) (x & 0xFF),((x >> 8) & 0xFF),((x >> 16) & 0xFF),((x >> 24) & 0xFF)

#define WIDTH                                         (unsigned int)256
#define HEIGHT                                        (unsigned int)224
#define CAM_FPS                                       8
#define VIDEO_PACKET_SIZE                             (unsigned int)((672)+2)	//672	862!
#define MIN_BIT_RATE                                  (unsigned long)(WIDTH*HEIGHT*16*CAM_FPS)//16 bit
#define MAX_BIT_RATE                                  (unsigned long)(WIDTH*HEIGHT*16*CAM_FPS)
//#define MAX_FRAME_SIZE                                (unsigned long)(WIDTH*HEIGHT*2)//yuy2
#define MAX_FRAME_SIZE                                (unsigned long)(WIDTH*HEIGHT*3/2)//nv12
#define INTERVAL                                      (unsigned long)(10000000/CAM_FPS)
#define PACKETS_IN_FRAME                              (unsigned int)(MAX_FRAME_SIZE/(VIDEO_PACKET_SIZE-2))

#define USB_VIDEO_DESC_SIZ (unsigned long)(\
			USB_CONFIGUARTION_DESC_SIZE +\
		    UVC_INTERFACE_ASSOCIATION_DESC_SIZE +\
		    USB_INTERFACE_DESC_SIZE +  \
		    UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) + \
		    UVC_CAMERA_TERMINAL_DESC_SIZE(2) + \
		    UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + \
		    UVC_PROCESSING_UNIT_DESC_SIZE(2) + \
		    USB_INTERFACE_DESC_SIZE +   \
		    UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) + \
		    VS_FORMAT_UNCOMPRESSED_DESC_SIZE +  \
		    VS_FRAME_UNCOMPRESSED_DESC_SIZE  +  \
		    VS_COLOR_MATCHING_DESC_SIZE  +\
		    USB_INTERFACE_DESC_SIZE +  \
		    USB_ENDPOINT_DESC_SIZE)

#define VC_TERMINAL_SIZ (unsigned int)(UVC_VC_INTERFACE_HEADER_DESC_SIZE(1) + UVC_CAMERA_TERMINAL_DESC_SIZE(2) + UVC_OUTPUT_TERMINAL_DESC_SIZE(0) + UVC_PROCESSING_UNIT_DESC_SIZE(2))
#define VC_HEADER_SIZ (unsigned int)(UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1) + VS_FORMAT_UNCOMPRESSED_DESC_SIZE + VS_FRAME_UNCOMPRESSED_DESC_SIZE + VS_COLOR_MATCHING_DESC_SIZE)


#define USB_DEVICE_DESC_SIZE        (sizeof(USB_DEVICE_DESCRIPTOR))
#define USB_CONFIGUARTION_DESC_SIZE (char)9
#define USB_INTERFACE_DESC_SIZE     (char)9
#define USB_ENDPOINT_DESC_SIZE      (char)7
#define UVC_INTERFACE_ASSOCIATION_DESC_SIZE (char)8
#define UVC_VC_ENDPOINT_DESC_SIZE   (char)5

#define UVC_VC_INTERFACE_HEADER_DESC_SIZE(n)  (char)(12+n)
#define UVC_CAMERA_TERMINAL_DESC_SIZE(n)      (char)(15+n)
#define UVC_OUTPUT_TERMINAL_DESC_SIZE(n)      (char)(9+n)
#define UVC_PROCESSING_UNIT_DESC_SIZE(n)      (char)(10+n)
#define UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(a,b) (char)(13+a*b)


#define VS_FORMAT_UNCOMPRESSED_DESC_SIZE  (char)(27)
#define VS_FRAME_UNCOMPRESSED_DESC_SIZE   (char)(30)
#define VS_COLOR_MATCHING_DESC_SIZE   (char)(6)

#define USB_UVC_VCIF_NUM 0
#define USB_UVC_VSIF_NUM            (char)1

#define VIDEO_TOTAL_IF_NUM 2

//UVC 1.0 uses only 26 first bytes
typedef struct {
	uint8_t bmHint[2];                      // 2
	uint8_t bFormatIndex[1];                // 3
	uint8_t bFrameIndex[1];                 // 4
	uint8_t dwFrameInterval[4];             // 8
	uint8_t wKeyFrameRate[2];               // 10
	uint8_t wPFrameRate[2];                 // 12
	uint8_t wCompQuality[2];                // 14
	uint8_t wCompWindowSize[2];             // 16
	uint8_t wDelay[2];                      // 18
	uint8_t dwMaxVideoFrameSize[4];         // 22
	uint8_t dwMaxPayloadTransferSize[4];    // 26
	uint8_t dwClockFrequency[4];
	uint8_t bmFramingInfo[1];
	uint8_t bPreferedVersion[1];
	uint8_t bMinVersion[1];
	uint8_t bMaxVersion[1];
} VideoControl;

/* bmRequestType.Dir */
#define REQUEST_HOST_TO_DEVICE     0
#define REQUEST_DEVICE_TO_HOST     1

/* bmRequestType.Type */
#define REQUEST_STANDARD           0
#define REQUEST_CLASS              1
#define REQUEST_VENDOR             2
#define REQUEST_RESERVED           3

/* bmRequestType.Recipient */
#define REQUEST_TO_DEVICE          0
#define REQUEST_TO_INTERFACE       1
#define REQUEST_TO_ENDPOINT        2
#define REQUEST_TO_OTHER           3

#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE       6
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE     7
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE        8
#define USB_OTG_DESCRIPTOR_TYPE                    9
#define USB_DEBUG_DESCRIPTOR_TYPE                 10
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE 11

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_MISCELLANEOUS         0xEF
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK                0xC0
#define USB_CONFIG_BUS_POWERED                 0x80

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/* bEndpointAddress in Endpoint Descriptor */
#define USB_ENDPOINT_DIRECTION_MASK            0x80
#define USB_ENDPOINT_OUT(addr)                 ((addr) | 0x00)
#define USB_ENDPOINT_IN(addr)                  ((addr) | 0x80)

/* bmAttributes in Endpoint Descriptor */
#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03
#define USB_ENDPOINT_SYNC_MASK                 0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C
#define USB_ENDPOINT_USAGE_MASK                0x30
#define USB_ENDPOINT_USAGE_DATA                0x00
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
#define USB_ENDPOINT_USAGE_RESERVED            0x30

typedef struct {
    uint8_t  (*VideoInit)(void);
    uint8_t  (*VideoDeInit)(void);
    uint8_t  (*VideoCmd)(uint8_t* pbuf, uint32_t size, uint8_t cmd);
    uint8_t  (*VideoGetState)(void);
} VIDEO_FOPS_TypeDef;

extern USBD_ClassTypeDef  USBD_VIDEO;

#endif  /* __USB_AUDIO_H */
