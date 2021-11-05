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
mod_egr::mod_egr(sc_module_name name, func_stat *base_top_stat):
    sc_module(name),
    clk_gap(G_FREQ_MHZ)
{
    //for stat
    top_stat = base_top_stat;

    for (int i = 0; i < G_INTER_NUM; i++) {
        out_port[i] = new sc_out<s_pkt_desc>();
        port_token_bucket[i] = clk_gap;
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
    int port_speed_tmp = g_port_rule_tab[port];
    if (port_speed_tmp < port_token_bucket[port]) {
        port_token_bucket[port] = port_speed_tmp;
    }
    return port_token_bucket[port];
}

void mod_egr::rev_pkt_process()
{
    if (in_port.event()) {
        s_pkt_desc tmp_pkt = in_port->read();
        fifo_port.push_back(tmp_pkt);
        get_token(tmp_pkt.dport);
        add_token(tmp_pkt.len, tmp_pkt.dport);
        top_stat->input_comm_stat_func(tmp_pkt);
    }
}

void mod_egr::send_pkt_process()
{
    if (fifo_port.empty())
        return;

    s_pkt_desc &pkt = fifo_port.front();
    if ((pkt.len > get_token(pkt.dport)))
        return;

    //增加时戳信息
    pkt.time_stamp.egr_out_clock = g_cycle_cnt;
    port_send_bytes[pkt.dport] += pkt.len;

    //for stat output bw
    top_stat->output_comm_stat_func(pkt);
    int top_delay = pkt.time_stamp.egr_out_clock - pkt.time_stamp.stm_out_clock;
    top_stat->record_comm_latency_func(top_delay);

    out_port[pkt.dport]->write(pkt);
    fifo_port.pop_front();
    sub_token(pkt.len, pkt.dport);
}
