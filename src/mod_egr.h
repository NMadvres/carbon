#ifndef __MOD_EGR_H__
#define __MOD_EGR_H__

#include "comm_def.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: mod_ing 入口ing模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////

#define TOKEN_MAX_BYTE 125 * 100 * 2

class mod_egr: public sc_module
{
public:
    mod_egr(sc_module_name name, func_stat *base_top_stat);
    SC_HAS_PROCESS(mod_egr);
    void rev_pkt_process(void);
    void send_pkt_process(void);

    void rev_pkt_from_ing_process(void);
    void send_pkt_to_cpu_process(void);

public:
    sc_in<s_pkt_desc> in_port;
    std::array<sc_out<s_pkt_desc> *, G_INTER_NUM> out_port;

    sc_out<s_pkt_desc> out_bcpu;
    sc_in<int> in_clk_cnt;

    sc_fifo_in<s_pkt_desc> in_pkt_bcpu;

    vector<std::deque<s_pkt_desc>> fifo_port;
    std::deque<s_pkt_desc> fifo_bcpu;
    //fc signal
    sc_out<int> out_egress_busy;
    bool is_busy;

    int pkt_count_port;
    func_stat *top_stat;
    int cycle_cnt;

private:
    std::array<int, G_INTER_NUM> port_send_bytes;
    std::array<int, G_INTER_NUM> port_token_bucket = {0};
    const int clk_gap;

public:
    void add_token(const int &add_token_val, const int port);
    void sub_token(const int &add_token_val, const int port);
    int get_token(const int port);
};
#endif // __MOD_EGR_H__
