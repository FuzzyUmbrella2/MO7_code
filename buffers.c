#include "buffers.h"
#include "defines.h"


typedef struct buffer
{
	int index;
	float bufferR[order];
	float bufferL[order];
} buffer;

buffer* inputBuffer;
buffer* outputBuffers;

void setupBuffer()
{
	inputBuffer = (buffer*)calloc(1, sizeof(buffer));
	if (inputBuffer == NULL)
	{
		printf("Failed to setup buffer.\n Not enough memory.\n");
		return;
	}

	outputBuffers = (buffer*)calloc(amount, sizeof(buffer));
	if (outputBuffers == NULL)
	{
		printf("Failed to setup buffers. \n Not enough memory\n");
		return;
	}
}

void gain(float gain)
{
	for(int nmr = 0; nmr<amount; nmr++)
	{
		int index = (outputBuffers+nmr)->index;
		(outputBuffers+nmr)->bufferR[index] = ((outputBuffers+nmr)->bufferR[index])*gain;
		(outputBuffers+nmr)->bufferL[index] = ((outputBuffers+nmr)->bufferL[index])*gain;
	}

}

void updateInputBuffer()
{
	s32 in_right = (Xil_In32(I2S_DATA_RX_R_REG)<<8);
	s32 in_left = (Xil_In32(I2S_DATA_RX_L_REG)<<8);
	float fRight = ((float)(in_right))/256;
	float fLeft = ((float)(in_left))/256;

	int index = inputBuffer->index;

	if (index+1>order)
	{
		index = 0;
		inputBuffer->index = 0;
	}
	else
	{
		index++;
		inputBuffer->index++;
	}

	inputBuffer->bufferR[index] = fRight;
	inputBuffer->bufferL[index] = fLeft;
}

void updateOutputBuffer(int nmr, float sumR, float sumL)
{
	int index = (outputBuffers+nmr)->index;
	if (index+1>order)
	{
		index = 0;
		(outputBuffers+nmr)->index = 0;
	}
	else
	{
		index++;
		(outputBuffers+nmr)->index++;
	}

	(outputBuffers+nmr)->bufferR[index] = sumR;
	(outputBuffers+nmr)->bufferL[index] = sumL;
}

void calculator()
{
	float sumR = 0;
	float sumL = 0;
	float inputR = 0;
	float inputL = 0;
	int index = 0;

	for (int nmr = 0; nmr<amount; nmr++)	//through all the filters
	{
		index = inputBuffer->index;
		for(int i = 0; i<order; i++)		//calculate sum of a0*x[n] ... a_max*x[n-order]
		{
			if(index+1>order)
			{
				index = 0;
			}
			else
			{
				index++;
			}
			inputR = inputBuffer->bufferR[index];
			inputL = inputBuffer->bufferL[index];
			sumR += inputR*x_terms[nmr][i];
			sumL += inputL*x_terms[nmr][i];
		}

		if (IIROrder[nmr] == 1)		//IIR filter
		{
			float outputR = 0;
			float outputL = 0;
			float ySumR = 0;
			float ySumL = 0;

			index = (outputBuffers+nmr)->index;

			for(int i = 1; i<order-1; i++)	//calculate sum of b0*y[n+1] ... b_max*y[n-order]
			{
				if(index+i>order-1)
				{
					index = 0;
				}
				outputR = (outputBuffers+nmr)->bufferR[index+i];
				outputL = (outputBuffers+nmr)->bufferL[index+i];
				ySumR += outputR*y_terms_IIR[nmr][i];
				ySumL += outputL*y_terms_IIR[nmr][i];
			}
			sumR = sumR-ySumR;
			sumL = sumL-ySumL;
		}

		updateOutputBuffer(nmr, sumR, sumL);
	}
}

void outputData()
{
	for(int nmr = 0; nmr<amount; nmr++)
	{
		int index = (outputBuffers+nmr)->index;
		float fRight = (outputBuffers+nmr)->bufferR[index];
		float fLeft = (outputBuffers+nmr)->bufferL[index];
		s32 out_right = (s32)(fRight*256);
		s32 out_left = (s32)(fLeft*256);

		Xil_Out32(I2S_DATA_TX_R_REG, (out_right>>8));
		Xil_Out32(I2S_DATA_TX_L_REG, (out_left>>8));
	}
}
