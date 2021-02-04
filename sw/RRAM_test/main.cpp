#include <string.h>
#include <iostream>


static char* const RRAM_START_ADDR = reinterpret_cast<char* const>(0x03000000);




int main() {


	uint16_t buffer[256]; // having 2 bytes for each value
	for (int i=0;i<=255;i++)
		{
			buffer[i]=0x0001;
			std::cout << "initial value of buffer ["<< i<<"] is "<<buffer[i]<< std::endl;
		}



	// copying values from RRAM to buffer should 0 as RRAM is initialized to NULL
	memcpy(buffer, RRAM_START_ADDR,512); //Invoking TLM Read command
	for (int i=0;i<=255;i++)
		{
		std::cout << "value of buffer ["<< i<<"] copied from RRAM is "<<buffer[i]<< std::endl;
		}


	// Putting 0xFFFF in all buffer elements
	for (int i=0;i<=255;i++)
		{
		buffer[i] = 0xFFFF;
		}

	// copying values from buffer to RRAM
	memcpy(RRAM_START_ADDR, buffer,512); //Invoking TLM write command

	// reset the values in buffer
	for (int i=0;i<=255;i++)
		{
		buffer[i] = 0x0000;
		}

	// copying values from RRAM to buffer should be multiplied
	memcpy(buffer, RRAM_START_ADDR,512); //Invoking TLM Read command
	for (int i=0;i<=255;i++)
		{
		std::cout << "value of buffer ["<< i<<"] copied from RRAM is "<<buffer[i]<< std::endl;
		}


	return 0;
}



