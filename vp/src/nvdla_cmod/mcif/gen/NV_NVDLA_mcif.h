// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: NV_NVDLA_mcif.h

#ifndef _NV_NVDLA_MCIF_H_
#define _NV_NVDLA_MCIF_H_

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <systemc.h>
#include <tlm.h>
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h" 

#include "../../../nvdla_cmod/include/nvdla_ness_header/nvdla_dma_wr_req_iface.h"
#include "../../../nvdla_cmod/include/scsim_common.h"
#include "../../../nvdla_cmod/nvdla_payload/nvdla_dbb_extension.h"
#include "../../../nvdla_cmod/nvdla_payload/dla_b_transport_payload.h"
#include "systemc.h"
#include "../../../nvdla_cmod/include/nvdla_config.h"

#include "../../../nvdla_cmod/include/NV_NVDLA_mcif_base.h"

#define MCIF_MAX_MEM_TRANSACTION_SIZE      (NVDLA_PRIMARY_MEMIF_WIDTH*NVDLA_PRIMARY_MEMIF_MAX_BURST_LENGTH/8)
#undef  MEM_TRANSACTION_SIZE 
#define MEM_TRANSACTION_SIZE                        (NVDLA_PRIMARY_MEMIF_WIDTH/8) 
#define DMA_TRANSACTION_SIZE                        (DMAIF_WIDTH)

#define AXI_ALIGN_SIZE                              (MEM_TRANSACTION_SIZE)
// NOTE: DMA_ATOMIC is different with DLA ATOMIC-M term: 1 DMA_ATOMIC means MIN_BUS_WIDTH bytes
// while DLA ATOMI_M size equals to DLA_ATOM_SIZE bytes
#define TRANSACTION_DMA_ATOMIC_NUM                  (DMAIF_WIDTH/MIN_BUS_WIDTH)
#define TAG_CMD                             0
#define TAG_DATA                            1


#define CDMA_DAT_AXI_ID 8
#define CDMA_WT_AXI_ID 9
#define BDMA_AXI_ID 0
#define SDP_AXI_ID 1
#define PDP_AXI_ID 2
#define CDP_AXI_ID 3
#define SDP_B_AXI_ID 5
#define SDP_N_AXI_ID 6
#define SDP_E_AXI_ID 7
#define RBK_AXI_ID 4


SCSIM_NAMESPACE_START(clib)
    // clib class forward declaration
SCSIM_NAMESPACE_END()

SCSIM_NAMESPACE_START(cmod)

struct client_wr_req_t {
    uint64_t addr;
    uint32_t size;
    uint32_t require_ack;
};

class NV_NVDLA_mcif:
    public  NV_NVDLA_mcif_base  // ports
{
    public:
        SC_HAS_PROCESS(NV_NVDLA_mcif);
        NV_NVDLA_mcif( sc_module_name module_name, bool headless_ntb_env_in, uint8_t nvdla_id_in );
        ~NV_NVDLA_mcif();

        bool             headless_ntb_env;
        uint8_t          nvdla_id;
        // Initiator Socket
        // # Write request
        // tlm::tlm_generic_payload                                            mcif2ext_wr_req_gp;
        tlm_utils::multi_passthrough_initiator_socket<NV_NVDLA_mcif, 512>   mcif2ext_wr_req;

        // # Read request
        // tlm::tlm_generic_payload                                            mcif2ext_rd_req_gp;
        tlm_utils::multi_passthrough_initiator_socket<NV_NVDLA_mcif, 512>   mcif2ext_rd_req;

        // Target Socket
        // # AXI Write response
        tlm_utils::multi_passthrough_target_socket<NV_NVDLA_mcif, 512>      ext2mcif_wr_rsp;
        virtual void ext2mcif_wr_rsp_b_transport(int ID, tlm::tlm_generic_payload& tlm_gp, sc_time& delay);

        // # AXI Read response
        tlm_utils::multi_passthrough_target_socket<NV_NVDLA_mcif, 512>      ext2mcif_rd_rsp;
        virtual void ext2mcif_rd_rsp_b_transport(int ID, tlm::tlm_generic_payload& tlm_gp, sc_time& delay);

        // Overload virtual target functions in base class
        void bdma2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void sdp2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void pdp2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void cdp2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void rbk2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void sdp_b2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void sdp_n2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void sdp_e2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void cdma_dat2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void cdma_wt2mcif_rd_req_b_transport   (int ID, nvdla_dma_rd_req_t*, sc_core::sc_time&);
        void bdma2mcif_wr_req_b_transport   (int ID, nvdla_dma_wr_req_t*, sc_core::sc_time&);
        void sdp2mcif_wr_req_b_transport   (int ID, nvdla_dma_wr_req_t*, sc_core::sc_time&);
        void pdp2mcif_wr_req_b_transport   (int ID, nvdla_dma_wr_req_t*, sc_core::sc_time&);
        void cdp2mcif_wr_req_b_transport   (int ID, nvdla_dma_wr_req_t*, sc_core::sc_time&);
        void rbk2mcif_wr_req_b_transport   (int ID, nvdla_dma_wr_req_t*, sc_core::sc_time&);


    private:
        // Variables
        bool is_there_ongoing_csb2mcif_response_;

        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *bdma_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *bdma_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * bdma2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * bdma_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2bdma_rd_rsp_fifo_;
        int32_t                 credit_mcif2bdma_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *sdp_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *sdp_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * sdp2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * sdp_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2sdp_rd_rsp_fifo_;
        int32_t                 credit_mcif2sdp_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *pdp_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *pdp_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * pdp2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * pdp_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2pdp_rd_rsp_fifo_;
        int32_t                 credit_mcif2pdp_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *cdp_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *cdp_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * cdp2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * cdp_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2cdp_rd_rsp_fifo_;
        int32_t                 credit_mcif2cdp_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *rbk_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *rbk_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * rbk2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * rbk_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2rbk_rd_rsp_fifo_;
        int32_t                 credit_mcif2rbk_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *sdp_b_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *sdp_b_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * sdp_b2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * sdp_b_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2sdp_b_rd_rsp_fifo_;
        int32_t                 credit_mcif2sdp_b_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *sdp_n_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *sdp_n_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * sdp_n2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * sdp_n_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2sdp_n_rd_rsp_fifo_;
        int32_t                 credit_mcif2sdp_n_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *sdp_e_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *sdp_e_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * sdp_e2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * sdp_e_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2sdp_e_rd_rsp_fifo_;
        int32_t                 credit_mcif2sdp_e_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *cdma_dat_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *cdma_dat_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * cdma_dat2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * cdma_dat_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2cdma_dat_rd_rsp_fifo_;
        int32_t                 credit_mcif2cdma_dat_rd_rsp_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *cdma_wt_rd_req_fifo_;
        sc_core::sc_fifo    <bool> *cdma_wt_rd_atom_enable_fifo_;

        // fifos storing number of atoms in read request
        sc_core::sc_fifo <uint32_t>  * cdma_wt2mcif_rd_req_atom_num_fifo_;
        scsim::clib::dla_b_transport_payload  * cdma_wt_rd_req_payload_;

        // Data return fifos (from mcif to clients)
        sc_core::sc_fifo <uint8_t*>  *mcif2cdma_wt_rd_rsp_fifo_;
        int32_t                 credit_mcif2cdma_wt_rd_rsp_fifo_;

        // Write response ack control
        uint32_t    bdma_wr_req_count_;
        uint32_t    bdma_wr_rsp_count_;
        //uint32_t    bdma_wr_req_expected_ack_id_;
        uint32_t    bdma_wr_req_expected_ack;
        bool        bdma_wr_req_ack_is_got_;
        uint32_t    bdma_wr_req_size_;
        uint32_t    bdma_wr_req_got_size_;
        bool        has_bdma_onging_wr_req_;
        sc_core::sc_fifo    <bool> *bdma_wr_required_ack_fifo_;
        //sc_core::sc_fifo    <uint32_t> *bdma_wr_cmd_count_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *bdma_wr_req_fifo_;

        // Client's write cmd to mcif
        sc_core::sc_fifo <client_wr_req_t*>  *bdma2mcif_wr_cmd_fifo_;

        // Client's write data to mcif (each entry is 32B)
        sc_core::sc_fifo <uint8_t*>  *bdma2mcif_wr_data_fifo_;
        // Write response ack control
        uint32_t    sdp_wr_req_count_;
        uint32_t    sdp_wr_rsp_count_;
        //uint32_t    sdp_wr_req_expected_ack_id_;
        uint32_t    sdp_wr_req_expected_ack;
        bool        sdp_wr_req_ack_is_got_;
        uint32_t    sdp_wr_req_size_;
        uint32_t    sdp_wr_req_got_size_;
        bool        has_sdp_onging_wr_req_;
        sc_core::sc_fifo    <bool> *sdp_wr_required_ack_fifo_;
        //sc_core::sc_fifo    <uint32_t> *sdp_wr_cmd_count_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *sdp_wr_req_fifo_;

        // Client's write cmd to mcif
        sc_core::sc_fifo <client_wr_req_t*>  *sdp2mcif_wr_cmd_fifo_;

        // Client's write data to mcif (each entry is 32B)
        sc_core::sc_fifo <uint8_t*>  *sdp2mcif_wr_data_fifo_;
        // Write response ack control
        uint32_t    pdp_wr_req_count_;
        uint32_t    pdp_wr_rsp_count_;
        //uint32_t    pdp_wr_req_expected_ack_id_;
        uint32_t    pdp_wr_req_expected_ack;
        bool        pdp_wr_req_ack_is_got_;
        uint32_t    pdp_wr_req_size_;
        uint32_t    pdp_wr_req_got_size_;
        bool        has_pdp_onging_wr_req_;
        sc_core::sc_fifo    <bool> *pdp_wr_required_ack_fifo_;
        //sc_core::sc_fifo    <uint32_t> *pdp_wr_cmd_count_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *pdp_wr_req_fifo_;

        // Client's write cmd to mcif
        sc_core::sc_fifo <client_wr_req_t*>  *pdp2mcif_wr_cmd_fifo_;

        // Client's write data to mcif (each entry is 32B)
        sc_core::sc_fifo <uint8_t*>  *pdp2mcif_wr_data_fifo_;
        // Write response ack control
        uint32_t    cdp_wr_req_count_;
        uint32_t    cdp_wr_rsp_count_;
        //uint32_t    cdp_wr_req_expected_ack_id_;
        uint32_t    cdp_wr_req_expected_ack;
        bool        cdp_wr_req_ack_is_got_;
        uint32_t    cdp_wr_req_size_;
        uint32_t    cdp_wr_req_got_size_;
        bool        has_cdp_onging_wr_req_;
        sc_core::sc_fifo    <bool> *cdp_wr_required_ack_fifo_;
        //sc_core::sc_fifo    <uint32_t> *cdp_wr_cmd_count_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *cdp_wr_req_fifo_;

        // Client's write cmd to mcif
        sc_core::sc_fifo <client_wr_req_t*>  *cdp2mcif_wr_cmd_fifo_;

        // Client's write data to mcif (each entry is 32B)
        sc_core::sc_fifo <uint8_t*>  *cdp2mcif_wr_data_fifo_;
        // Write response ack control
        uint32_t    rbk_wr_req_count_;
        uint32_t    rbk_wr_rsp_count_;
        //uint32_t    rbk_wr_req_expected_ack_id_;
        uint32_t    rbk_wr_req_expected_ack;
        bool        rbk_wr_req_ack_is_got_;
        uint32_t    rbk_wr_req_size_;
        uint32_t    rbk_wr_req_got_size_;
        bool        has_rbk_onging_wr_req_;
        sc_core::sc_fifo    <bool> *rbk_wr_required_ack_fifo_;
        //sc_core::sc_fifo    <uint32_t> *rbk_wr_cmd_count_fifo_;
        // Request fifos
        sc_core::sc_fifo <scsim::clib::dla_b_transport_payload*>  *rbk_wr_req_fifo_;

        // Client's write cmd to mcif
        sc_core::sc_fifo <client_wr_req_t*>  *rbk2mcif_wr_cmd_fifo_;

        // Client's write data to mcif (each entry is 32B)
        sc_core::sc_fifo <uint8_t*>  *rbk2mcif_wr_data_fifo_;


        // Payloads for write dma request

        // Delay
        sc_core::sc_time dma_delay_;
        sc_core::sc_time csb_delay_;
        sc_core::sc_time axi_delay_;

        // Events

        // Function declaration 
        void Reset();
        // # Threads
        void ReadRequestArbiter();
        void WriteRequestArbiter();


        void ReadResp_mcif2bdma();

        void ReadResp_mcif2sdp();

        void ReadResp_mcif2pdp();

        void ReadResp_mcif2cdp();

        void ReadResp_mcif2rbk();

        void ReadResp_mcif2sdp_b();

        void ReadResp_mcif2sdp_n();

        void ReadResp_mcif2sdp_e();

        void ReadResp_mcif2cdma_dat();

        void ReadResp_mcif2cdma_wt();


        void WriteRequest_bdma2mcif();

        void WriteRequest_sdp2mcif();

        void WriteRequest_pdp2mcif();

        void WriteRequest_cdp2mcif();

        void WriteRequest_rbk2mcif();

        void McifSendCsbResponse(uint8_t type, uint32_t data, uint8_t error_id);
};

SCSIM_NAMESPACE_END()


//extern "C" scsim::cmod::NV_NVDLA_mcif * NV_NVDLA_mcifCon(sc_module_name module_name, uint8_t nvdla_id_in);

#endif

