#ifndef __MOD_ING_H__
#define __MOD_ING_H__
#include "comm_def.h"
#include "comm_func.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: mod_ing 入口ing模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_ing : public sc_module
{
public:
    sc_in<int> clkcnt;
    array<sc_in<s_pkt_desc> *, g_inter_num> in_port;
    // output
    sc_out<s_pkt_desc> out_cell_que;

public:
    mod_ing(sc_module_name mod_ing);
    SC_HAS_PROCESS(mod_ing);
    void main_process();
    void rev_pkt_process();
    void port_rr_sch_process();
    void lut_process();
    void pkt_to_cell_process();

private:
    s_pkt_desc s_port_sch_result;
    int que_id;
    int flow_id;
    int pkt_tmp_len = 0;
    int pkt_out_flag = 0;
    s_flow_rule flow_rule;

    array<deque<s_pkt_desc>, g_inter_num>
        fifo_port;
    array<int, g_inter_num> pkt_count_port;
    array<int, g_inter_num> infifo_count_port;
    array<int, g_inter_num> drop_count_port;

    RR_SCH *rr_sch;
};
#endif