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
mod_stat::mod_stat(sc_module_name name):
    sc_module(name)
{
    for (int i = 0; i < G_INTER_NUM; i++) {
        in_pkt_stat[i] = new sc_in<s_pkt_desc>();
    }
    SC_METHOD(recv_pkt_process);
    for (int i = 0; i < G_INTER_NUM; i++) {
        sensitive << *in_pkt_stat[i];
    }
    dont_initialize();
}

void mod_stat::recv_pkt_process()
{
    s_pkt_desc rd_pkt;

    for (int i = 0; i < G_INTER_NUM; i++) {
        if ((*in_pkt_stat[i]).event()) {
            rd_pkt = in_pkt_stat[i]->read();
            rd_pkt.len = rd_pkt.len;
            pkt_stat_err_check(rd_pkt);
        }
    }
}

void mod_stat::pkt_stat_err_check(s_pkt_desc pkt)
{
    s_flow_rule flow_rule;
    s_hash_rule_key hash_pkt_lut_key;
    s_err_list err_list_stat;

    if (pkt.fid > 16) {
        err_list_stat.fid_err_cnt++;
        return;
    }

    flow_rule = g_flow_rule_tab[pkt.fid];
    if (flow_rule.sid != pkt.sid) {
        err_list_stat.sport_err_cnt++;

        if (err_list_stat.sport_err_cnt == 1)
            err_list_stat.err_var_sum++;
    }

    if (flow_rule.did != pkt.did) {
        err_list_stat.dport_err_cnt++;

        if (err_list_stat.dport_err_cnt == 1)
            err_list_stat.err_var_sum++;
    }

    if ((flow_rule.len + flow_rule.len2add) != pkt.len) {
        err_list_stat.len_err_cnt++;

        if (err_list_stat.len_err_cnt)
            err_list_stat.err_var_sum++;
    }

    if (DESC_TYPE_PKT != pkt.type) {
        err_list_stat.type_err_cnt++;

        if (err_list_stat.type_err_cnt)
            err_list_stat.err_var_sum++;
    }

    hash_pkt_lut_key.sid = pkt.sid;
    hash_pkt_lut_key.did = pkt.did;
    hash_pkt_lut_key.pri = pkt.pri;
    auto iter = g_hash_rule_tab.find(hash_pkt_lut_key);
    if (iter == g_hash_rule_tab.end()) { //check hash
        err_list_stat.hash_err_cnt++;

        if (err_list_stat.hash_err_cnt == 1)
            err_list_stat.err_var_sum++;
    }

    //TODO
    /*
		* inport error check & fsn error check
		*
		*/

    MOD_LOG_ERROR << "stat error packet" << err_list_stat;
}
