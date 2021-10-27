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
    input_drop_flag.resize(G_QUE_NUM, 0);
    input_cell_que.resize(G_QUE_NUM);
    pri_group_num = G_PRI_NUM;
    que_per_group = G_QUE_NUM / G_PRI_NUM;
    wrr_sch.resize(pri_group_num);
    que_status.resize(pri_group_num);
    for (int i = 0; i < pri_group_num; i++) {
        vector<s_tab_que> tmp_weight;
        for (int j = 0; j < que_per_group; j++) {
            tmp_weight.push_back(g_que_rule_tab[i * pri_group_num + j]);
        }
        wrr_sch[i] = new WRR_SCH(que_per_group, tmp_weight);
        que_status[i].resize(que_per_group, 0);
    }
    //反压状态赋初值
    fc_status = 0;

    SC_METHOD(rev_pkt_process); //接包处理
    sensitive << in_cell_que;
    dont_initialize();

    SC_METHOD(main_process); //调度发送处理
    sensitive << in_clk_cnt;
    dont_initialize();

    SC_METHOD(rev_pe_fc_process); //反压处理
    sensitive << in_fc_port;
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
    if (in_cell_que.event()) {
        s_pkt_desc rd_pkt = in_cell_que.read();
        int que_id = rd_pkt.qid;
        //增加状态判断，确定是否处于丢弃状态，对于SOP切片判断，如需丢弃则flag拉起
        cout << "cur_cycle" << g_cycle_cnt << "   recv ing packet " << rd_pkt << "que size" << input_cell_que[que_id].size() << endl;
        if (rd_pkt.sop) {
            if (input_drop_flag[que_id] == 0) {
                if (input_cell_que[que_id].size() > 100) {
                    input_drop_flag[que_id] = 1;
                } else {
                    input_drop_flag[que_id] = 0;
                }
            }
            if (input_drop_flag[que_id] == 1) {
                if (input_cell_que[que_id].size() > 100) {
                    input_drop_flag[que_id] = 1;
                } else {
                    input_drop_flag[que_id] = 0;
                }
            }
        }

        //如不丢弃则写入
        if (input_drop_flag[que_id] == 0) {
            input_cell_que[que_id].push_back(rd_pkt);
            //队列状态刷新，如果遇到EOP尾切片，则认为该队列可以参与调度
            if (rd_pkt.eop) {
                que_status[que_id / G_PRI_NUM][que_id % G_PRI_NUM] = 1;
            }
        } else {
            cout << "cur_cycle" << g_cycle_cnt << "drop packet" << rd_pkt << endl;
        }

        if (input_cell_que[que_id].size() > 1000) {
            cout << "input_cell_que overflow size:" << input_cell_que[que_id].size() << endl;
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
    //如果当前是反压状态，直接退出，不再进行调度
    if (fc_status == 1) {
        return;
    }

    //轮询
    for (int group_id = 0; group_id < pri_group_num; group_id++) {
        for (int que_id = 0; que_id < que_per_group; que_id++) {
            if (que_status[group_id][que_id] == 1) {
                wrr_sch[group_id]->set_que_valid(que_id, 1);
            } else {
                wrr_sch[group_id]->set_que_valid(que_id, 0);
            }
        }
    }

    //sch 调度,如果可以选中，则持续发送该队列切片，直到发送到EOP为止
    for (int group_id = 0; group_id < pri_group_num; group_id++) {
        int rst_que = -1;
        bool rst_flag = false;
        rst_flag = wrr_sch[group_id]->get_sch_result(rst_que);
        if (rst_flag == true) {
            int real_que = group_id * que_per_group + rst_que;
            send_cell_to_pe(real_que);
            check_que_status(real_que);
            break; //四个WRR是有优先级概念的，现在从group0（对应pri0)开始调度。哪个group调度成功，即退出调度，保证了SP效果；待加反压响应
        }
    }
}
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: 刷新队列状态，每次调度完成后，对调度成功的队列进行刷新
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_sch::check_que_status(int que_id)
{
    int goup_id = que_id / que_per_group;
    int que_in_group = que_id % que_per_group;
    if (input_cell_que[que_id].size() == 0) {
        que_status[goup_id][que_in_group] = 0;
    } else {
        int cell_num = input_cell_que[que_id].size();
        for (int i = 0; i < cell_num; i++) {
            s_pkt_desc cur_cell = input_cell_que[que_id][i];
            if (cur_cell.eop) {
                que_status[goup_id][que_in_group] = 1;
                break;
            } else {
                que_status[goup_id][que_in_group] = 0;
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
void mod_sch::send_cell_to_pe(int que_id)
{
    int cell_num = input_cell_que[que_id].size();
    for (int i = 0; i < cell_num; i++) {
        s_pkt_desc cur_cell = input_cell_que[que_id].front();
        input_cell_que[que_id].pop_front();
        out_cell_que.write(cur_cell);
        cout << "cur_cycle" << g_cycle_cnt << "   send packet to PE " << cur_cell << endl;
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
    if (in_fc_port.event()) {
        fc_status = in_fc_port.read();
    }
}