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

USBH_HandleTypeDef husbh;

void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);


/*Please note that the application can register multiple classes, for example:
 Add Supported Class: example HID and MSC classes
 USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
 USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
The user application can determine the enumerated class using core API function
USBH_GetActiveClass() when HOST_USER_CLASS_ACTIVE event occurs.*/


int main(void){
	 // System Initializations
	Sys_Init();





	/* Start Host Process */
	// Application Initializations



	// USBH Driver Initialization
	// USB Driver Class Registrations: Add device types to handle.

	/* Init Host Library */
	USBH_Init(&hUSBHost, USBH_UserProcess, 0);

	/* Add Supported Class: example HID class*/
	USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);

	// Start USBH Driver
	USBH_Start(&hUSBHost);

	while(1){
		USBH_Process(&husbh);
		// Other stuff
	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	/*The USBH_UserProcess callback handles theUSB host core events (for example
disconnection, connection, class active). Typically, it should be implemented as shown
hereafter to handle application process state machine:*/
	switch (id)
	 {
	 case HOST_USER_DISCONNECTION:
	 Appli_state = APPLICATION_DISCONNECT;
	 break;

	/* when HOST_USER_CLASS_ACTIVE event is received, application can start
	 communication with device*/
	 case HOST_USER_CLASS_ACTIVE:
	 Appli_state = APPLICATION_READY;
	 break;

	 case HOST_USER_CONNECTION:
	 Appli_state = APPLICATION_START;
	 break;
	default:
	 break;
	 }
}

// Interrupts and Callbacks...

