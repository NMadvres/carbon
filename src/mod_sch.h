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
    sc_fifo_in<s_pkt_desc> in_cell_que;
    sc_fifo_out<s_pkt_desc> out_cell_que;
    sc_in<int> in_clk_cnt;
    vector<vector<deque<s_pkt_desc>>> input_cell_que; //三个维度，第一维端口port，第二维优先级队列基于4个优先级 = qid%4，第三维存储的报文实体
    vector<vector<int>> input_drop_flag;              //2个维度，第一维端口port， 第二维优先级队列基于4个优先级 = qid%4
    vector<vector<int>> que_status;                   //2个维度，第一维端口port， 第二维优先级队列基于4个优先级 = qid%4
    WRR_SCH *wrr_sch;                                 //第二级调度，基于16个队列
    vector<deque<s_pkt_desc>> input_wrr_que;          //2个维度，第一维队列，第二维报文实体，

    vector<sc_in<int> *> in_fc_port;
    vector<int> fc_status;

    //for stat
    func_stat *sch_stat;

public:
    void main_process();
    void rev_pkt_process();
    void sch_pkt_process();
    void check_firsch_que_status(int port_id, int que_id);
    void send_cell_to_secsch_que(int port_id, int que_id);
    void send_cell_to_pe(int que_id);
    void rev_pe_fc_process();
};

#endif // __MOD_SCH_H__
