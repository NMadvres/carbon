#ifndef __MOD_PE_H__
#define __MOD_PE_H__

#include "comm_def.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_pe
// Description: PE 模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_pe: public sc_module
{
public:
    mod_pe(sc_module_name name);
    SC_HAS_PROCESS(mod_pe);

    void on_recv_cell();
    void on_send_pkt();
    void rev_fc_process();

public:
    sc_fifo_in<s_pkt_desc> in_cell_que;
    sc_out<s_pkt_desc> out_cell_que;
    sc_in<int> in_clk_cnt;

    //发送流控给sch同时接收egress的流控
    sc_out<int> out_pe_busy;
    sc_in<int> in_fc_port;
    int fc_status;

private:
    const int clk_gap;
    int clk_wait;
    bool is_busy;
    std::deque<s_pkt_desc> pkt_que;
};

#endif // __MOD_PE_H__
