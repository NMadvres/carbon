#ifndef __DUMMY_BCPU_H__
#define __DUMMY_BCPU_H__

#include "../../src/comm_def.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   dummy_bcpu
// Description: 板上CPU占位，模拟未命中报文去向并统计
// Group：预研组
// Author: Shawn
// Date: from 2021.10.27
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class dummy_bcpu: public sc_module
{
public:                                  // 例化及互联部分
    sc_in<s_pkt_desc> in_port_from_dut;  // 从 dut 的 egr 转发来的报文
    sc_out<s_pkt_desc> out_port_to_stat; // 报文透传到 stat 做统计

public:
    dummy_bcpu(sc_module_name name):
        sc_module(name)
    {
        SC_METHOD(passtrough);
        sensitive << in_port_from_dut;
        dont_initialize();
    }

    SC_HAS_PROCESS(dummy_bcpu);

    void passtrough()
    {
        auto pkt = in_port_from_dut.read();
        out_port_to_stat.write(pkt);
        MOD_LOG << "passthrough_bcpu_pkt" << pkt;
    }
};

#endif // __DUMMY_BCPU_H__
