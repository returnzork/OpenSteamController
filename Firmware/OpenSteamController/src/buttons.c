/**
 * \file buttons.c
 * \brief Encompasses functions for reading Steam Controller digital button 
 *	states.
 *
 * MIT License
 *
 * Copyright (c) 2018 Gregory Gluszek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "buttons.h"

#include "lpc_types.h"
#include "chip.h"
#include "gpio_11xx_1.h"
#include "usb.h"
#include "time.h"

#include "eeprom_access.h"
#include <stdlib.h>

#include <stdio.h>

#define GPIO_ANALOG_JOY_CLICK 1, 0
#define GPIO_L_GRIP 1, 25
#define GPIO_STEAM_BTN 1, 19
#define GPIO_L_TRACKPAD 1, 26
#define GPIO_L_TRIGGER 1, 27
#define GPIO_L_BUMPER 1, 4
#define GPIO_FRONT_L 1, 20
#define GPIO_FRONT_R 1, 2
#define GPIO_R_TRACKPAD 1, 21
#define GPIO_Y_BTN 1, 11
#define GPIO_R_TRIGGER 1, 13
#define GPIO_R_BUMPER 1, 14
#define GPIO_R_GRIP 1, 3
#define GPIO_B_BTN 1, 22
#define GPIO_X_BTN 1, 9
#define GPIO_A_BTN 0, 17


#define OPTIONS_OFFSET (0x100)
int analogJoyClickEnabled = 1;
int leftGripEnabled = 1;
int steamButtonEnabled = 1;
int leftTrackpadEnabled = 1;
int leftTriggerEnabled = 1;
int leftBumperEnabled = 1;
int frontLeftEnabled = 1;
int frontRightEnabled = 1;
int rightTrackpadEnabled = 1;
int yButtonEnabled = 1;
int rightTriggerEnabled = 1;
int rightBumperEnabled = 1;
int rightGripEnabled = 1;
int bButtonEnabled = 1;
int xButtonEnabled = 1;
int aButtonEnabled = 1;



/**
 * Initialize GPIOs used to read button states.
 *
 * \return None.
 */
void initButtons(void) {
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_ANALOG_JOY_CLICK, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_GRIP, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_STEAM_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRACKPAD, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_TRIGGER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_L_BUMPER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_FRONT_L, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_FRONT_R, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRACKPAD, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_Y_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_TRIGGER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_BUMPER, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_R_GRIP, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_B_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_X_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_PinMux(LPC_IOCON, GPIO_A_BTN, IOCON_HYS_EN | 
		IOCON_MODE_PULLUP, IOCON_FUNC0);


	void* ptr = malloc(sizeof(int));
	for(int i = 0; i < 16; i++)
	{
		eepromRead(OPTIONS_OFFSET, ptr, 8);
		switch((uint8_t)ptr) {
		case 0:
			analogJoyClickEnabled = 0;
			break;
		case 1:
			leftGripEnabled = 0;
			break;
		case 2:
			steamButtonEnabled = 0;
			break;
		case 3:
			leftTrackpadEnabled = 0;
			break;
		case 4:
			leftTriggerEnabled = 0;
			break;
		case 5:
			leftBumperEnabled = 0;
			break;
		case 6:
			frontLeftEnabled = 0;
			break;
		case 7:
			frontRightEnabled = 0;
			break;
		case 8:
			rightTrackpadEnabled = 0;
			break;
		case 9:
			yButtonEnabled = 0;
			break;
		case 10:
			rightTriggerEnabled = 0;
			break;
		case 11:
			rightBumperEnabled = 0;
			break;
		case 12:
			rightGripEnabled = 0;
			break;
		case 13:
			bButtonEnabled = 0;
			break;
		case 14:
			xButtonEnabled = 0;
			break;
		case 15:
			aButtonEnabled = 0;
			break;
		}
	}

	free(ptr);
}

/**
 * \return true if Steam Button is being pressed. False otherwise.
 */
int getSteamButtonState(void) {
	return steamButtonEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_STEAM_BTN);
}

/**
 * \return true if Front Left Arrow Button is being pressed. False otherwise.
 */
int getFrontLeftButtonState(void) {
	return frontLeftEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_FRONT_L);
}

/**
 * \return true if Front Right Arrow Button is being pressed. False otherwise.
 */
int getFrontRightButtonState(void) {
	return frontRightEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_FRONT_R);
}

/**
 * \return true if Analog Joystick Click Button is being pressed. False otherwise.
 */
int getJoyClickState(void) {
	return analogJoyClickEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_ANALOG_JOY_CLICK);
}

/**
 * \return true if X Button is being pressed. False otherwise.
 */
int getXButtonState(void) {
	return xButtonEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_X_BTN);
}

/**
 * \return true if Y Button is being pressed. False otherwise.
 */
int getYButtonState(void) {
	return yButtonEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_Y_BTN);
}

/**
 * \return true if B Button is being pressed. False otherwise.
 */
int getBButtonState(void) {
	return bButtonEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_B_BTN);
}

/**
 * \return true if A Button is being pressed. False otherwise.
 */
int getAButtonState(void) {
	return aButtonEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_A_BTN);
}

/**
 * \return true if Inner Right Grip Button is being pressed. False otherwise.
 */
int getRightGripState(void) {
	return rightGripEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_GRIP);
}

/**
 * \return true if Inner Left Grip Button is being pressed. False otherwise.
 */
int getLeftGripState(void) {
	return leftGripEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_GRIP);
}

/**
 * \return true if Right Trackpad Click Button is being pressed. False otherwise.
 */
int getRightTrackpadClickState(void) {
	return rightTrackpadEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRACKPAD);
}

/**
 * \return true if Left Trackpad Click Button is being pressed. False otherwise.
 */
int getLeftTrackpadClickState(void) {
	return leftTrackpadEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_TRACKPAD);
}

/**
 * \return true if Right Digital Trigger Button is being pressed. False otherwise.
 */
int getRightTriggerState(void) {
	return rightTriggerEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_TRIGGER);
}

/**
 * \return true if Left Digital Trigger Button is being pressed. False otherwise.
 */
int getLeftTriggerState(void) {
	return leftTriggerEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_TRIGGER);
}

/**
 * \return true if Right Bumper Button is being pressed. False otherwise.
 */
int getRightBumperState(void) {
	return rightBumperEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_R_BUMPER);
}

/**
 * \return true if Left Bumper Button is being pressed. False otherwise.
 */
int getLeftBumperState(void) {
	return leftBumperEnabled && !Chip_GPIO_ReadPortBit(LPC_GPIO, GPIO_L_BUMPER);
}

/**
 * Print command usage details to console.
 *
 * \return None.
 */
void buttonsCmdUsage(void) {
	printf(
		"usage: buttons\n"
		"\n"
		"Enter a loop giving updates on all digital button states.\n"
		"Press any key to exit loop.\n"
	);
}

/**
 * Handle command line function.
 *
 * \param argc Number of arguments (i.e. size of argv)
 * \param argv Command line entry broken into array argument strings.
 *
 * \return 0 on success.
 */
int buttonsCmdFnc(int argc, const char* argv[]) {
	printf("Digital Button States (Press any key to exit):\n");
	printf("Legend:\n");
	printf("\tLB/RB = Left/Right Bumper\n");
	printf("\tLT/RT = Left/Right Trigger\n");
	printf("\tLTP/RTP = Left/Right Trackpad Click\n");
	printf("\tJoy = Joystick Click\n");
	printf("\tLG/RG = Left/Right Grip\n");
	printf("\tLA/RA = Left/Right Arrow\n");
	printf("\n");
	printf("Time       LB LT LTP Joy LG LA Steam X Y A B RA RG RTP RT RB\n");
	printf("------------------------------------------------------------\n");

	while (!usb_tstc()) {

		printf("0x%08x ", getUsTickCnt());

		printf(" %d ", getLeftBumperState());
		printf(" %d ", getLeftTriggerState());
		printf("  %d ", getLeftTrackpadClickState());
		printf("  %d ", getJoyClickState());
		printf(" %d ", getLeftGripState());
		printf(" %d ", getFrontLeftButtonState());
		printf("    %d ", getSteamButtonState());
		printf("%d ", getXButtonState());
		printf("%d ", getYButtonState());
		printf("%d ", getAButtonState());
		printf("%d ", getBButtonState());
		printf(" %d ", getFrontRightButtonState());
		printf(" %d ", getRightGripState());
		printf("  %d ", getRightTrackpadClickState());
		printf(" %d ", getRightTriggerState());
		printf(" %d", getRightBumperState());

		printf("\r");
		usb_flush();

		usleep(20000);
	}

	return 0;
}
