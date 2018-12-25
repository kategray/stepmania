#include "global.h"
#include "LightsDriver_Win32ITGIO.h"
#include "windows.h"
#include "RageLog.h"
#include "RageUtil.h"
#include "ScreenManager.h"
#include <iomanip>

/*
 * StepMania 5 ITGIO Driver (using HIDAPI.dll)
 */

#define ITGIO_DEBUG

REGISTER_LIGHTS_DRIVER_CLASS(Win32ITGIO);

HINSTANCE hidDLL = nullptr;

LightsDriver_Win32ITGIO::LightsDriver_Win32ITGIO()
{
	hidDLL = LoadLibrary("hidapi.dll");
	if(nullptr == hidDLL)
	{
		MessageBox(nullptr, "Could not LoadLibrary (hidapi.dll).", "ERROR", MB_OK );
		return;

#ifdef ITGIO_DEBUG
		LOG->Info("ITGIO: Loaded HIDAPI.");
#endif
	}
	else {
		// Load Exports
		hid_init = (HID_INIT)GetProcAddress(hidDLL, "hid_init");
		hid_exit = (HID_EXIT)GetProcAddress(hidDLL, "hid_exit");
		hid_open = (HID_OPEN)GetProcAddress(hidDLL, "hid_open");
		hid_write = (HID_WRITE)GetProcAddress(hidDLL, "hid_write");

#ifdef ITGIO_DEBUG
		LOG->Info("ITGIO: HIDAPI Exports Loaded.");
#endif

		// Initialize HIDAPI
		if (HIDAPI_SUCCESS != hid_init()) {
			MessageBox(nullptr, "Unable to call hid_init().", "ERROR", MB_OK);
			return;
		};

		// Attempt to load the ITGIO
		ITGIO = hid_open(ITGIO_VENDOR, ITGIO_PRODUCT, NULL);
		if (nullptr == ITGIO) {
			LOG->Trace("ITGIO: hid_open() failed.");
			return;
		}
		else {
			itgio_initialized = TRUE;
#ifdef ITGIO_DEBUG
			LOG->Info("ITGIO: Connected to device.");
		}
	}
}

LightsDriver_Win32ITGIO::~LightsDriver_Win32ITGIO()
{
	if (HIDAPI_SUCCESS != hid_exit()) {
#ifdef ITGIO_DEBUG
		LOG->Info("ITGIO: HIDAPI Exit Successful.");
#endif
	}
	else {
		LOG->Trace("ITGIO: HIDAPI Exit Unsuccessful.");
	}
	FreeLibrary( hidDLL );
}

void LightsDriver_Win32ITGIO::Set( const LightsState *ls )
{
	uint8_t hidapi_report[ITGIO_REPORT_LENGTH + ITGIO_REPORT_ID_LENGTH];
	uint8_t last_report[ITGIO_REPORT_LENGTH + ITGIO_REPORT_ID_LENGTH];

	// Initialize the report to all zeroes
	memset(hidapi_report, 0u, sizeof(hidapi_report));

	// Set the proper report ID
	hidapi_report[ITGIO_REPORT_ID_POS] = ITGIO_REPORT_ID;

	/*
	 * Build the Report
	 */
	 // Player Lights
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_UP]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P1_UP; }
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_DOWN]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P1_DOWN; }
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_LEFT]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P1_LEFT; }
	if (ls->m_bGameButtonLights[GameController_1][DANCE_BUTTON_RIGHT]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P1_RIGHT; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_UP]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P2_UP; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_DOWN]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P2_DOWN; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_LEFT]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P2_LEFT; }
	if (ls->m_bGameButtonLights[GameController_2][DANCE_BUTTON_RIGHT]) { hidapi_report[ITGIO_LIGHT_PORT_0] |= ITGIO_LIGHT_P2_RIGHT; }

    // Cabinet lights
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_LEFT])  { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_MARQUEE_UP_LEFT; }
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_UP_RIGHT]) { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_MARQUEE_UP_RIGHT; }
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_LEFT])  { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_MARQUEE_LR_LEFT; }
	if (ls->m_bCabinetLights[LIGHT_MARQUEE_LR_RIGHT]) { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_MARQUEE_LR_RIGHT; }
	if (ls->m_bCabinetLights[LIGHT_BASS_LEFT] || ls->m_bCabinetLights[LIGHT_BASS_RIGHT]) {hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_SUB;}
	if (ls->m_bCoinCounter) { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_COIN; }
	if (ls->m_bGameButtonLights[GameController_1][GAME_BUTTON_START]) { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_CAB_P1; }
	if (ls->m_bGameButtonLights[GameController_2][GAME_BUTTON_START]) { hidapi_report[ITGIO_LIGHT_PORT_1] |= ITGIO_LIGHT_CAB_P2; }

	if (TRUE == itgio_initialized) {
		/*
		* Write the Report
		*/
		hid_write(ITGIO, hidapi_report, sizeof(hidapi_report));
	}
}
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
