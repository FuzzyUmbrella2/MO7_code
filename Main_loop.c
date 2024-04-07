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

void loop()
{
	setupBuffer();
	updateInputBuffer();
	calculator();
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




    loop();
    return 0;
}


