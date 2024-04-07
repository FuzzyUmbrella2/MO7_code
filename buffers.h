#ifndef __BUFFERS_H_
#define __BUFFERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <xil_io.h>

#include "audio.h"

void setupBuffer();
void gain();
void updateInputBuffer();
void calculator();

#endif
