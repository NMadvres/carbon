#ifndef __MOD_SCH_H__
#define __MOD_SCH_H__

#include "comm_def.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: mod_sch sch模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_sch: public sc_module
{
public:
    mod_sch(sc_module_name name);
    SC_HAS_PROCESS(mod_sch);

public:
    sc_in<s_pkt_desc> in_cell_que;
    sc_out<s_pkt_desc> out_cell_que;
    sc_in<int> in_clk_cnt;
    vector<deque<s_pkt_desc>> input_cell_que;
    vector<int> input_drop_flag;
    vector<vector<int>> que_status; //按照优先级分四组
    vector<WRR_SCH *> wrr_sch;      //按照优先级分4个WRR调度， 队列0~3 对应优先级0 ，用WRR0调度，队列4~7 对应优先级1，用WRR1调度。。。。
    int pri_group_num;
    int que_per_group;
    sc_in<int> in_fc_port;
    int fc_status;

public:
    void main_process();
    void rev_pkt_process();
    void sch_pkt_process();
    void check_que_status(int que_id);
    void send_cell_to_pe(int que_id);
    void rev_pe_fc_process();
};

#endif // __MOD_SCH_H__
