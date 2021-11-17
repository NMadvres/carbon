#include "mod_stat.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_stat
// Description: mod_stat 模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_stat::mod_stat(sc_module_name name, func_stat *base_top_stat):
    sc_module(name)
{
    top_stat = base_top_stat;
    for (int i = 0; i < G_INTER_NUM; i++) {
        in_pkt_stat[i] = new sc_in<s_pkt_desc>();
    }
    for (int i = 0; i < G_FLOW_NUM; i++) {
        fsn_cache[i] = -1;
    }
    SC_METHOD(recv_pkt_process);
    for (int i = 0; i < G_INTER_NUM; i++) {
        sensitive << *in_pkt_stat[i];
    }
    dont_initialize();
    //for bcpu stat
    SC_METHOD(recv_bcpu_pkt_process);
    sensitive << in_bcpu;
    dont_initialize();
}

void mod_stat::recv_pkt_process()
{
    s_pkt_desc rd_pkt;

    for (int i = 0; i < G_INTER_NUM; i++) {
        if ((*in_pkt_stat[i]).event()) {
            rd_pkt = in_pkt_stat[i]->read();
            if (rd_pkt.dport != i) err_list_stat.inport_err_cnt++; // check input source inport_err_cnt
            pkt_stat_err_check(rd_pkt);
        }
    }
}

void mod_stat::recv_bcpu_pkt_process()
{
    if (in_bcpu.event()) {
        s_pkt_desc rd_pkt;
        rd_pkt = in_bcpu.read();
        pkt_stat_err_check(rd_pkt);
        //for stat output bw
        top_stat->input_comm_stat_func(rd_pkt);
    }
}

void mod_stat::pkt_stat_err_check(s_pkt_desc pkt)
{
    // cache the flow rule table item
    flow_rule = g_flow_rule_tab[pkt.fid];
    //check type and update type_err_cnt
    if (pkt.type != DESC_TYPE_PKT) err_list_stat.type_err_cnt++;
    //check fid and update fid_err_cnt
    if (pkt.fid > int(g_flow_rule_tab.size()))
        err_list_stat.fid_err_cnt++;
    else {
        //check fsn and update fsn_err_cnt
        if (pkt.fsn != fsn_cache[pkt.fid] + 1) err_list_stat.fsn_err_cnt++;
        fsn_cache[pkt.fid] = pkt.fsn;
        //check sid/did/pri and update hash_err_cnt
        if ((flow_rule.sid != pkt.sid) || (flow_rule.did != pkt.did) || (flow_rule.pri != pkt.pri)) err_list_stat.hash_err_cnt++;
        //check len and update len_err_cnt
        if (pkt.len != (flow_rule.len + flow_rule.len2add)) err_list_stat.len_err_cnt++;
        //check sport and update sport_err_cnt
        if (pkt.sport != flow_rule.sport) err_list_stat.sport_err_cnt++;
        //check qid and update qid_err_cnt
        if (pkt.qid != flow_rule.qid) err_list_stat.qid_err_cnt++;
        //check dport and update dport_err_cnt
        if (pkt.dport != flow_rule.dport) err_list_stat.dport_err_cnt++;
    }
    //sum up err_var_sum
    err_list_stat.err_var_sum = int(err_list_stat.inport_err_cnt > 0)
                                + int(err_list_stat.fid_err_cnt > 0)
                                + int(err_list_stat.type_err_cnt > 0)
                                + int(err_list_stat.fsn_err_cnt > 0)
                                + int(err_list_stat.hash_err_cnt > 0)
                                + int(err_list_stat.len_err_cnt > 0)
                                + int(err_list_stat.sport_err_cnt > 0)
                                + int(err_list_stat.qid_err_cnt > 0)
                                + int(err_list_stat.dport_err_cnt > 0);

    top_stat->record_err_info(err_list_stat);
    if ((err_list_stat.err_var_sum > 0) || (in_clk_cnt > G_CYCLES_TOSTIM)) {
        MOD_LOG_ERROR << "stat error packet" << err_list_stat;
    }
}
