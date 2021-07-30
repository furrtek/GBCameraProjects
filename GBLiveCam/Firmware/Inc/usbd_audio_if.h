#ifndef __USBD_AUDIO_IF_H__
#define __USBD_AUDIO_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_uvc.h"

//extern USBD_VIDEO_ItfTypeDef USBD_VIDEO_fops_FS;

void TransferComplete_CallBack_FS(void);
void HalfTransfer_CallBack_FS(void);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_AUDIO_IF_H__ */
