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
    cycle_cnt = 0;
    is_busy = false;

    for (int i = 0; i < G_INTER_NUM; i++) {
        out_port[i] = new sc_out<s_pkt_desc>();
        port_token_bucket[i] = clk_gap;
    }
    fifo_port.resize(G_INTER_NUM);

    assert(clk_gap > 0);

    SC_METHOD(rev_pkt_process);
    sensitive << in_port;
    dont_initialize();

    SC_METHOD(send_pkt_process);
    sensitive << in_clk_cnt;
    dont_initialize();

    SC_METHOD(send_pkt_to_cpu_process);
    sensitive << in_clk_cnt;
    dont_initialize();

    SC_METHOD(rev_pkt_from_ing_process);
    sensitive << in_clk_cnt;
    dont_initialize();
}

void mod_egr::rev_pkt_from_ing_process()
{
    if (in_pkt_bcpu.num_available()) {
        s_pkt_desc pkt;
        in_pkt_bcpu.nb_read(pkt);
        if (pkt.type != DESC_TYPE_PKT) {
            MOD_LOG_ERROR << "Packet type error" << pkt;
            return;
        }

        fifo_bcpu.push_back(pkt);
    }
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
    if (in_port.event()) {
        s_pkt_desc tmp_pkt = in_port->read();
        int dport_id = tmp_pkt.dport;
        fifo_port[dport_id].push_back(tmp_pkt);
        MOD_LOG << "Egress recv packet from PE" << tmp_pkt;
    }
}

void mod_egr::send_pkt_process()
{
    //填桶，每周期填充1B，100M，就是100MBPS，现在100拍填1次，1次填N个Byte，就是N MBPS
    if (cycle_cnt % 100 == 0) {
        for (int port_id = 0; port_id < G_INTER_NUM; port_id++) {
            int token_value = g_port_rule_tab[port_id];
            add_token(token_value, port_id);
        }
    }
    cycle_cnt++;

    //流控状态判断
    for (int port_id = 0; port_id < G_INTER_NUM; port_id++) {
        if (fifo_port[port_id].size() >= 50) {
            if (is_busy == false) {
                is_busy = true;
                out_egress_busy.write(1);
                MOD_LOG << "Egress generate flow ctrl to PE";
            }
        }
    }

    //4个端口轮询，获取发送
    for (int port_id = 0; port_id < G_INTER_NUM; port_id++) {
        if (fifo_port[port_id].empty())
            continue;
        s_pkt_desc &pkt = fifo_port[port_id].front();

        if ((pkt.len > get_token(pkt.dport)))
            continue;

        //增加时戳信息
        pkt.time_stamp.egr_out_clock = g_cycle_cnt;
        //port_send_bytes[pkt.dport] += pkt.len;

        //for stat output bw
        top_stat->input_comm_stat_func(pkt);
        int top_delay = pkt.time_stamp.egr_out_clock - pkt.time_stamp.stm_out_clock;
        top_stat->record_comm_latency_func(pkt, top_delay);

        out_port[pkt.dport]->write(pkt);
        fifo_port[port_id].pop_front();
        sub_token(pkt.len, pkt.dport);

        //流控状态判断
        if (fifo_port[port_id].size() < 50) {
            if (is_busy == true) {
                is_busy = false;
                out_egress_busy.write(0);
                MOD_LOG << "Egress cancel flow ctrl to PE" << pkt;
            }
        }
    }
}

void mod_egr::send_pkt_to_cpu_process()
{
    if (fifo_bcpu.empty())
        return;

    s_pkt_desc &pkt = fifo_bcpu.front();
    if (pkt.qid == -2) {
        MOD_LOG << "egr_out_bcpu_pkt" << pkt;
        out_bcpu->write(pkt);
        fifo_bcpu.pop_front();
    }
}
