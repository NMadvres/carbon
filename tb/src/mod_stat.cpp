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
mod_stat_base::mod_stat_base(string file_name, Module_TYPE mod_name, Stat_BASE stat_base_on, int que_num)
{
    m_file_name = "run_log/" + file_name;
    m_que_num = que_num;
    //带宽相关
    print_cnt = 0;
    input_que_pktlen_stat.resize(m_que_num, 0);
    input_que_pktnum_stat.resize(m_que_num, 0);
    output_que_pktlen_stat.resize(m_que_num, 0);
    output_que_pktnum_stat.resize(m_que_num, 0);
    drop_que_pktnum_stat.resize(m_que_num, 0);
    record_cycle = 0;
    //延时相关
    record_max_delay = 0;
    record_min_delay = 0;
    record_avg_delay = 0;
    record_delay_cnt = 0;

    //根据stat_base_on确定基于哪个纬度统计
    switch (stat_base_on) {
    case sport_level:
        pre_print_name = "sport";
        break;
    case dport_level:
        pre_print_name = "dport";
        break;
    case fqid_level:
        pre_print_name = "fqid";
        break;
    case que_level:
        pre_print_name = "que";
        break;
    case pri_level:
        pre_print_name = "pri";
        break;
    }

    //文件操作相关
    m_fp = fopen(m_file_name.c_str(), "w+");
    if (m_fp == NULL) {
        system("mkdir -p run_log");
        m_fp = fopen(m_file_name.c_str(), "w+");
    }
    fclose(m_fp);
}
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: mod_stat模块cpp文件,入口、出口、丢弃带宽统计
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_stat_base::input_record_bw_info(int que_id, int valid_len, int is_eop)
{
    if (!is_eop) {
        input_que_pktlen_stat[que_id] += valid_len;
    } else {
        input_que_pktlen_stat[que_id] += valid_len;
        input_que_pktnum_stat[que_id] += 1;
    }
}

void mod_stat_base::output_record_bw_info(int que_id, int valid_len, int is_eop)
{
    if (!is_eop) {
        output_que_pktlen_stat[que_id] += valid_len;
    } else {
        output_que_pktlen_stat[que_id] += valid_len;
        output_que_pktnum_stat[que_id] += 1;
    }
}

void mod_stat_base::drop_record_bw_info(int que_id, int valid_len, int is_eop)
{
    if (is_eop) {
        drop_que_pktnum_stat[que_id] += 1;
    }
}

void mod_stat_base::record_latency_info(int delay_cnt)
{
    if (delay_cnt < record_min_delay) {
        record_min_delay = delay_cnt;
    }
    if (delay_cnt > record_max_delay) {
        record_max_delay = delay_cnt;
    }
    record_avg_delay = (record_avg_delay * record_delay_cnt + delay_cnt) / (record_delay_cnt + 1);
    record_delay_cnt++;
}

void mod_stat_base::print_info(int stat_period)
{
    m_fp = fopen(m_file_name.c_str(), "aw+");
    if (m_fp == NULL) {
        ASSERT(0);
    }
    print_cnt++;
    fprintf(m_fp, "\n---------------------cur_time:%8d----------------------------------\n", stat_period * print_cnt);
    int send_bytes;
    double send_mbps;
    int send_pkts;
    double send_mpps;
    int rcvd_bytes;
    double rcvd_mbps;
    int rcvd_pkts;
    double rcvd_mpps;
    int dpd_pkts;
    double dpd_mpps;
    int avg_dly;
    int min_dly;
    int max_dly;
    fprintf(m_fp, "%-10s    send_bytes    send_mbps   send_pkts   send_mpps   rcvd_bytes  rcvd_mbps   rcvd_pkts   rcvd_mpps   dpd_pkts    dpd_mpps    avg_dly     min_dly    max_dly  \n",
            pre_print_name.c_str());
    for (int i = 0; i < m_que_num; i++) {
        //计算que带宽bps和PPS，基于que计算
        send_bytes = output_que_pktlen_stat[i];                                   // send_bytes
        send_mbps = (double)output_que_pktlen_stat[i] * G_FREQ_MHZ / stat_period; // MBPS
        send_pkts = output_que_pktnum_stat[i];                                    // send packet
        send_mpps = (double)output_que_pktnum_stat[i] * G_FREQ_MHZ / stat_period; // send_Mpps
        rcvd_bytes = input_que_pktlen_stat[i];                                    // recv_bytes
        rcvd_mbps = (double)input_que_pktlen_stat[i] * G_FREQ_MHZ / stat_period;  // MBPS
        rcvd_pkts = input_que_pktnum_stat[i];                                     // recv packet
        rcvd_mpps = (double)input_que_pktnum_stat[i] * G_FREQ_MHZ / stat_period;  // recv_Mpps
        dpd_pkts = drop_que_pktnum_stat[i];                                       // drop packet
        dpd_mpps = (double)drop_que_pktnum_stat[i] * G_FREQ_MHZ / stat_period;    // drop_Mpps
        avg_dly = record_avg_delay;
        min_dly = record_min_delay;
        max_dly = record_max_delay;

        fprintf(m_fp, "%-10d    %-10d    %-8.2f    %-8d    %-8.2f    %-8d    %-8.2f    %-8d    %-8.2f    %-8d    %-8.2f    %-8d    %-8d    %-8d\n",
                i, send_bytes, send_mbps, send_pkts, send_mpps, rcvd_bytes, rcvd_mbps, rcvd_pkts, rcvd_mpps, dpd_pkts, dpd_mpps, avg_dly, min_dly, max_dly);
        //清零
        input_que_pktlen_stat[i] = 0;
        output_que_pktlen_stat[i] = 0;
    }
    fprintf(m_fp, "\n\n\n");

    fclose(m_fp);
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_stat
// Description: mod_stat模块cpp文件,入口、出口、丢弃带宽统计
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_stat::mod_stat(string file_name, Module_TYPE base_mod_name)
{
    mod_name = base_mod_name;
    string file_add;
    fqid_enable_flag = 0;
    que_enable_flag = 0;
    pri_enable_flag = 0;
    sport_enable_flag = 0;
    dport_enable_flag = 0;
    switch (mod_name) {
    case Module_stim:
        file_add = "_stim";
        sport_enable_flag = 1;
        fqid_enable_flag = 1;
        break;
    case Module_ing:
        file_add = "_ing";
        sport_enable_flag = 1;
        fqid_enable_flag = 1;
        break;
    case Module_sch:
        file_add = "_sch";
        pri_enable_flag = 1;
        fqid_enable_flag = 1;
        break;
    case Module_pe:
        file_add = "_pe";
        dport_enable_flag = 1;
        fqid_enable_flag = 1;
        break;
    case Module_egr:
        file_add = "_egr";
        dport_enable_flag = 1;
        fqid_enable_flag = 1;
        break;
    case Module_top:
        file_add = "_top";
        sport_enable_flag = 1;
        dport_enable_flag = 1;
        fqid_enable_flag = 1;
        break;
    }
    file_name = file_name + file_add;
    //赋值统计数目
    pri_size = G_PRI_NUM;
    que_size = g_que_rule_tab.size();
    fid_size = g_flow_rule_tab.size();
    port_size = g_port_rule_tab.size();

    //例化统计子控件
    fqid_stat = new mod_stat_base(file_name, mod_name, fqid_level, fid_size);
    que_stat = new mod_stat_base(file_name, mod_name, que_level, que_size);
    pri_stat = new mod_stat_base(file_name, mod_name, pri_level, pri_size);
    sport_stat = new mod_stat_base(file_name, mod_name, sport_level, port_size);
    dport_stat = new mod_stat_base(file_name, mod_name, dport_level, port_size);
}

void mod_stat::input_comm_stat_func(s_pkt_desc pkt_stat)
{
    if (fqid_enable_flag == 1) {
        fqid_stat->input_record_bw_info(pkt_stat.fid, pkt_stat.len, pkt_stat.eop);
    }
    if (que_enable_flag == 1) {
        if (mod_name == Module_sch) {
            que_stat->input_record_bw_info(pkt_stat.qid, pkt_stat.vldl, pkt_stat.eop);
        } else {
            que_stat->input_record_bw_info(pkt_stat.qid, pkt_stat.len, pkt_stat.eop);
        }
    }
    if (pri_enable_flag == 1) {
        if (mod_name == Module_sch) {
            pri_stat->input_record_bw_info(pkt_stat.pri, pkt_stat.vldl, pkt_stat.eop);
        } else {
            pri_stat->input_record_bw_info(pkt_stat.pri, pkt_stat.len, pkt_stat.eop);
        }
    }
    if (sport_enable_flag == 1) {
        sport_stat->input_record_bw_info(pkt_stat.sport, pkt_stat.len, pkt_stat.eop);
    }
    if (dport_enable_flag == 1) {
        dport_stat->input_record_bw_info(pkt_stat.dport, pkt_stat.len, pkt_stat.eop);
    }
}

void mod_stat::output_comm_stat_func(s_pkt_desc pkt_stat)
{
    if (fqid_enable_flag == 1) {
        fqid_stat->output_record_bw_info(pkt_stat.fid, pkt_stat.len, pkt_stat.eop);
    }
    if (que_enable_flag == 1) {
        if (mod_name == Module_sch) {
            que_stat->output_record_bw_info(pkt_stat.qid, pkt_stat.vldl, pkt_stat.eop);
        } else {
            que_stat->output_record_bw_info(pkt_stat.qid, pkt_stat.len, pkt_stat.eop);
        }
    }
    if (pri_enable_flag == 1) {
        if (mod_name == Module_sch) {
            pri_stat->output_record_bw_info(pkt_stat.pri, pkt_stat.vldl, pkt_stat.eop);
        } else {
            pri_stat->output_record_bw_info(pkt_stat.pri, pkt_stat.len, pkt_stat.eop);
        }
    }
    if (sport_enable_flag == 1) {
        sport_stat->output_record_bw_info(pkt_stat.sport, pkt_stat.len, pkt_stat.eop);
    }
    if (dport_enable_flag == 1) {
        dport_stat->output_record_bw_info(pkt_stat.dport, pkt_stat.len, pkt_stat.eop);
    }
}

void mod_stat::drop_comm_stat_func(s_pkt_desc pkt_stat)
{
    if (fqid_enable_flag == 1) {
        fqid_stat->drop_record_bw_info(pkt_stat.fid, pkt_stat.len, pkt_stat.eop);
    }
    if (que_enable_flag == 1) {
        que_stat->drop_record_bw_info(pkt_stat.qid, pkt_stat.len, pkt_stat.eop);
    }
    if (pri_enable_flag == 1) {
        pri_stat->drop_record_bw_info(pkt_stat.pri, pkt_stat.len, pkt_stat.eop);
    }
    if (sport_enable_flag == 1) {
        sport_stat->drop_record_bw_info(pkt_stat.sport, pkt_stat.len, pkt_stat.eop);
    }
    if (dport_enable_flag == 1) {
        dport_stat->drop_record_bw_info(pkt_stat.sport, pkt_stat.len, pkt_stat.eop);
    }
}

void mod_stat::record_comm_latency_func(int delay_cnt)
{
    if (fqid_enable_flag == 1) {
        fqid_stat->record_latency_info(delay_cnt);
    }
    if (que_enable_flag == 1) {
        que_stat->record_latency_info(delay_cnt);
    }
    if (pri_enable_flag == 1) {
        pri_stat->record_latency_info(delay_cnt);
    }
    if (sport_enable_flag == 1) {
        sport_stat->record_latency_info(delay_cnt);
    }
    if (dport_enable_flag == 1) {
        dport_stat->record_latency_info(delay_cnt);
    }
}
void mod_stat::print_info(int stat_period)
{
    if (fqid_enable_flag == 1) {
        fqid_stat->print_info(stat_period);
    }
    if (que_enable_flag == 1) {
        que_stat->print_info(stat_period);
    }
    if (pri_enable_flag == 1) {
        pri_stat->print_info(stat_period);
    }
    if (sport_enable_flag == 1) {
        sport_stat->print_info(stat_period);
    }
    if (dport_enable_flag == 1) {
        dport_stat->print_info(stat_period);
    }
}
