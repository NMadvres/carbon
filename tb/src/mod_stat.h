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

enum Module_TYPE
{
    Module_stim = 0,
    Module_ing = 1,
    Module_sch = 2,
    Module_pe = 3,
    Module_egr = 4,
    Module_top = 5
};

enum Stat_BASE
{
    sport_level = 0,
    dport_level = 1,
    fqid_level = 2,
    que_level = 3,
    pri_level = 4
};

class mod_stat_base
{
public:
    mod_stat_base(string file_name, Module_TYPE mod_name, Stat_BASE stat_base_on, int que_num);

public:
    int m_que_num;
    //delay relate
    int record_cycle;
    int record_max_delay;
    int record_min_delay;
    int record_avg_delay;
    int record_delay_cnt;
    //bw relate
    int print_cnt;
    vector<int> input_que_pktlen_stat;
    vector<int> input_que_pktnum_stat;
    vector<int> output_que_pktlen_stat;
    vector<int> output_que_pktnum_stat;
    vector<int> drop_que_pktnum_stat;
    string m_file_name;
    FILE *m_fp;
    string pre_print_name;

public:
    void input_record_bw_info(int que_id, int valid_len, int is_eop);
    void output_record_bw_info(int que_id, int valid_len, int is_eop);
    void drop_record_bw_info(int que_id, int valid_len, int is_eop);
    void record_latency_info(int delay_cnt);
    void print_info(int stat_period);
};

class mod_stat
{
public:
    mod_stat(string file_name, Module_TYPE base_mod_name);
    void input_comm_stat_func(s_pkt_desc pkt_stat);
    void output_comm_stat_func(s_pkt_desc pkt_stat);
    void drop_comm_stat_func(s_pkt_desc pkt_stat);
    void record_comm_latency_func(int delay_cnt);
    void print_info(int stat_period);

public:
    mod_stat_base *fqid_stat;
    mod_stat_base *que_stat;
    mod_stat_base *pri_stat;
    mod_stat_base *sport_stat;
    mod_stat_base *dport_stat;
    int fqid_enable_flag;
    int que_enable_flag;
    int pri_enable_flag;
    int sport_enable_flag;
    int dport_enable_flag;
    int que_size;
    int fid_size;
    int port_size;
    int pri_size;
    Module_TYPE mod_name;
};

extern mod_stat *top_stat;

#endif // __MOD_STAT_H__
