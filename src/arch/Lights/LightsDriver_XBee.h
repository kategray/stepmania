#ifndef LightsDriver_XBee_H
#define LightsDriver_XBee_H

// All XBEE API frames start with this
#define XBEE_FRAME_START 0x7E

// This command indicates that we want to send data to the MCU
#define XBEE_FRAME_TYPE_TRANSMIT 0x10

#define XBEE_FRAME_LENGTH_POS 0x01
#define XBEE_FRAME_LENGTH_SIZE 0x02

// 00 = No response requested
#define XBEE_LIGHT_FRAME_ID 0x00

// Length and size of the frame.  Since we are hard-coding the frame, we don't need to adjust this
#define XBEE_LIGHT_FRAME_SIZE 24
#define XBEE_LIGHT_FRAME_LENGTH 0x00, 0x14

// Magic value to indicate a light packet to the MCU
#define XBEE_LIGHT_FRAME_MAGIC 0xBE, 0xEF

// Default light data (32 bits)
#define XBEE_LIGHT_FRAME_DEFAULT 0x00, 0x00, 0x00, 0x00

// Where in the frame the cabinet lights are
#define XBEE_LIGHT_FRAME_CAB_POS 0x13
#define XBEE_LIGHT_FRAME_FOURPANEL_POS 0x13

#define XBEE_DEST_BROADCAST 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF
#define XBEE_DEST_BROADCAST_16 0xFF, 0xFE

#define XBEE_BROADCAST_RADIUS_MAX 0x00

#define XBEE_POINT_TO_MULTIPOINT 0x80

#define XBEE_CRC_EMPTY 0x00

#ifdef WIN32
const std::string DEFAULT_LIGHTS_DEVICE = "COM1";
#else
const std::string DEFAULT_LIGHTS_DEVICE = "/dev/ttyS0";
#endif

#include "LightsDriver.h"
#include "RageFile.h"

class LightsDriver_XBee: public LightsDriver
{
protected:
	RageFile * port;
	bool lightOn = false;
public:
	LightsDriver_XBee();
	void correctFrame(uint8_t *frameData, uint16_t size);
	void openPort(const std::string& portName);
	virtual ~LightsDriver_XBee();
	virtual void Set(const LightsState *ls);

};


#endif

/*
 * Copyright © 2018 Kate Gray
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
