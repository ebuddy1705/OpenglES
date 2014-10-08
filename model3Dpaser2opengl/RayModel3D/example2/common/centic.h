
#ifdef __cplusplus
extern "C" {
#endif
#ifndef MIN
#define MIN(x,y) (((x)>(y)) ? (y) : (x))
#endif
#ifndef MAX
#define MAX  (x,y) (((x)>(y)) ? (x) : (y))
#endif  

#define SCALE_FULL_SCREEN 1
#define SCREEN_W 800
#define SCREEN_H 480
  
#define SCREEN_VIRTUAL_W 800
#define SCREEN_VIRTUAL_H 400
#define SCREEN_VIRTUAL_OFFSET_X ((SCREEN_W-SCREEN_VIRTUAL_W)/2)
#define SCREEN_VIRTUAL_OFFSET_Y ((SCREEN_H-SCREEN_VIRTUAL_H)/2)

#define SCREEN_RES_X SCREEN_W
#define SCREEN_RES_Y SCREEN_H

#ifdef SCALE_1920x720_MODE
#define SCREEN_USED_W SCREEN_W
#define SCREEN_USED_H (SCREEN_W * 720 / 1920)
#elif (defined SCALE_FULL_SCREEN)
#define SCREEN_USED_W SCREEN_W
#define SCREEN_USED_H SCREEN_H
#endif

#define SCREEN_CENTER_OFFSET_X (SCREEN_USED_W  > SCREEN_USED_H ? (SCREEN_USED_W - SCREEN_USED_H)/2 : 0 )
#define SCREEN_CENTER_OFFSET_Y (SCREEN_USED_H  > SCREEN_USED_W ? (SCREEN_USED_H - SCREEN_USED_W)/2 : 0 )

#define ASPECT_RATIO (float)(SCREEN_USED_W/SCREEN_USED_H)

#define COMPONENT_NO_MOTION 	(0x00)
#define COMPONENT_MOTION 		(0x01<<0)
#define COMPONENT_TEXTURE		(0x01<<1)
#define COMPONENT_SCALE			(0x01<<2)

#ifdef __cplusplus
}//extern "C" {
#endif
