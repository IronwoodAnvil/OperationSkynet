//--------------------------------
// Microprocessor Systems Lab 6 - Template - Lab06_p1_sample.c
//--------------------------------
//
//

#include "init.h"
#include "usbh_conf.h"
#include "usbh_hid.h"
#include "usbh_core.h"
#include "ff_gen_drv.h"
#include "usbh_diskio.h"
#include <stdbool.h>
#include "mps_paint.h"

USBH_HandleTypeDef husbh;

bool mouse_connected = false;

void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);

// Receive mouse events when HID interface is active
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	HID_MOUSE_Info_TypeDef* mouse_info =  USBH_HID_GetMouseInfo(phost);
	printf("v=(%d, %d)\tb=%d%d%d\r\n",mouse_info->x,mouse_info->y,mouse_info->buttons[0],mouse_info->buttons[1],mouse_info->buttons[2]);
//	MPS_Paint_OnMouse(mouse_info->x, mouse_info->y, mouse_info->buttons[0], mouse_info->buttons[2]);
}

/*Please note that the application can register multiple classes, for example:
 Add Supported Class: example HID and MSC classes
 USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
 USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
The user application can determine the enumerated class using core API function
USBH_GetActiveClass() when HOST_USER_CLASS_ACTIVE event occurs.*/


int main(void){
	 // System Initializations
	Sys_Init();

	// Application Initializations


	// USBH Driver Initialization
	USBH_Init(&husbh, USBH_UserProcess, 0);

	/* Add Supported Class*/
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);

	// Start USBH Driver
	USBH_Start(&husbh);

	while(1){
		USBH_Process(&husbh); // Calls HID process automagically
		// Other stuff
		if(mouse_connected)
		{
//			MPS_Paint_Tasks();
		}

	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	if(id==HOST_USER_CLASS_ACTIVE) // This means HID init was already called and processed enough to finish class request
	{
		uint8_t dev_class = USBH_GetActiveClass(&husbh);
		// When device connected
		if( (dev_class==USB_HID_CLASS) && (USBH_HID_GetDeviceType(&husbh)==HID_MOUSE) )
		{
			USBH_HID_MouseInit(&husbh);
//			MPS_Paint_Init();
			mouse_connected = true;
		}
		else if(dev_class==USB_MSC_CLASS)
		{
			//Set some sort of flag
		}
	}
	else if(id==HOST_DEV_DISCONNECTED)
	{
		mouse_connected = false; // Nothing connected, definitely not a mouse
	}
}

// Interrupts and Callbacks...

