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
    mod_stat(sc_module_name name, func_stat *base_top_stat);
    SC_HAS_PROCESS(mod_stat);

public:
    array<sc_in<s_pkt_desc> *, G_INTER_NUM> in_pkt_stat;
    sc_in<int> in_clk_cnt;
    sc_in<s_pkt_desc> in_bcpu;
    void recv_pkt_process();
    void recv_bcpu_pkt_process();
    void pkt_stat_err_check(s_pkt_desc pkt);
    func_stat *top_stat;
    s_flow_rule flow_rule;
    s_hash_rule_key hash_pkt_lut_key;
    s_err_list err_list_stat;
    array<int, G_FLOW_NUM> fsn_cache;
};

#endif // __MOD_STAT_H__
