#ifndef __MOD_ING_H__
#define __MOD_ING_H__

#include "comm_def.h"
// FIXME 临时编译修改
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: mod_ing 入口ing模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_ing: public sc_module
{
public:
    sc_in<int> in_clk_cnt;
    std::array<sc_in<s_pkt_desc> *, G_INTER_NUM> in_port;
    // output
     sc_fifo_out<s_pkt_desc> out_cell_que;  
//    sc_out<s_pkt_desc> out_cell_que;

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
    int dport_id;
    int flow_id;
    int pkt_tmp_len = 0;
    int pkt_out_flag = 0;
    int pkt_head_flag = 0;
    s_flow_rule flow_rule;

    std::array<std::deque<s_pkt_desc>, G_INTER_NUM> fifo_port;
    std::array<int, G_INTER_NUM> pkt_count_port;
    std::array<int, G_INTER_NUM> infifo_count_port;
    std::array<int, G_INTER_NUM> drop_count_port;

    RR_SCH *rr_sch;
};
#endif // __MOD_ING_H__
