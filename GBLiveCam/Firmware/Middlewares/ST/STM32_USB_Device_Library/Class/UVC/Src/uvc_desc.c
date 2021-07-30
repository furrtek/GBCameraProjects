#include "main.h"
#include "uvc_desc.h"

VideoControl videoCommitControl = {
	{0x00, 0x00},					// bmHint
	{0x01},							// bFormatIndex
	{0x01},							// bFrameIndex
	{DBVAL(INTERVAL),},          	// dwFrameInterval
	{0x00, 0x00,},					// wKeyFrameRate
	{0x00, 0x00,},					// wPFrameRate
	{0x00, 0x00,},					// wCompQuality
	{0x00, 0x00,},					// wCompWindowSize
	{0x00, 0x00},					// wDelay
	{DBVAL(MAX_FRAME_SIZE)},    	// dwMaxVideoFrameSize
	{0x00, 0x00, 0x00, 0x00},		// dwMaxPayloadTransferSize
	{0x00, 0x00, 0x00, 0x00},		// dwClockFrequency
	{0x00},							// bmFramingInfo
	{0x00},							// bPreferedVersion
	{0x00},							// bMinVersion
	{0x00},							// bMaxVersion
};

VideoControl videoProbeControl = {
	{0x00, 0x00},					// bmHint
	{0x01},							// bFormatIndex
	{0x01},							// bFrameIndex
	{DBVAL(INTERVAL),},				// dwFrameInterval
	{0x00, 0x00,},					// wKeyFrameRate
	{0x00, 0x00,},					// wPFrameRate
	{0x00, 0x00,},					// wCompQuality
	{0x00, 0x00,},					// wCompWindowSize
	{0x00, 0x00},					// wDelay
	{DBVAL(MAX_FRAME_SIZE)},		// dwMaxVideoFrameSize
	{0x00, 0x00, 0x00, 0x00},		// dwMaxPayloadTransferSize
	{0x00, 0x00, 0x00, 0x00},		// dwClockFrequency
	{0x00},							// bmFramingInfo
	{0x00},							// bPreferedVersion
	{0x00},							// bMinVersion
	{0x00},							// bMaxVersion
};

// USB VIDEO device Configuration Descriptor
// See UVC 1.5 Class specification.pdf page 44 for a map
__ALIGN_BEGIN const uint8_t USBD_VIDEO_CfgDesc[USB_VIDEO_DESC_SIZ] __ALIGN_END = {
	// Configuration 1, the only one available
	USB_CONFIGUARTION_DESC_SIZE,				// bLength					9
	USB_DESC_TYPE_CONFIGURATION,				// bDescriptorType
	WBVAL(USB_VIDEO_DESC_SIZ),					// wTotalLength
	0x02,										// bNumInterfaces
	0x01,										// bConfigurationValue
	0x00,										// iConfiguration			0:No string
	USB_CONFIG_BUS_POWERED,						// bmAttributes
	USB_CONFIG_POWER_MA(100),					// bMaxPower				100mA

	// Interface Association Descriptor (IAD)
	UVC_INTERFACE_ASSOCIATION_DESC_SIZE,		// bLength					8
	USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,	// bDescriptorType			11
	0x00,										// bFirstInterface			0
	0x02,										// bInterfaceCount			2 (VideoControl, VideoStreaming)
	CC_VIDEO,									// bFunctionClass			14 Video
	SC_VIDEO_INTERFACE_COLLECTION,				// bFunctionSubClass		3 Video Interface Collection
	PC_PROTOCOL_UNDEFINED,						// bInterfaceProtocol		0 Protocol undefined
	0x02,										// iFunction				2

	// VideoControl Interface Descriptor
	// Standard VC Interface Descriptor: interface 0
	USB_INTERFACE_DESC_SIZE,					// bLength					9
	USB_DESC_TYPE_INTERFACE,					// bDescriptorType			4
	USB_UVC_VCIF_NUM,							// bInterfaceNumber         0 index of this interface (VC)
	0x00,										// bAlternateSetting        0 index of this setting
	0x00,										// bNumEndpoints            0 no endpoints
	CC_VIDEO,									// bInterfaceClass         	14 Video
	SC_VIDEOCONTROL,							// bInterfaceSubClass       1 Video Control
	PC_PROTOCOL_UNDEFINED,						// bInterfaceProtocol       0 (protocol undefined)
	0x02,										// iFunction                2

	// Class-specific VC Interface Descriptor
	UVC_VC_INTERFACE_HEADER_DESC_SIZE(1),      	// bLength                 	13
	CS_INTERFACE,                              	// bDescriptorType         	36 (INTERFACE)
	VC_HEADER,                                 	// bDescriptorSubtype      	1 (HEADER)
	WBVAL(UVC_VERSION),                        	// bcdUVC                  	1.10 or 1.00
	WBVAL(VC_TERMINAL_SIZ),			           	// wTotalLength            	header+units+terminals
	DBVAL(6000000),								// dwClockFrequency			6MHz (deprecated)
	0x01,                                      	// bInCollection            1 one streaming interface
	0x01,                                      	// baInterfaceNr(0)        	1 VS interface 1 belongs to this VC interface

	// Input Terminal 1 -> Processing Unit 2 -> Output Terminal 3

	// Input Terminal Descriptor (Camera)
	UVC_CAMERA_TERMINAL_DESC_SIZE(2),          	// bLength                 17 15 + 2 controls
	CS_INTERFACE,                              	// bDescriptorType         36 (INTERFACE)
	VC_INPUT_TERMINAL,                         	// bDescriptorSubtype       2 (INPUT_TERMINAL)
	0x01,                                      	// bTerminalID              1 ID of this Terminal
	WBVAL(ITT_CAMERA),                         	// wTerminalType       0x0201 Camera Sensor
	0x00,                                      	// bAssocTerminal           0 no Terminal associated
	0x00,                                      	// iTerminal                No description available
	WBVAL(0x0000),                             	// wObjectiveFocalLengthMin Special fields for ITT_CAMERA...
	WBVAL(0x0000),                             	// wObjectiveFocalLengthMax
	WBVAL(0x0000),                             	// wOcularFocalLength
	0x02,                                      	// bControlSize             2
	0x0A, 0x00,                                	// bmControls          0x000A Only auto-exposure & absolute exposure controls supported

	// Processing Unit Descriptor
	UVC_PROCESSING_UNIT_DESC_SIZE(2),          	// bLength                  10
	CS_INTERFACE,                              	// bDescriptorType         	36 (INTERFACE)
	VC_PROCESSING_UNIT,                        	// bDescriptorSubtype       3 (VC_PROCESSING_UNIT)
	0x02,                                      	// bUnitID              	2 ID of this Terminal
	0x01,                                      	// bSourceID                1 input pin connected to output pin of unit 1
	0x00, 0x00,									// wMaxMultiplier			0 unused
	0x02,										// bControlSize             2 (should be 3 ?)
	0b00010111, 0b00000010,						// bmControls				Bitmap, hue control, brightness control, contrast control, sharpness control (for debugging), gain control
	0x00,                                      	// iProcessing         		No description available
	0x00,                                      	// bmVideoStandards      	Bitmap, none

	// Output Terminal Descriptor
	UVC_OUTPUT_TERMINAL_DESC_SIZE(0),          	// bLength                  9
	CS_INTERFACE,                              	// bDescriptorType         36 (INTERFACE)
	VC_OUTPUT_TERMINAL,                        	// bDescriptorSubtype       3 (OUTPUT_TERMINAL)
	0x03,                                      	// bTerminalID              3 ID of this Terminal
	WBVAL(TT_STREAMING),                       	// wTerminalType       0x0101 USB streaming terminal
	0x00,                                      	// bAssocTerminal           0 no terminal associated
	0x02,                                      	// bSourceID                2 input pin connected to output pin of unit 2
	0x00,                                      	// iTerminal                0 no description available

	// Video Streaming (VS) Interface Descriptor
	// Standard VS Interface Descriptor: interface 1
	// Alternate setting 0 = Zero Bandwidth
	USB_INTERFACE_DESC_SIZE,					// bLength                  9
	USB_DESC_TYPE_INTERFACE,					// bDescriptorType          4
	USB_UVC_VSIF_NUM,                          	// bInterfaceNumber         1 index of this interface
	0x00,                                      	// bAlternateSetting        0 index of this setting
	0x00,                                      	// bNumEndpoints            0 no EP used
	CC_VIDEO,                                  	// bInterfaceClass         14 Video
	SC_VIDEOSTREAMING,                         	// bInterfaceSubClass       2 Video Streaming
	PC_PROTOCOL_UNDEFINED,                     	// bInterfaceProtocol       0 (protocol undefined)
	0x00,                                      	// iInterface               No description available

	// Class-specific VS Header Descriptor (Input)
	UVC_VS_INTERFACE_INPUT_HEADER_DESC_SIZE(1,1),// bLength               14
	CS_INTERFACE,                              	// bDescriptorType         36 (INTERFACE)
	VS_INPUT_HEADER,                           	// bDescriptorSubtype       1 (INPUT_HEADER)
	0x01,                                      	// bNumFormats              1 one format descriptor follows
	WBVAL(VC_HEADER_SIZ),
	USB_ENDPOINT_IN(1),                        	// bEndPointAddress      0x81 EP IN
	0x00,                                      	// bmInfo                   0 no dynamic format change supported
	0x03,                                      	// bTerminalLink            3 supplies terminal ID 3 (Output terminal)
	0x00,                                      	// bStillCaptureMethod      0 No support for still image capture
	0x00,                                      	// bTriggerSupport          0 HW trigger supported for still image capture
	0x00,                                      	// bTriggerUsage            0 HW trigger initiate a still image capture
	0x01,                                      	// bControlSize             1 one byte bmaControls field size
	0x00,                                      	// bmaControls(0)           0 no VS specific controls

	// Class-specific VS Format Descriptor
	// See USB_Video_Payload_Uncompressed_1.5.pdf
	VS_FORMAT_UNCOMPRESSED_DESC_SIZE,     		// bLength 27
	CS_INTERFACE,                         		// bDescriptorType : CS_INTERFACE */
	VS_FORMAT_UNCOMPRESSED,               		// bDescriptorSubType : VS_FORMAT_UNCOMPRESSED subtype */
	0x01,                                 		// bFormatIndex : First (and only) format descriptor */
	0x01,                                 		// bNumFrameDescriptors : One frame descriptor for this format follows. */
	//0x59,0x55,0x59,0x32,                  	// GUID Format YUY2 {32595559-0000-0010-8000-00AA00389B71} */
	0x4E,0x56,0x31,0x32,                  		// NV12:           3231564E-0000-0010-8000-00AA00389B71 */
	0x00,0x00,
	0x10,0x00,
	0x80,0x00,
	0x00,0xAA,0x00,0x38,0x9B,0x71,
	//16,                                 		// bBitsPerPixel : Number of bits per pixel used to specify color in the decoded video frame - 16 for yuy2
	12,											// 12 for NV12
	0x01,                                 		// bDefaultFrameIndex : Default frame index is 1. */
	0x00,                                 		// bAspectRatioX : Non-interlaced stream not required. */
	0x00,                                 		// bAspectRatioY : Non-interlaced stream not required. */
	0x00,                                 		// bmInterlaceFlags : Non-interlaced stream */
	0x00,                                 		// bCopyProtect : No restrictions imposed on the duplication of this video stream. */

	// Class-specific VS Frame Descriptor
	VS_FRAME_UNCOMPRESSED_DESC_SIZE,      		/* bLength 30*/
	CS_INTERFACE,                         		/* bDescriptorType : CS_INTERFACE */
	VS_FRAME_UNCOMPRESSED,                		/* bDescriptorSubType : VS_FRAME_UNCOMPRESSED */
	0x01,                                 		/* bFrameIndex : First (and only) frame descriptor */
	0x02,                                 		/* bmCapabilities : Still images using capture method 0 are supported at this frame setting.D1: Fixed frame-rate. */
	WBVAL(WIDTH),                         		/* wWidth (2bytes): Width of frame */
	WBVAL(HEIGHT),                       		/* wHeight (2bytes): Height of frame */
	DBVAL(MIN_BIT_RATE),                  		/* dwMinBitRate (4bytes): Min bit rate in bits/s  */ // 128*64*16*5 = 655360 = 0x000A0000 //5fps
	DBVAL(MAX_BIT_RATE),                  		/* dwMaxBitRate (4bytes): Max bit rate in bits/s  */ // 128*64*16*5 = 655360 = 0x000A0000
	DBVAL(MAX_FRAME_SIZE),                		/* dwMaxVideoFrameBufSize (4bytes): Maximum video or still frame size, in bytes. */
	DBVAL(INTERVAL),				        	/* dwDefaultFrameInterval : 1,000,000 * 100ns -> 10 FPS */ // 5 FPS -> 200ms -> 200,000 us -> 2,000,000 X 100ns = 0x001e8480
	0x01,                                 		/* bFrameIntervalType : Continuous frame interval - Should be 0 ?*/
	DBVAL(INTERVAL),                      		/* dwMinFrameInterval : 1,000,000 ns  *100ns -> 10 FPS */

	// Color Matching Descriptor
	VS_COLOR_MATCHING_DESC_SIZE,          /* bLength */
	CS_INTERFACE,                         /* bDescriptorType : CS_INTERFACE */
	0x0D,                                 /* bDescriptorSubType : VS_COLORFORMAT */
	0x01,                                 /* bColorPrimaries : 1: BT.709, sRGB (default) */
	0x01,                                 /* bTransferCharacteristics : 1: BT.709 (default) */
	0x04,                                 /* bMatrixCoefficients : 1: BT.709. */

	// Standard VS Interface Descriptor: interface 1
	// Alternate setting 1 = operational setting
	USB_INTERFACE_DESC_SIZE,                   	// bLength                  9
	USB_DESC_TYPE_INTERFACE,             		// bDescriptorType          4
	USB_UVC_VSIF_NUM,                          	// bInterfaceNumber         1 index of this interface
	0x01,                                      	// bAlternateSetting        1 index of this setting
	0x01,                                      	// bNumEndpoints            1 one EP used
	CC_VIDEO,                                  	// bInterfaceClass         14 Video
	SC_VIDEOSTREAMING,                         	// bInterfaceSubClass       2 Video Streaming
	PC_PROTOCOL_UNDEFINED,                     	// bInterfaceProtocol       0 (protocol undefined)
	0x00,                                      	// iInterface               0 no description available

	/* Standard VS Isochronous Video data Endpoint Descriptor */
	USB_ENDPOINT_DESC_SIZE,                   	// bLength                  7
	USB_DESC_TYPE_ENDPOINT,             		// bDescriptorType          5 (ENDPOINT)
	USB_ENDPOINT_IN(1),                       	// bEndpointAddress      0x81 EP IN
	USB_ENDPOINT_TYPE_ISOCHRONOUS,            	// bmAttributes             1 isochronous transfer type
	WBVAL(VIDEO_PACKET_SIZE),                 	// wMaxPacketSize
	0x01                                      	// bInterval                1 One frame interval
} ;

// USB Standard Device Descriptor
__ALIGN_BEGIN const uint8_t USBD_VIDEO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END= {
	USB_LEN_DEV_QUALIFIER_DESC,
	USB_DESC_TYPE_DEVICE_QUALIFIER,
	0x00,		// bcdUSB USB 2.0
	0x02,
	0x00,		// bDeviceClass Each interface has its own class info
	0x00,		// bDeviceSubClass
	0x00,		// bDeviceProtocol
	0x40,		// bMaxPacketSize0 Max EP0 packet size
	0x01,		// bNumConfigurations Only 1 config
	0x00,		// bReserved Reserved
};
