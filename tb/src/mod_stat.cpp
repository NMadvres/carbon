#include "mod_stat.h"
#include <iomanip>

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

    pkt_receive_file.open(pkt_receive_filename);

    SC_METHOD(stat_pkt_process);
    for (int i = 0; i < G_INTER_NUM; i++) {
        sensitive << *in_pkt_stat[i];
    }
    dont_initialize();
}

void mod_stat::stat_pkt_process()
{
    for (int i = 0; i < G_INTER_NUM; i++) {
        if (in_pkt_stat[i]->event()) {
            const s_pkt_desc &pkt = in_pkt_stat[i]->read();
            stat_flow_process(&pkt);
            stat_sport_process(&pkt);
            stat_dport_process(&pkt);
            stat_priority_process(&pkt);
            stat_qid_process(&pkt);
        }
    }
    pkt_receive_file << setiosflags(ios::left) << setw(14) << "Province" << resetiosflags(ios::left) // 用完之后清除
                     << setiosflags(ios::right) << setw(9) << "Area(km2)" << setw(12) << "Pop.(10K)"
                     << resetiosflags(ios::right) << endl;
    for (int i = 0; i < G_INTER_NUM; i++) {
        ;
    }
}

void mod_stat::stat_flow_process(const s_pkt_desc *pkt)
{
    in_flow_rev_pkt[pkt->fid]++;
    in_flow_rev_bytes[pkt->fid] += pkt->len;
    int rcvd_mbps = in_flow_rev_bytes[pkt->fid] * G_FREQ_MHZ / in_clk_cnt;
    int rcvd_mpps = in_flow_rev_pkt[pkt->fid] * G_FREQ_MHZ / in_clk_cnt;
    pkt_receive_file << "-------------flow_stat----" << pkt->fid << "-----------" << endl;
    pkt_receive_file << "revd_pkts =" << in_flow_rev_pkt[pkt->fid] << endl;
    pkt_receive_file << "revd_bytes = " << in_flow_rev_bytes[pkt->fid] << endl;
    pkt_receive_file << "revd_mbps = " << rcvd_mbps << endl;
    pkt_receive_file << "revd_mpps = " << rcvd_mpps << endl;
}

void mod_stat::stat_sport_process(const s_pkt_desc *pkt)
{
    in_sport_rev_pkt[pkt->sport]++;
    in_sport_rev_bytes[pkt->sport] += pkt->len;
    int rcvd_mbps = in_sport_rev_bytes[pkt->sport] * G_FREQ_MHZ / in_clk_cnt;
    int rcvd_mpps = in_sport_rev_pkt[pkt->sport] * G_FREQ_MHZ / in_clk_cnt;
    pkt_receive_file << "-------------sport_stat----" << pkt->sport << "-----------" << endl;
    pkt_receive_file << "revd_pkts =" << in_sport_rev_pkt[pkt->sport] << endl;
    pkt_receive_file << "revd_bytes = " << in_sport_rev_bytes[pkt->sport] << endl;
    pkt_receive_file << "revd_mbps = " << rcvd_mbps << endl;
    pkt_receive_file << "revd_mpps = " << rcvd_mpps << endl;
}

void mod_stat::stat_dport_process(const s_pkt_desc *pkt)
{
    in_dport_rev_pkt[pkt->dport]++;
    in_dport_rev_bytes[pkt->dport] += pkt->len;
    int rcvd_mbps = in_dport_rev_bytes[pkt->dport] * G_FREQ_MHZ / in_clk_cnt;
    int rcvd_mpps = in_dport_rev_pkt[pkt->dport] * G_FREQ_MHZ / in_clk_cnt;
    pkt_receive_file << "-------------dport_stat----" << pkt->dport << "-----------" << endl;
    pkt_receive_file << "revd_pkts =" << in_dport_rev_pkt[pkt->dport] << endl;
    pkt_receive_file << "revd_bytes = " << in_dport_rev_bytes[pkt->dport] << endl;
    pkt_receive_file << "revd_mbps = " << rcvd_mbps << endl;
    pkt_receive_file << "revd_mpps = " << rcvd_mpps << endl;
}

void mod_stat::stat_priority_process(const s_pkt_desc *pkt)
{
    in_pri_rev_pkt[pkt->pri]++;
    in_pri_rev_bytes[pkt->pri] += pkt->len;
    int rcvd_mbps = in_pri_rev_bytes[pkt->pri] * G_FREQ_MHZ / in_clk_cnt;
    int rcvd_mpps = in_pri_rev_pkt[pkt->pri] * G_FREQ_MHZ / in_clk_cnt;
    pkt_receive_file << "-------------priority_stat----" << pkt->pri << "-----------" << endl;
    pkt_receive_file << "revd_pkts =" << in_pri_rev_pkt[pkt->pri] << endl;
    pkt_receive_file << "revd_bytes = " << in_pri_rev_bytes[pkt->pri] << endl;
    pkt_receive_file << "revd_mbps = " << rcvd_mbps << endl;
    pkt_receive_file << "revd_mpps = " << rcvd_mpps << endl;
}

void mod_stat::stat_qid_process(const s_pkt_desc *pkt)
{
    in_qid_rev_pkt[pkt->qid]++;
    in_qid_rev_bytes[pkt->qid] += pkt->len;
    int rcvd_mbps = in_qid_rev_bytes[pkt->qid] * G_FREQ_MHZ / in_clk_cnt;
    int rcvd_mpps = in_qid_rev_pkt[pkt->qid] * G_FREQ_MHZ / in_clk_cnt;
    pkt_receive_file << "-------------qid_stat----" << pkt->qid << "-----------" << endl;
    pkt_receive_file << "revd_pkts =" << in_qid_rev_pkt[pkt->qid] << endl;
    pkt_receive_file << "revd_bytes = " << in_qid_rev_bytes[pkt->qid] << endl;
    pkt_receive_file << "revd_mbps = " << rcvd_mbps << endl;
    pkt_receive_file << "revd_mpps = " << rcvd_mpps << endl;
}
