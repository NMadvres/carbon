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
    SC_METHOD(rev_pkt_process);
    sensitive << in_cell_que;
    dont_initialize();

    SC_METHOD(main_process);
    sensitive << in_clk_cnt;
    dont_initialize();
}

void mod_sch::main_process()
{
    //rev_pkt_process();
    sch_pkt_process();
}

void mod_sch::rev_pkt_process()
{
    if (in_cell_que.event()) {
        s_pkt_desc rd_pkt = in_cell_que.read();
        int que_id = rd_pkt.qid;
        input_cell_que[que_id].push_back(rd_pkt);
        if (input_cell_que[que_id].size() > 1000) {
            cout << "input_cell_que overflow size:" << input_cell_que[que_id].size() << endl;
            ASSERT(0);
        }

        //队列状态刷新，如果遇到EOP尾切片，则认为该队列可以参与调度
        if (rd_pkt.eop) {
            que_status[que_id / G_PRI_NUM][que_id % G_PRI_NUM] = 1;
        }
    }
}

void mod_sch::sch_pkt_process()
{
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
void mod_sch::send_cell_to_pe(int que_id)
{
    int cell_num = input_cell_que[que_id].size();
    for (int i = 0; i < cell_num; i++) {
        s_pkt_desc cur_cell = input_cell_que[que_id].front();
        input_cell_que[que_id].pop_front();
        out_cell_que.write(cur_cell);
        if (cur_cell.eop == true) {
            break;
        }
    }
}