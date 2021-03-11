// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: fp16_min.cpp

#include "../../../nvdla_cmod/hls/vlibs/vlibs.h"
#ifdef SYNTHESIS
#pragma hls_design top
#endif
void HLS_fp16_min (
     ac_channel<vFp16Type>  & chn_a
    ,ac_channel<vFp16Type>  & chn_b
    ,ac_channel<vFp16Type>  & chn_o
    )
{
    vFp16Type  a = chn_a.read();
    vFp16Type  b = chn_b.read();
   
    vFp16Type o = FpMin<vFp16ExpoSize,vFp16MantSize>(a,b);
    chn_o.write(o);
}
