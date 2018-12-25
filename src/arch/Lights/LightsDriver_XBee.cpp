#include "global.h"
#include "LightsDriver_XBee.h"
#include "PrefsManager.h"
#include "RageLog.h"
#include "RageUtil.h"

#include <cstring>

using namespace std;
static Preference<std::string> g_sLightsDevice("LightsDevice", DEFAULT_LIGHTS_DEVICE); // "COM1" or "/dev/ttyS0" == DEFAULT_LIGHTS_DEVICE

const uint8_t light_on_packet[] = { 0x7E, 0x00, 0x05, 0x08, 0x01, 0x50, 0x30, 0x05, 0x71 };
const uint8_t light_off_packet[] = { 0x7E, 0x00, 0x05, 0x08, 0x01, 0x50, 0x30, 0x04, 0x72 };

// Light packets are a broadcast light packet
const uint8_t xbee_default_light_packet[XBEE_LIGHT_FRAME_SIZE] = {
	XBEE_FRAME_START,
	XBEE_LIGHT_FRAME_LENGTH,
	XBEE_FRAME_TYPE_TRANSMIT,
	XBEE_LIGHT_FRAME_ID,
	XBEE_DEST_BROADCAST,
	XBEE_DEST_BROADCAST_16,
	XBEE_BROADCAST_RADIUS_MAX,
	XBEE_POINT_TO_MULTIPOINT,
	XBEE_LIGHT_FRAME_MAGIC,
	XBEE_LIGHT_FRAME_DEFAULT,
	XBEE_CRC_EMPTY
};

uint8_t last_light_packet[XBEE_LIGHT_FRAME_SIZE];

void LightsDriver_XBee::correctFrame(uint8_t *frameData, uint16_t size) {
	// Used to calculate the checksum
	uint16_t checksum = 0x00;

	// Correct the length first
	uint8_t low_byte = size & 0xFF;
	uint8_t high_byte = size >> 8;
	frameData[XBEE_FRAME_LENGTH_POS] = high_byte;
	frameData[XBEE_FRAME_LENGTH_POS + 1] = low_byte - 0x04;

	// Calculate the correct CRC.  Start by zeroing any existing CRC
	frameData[size - 1] = 0x00;
	for (int i = XBEE_FRAME_LENGTH_POS + XBEE_FRAME_LENGTH_SIZE; i < size; i++) {
		checksum += frameData[i];
	}

	// The checksum is calculated by subtracting the lowest 8 bits from 0xFF
	checksum = 0xFF - (checksum & 0xFF);

	// Write the checksum
	frameData[size - 1] = (uint8_t)checksum;
}


void LightsDriver_XBee::openPort (const std::string& portName)
{
	port = new RageFile;

	if (!port->Open(portName, RageFile::WRITE | RageFile::STREAMED))
	{
		LOG->Warn("Error opening port '%s' for output: %s", portName.c_str(), port->GetError().c_str());
		Rage::safe_delete(port);
		port = nullptr;
	}

	return;
}


LightsDriver_XBee::LightsDriver_XBee()
{
	// Initialize the last light packet
	memset(&last_light_packet, 0, sizeof(last_light_packet));

	LOG->Trace("Initializing XBEE Driver using port: %s", g_sLightsDevice.Get());
	openPort(g_sLightsDevice.Get());
	if (port != nullptr)
	{
		LOG->Trace("Successfully opened port %s", port->GetPath());
	}

}

LightsDriver_XBee::~LightsDriver_XBee()
{
}

void LightsDriver_XBee::Set(const LightsState *ls)
{
	// Start with a blank light packet
	uint8_t lights[XBEE_LIGHT_FRAME_SIZE];
	memcpy(&lights, xbee_default_light_packet, sizeof(lights));

	// Cabinet lights
	uint8_t lights_cabinet = 0x00;
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT]) { lights_cabinet |= 0x01; }
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT]) { lights_cabinet |= 0x02; }
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT]) { lights_cabinet |= 0x04; }
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT]) { lights_cabinet |= 0x08; }
	if (ls->m_bCabinetLights[LIGHT_BASS_LEFT]) { lights_cabinet |= 0x10; }
	if (ls->m_bCabinetLights[LIGHT_BASS_RIGHT]) { lights_cabinet |= 0x20; }
	if (ls->m_bGameButtonLights[GameController_1][GAME_BUTTON_START]) { lights_cabinet |= 0x40; }
	if (ls->m_bGameButtonLights[GameController_2][GAME_BUTTON_START]) { lights_cabinet |= 0x80; }
	lights[XBEE_LIGHT_FRAME_CAB_POS] = lights_cabinet;

	// P1 and P2 panel lights
	uint8_t lights_fourpanel = 0x00;
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_LEFT]) { lights_fourpanel |= 0x01; }
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_RIGHT]) { lights_fourpanel |= 0x02; }
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_UP]) { lights_fourpanel |= 0x04; }
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_DOWN]) { lights_fourpanel |= 0x08; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_LEFT]) { lights_fourpanel |= 0x10; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_RIGHT]) { lights_fourpanel |= 0x20; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_UP]) { lights_fourpanel |= 0x40; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_DOWN]) { lights_fourpanel |= 0x80; }
	lights[XBEE_LIGHT_FRAME_FOURPANEL_POS] = lights_fourpanel;


	// Correct the frame before determining whether or not to send it
	correctFrame(lights, sizeof(lights));

	// Don't bother with duplicate frames
	if (!memcmp(lights, last_light_packet, sizeof(lights))) {
		return;
	}

	// Copy the light packet over
	memcpy(&last_light_packet, &lights, sizeof(last_light_packet));

	/*
	LOG->Trace("Packet: %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\n", 
		lights[0], lights[1], lights[2], lights[3], lights[4], lights[5], lights[6], lights[7], lights[8], lights[9], lights[10], lights[11],
		lights[12], lights[13], lights[14], lights[15], lights[16], lights[17], lights[18], lights[19], lights[20], lights[21], lights[22], lights[23]
	);
	*/

	port->Write(lights, sizeof(lights));
	port->Flush();
}

REGISTER_LIGHTS_DRIVER_CLASS(XBee);

/*
 * Copyright © 2018, Kate Gray
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
