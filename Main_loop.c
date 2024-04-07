#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include <xil_io.h>
#include <sleep.h>
#include "xiicps.h"
#include "xgpio.h"
#include "xuartps.h"
#include "stdlib.h"

#include "buffers.h"
#include "audio.h"

u32 status_reg = 0;		// Buffer to store status register of I2S
u8 is_data_ready = 0;	// unsigned 8 bit integer used as a boolean to indicate if new audio sample is available from I2S

void loop()
{

	/*
	 * Wait for a new audio sample to be available (48KHz)
	 */
	while (is_data_ready == 0) {
		// A new audio sample is available when bit21 of I2S_STATUS_REG becomes 1
		// (see https://byu-cpe.github.io/ecen427/documentation/audio-hw/)
		status_reg = Xil_In32(I2S_STATUS_REG);
		// Read bit21 of register I2S_STATUS_REG
		is_data_ready = (status_reg >> 21 ) & 1;
	}
	is_data_ready = 0;
	// Clear bit21 of I2S_STATUS_REG, i.e. set bit21 to 0
	status_reg = status_reg & (u32)(~(1<<21));
	Xil_Out32(I2S_STATUS_REG, status_reg);

	updateInputBuffer();
	calculator();
	gain(0.2);
	outputData();

	loop();
}


int main()
{
    init_platform();

	//Configure the IIC data structure
	IicConfig(XPAR_XIICPS_1_DEVICE_ID);

	//Configure the Audio Codec's PLL
	AudioPllConfig();

	//Configure the Line in and Line out ports.
	//Call LineInLineOutConfig() for a configuration that
	//enables the HP jack too.
	AudioConfigureJacks();

	xil_printf("ADAU1761 configured\n\r");

	setupBuffer();

    loop();
    return 0;
}


