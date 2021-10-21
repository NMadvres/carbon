#ifndef __MOD_STIM_H__
#define __MOD_STIM_H__
#include "comm_def.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_stim
// Description: mod_stim stim激励模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_stim: public sc_module
{
public:
    mod_stim(sc_module_name name);
    SC_HAS_PROCESS(mod_stim);

public:
    array<sc_out<s_pkt_desc> *, g_inter_num> out_pkt_ports;
    sc_in<int> in_clk_cnt;
    void process_gen_packet();
};

#endif