////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   commdef
// Description: 全局定义文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////

#include "comm_def.h"

#define FMT_BUF_SIZE 512

// 用于实现全局cycle_cnt计数

std::vector<s_flow_rule> g_flow_rule_tab;
std::unordered_map<s_hash_rule_key, int, has_rule_key_hash> g_hash_rule_tab;
std::vector<s_tab_que> g_que_rule_tab;
std::vector<s_tab_port> g_port_rule_tab;
mod_logger mod_lg_inst;

const char *s_pkt_desc::to_string() const
{
    static char buf[FMT_BUF_SIZE];

    snprintf(buf, FMT_BUF_SIZE, "{"
                                "type:%d,"
                                "fid:%d,"
                                "sid:%d,"
                                "did:%d,"
                                "fsn:%d,"
                                "len:%d,"
                                "pri:%d,"
                                "sport:%d,"
                                "dport:%d,"
                                "qid:%d,"
                                "vldl:%d,"
                                "csn:%d,"
                                "sop:%d,"
                                "eop:%d"
                                "}",
             type, fid, sid, did, fsn, len, pri, sport, dport, qid, vldl, csn,
             sop, eop);

    return buf;
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   glb_cfg_c
// Description: glb_cfg_c 生成配置表格
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
glb_cfg_c::glb_cfg_c(std::string file_name)
{
    read_cfg_file(file_name);
    gen_cfg_table();
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: read_cfg_file
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void glb_cfg_c::read_cfg_file(std::string file_name)
{
    ifstream in_file(file_name.c_str());
    if (!in_file.is_open()) {
        cout << "cfg file error" << endl;
        return;
    }
    std::string read_line;
    int file_pos_status = -255; // 如果status =1,读取流表 status =2,读取HASH表  status =3,读取队列权重表  status =4,读取Port端口表
    while (getline(in_file, read_line)) {
        if (read_line.find("g_flow_rule_tab") != read_line.npos) // 标识符不写入
        {
            file_pos_status = 1;
            continue;
        } else if (read_line.find("g_hash_rule_tab") != read_line.npos) {
            file_pos_status = 2;
            continue;
        } else if (read_line.find("g_que_rule_tab") != read_line.npos) {
            file_pos_status = 3;
            continue;
        } else if (read_line.find("g_port_rule_tab") != read_line.npos) {
            file_pos_status = 4;
            continue;
        }
        // 根据状态填充表格
        std::string tmp_value;
        int index = 0;
        char *split_char;
        std::vector<int> tmp_value_vc;
        const char *sep = " ";
        char *line_str = new char[read_line.size() + 1];
        strcpy(line_str, read_line.c_str());
        // 获取第一个子字符串
        split_char = strtok(line_str, sep);
        while (split_char != NULL) {
            //cout << split_char <<endl;
            if ((index != 0) && (*split_char != '\r')) // 全面的标识符sid /did 等不写入
            {
                std::string tmp_str = std::string(split_char);
                tmp_value_vc.push_back(stoi(tmp_str));
            }
            index++;
            split_char = strtok(NULL, sep);
        }
        delete[] line_str;
        if (file_pos_status == 1) {
            g_flow_tab_relate.push_back(tmp_value_vc);
        }
        if (file_pos_status == 2) {
            g_hash_tab_relate.push_back(tmp_value_vc);
        }
        if (file_pos_status == 3) {
            g_que_tab_relate.push_back(tmp_value_vc);
        }
        if (file_pos_status == 4) {
            g_port_tab_relate.push_back(tmp_value_vc);
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: gen_cfg_table
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void glb_cfg_c::gen_cfg_table()
{
    // 填充flow表格
    if (g_flow_tab_relate.size() != 0) {
        int flow_num = g_flow_tab_relate[0].size(); // index0对应fid号
        // 填充真实flow表
        for (int i = 0; i < flow_num; i++) {
            s_flow_rule fill_flow_rule;
            fill_flow_rule.sid = g_flow_tab_relate[1][i];        // index1对应sid号
            fill_flow_rule.did = g_flow_tab_relate[2][i];        // index2对应did号
            fill_flow_rule.pri = g_flow_tab_relate[3][i];        // index3对应pri
            fill_flow_rule.len = g_flow_tab_relate[4][i];        // index4对应len
            fill_flow_rule.sport = g_flow_tab_relate[5][i];      // index5对应sport号
            fill_flow_rule.dport = g_flow_tab_relate[6][i];      // index6对应dport号
            fill_flow_rule.qid = g_flow_tab_relate[7][i];        // index7对应qid号
            fill_flow_rule.len2add = g_flow_tab_relate[8][i];    // index8对应len2add
            fill_flow_rule.flow_speed = g_flow_tab_relate[9][i]; // index9对应flow_speed
            g_flow_rule_tab.push_back(fill_flow_rule);
        }
    }

    // 填充HASH表格
    if (g_hash_tab_relate.size() != 0) {
        int hash_rule_num = g_hash_tab_relate[0].size(); // index0对应key_sid
        // 填充真实HASH表
        for (int i = 0; i < hash_rule_num; i++) {
            s_hash_rule_key fill_hash_key;
            fill_hash_key.sid = g_hash_tab_relate[0][i]; // index0对应key_sid
            fill_hash_key.did = g_hash_tab_relate[1][i]; // index0对应key_sid
            fill_hash_key.pri = g_hash_tab_relate[2][i]; // index0对应key_sid
            int fill_fid = g_hash_tab_relate[3][i];
            g_hash_rule_tab.insert({fill_hash_key, fill_fid});
        }
    }

    // 填充que表格
    if (g_que_tab_relate.size() != 0) {
        int que_num = g_que_tab_relate[0].size(); // index0对应que_id
        // 填充真实que表
        for (int i = 0; i < que_num; i++) {
            int fill_weight = g_que_tab_relate[2][i]; // index2对应weight
            g_que_rule_tab.push_back(fill_weight);
        }
    }

    // 填充port表格
    if (g_port_tab_relate.size() != 0) {
        int port_num = g_port_tab_relate[0].size(); // index0对应port_id
        // 填充真实port表
        for (int i = 0; i < port_num; i++) {
            int fill_speed = g_port_tab_relate[1][i]; // index1对应port speed
            g_port_rule_tab.push_back(fill_speed);
        }
    }
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   comm_def
// Description: RR_SCH调度算法
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
RR_SCH::RR_SCH(int tmp_que_num)
{
    que_num = tmp_que_num;
    que_status.resize(que_num, 0);
    sch_pos = 0;
}

void RR_SCH::set_que_valid(int que_id, bool valid_flag)
{
    if (que_id >= que_num) {
        cout << "error que_id" << que_id << endl;
    } else {
        que_status[que_id] = valid_flag;
    }
}

bool RR_SCH::get_sch_result(int &rst_que)
{
    int tmp_pos = sch_pos;
    for (int i = 0; i < que_num; i++) {
        tmp_pos = (sch_pos + i) % que_num;
        if (que_status[tmp_pos] == 1) {
            sch_pos = (tmp_pos + 1) % que_num;
            rst_que = tmp_pos;
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   comm_def
// Description: WRR_SCH调度算法
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
WRR_SCH::WRR_SCH(int tmp_que_num, vector<int> tmp_weight)
{
    assert(tmp_que_num > 0);

    que_num = tmp_que_num;
    if (tmp_weight.size() != (unsigned int)que_num) {
        cout << "cfg a wrong weight value" << endl;
        return;
    }
    que_status.resize(que_num, 0);
    init_weight = tmp_weight;
    cur_weight = tmp_weight;
    sch_pos = 0;
}

void WRR_SCH::set_que_valid(int que_id, bool valid_flag)
{
    if (que_id >= que_num) {
        cout << "error que_id" << que_id << endl;
    } else {
        que_status[que_id] = valid_flag;
    }
}

bool WRR_SCH::get_sch_result(int &rst_que)
{
    //如果所有队列weight 减为0，统一刷新
    bool update_weight_flag = true;
    for (int index = 0; index < que_num; index++) {
        if ((cur_weight[index] > 0) && (que_status[index] == 1)) {
            update_weight_flag = false;
            break;
        }
    }
    //如果全为0，reload value
    if (update_weight_flag == true) {
        reload_weight_value();
    }

    int tmp_pos = sch_pos;
    for (int i = 0; i < que_num; i++) {
        tmp_pos = (sch_pos + i) % que_num;
        if ((que_status[tmp_pos] == 1) && (cur_weight[tmp_pos] > 0)) {
            sch_pos = (tmp_pos + 1) % que_num;
            rst_que = tmp_pos;
            cur_weight[tmp_pos] -= 1; //扣权重
            return true;
        }
    }

    return false;
}

void WRR_SCH::reload_weight_value()
{
    cur_weight = init_weight;
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   comm_def
// Description: SP调度算法
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
SP_SCH::SP_SCH(int tmp_que_num)
{
    que_num = tmp_que_num;
    que_status.resize(que_num, 0);
    sch_pos = 0;
}

void SP_SCH::set_que_valid(int que_id, bool valid_flag)
{
    if (que_id >= que_num) {
        cout << "error que_id" << que_id << endl;
    } else {
        que_status[que_id] = valid_flag;
    }
}

void SP_SCH::set_que_hpri(int que_id)
{
    if (que_id >= que_num) {
        cout << "error que_id" << que_id << endl;
    } else {
        sch_pos = que_id;
    }
}

bool SP_SCH::get_sch_result(int &rst_que)
{
    int tmp_pos = sch_pos;
    if (que_status[tmp_pos] == 1) {
        rst_que = tmp_pos;
        return true;
    } else {
        for (int i = 0; i < que_num; i++) {
            tmp_pos = (sch_pos + i) % que_num;
            if (que_status[tmp_pos] == 1) {
                rst_que = tmp_pos;
                return true;
            }
        }
    }

    return false;
}

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   func_stat_base
// Description: 统计算法相关
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////

func_stat_base::func_stat_base(string file_name, MODULE_TYPE mod_name, Stat_BASE stat_base_on, int que_num)
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
    case fid_level:
        pre_print_name = "fid";
        break;
    case que_level:
        pre_print_name = "qid";
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
void func_stat_base::input_record_bw_info(int que_id, int valid_len, int is_eop)
{
    if (!is_eop) {
        input_que_pktlen_stat[que_id] += valid_len;
    } else {
        input_que_pktlen_stat[que_id] += valid_len;
        input_que_pktnum_stat[que_id] += 1;
    }
}

void func_stat_base::output_record_bw_info(int que_id, int valid_len, int is_eop)
{
    if (!is_eop) {
        output_que_pktlen_stat[que_id] += valid_len;
    } else {
        output_que_pktlen_stat[que_id] += valid_len;
        output_que_pktnum_stat[que_id] += 1;
    }
}

void func_stat_base::drop_record_bw_info(int que_id, int valid_len, int is_eop)
{
    if (is_eop) {
        drop_que_pktnum_stat[que_id] += 1;
    }
}

void func_stat_base::record_latency_info(int delay_cnt)
{
    if (record_delay_cnt == 0) {
        record_min_delay = delay_cnt;
        record_max_delay = delay_cnt;
        record_avg_delay = delay_cnt;
    }

    if (delay_cnt < record_min_delay) {
        record_min_delay = delay_cnt;
    }
    if (delay_cnt > record_max_delay) {
        record_max_delay = delay_cnt;
    }
    record_avg_delay = (record_avg_delay * record_delay_cnt + delay_cnt) / (record_delay_cnt + 1);
    record_delay_cnt++;
}

void func_stat_base::print_info(int stat_period)
{
    m_fp = fopen(m_file_name.c_str(), "aw+");
    if (m_fp == NULL) {
        ASSERT(0);
    }
    print_cnt++;
    //fprintf(m_fp, "\n---------------------cur_time:%8d----------------------------------\n", stat_period * print_cnt);
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
    fprintf(m_fp, "%-10s    send_bytes    send_mbps   send_pkts   send_mpps   rcvd_bytes  rcvd_mbps   rcvd_pkts   rcvd_mpps   dpd_pkts    dpd_mpps    avg_dly     min_dly    max_dly  %c",
            pre_print_name.c_str(), 13);
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
        if (drop_que_pktnum_stat[i] == 0) {
            drop_que_pktnum_stat[i] = output_que_pktnum_stat[i] - input_que_pktnum_stat[i];
        }
        dpd_pkts = drop_que_pktnum_stat[i];                                    // drop packet
        dpd_mpps = (double)drop_que_pktnum_stat[i] * G_FREQ_MHZ / stat_period; // drop_Mpps
        avg_dly = record_avg_delay;
        min_dly = record_min_delay;
        max_dly = record_max_delay;

        fprintf(m_fp, "%-10d    %-10d    %-8.2f    %-8d    %-8.2f    %-8d    %-8.2f    %-8d    %-8.2f    %-8d    %-8.2f    %-8d    %-8d    %-8d  %c",
                i, send_bytes, send_mbps, send_pkts, send_mpps, rcvd_bytes, rcvd_mbps, rcvd_pkts, rcvd_mpps, dpd_pkts, dpd_mpps, avg_dly, min_dly, max_dly, 13);
        //清零
        //input_que_pktlen_stat[i] = 0;
        //output_que_pktlen_stat[i] = 0;
    }
    fprintf(m_fp, "%c", 13);

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
func_stat::func_stat(string file_name, MODULE_TYPE base_mod_name)
{
    mod_name = base_mod_name;
    string file_add;
    fid_enable_flag = 0;
    que_enable_flag = 0;
    pri_enable_flag = 0;
    sport_enable_flag = 0;
    dport_enable_flag = 0;
    switch (mod_name) {
    case Module_stim:
        file_add = "_stim";
        sport_enable_flag = 1;
        fid_enable_flag = 1;
        break;
    case Module_ing:
        file_add = "_ing";
        sport_enable_flag = 1;
        fid_enable_flag = 1;
        break;
    case Module_sch:
        file_add = "_sch";
        pri_enable_flag = 1;
        fid_enable_flag = 1;
        break;
    case Module_pe:
        file_add = "_pe";
        dport_enable_flag = 1;
        fid_enable_flag = 1;
        break;
    case Module_egr:
        file_add = "_egr";
        dport_enable_flag = 1;
        fid_enable_flag = 1;
        break;
    case Module_top:
        file_add = "_top";
        sport_enable_flag = 1;
        dport_enable_flag = 1;
        fid_enable_flag = 1;
        que_enable_flag = 1;
        pri_enable_flag = 1;
        break;
    }
    //file_name = file_name + file_add;
    file_name = file_name;
    //赋值统计数目
    pri_size = G_PRI_NUM;
    que_size = g_que_rule_tab.size();
    fid_size = g_flow_rule_tab.size();
    port_size = g_port_rule_tab.size();

    //例化统计子控件
    fid_stat = new func_stat_base(file_name, mod_name, fid_level, fid_size);
    que_stat = new func_stat_base(file_name, mod_name, que_level, que_size);
    pri_stat = new func_stat_base(file_name, mod_name, pri_level, pri_size);
    sport_stat = new func_stat_base(file_name, mod_name, sport_level, port_size);
    dport_stat = new func_stat_base(file_name, mod_name, dport_level, port_size);
}

void func_stat::input_comm_stat_func(s_pkt_desc pkt_stat)
{
    if (fid_enable_flag == 1) {
        fid_stat->input_record_bw_info(pkt_stat.fid, pkt_stat.len, pkt_stat.eop);
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

void func_stat::output_comm_stat_func(s_pkt_desc pkt_stat)
{
    if (fid_enable_flag == 1) {
        fid_stat->output_record_bw_info(pkt_stat.fid, pkt_stat.len, pkt_stat.eop);
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

void func_stat::drop_comm_stat_func(s_pkt_desc pkt_stat)
{
    if (fid_enable_flag == 1) {
        fid_stat->drop_record_bw_info(pkt_stat.fid, pkt_stat.len, pkt_stat.eop);
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

void func_stat::record_comm_latency_func(int delay_cnt)
{
    if (fid_enable_flag == 1) {
        fid_stat->record_latency_info(delay_cnt);
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
void func_stat::print_info(int stat_period)
{
    if (fid_enable_flag == 1) {
        fid_stat->print_info(stat_period);
    }
    if (sport_enable_flag == 1) {
        sport_stat->print_info(stat_period);
    }
    if (dport_enable_flag == 1) {
        dport_stat->print_info(stat_period);
    }
    if (pri_enable_flag == 1) {
        pri_stat->print_info(stat_period);
    }
    if (que_enable_flag == 1) {
        que_stat->print_info(stat_period);
    }
}
