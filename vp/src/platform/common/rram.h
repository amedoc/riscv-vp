#ifndef VP_SRC_PLATFORM_COMMON_RRAM_H_
#define VP_SRC_PLATFORM_COMMON_RRAM_H_

#include <unistd.h>
#include <string.h> // for using memcpy

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

using namespace std;

#define SC_INCLUDE_DYNAMIC_PROCESSES			//The simple sockets spawn dynamic processes



struct rram : public sc_core::sc_module
{

	tlm_utils::simple_target_socket<rram> tsock; // Declaring a socket for he rram



  //-----------Internal variables-------------------
  uint16_t (*rram_data)= new uint16_t[256];		// array[256] of 16 bits cells, inner data of the RRAM
  uint32_t m_size = 0x00000200;     			//512x8= 256x16 ;






  //-----------Methods------------------------------


//The target (RRAM) should copy data FROM the data array in the bus
//Writing data in the RRAM
  void  write_data (unsigned addr,const uint8_t *src, unsigned num_bytes)
  {
	  assert(addr + num_bytes <= m_size); // to test if data sent is not greater than rram size

	  //copying data from the memory array where the starting address of RRAM is assigned
	  memcpy(rram_data + addr/2, src, num_bytes); //4 bytes from the bus  will be written in the RRAM (first two cells)
	  	  	  	  	  	  	  	  	  	  	  	  // addr = i*4

  }

//the target (RRAM) should copy data TO the data array in the bus
// reading from RRAM and writing to the bus
  void  read_data(unsigned addr, uint8_t* dst, unsigned num_bytes)
  {
	 assert(addr + num_bytes <= m_size); // to test if data sent is not greater than rram size
	 memcpy(dst, rram_data + addr/2, num_bytes); // 4 bytes from the RRAM (first two cells) will be written in the bus
	 	 	 	 	 	 	 	 	 	 	 	 // addr = i*4
  }


// Reset the internal data of the RRAM
  void  reset_data ()
  {
	  uint16_t null_cell = 0x0000;

	  for(int i=0;i<256;i++)
	  {
		  *(rram_data+i) = null_cell;
	  }
  }

  // Multiplication algorithm via shifting and addition
  // m bit multiplicand * n bit multiplier == (m+n) bit product
  // accomplished via shifting and addition
  uint32_t multiply_two_cells(uint16_t cell_1,uint16_t cell_2)
  {

  	uint32_t result= 0x00000000;
  	result = cell_1 * cell_2 ;

     /* while( cell_2)
      {
          if(cell_2 & 1)
          {
          	result = result + cell_1;
          }
          cell_1 = cell_1 << 1;
          cell_2 = cell_2 >> 1;
      }
*/
      return result;
  }

  // Multiply operation of each adjacent two cells of the RRAM
  void multiply_data(unsigned addr)
    {
	  // we will use a temporary array to store the multiplication result
	  //then slice it to store it in the  adjacent cells

	  	  uint32_t tmp = 0x00000000 ;
	      tmp = multiply_two_cells( *(rram_data +addr/2) , *(rram_data + (addr/2)+1) );
	      *(rram_data +addr/2) = tmp & 0xFFFF;
	      *(rram_data + (addr/2)+1) = tmp >> 16;

    }




  // Transport function used to pass payload to other systemC modules with a delay
  void transport(tlm::tlm_generic_payload &trans , sc_core::sc_time &delay)
	{
		tlm::tlm_command cmd = trans.get_command();

		uint32_t addr = trans.get_address();			// the start address in the system memory map of
														// contiguous block of data being read or written
														// for RRAM must be 1 byte = 8 bits and in VP declared as 4 bytes

		auto *ptr = trans.get_data_ptr();				//The data pointer attribute points to a data buffer within the initiator(Bus)

		auto len = trans.get_data_length();				// gives the length of data in bytes



		if (cmd == tlm::TLM_WRITE_COMMAND) {
			write_data(addr,ptr,len);

		} else if (cmd == tlm::TLM_READ_COMMAND) {
			multiply_data(addr);
			read_data(addr,ptr,len);

		} else {
			sc_assert(false && "unsupported tlm command");
		}

		delay += sc_core::sc_time(10, sc_core::SC_NS);
	}


  ~rram(void) {
		delete rram_data;
	}


  //-----------Constructor--------------------------
  rram(sc_core::sc_module_name)
  {
	  	reset_data();
		tsock.register_b_transport(this, &rram::transport);


  }

};


//////////////////////////////////////////////////////////////////////
//-----------RRAM address in the memory map of risc-v vp-------------

//OFFSET = 0x01FF as mem_size = 4kbits = 256 * 16 = 2^9 * 8
//addr_t rram_start_addr = 0x03000000;
//addr_t rram_end_addr = mem_start_addr + OFFSET = 0x30001FF;

///////////////////////////////////////////////////////////////////////




#endif /* VP_SRC_PLATFORM_COMMON_RRAM_H_ */
