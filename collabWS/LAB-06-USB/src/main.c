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
bool check_hid_type = false;
bool read_flash = false;

void USBH_UserProcess(USBH_HandleTypeDef *, uint8_t);

// Receive mouse events when HID interface is active
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
	HID_MOUSE_Info_TypeDef* mouse_info =  USBH_HID_GetMouseInfo(phost);
	MPS_Paint_OnMouse(mouse_info->x, mouse_info->y, mouse_info->buttons[0], mouse_info->buttons[1]);
//	printf("v=(%d, %d)\tb=%d%d%d\r\n",mouse_info->x,mouse_info->y,mouse_info->buttons[0],mouse_info->buttons[1],mouse_info->buttons[2]);
}

/*Please note that the application can register multiple classes, for example:
 Add Supported Class: example HID and MSC classes
 USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
 USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
The user application can determine the enumerated class using core API function
USBH_GetActiveClass() when HOST_USER_CLASS_ACTIVE event occurs.*/


void ls_flash(USBH_HandleTypeDef *phost)
{
	FATFS fs; // filesystem object to be registered and cleared
	char dskPath[4]; //Pointer to the null-terminated string that specifies the logical drive. always only 4 characters

	DIR rootdir; //

	FATFS_LinkDriver(&USBH_Driver, dskPath); //  Links a compatible diskio driver id and increments the number of active linked drivers
	f_mount(&fs, dskPath, 0); //registers/unregisters filesystem object to the FatFs module
	f_opendir (&rootdir, dskPath); // opens a directory, in this case the root directory

	FILINFO info;    // holds information about the file
	printf("Listing of %s:\r\n",dskPath);
	f_readdir(&rootdir, &info); //reads directory at pointer, fills in info.
	while(*info.fname != 0) //. When all directory items have been read, a null string is stored into the fno->fname[] with no error
	{
		char* fname = info.fname;
		if(strchr(fname,'~')) fname = info.lfname;
		bool isdir = info.fattrib & AM_DIR;
		printf("  %-40s%c\r\n",fname,isdir ? 'D' : 'F');
		f_readdir(&rootdir, &info);
	}

	f_closedir(&rootdir); //closes dir
	FATFS_UnLinkDriver(dskPath);
}

int main(void){
	 // System Initializations
	Sys_Init();

	// Application Initializations


	// USBH Driver Initialization
	USBH_Init(&husbh, USBH_UserProcess, 0);

	/* Add Supported Class*/
	USBH_RegisterClass(&husbh, USBH_HID_CLASS);
	USBH_RegisterClass(&husbh, USBH_MSC_CLASS);

	// Start USBH Driver
	USBH_Start(&husbh);

//	MPS_Paint_Init(); // Start MPS Paint Immediately (e.g. for keyboard testing without mouse/OTG adapter)
//	mouse_connected = true;

	while(1){
		USBH_Process(&husbh); // Calls HID process automagically
		// Other stuff
		if(check_hid_type)
		{
			if (USBH_HID_GetDeviceType(&husbh)==HID_MOUSE)
			{
				USBH_HID_MouseInit(&husbh);
				MPS_Paint_Init();
				mouse_connected = true;
			}
			else {
				printf("MPS Paint only supports mouse input at this time.\r\n");
			}
			check_hid_type = false;
		}
		if(mouse_connected)
		{
			MPS_Paint_Tasks();
		}
		if(read_flash)
		{
			ls_flash(&husbh);
			read_flash = false;
		}
	}
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	if(id==HOST_USER_CLASS_ACTIVE) // This means HID init was already called and processed enough to finish class request
	{
		uint8_t dev_class = USBH_GetActiveClass(&husbh);
		// When device connected
		if(dev_class==USB_HID_CLASS)
		{
			check_hid_type = true; // Have to wait for this to return for HID devicetype to work
		}
		else if(dev_class==USB_MSC_CLASS)
		{
			//Set some sort of flag
			read_flash = true;
		}
	}
	else if(mouse_connected && id==HOST_USER_DISCONNECTION)
	{
		MPS_Paint_End();
		mouse_connected = false;
	}
}

// Interrupts and Callbacks...

