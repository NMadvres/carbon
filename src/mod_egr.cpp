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
    sc_module(name),
    clk_gap(G_FREQ_MHZ)
{
    for (int i = 0; i < G_INTER_NUM; i++) {
        out_port[i] = new sc_out<s_pkt_desc>();
        port_token_bucket[i] = port_token_bucket[i];
    }

    assert(clk_gap > 0);

    SC_METHOD(rev_pkt_process);
    sensitive << in_port;
    dont_initialize();

    SC_METHOD(send_pkt_process);
    sensitive << in_clk_cnt;
    dont_initialize();
}

void mod_egr::add_token(const int &add_token_val, const int port)
{
    if ((port_token_bucket[port] + add_token_val) > TOKEN_MAX_BYTE)
        port_token_bucket[port] = TOKEN_MAX_BYTE;
    else
        port_token_bucket[port] = port_token_bucket[port] + add_token_val;
}

void mod_egr::sub_token(const int &sub_token_val, const int port)
{
    if ((port_token_bucket[port] - sub_token_val) < 0)
        port_token_bucket[port] = 0;
    else
        port_token_bucket[port] = port_token_bucket[port] - sub_token_val;
}

int mod_egr::get_token(const int port)
{
    return port_token_bucket[port];
}

void mod_egr::rev_pkt_process()
{
    fifo_port.push_back(in_port->read());
}

void mod_egr::send_pkt_process()
{
    if (fifo_port.empty())
        return;

    s_pkt_desc &pkt = fifo_port.front();
    add_token(pkt.len, pkt.dport);

    port_token_bucket[pkt.dport] = g_port_rule_tab[pkt.dport];
    if ((pkt.len > get_token(pkt.dport)))
        return;

    //增加时戳信息
    pkt.time_stamp.egr_out_clock = g_cycle_cnt;
    port_send_bytes[pkt.dport] += pkt.len;

    out_port[pkt.dport]->write(pkt);
    fifo_port.pop_front();
    sub_token(pkt.len, pkt.dport);
}
