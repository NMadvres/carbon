#include "mod_sch.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: mod_sch sch模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_sch::mod_sch(sc_module_name name):
    sc_module(name)
{
    input_drop_flag.resize(G_INTER_NUM);
    input_cell_que.resize(G_INTER_NUM);
    wrr_sch.resize(G_INTER_NUM);
    que_status.resize(G_INTER_NUM);
    input_sp_que.resize(G_PRI_NUM);
    for (int i = 0; i < G_INTER_NUM; i++) {
        input_drop_flag[i].resize(G_QUE_NUM, 0);
        input_cell_que[i].resize(G_QUE_NUM);
        que_status[i].resize(G_QUE_NUM, 0);
        wrr_sch[i] = new WRR_SCH(G_QUE_NUM, g_que_rule_tab);
    }
    //input_drop_flag.resize(G_QUE_NUM, 0);
    //反压状态赋初值
    in_fc_port.resize(G_INTER_NUM);
    for (int i = 0; i < G_INTER_NUM; i++) {
        in_fc_port[i] = new sc_in<int>();
    }
    fc_status.resize(G_INTER_NUM, 0);

    //for stat
    //sch_stat = new func_stat("sch_detail_info", Module_sch);

    SC_METHOD(rev_pkt_process); //接包处理
    sensitive << in_clk_cnt;
    dont_initialize();

    SC_METHOD(main_process); //调度发送处理
    sensitive << in_clk_cnt;
    dont_initialize();

    SC_METHOD(rev_pe_fc_process); //反压处理
    for (int i = 0; i < G_INTER_NUM; i++) {
        sensitive << *in_fc_port[i];
    }
    dont_initialize();
}

void mod_sch::main_process()
{
    //rev_pkt_process();
    sch_pkt_process();
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 收包过程
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::rev_pkt_process()
{
    while (in_cell_que.num_available()) {
        s_pkt_desc rd_pkt;
        bool flag = in_cell_que.nb_read(rd_pkt);
        ASSERT(flag == true);

        int dport_id = rd_pkt.dport;
        int que_id = rd_pkt.qid;
        //临时解决mcpu报文问题
        if (que_id < 0) {
            continue;
        }
        //增加状态判断，确定是否处于丢弃状态，对于SOP切片判断，如需丢弃则flag拉起;9600/64=150.需要设置成150个
        MOD_LOG << "cur_cycle" << g_cycle_cnt << "   recv ing packet " << rd_pkt << "que size" << input_cell_que[que_id].size();
        if (rd_pkt.sop) {
            if (input_drop_flag[dport_id][que_id] == 0) {
                if (input_cell_que[dport_id][que_id].size() > 300) {
                    input_drop_flag[dport_id][que_id] = 1;
                } else {
                    input_drop_flag[dport_id][que_id] = 0;
                }
            }
            if (input_drop_flag[dport_id][que_id] == 1) {
                if (input_cell_que[dport_id][que_id].size() > 300) {
                    input_drop_flag[dport_id][que_id] = 1;
                } else {
                    input_drop_flag[dport_id][que_id] = 0;
                }
            }
        }

        //如不丢弃则写入
        if (input_drop_flag[dport_id][que_id] == 0) {
            input_cell_que[dport_id][que_id].push_back(rd_pkt);
        } else {
            MOD_LOG << "cur_cycle" << g_cycle_cnt << "  drop packet" << rd_pkt;
        }

        if (input_cell_que[dport_id][que_id].size() > 500) {
            MOD_LOG << "input_cell_que overflow size:" << input_cell_que[dport_id][que_id].size();
            ASSERT(0);
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 调度过程
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::sch_pkt_process()
{
    //轮询
    for (int port_id = 0; port_id < G_INTER_NUM; port_id++) {
        for (int que_id = 0; que_id < G_QUE_NUM; que_id++) {
            check_wrr_que_status(port_id, que_id);
            if (que_status[port_id][que_id] == 1) {
                wrr_sch[port_id]->set_que_valid(que_id, 1);
            } else {
                wrr_sch[port_id]->set_que_valid(que_id, 0);
            }
        }
    }

    //WRR 调度,如果可以选中，则持续发送该队列切片，直到发送到EOP为止
    for (int port_id = 0; port_id < G_INTER_NUM; port_id++) {
        int rst_que = -1;
        bool rst_flag = false;
        rst_flag = wrr_sch[port_id]->get_sch_result(rst_que);
        if (rst_flag == true) {
            send_cell_to_sp_que(port_id, rst_que);
            check_wrr_que_status(port_id, rst_que);
        }
    }

    //SP 调度，如果可以选中，则持续发送该队列切片，直到发送到EOP为止
    for (int pri_id = 0; pri_id < G_PRI_NUM; pri_id++) {
        if (input_sp_que[pri_id].size() > 0) {
            //一定是整包的基于整包调度
            send_cell_to_pe(pri_id);
            break;
        }
    }
}
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 刷新队列状态，对WRR队列进行刷新
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::check_wrr_que_status(int port_id, int que_id)
{
    if (fc_status[port_id] == 1) {
        que_status[port_id][que_id] = 0;
        return;
    }

    if (input_cell_que[port_id][que_id].size() == 0) {
        que_status[port_id][que_id] = 0;
    } else {
        int cell_num = input_cell_que[port_id][que_id].size();
        for (int i = 0; i < cell_num; i++) {
            s_pkt_desc cur_cell = input_cell_que[port_id][que_id][i];
            if (cur_cell.eop) {
                que_status[port_id][que_id] = 1;
                break;
            } else {
                que_status[port_id][que_id] = 0;
            }
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 发送报文到PE
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::send_cell_to_sp_que(int port_id, int que_id)
{
    int cell_num = input_cell_que[port_id][que_id].size();
    for (int i = 0; i < cell_num; i++) {
        s_pkt_desc cur_cell = input_cell_que[port_id][que_id].front();
        input_cell_que[port_id][que_id].pop_front();
        int pri_id = cur_cell.pri;
        input_sp_que[pri_id].push_back(cur_cell);

        if (cur_cell.eop == true) {
            break;
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 发送报文到PE
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::send_cell_to_pe(int pri_id)
{
    int cell_num = input_sp_que[pri_id].size();
    for (int i = 0; i < cell_num; i++) {
        s_pkt_desc cur_cell = input_sp_que[pri_id].front();
        input_sp_que[pri_id].pop_front();
        //增加时戳信息
        cur_cell.time_stamp.sch_out_clock = g_cycle_cnt;
        out_cell_que.nb_write(cur_cell);
        MOD_LOG << "cur_cycle" << g_cycle_cnt << "   send packet to PE " << cur_cell;

        if (cur_cell.eop == true) {
            break;
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 接收PE的反压状态，　１反压　０　撤销反压
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::rev_pe_fc_process()
{
    for (int i = 0; i < G_INTER_NUM; i++) {
        if (in_fc_port[i]->event()) {
            fc_status[i] = in_fc_port[i]->read();
        }
    }
}