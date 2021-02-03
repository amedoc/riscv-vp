#ifndef VP_SRC_PLATFORM_COMMON_RRAM_H_
#define VP_SRC_PLATFORM_COMMON_RRAM_H_

#include <unistd.h>
#include <string.h> // for using memcpy

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

using namespace std;

#define SC_INCLUDE_DYNAMIC_PROCESSES			//The simple sockets spawn dynamic processes



#define DATA_WIDTH        16    				// Data space in a cell
#define ADDR_WIDTH        8     				// 2^8 = 256 --> rram can store 256  cells == 256*16 == 4096 bits
                                				// data space in the chip = 4 kbits


struct rram : public sc_core::sc_module
{

	tlm_utils::simple_target_socket<rram> tsock; // Declaring a socket/port for he rram



  //-----------Internal variables-------------------
  uint16_t (*rram_data)= new uint16_t[256];		// array[256] of 16 bits cells, inner data of the RRAM
  //uint32_t m_size = 256 ;






  //-----------Methods------------------------------


  //The target (RRAM) should copy data FROM the data array in the bus
  //Writing data in the RRAM
  void  write_data (unsigned addr,const uint8_t *src, unsigned num_bytes)
  {
	  std::cout << "@" << sc_time_stamp() <<" Start copying data from bus to the RRAM (Write operation) " << endl; // logging message

	  //assert(addr + num_bytes <= m_size); // to test if data sent is not greater to rram size

	  //memcpy(rram_data,src+addr,num_bytes);
	  //copying data from the memory array where the starting address of RRAM is assigned

		memcpy(rram_data + addr, src, num_bytes);

	 std::cout << "@" << sc_time_stamp() <<" Copying data from BUS to RRAM (Write operation) is finished " << endl; // logging message
  }

//the target (RRAM) should copy data TO the data array in the bus
// reading from RRAM and writing to the bus
  void  read_data(unsigned addr, uint8_t* dst, unsigned num_bytes)
  {
	  std::cout << "@" << sc_time_stamp() <<" Start copying data from RRAM to the bus (Read operation)" << endl; // logging message

	  //assert(addr + num_bytes <= m_size); // to test if data sent is not greater to rram size

	  multiply_data();

	  //memcpy(dst+addr,rram_data,num_bytes);
	  memcpy(dst, rram_data + addr, num_bytes);

	  std::cout << "@" << sc_time_stamp() <<" Copying data from RRAM to the bus (Read operation) is finished" << endl; // logging message
  }


  // Reset the internal data of the RRAM
  void  reset_data ()
  {
	  std::cout << "@"<< sc_time_stamp() <<" Start setting all inner cells to null" << endl; //logging message
	  uint16_t null_cell = 0x0000;

	  for(int i=0;i<256;i++)
	  {
		  *(rram_data+i) = null_cell;
	  }
	  std::cout << "@" << sc_time_stamp() <<" Finish setting all inner cells to null" << endl; //logging message
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
  void  multiply_data()
  // we will use a temporary array to store the multiplication result
  //then slice it to store it in the  adjacent cells
  {
	  std::cout << "@" << sc_time_stamp() <<" Start of inner cells multiplication" << endl; // logging message
      for(int i=0;i<255;i=i+2)
      {
      	uint32_t tmp = 0x00000000 ;
        tmp = multiply_two_cells( *(rram_data + i) , *(rram_data + (i+1)) );
        *(rram_data + i) = tmp & 0xFFFF;
        *(rram_data + (i+1)) = tmp >> 16;
      }
      std::cout << "@" << sc_time_stamp() <<" Multiplication is finished" << endl; // logging message
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
//-----------------------RISC-V VP memory map-------------------------

//addr_t mem_size = 1024 * 1024 * 32;  // 32 MB ram, to place it before the CLINT

//addr_t mem_start_addr = 0x00000000;
//addr_t mem_end_addr = mem_start_addr + mem_size - 1;
//addr_t clint_start_addr = 0x02000000;
//addr_t clint_end_addr = 0x0200ffff;
//addr_t sys_start_addr = 0x02010000;
//addr_t sys_end_addr = 0x020103ff;

//-----------RRAM address in the memory map of risc-v vp-------------

// OFFset = 0x00FF as mem_size = 4kbits = 256 * 16 = 2^8 * 16
//addr_t rram_start_addr = 0x03000000;
//addr_t rram_end_addr = mem_start_addr + OFFset = 0x30000FF;

///////////////////////////////////////////////////////////////////////




#endif /* VP_SRC_PLATFORM_COMMON_RRAM_H_ */
