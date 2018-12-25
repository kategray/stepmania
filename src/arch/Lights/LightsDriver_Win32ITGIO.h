/* LightsDriver_Win32ITGIO - Control an ITG/IO with HIDAPI */
#include "arch/Lights/LightsDriver.h"

#ifndef LightsDriver_Win32ITGIO_H
#define LightsDriver_Win32ITGIO_H
/*
 * Status Codes
 */
#define HIDAPI_SUCCESS 0

/*
 * Constants
 */
#define ITGIO_VENDOR  0x07C0
#define ITGIO_PRODUCT 0x1584

// Used for report byte addressing
#define ITGIO_REPORT_ID_POS 0u

#define ITGIO_REPORT_ID 0u
#define ITGIO_REPORT_LENGTH 5u
#define ITGIO_REPORT_ID_LENGTH 1u

// First Port
#define ITGIO_LIGHT_PORT_0            1
#define ITGIO_LIGHT_P1_RIGHT          0x01u
#define ITGIO_LIGHT_P1_LEFT           0x02u
#define ITGIO_LIGHT_P1_DOWN           0x04u
#define ITGIO_LIGHT_P1_UP             0x08u
#define ITGIO_LIGHT_P2_RIGHT          0x10u
#define ITGIO_LIGHT_P2_LEFT           0x20u
#define ITGIO_LIGHT_P2_DOWN           0x40u
#define ITGIO_LIGHT_P2_UP             0x80u

// Second Port
#define ITGIO_LIGHT_PORT_1            2
#define ITGIO_LIGHT_MARQUEE_UP_LEFT   0x01u
#define ITGIO_LIGHT_MARQUEE_LR_LEFT   0x02u
#define ITGIO_LIGHT_MARQUEE_UP_RIGHT  0x04u
#define ITGIO_LIGHT_MARQUEE_LR_RIGHT  0x08u
#define ITGIO_LIGHT_CAB_P2            0x10u
#define ITGIO_LIGHT_CAB_P1            0x20u
#define ITGIO_LIGHT_COIN              0x40u
#define ITGIO_LIGHT_SUB               0x80u

/*
* Structures
*/
struct hid_device {};

struct hid_device_info {
	// HIDAPI Device Info
	// Copyright 2009, Alan Ott, Signal 11 Software.

	/** Platform-specific device path */
	char *path;
	/** Device Vendor ID */
	unsigned short vendor_id;
	/** Device Product ID */
	unsigned short product_id;
	/** Serial Number */
	wchar_t *serial_number;
	/** Device Release Number in binary-coded decimal,
	also known as Device Version Number */
	unsigned short release_number;
	/** Manufacturer String */
	wchar_t *manufacturer_string;
	/** Product string */
	wchar_t *product_string;
	/** Usage Page for this Device/Interface
	(Windows/Mac only). */
	unsigned short usage_page;
	/** Usage for this Device/Interface
	(Windows/Mac only).*/
	unsigned short usage;
	/** The USB interface which this logical device
	represents. Valid on both Linux implementations
	in all cases, and valid on the Windows implementation
	only if the device contains more than one interface. */
	int interface_number;

	/** Pointer to the next device */
	struct hid_device_info *next;
};


/*
 * Prototypes
 */
typedef int(*HID_INIT)(void);
typedef int(*HID_EXIT)(void);
typedef int(*HID_WRITE)(hid_device *device, const unsigned char *data, size_t length);
typedef hid_device_info(*HID_ENUMERATE)(unsigned short vendor_id, unsigned short product_id);
typedef hid_device * (*HID_OPEN)(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number);

/*
 * Exports
 */
static HID_INIT hid_init;
static HID_EXIT hid_exit;
static HID_ENUMERATE hid_enumerate;
static HID_OPEN hid_open;
static HID_WRITE hid_write;

/*
* Static Instances
*/
static hid_device * ITGIO;
static bool itgio_initialized = false;

class LightsDriver_Win32ITGIO : public LightsDriver
{
public:
	LightsDriver_Win32ITGIO();
	virtual ~LightsDriver_Win32ITGIO();

	virtual void Set(const LightsState *ls);
};
#endif

/*
 * (c) 2018 Kate Gray
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
