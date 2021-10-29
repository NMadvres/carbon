#ifndef __MOD_STAT_H__
#define __MOD_STAT_H__

#include "../../src/comm_def.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_stat
// Description: mod_stat stat统计模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_stat: public sc_module
{
public:
    mod_stat(sc_module_name name);
    SC_HAS_PROCESS(mod_stat);

public:
    std::array<sc_in<s_pkt_desc> *, G_INTER_NUM> in_pkt_stat;
    sc_in<int> in_clk_cnt;
    void stat_pkt_process();
    void stat_flow_process(const s_pkt_desc *pkt);
    void stat_sport_process(const s_pkt_desc *pkt);
    void stat_dport_process(const s_pkt_desc *pkt);
    void stat_priority_process(const s_pkt_desc *pkt);
    void stat_qid_process(const s_pkt_desc *pkt);

private:
    std::array<std::deque<s_pkt_desc>, G_INTER_NUM> fifo_port;
    char *pkt_receive_filename = (char *)"pkt_receive_file.log";
    ofstream pkt_receive_file;

    std::array<int, 16> in_flow_rev_pkt = {0};
    std::array<int, 16> in_flow_rev_bytes = {0};

    std::array<int, G_INTER_NUM> in_sport_rev_pkt = {0};
    std::array<int, G_INTER_NUM> in_sport_rev_bytes = {0};

    std::array<int, G_INTER_NUM> in_dport_rev_pkt = {0};
    std::array<int, G_INTER_NUM> in_dport_rev_bytes = {0};

    std::array<int, G_INTER_NUM> in_pri_rev_pkt = {0};
    std::array<int, G_INTER_NUM> in_pri_rev_bytes = {0};

    std::array<int, 16> in_qid_rev_pkt = {0};
    std::array<int, 16> in_qid_rev_bytes = {0};
};

#endif // __MOD_STAT_H__
