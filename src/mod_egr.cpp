#include "mod_egr.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_egr
// Description: mod_egr 出口egr模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_egr::mod_egr(sc_module_name name):
    sc_module(name)
{
    for (int i = 0; i < G_INTER_NUM; i++) {
        out_port[i] = new sc_out<s_pkt_desc>();
        pkt_count_port = 0;
    }

    SC_METHOD(rev_pkt_process);
    sensitive << in_port;
    dont_initialize();

    SC_METHOD(send_pkt_process);
    sensitive << in_clk_cnt;
    dont_initialize();
}

void mod_egr::rev_pkt_process()
{
    fifo_port.push_back(in_port->read());
    pkt_count_port++;
}

void mod_egr::send_pkt_process()
{
    s_tab_port port_rule;

    if (fifo_port.empty())
        return;
    s_pkt_desc &pkt = fifo_port.front();

    port_rule = g_port_rule_tab[pkt.dport];
    //增加时戳信息
    pkt.time_stamp.egr_out_clock = g_cycle_cnt;
    out_port[pkt.dport]->write(pkt);
    fifo_port.pop_front();
    (void)port_rule;
}
