////////////////////////////////////////////////////////
// Project： SystemC carbon
// Module:   ing.cpp
// Description: Pkt to cell, look up table then enter the queue (Pkt转Cell，查表入队列)
// Group：
// Author: Sandy
// Date: 2021.10.11  new
// Hierarchy : --top
//             --|--ing.cpp
//    pktgen------>|ing-->sch-->pe-->eng|----->stat
////////////////////////////////////////////////////////
#include "mod_ing.h"

#define mod_ing_print

mod_ing::mod_ing(sc_module_name name):
    sc_module(name)
{
    rr_sch = new RR_SCH(g_inter_num);

    for (int i = 0; i < g_inter_num; i++) {
        in_port[i] = new sc_in<s_pkt_desc>();
        pkt_count_port[i] = 0;
        infifo_count_port[i] = 0;
        drop_count_port[i] = 0;
    }
    SC_METHOD(main_process);
    sensitive << clkcnt;
}

void mod_ing::main_process()
{
    rev_pkt_process();
    port_rr_sch_process();
    lut_process();
    pkt_to_cell_process();
}

void mod_ing::rev_pkt_process()
{
    for (int i = 0; i < g_inter_num; i++) {
        if (in_port[i]->event()) {
            pkt_count_port[i]++;
            if (fifo_port[i].size() == 16) {
                drop_count_port[i]++;
            } else {
                fifo_port[i].push_back(in_port[i]->read());
                infifo_count_port[i]++;
            }
#ifdef mod_ing_print
            cout << "rev_pkt_process..."
                 << "port id: " << i << " pkts received: " << pkt_count_port[i]
                 << " pkts infifo: " << infifo_count_port[i] << " pkts dropped: " << drop_count_port[i]
                 << " fsn: " << in_port[i]->read().fsn << " sid: " << in_port[i]->read().sid << " did: " << in_port[i]->read().did
                 << " pri:" << in_port[i]->read().pri << " len:" << in_port[i]->read().len << endl;
#endif
        };
    }
}

void mod_ing::port_rr_sch_process()
{
    for (int i = 0; i < g_inter_num; i++) {
        if (fifo_port[i].size() > 0) {
            rr_sch->set_que_valid(i, true); // que非空的时候才参与sch
        } else {
            rr_sch->set_que_valid(i, false);
        }
    }

    int rst_que = -1;
    bool rst_flag = false;

    rst_flag = rr_sch->get_sch_result(rst_que);

    if ((rst_flag == true) and (pkt_out_flag == 0)) {
        s_pkt_desc front_trans = fifo_port[rst_que].front();

        fifo_port[rst_que].pop_front();

        s_port_sch_result = front_trans;

        pkt_tmp_len = front_trans.len;
        pkt_out_flag = 1;
        rst_flag = false;

#ifdef mod_ing_print
        cout << "port_rr_sch_process..."
             << "sch rslt flag: " << rst_flag << " sch rslt que id : " << rst_que << endl;
#endif
    }
}

void mod_ing::lut_process()
{
    int s_sid;
    int s_did;
    int s_pri;
    s_sid = s_port_sch_result.sid;
    s_did = s_port_sch_result.did;
    s_pri = s_port_sch_result.pri;

    s_hash_rule_key hash_pkt_lut_key;

    hash_pkt_lut_key.sid = s_sid;
    hash_pkt_lut_key.did = s_did;
    hash_pkt_lut_key.pri = s_pri;

    if (pkt_out_flag == 1) {
        auto iter = g_hash_rule_tab.find(hash_pkt_lut_key);
        if (iter != g_hash_rule_tab.end()) {
            flow_id = iter->second;
#ifdef mod_ing_print
            cout << "Fid " << flow_id << endl;
#endif
        }

        flow_rule = g_flow_rule_tab[flow_id];

        que_id = flow_rule.qid;
#ifdef mod_ing_print
        cout << "que_id " << que_id << endl;
#endif
    };
}

void mod_ing::pkt_to_cell_process()
{
    int cell_sn;
    s_pkt_desc cell_trans;
    cell_sn = 0;
    // FIXME 临时编译修改
    int cell_len = 0;

    if (pkt_tmp_len > 0) {
        while (pkt_tmp_len >= cell_len) {
            cell_trans = s_port_sch_result;
            cell_trans.qid = que_id;
            cell_trans.fid = flow_id;
            cell_trans.vldl = cell_len;
            cell_trans.csn = cell_sn;
            cell_trans.eop = false;

            out_cell_que.write(cell_trans);
            pkt_tmp_len -= cell_len;
            cell_sn++;
#ifdef mod_ing_print
            cout << "pkt_to_cell_process..."
                 << "cell_trans  : fsn: " << cell_trans.fsn << " sid: " << cell_trans.sid << " did: " << cell_trans.did
                 << " pri:" << cell_trans.pri << " qid: " << cell_trans.qid << " fid: " << cell_trans.fid
                 << " vldl: " << cell_trans.vldl << " csn: " << cell_trans.csn << " eop: " << cell_trans.eop << endl;
#endif
        }

        cell_trans = s_port_sch_result;
        cell_trans.qid = que_id;
        cell_trans.fid = flow_id;
        cell_trans.vldl = pkt_tmp_len;
        cell_trans.csn = cell_sn;
        cell_trans.eop = true;
        out_cell_que.write(cell_trans);

        pkt_tmp_len = 0;
        pkt_out_flag = 0;

#ifdef mod_ing_print
        cout << "pkt_to_cell_process..."
             << "cell_trans  : fsn: " << cell_trans.fsn << " sid: " << cell_trans.sid << " did: " << cell_trans.did
             << " pri:" << cell_trans.pri << " qid: " << cell_trans.qid << " fid: " << cell_trans.fid
             << " vldl: " << cell_trans.vldl << " csn: " << cell_trans.csn << " eop: " << cell_trans.eop << endl;
#endif
    }
}
