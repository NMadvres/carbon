#ifndef __COMM_DEF_H__
#define __COMM_DEF_H__

#include <stdio.h>
#include <memory>
#include <string.h>
#include <sstream>
#include <array>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <assert.h>
#include "systemc.h"
#include "logger.h"
using namespace std;
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   commdef
// Description: 全局定义文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////

const int G_FREQ_MHZ = 100; //100M HZ
const int G_INTER_NUM = 4;
const int G_QUE_NUM = 16;
const int G_FLOW_NUM = 16;
const int G_CELL_LEN = 64;
const int G_PRI_NUM = 4;
const int G_PE_MPPS = 50;

enum DESC_TYPE
{
    DESC_TYPE_NULL = -1,
    DESC_TYPE_PKT = 0,
    DESC_TYPE_CELL = 1
};

//时钟表内容
struct s_clock_stamp
{
    int stm_out_clock; // 用于记录stm输出时钟
    int ing_out_clock; // 用于记录ing输出时钟
    int sch_out_clock; // 用于记录sch输出时钟
    int pe_out_clock;  // 用于记录pe输出时钟
    int egr_out_clock; // 用于记录egr输出时钟

    //default value setting for s_clock_stamp constructor
    s_clock_stamp()
    {
        stm_out_clock = 0;
        ing_out_clock = 0;
        sch_out_clock = 0;
        pe_out_clock = 0;
        egr_out_clock = 0;
    }
};

struct s_pkt_desc
{
    int type; // 0 packet 1 cell
    int fid;  // flow id，对应Flow号，和激励对应
    int sid;  // 用于三元组匹配
    int did;  // 用于三元组匹配
    int fsn;
    int len;   // 包长度
    int pri;   // 报文优先级
    int sport; // 源端口号，与ing入口侧端口对应
    int dport; // 目的端口号，与engress出口侧端口对应
    int qid;   // queue 号，队列号，对应ing/sch里的que号
    int vldl;  // 有效报文长度，只有cell状态下有意义
    int csn;   // cell切片号， 只有cell状态下有意义
    int sop;   // 首切片，只有cell状态下有意义
    int eop;   // 尾切片，只有cell状态下有意义
    s_clock_stamp time_stamp;

    s_pkt_desc():
        type(DESC_TYPE_NULL),
        fid(-1),
        sid(-1),
        did(-1),
        fsn(-1),
        len(-1),
        pri(-1),
        sport(-1),
        dport(-1),
        qid(-1),
        vldl(-1),
        csn(-1),
        sop(-1),
        eop(-1)
    {
    }

    inline bool operator==(const s_pkt_desc &rhs) const
    {
        return ((rhs.type == type) && (rhs.fid == fid) && (rhs.sid == sid)
                && (rhs.did == did) && (rhs.fsn == fsn) && (rhs.len == len)
                && (rhs.pri == pri) && (rhs.sport == sport) && (rhs.dport == dport)
                && (rhs.qid == qid) && (rhs.vldl == vldl) && (rhs.csn == csn)
                && (rhs.sop == sop) && (rhs.eop == eop));
    }

    const char *to_string() const;
};

inline ostream &operator<<(ostream &os, const s_pkt_desc &a)
{
    os << a.to_string();

    return os;
}

//inline void sc_trace(sc_trace_file *tf, const s_pkt_desc &a, const std::string &name)
//{
//    sc_trace(tf, a.sport, name + ".data");
//}
inline void
#if defined(SC_API_VERSION_STRING)
sc_trace(sc_trace_file *tf, const s_pkt_desc &a, const std::string &name)
#else
sc_trace(sc_trace_file *tf, const s_pkt_desc &a, const sc_string &name)
#endif
{
    sc_trace(tf, a.type, name + ".type");
    sc_trace(tf, a.fid, name + ".fid");
    sc_trace(tf, a.sid, name + ".sid");
    sc_trace(tf, a.did, name + ".did");
    sc_trace(tf, a.fsn, name + ".fsn");
    sc_trace(tf, a.len, name + ".len");
    sc_trace(tf, a.pri, name + ".pri");
    sc_trace(tf, a.sport, name + ".sport");
    sc_trace(tf, a.dport, name + ".dport");
    sc_trace(tf, a.qid, name + ".qid");
    sc_trace(tf, a.vldl, name + ".vldl");
    sc_trace(tf, a.csn, name + ".csn");
    sc_trace(tf, a.sop, name + ".sop");
    sc_trace(tf, a.eop, name + ".eop");
}

// fid映射规则表内容
struct s_flow_rule
{
    int sid;
    int did;
    int len;
    int pri;
    int sport;
    int dport;
    int qid;
    int len2add;
    int flow_speed;

    s_flow_rule():
        sid(-1),
        did(-1),
        len(-1),
        pri(-1),
        sport(-1),
        dport(-1),
        qid(-1),
        len2add(-1),
        flow_speed(-1)
    {
    }
};

//hash规则表KEY值
struct s_hash_rule_key
{
    int sid;
    int did;
    int pri;

    s_hash_rule_key():
        sid(-1),
        did(-1),
        pri(-1)
    {
    }

    inline bool operator==(const s_hash_rule_key &rhs) const
    {
        return (rhs.sid == sid && rhs.did == did && rhs.pri == pri);
    }

    inline bool operator<(const s_hash_rule_key &rhs) const
    {
        return !(rhs.sid == sid && rhs.did == did && rhs.pri == pri);
    }
};

struct has_rule_key_hash
{
    std::size_t operator()(const s_hash_rule_key &key) const
    {
        return key.sid ^ key.did ^ key.pri;
    }
};

// 定义两个别名，用于que队列表和port端口表
typedef int s_tab_que;
typedef int s_tab_port;

// fid映射规则表项,key fid,查表得到映射表
extern std::vector<s_flow_rule> g_flow_rule_tab;
// Hash规则表项，查表得到fid号
extern std::unordered_map<s_hash_rule_key, int, has_rule_key_hash> g_hash_rule_tab;

// que_rule规则表项，查表得到rr_weight权重,key为qid号
extern std::vector<s_tab_que> g_que_rule_tab;

// port_rule规则表项，查表得到port_speed号,key为port号
extern std::vector<s_tab_port> g_port_rule_tab;

//产生全局时钟计数器
extern int g_cycle_cnt;

//产生全局配置
class glb_cfg_c
{
public:
    glb_cfg_c(std::string file_name);
    void read_cfg_file(std::string file_name);
    void gen_cfg_table();

public:
    std::vector<std::vector<int>> g_flow_tab_relate; //两维度；其中第一维是表项深度（包括key和value项），二维是index及value的横向展开
    std::vector<std::vector<int>> g_hash_tab_relate; //两维度；其中第一维是表项深度（包括key和value项），二维是index及value的横向展开
    std::vector<std::vector<int>> g_que_tab_relate;  //两维度；其中第一维是表项深度（包括key和value项），二维是index及value的横向展开
    std::vector<std::vector<int>> g_port_tab_relate; //两维度；其中第一维是表项深度（包括key和value项），二维是index及value的横向展开
};

//公共调度器函数
class WRR_SCH
{
public:
    int que_num;
    int sch_pos;
    std::vector<int> que_status;
    std::vector<int> init_weight;
    std::vector<int> cur_weight;

public:
    WRR_SCH(int tmp_que_num, std::vector<int> tmp_weight);
    void set_que_valid(int que_id, bool valid_flag);
    bool get_sch_result(int &rst_que);
    void reload_weight_value();
};
class RR_SCH
{
public:
    int que_num;
    int sch_pos;
    std::vector<int> que_status;

public:
    RR_SCH(int tmp_que_num);
    void set_que_valid(int que_id, bool valid_flag);
    bool get_sch_result(int &rst_que);
};

class SP_SCH
{
public:
    int que_num;
    int sch_pos;
    std::vector<int> que_status;

public:
    SP_SCH(int tmp_que_num);
    void set_que_valid(int que_id, bool valid_flag);
    void set_que_hpri(int que_id);
    bool get_sch_result(int &rst_que);
};

//STAT 函数相关
//STAT 函数相关
enum MODULE_TYPE
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
    fid_level = 2,
    que_level = 3,
    pri_level = 4
};

class func_stat_base
{
public:
    func_stat_base(string file_name, MODULE_TYPE mod_name, Stat_BASE stat_base_on, int que_num);

public:
    int m_que_num;
    //delay relate
    vector<int> record_total_delay;
    vector<int> record_max_delay;
    vector<int> record_min_delay;
    vector<int> record_avg_delay;
    vector<int> record_delay_cnt;
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
    void record_latency_info(int que_id, int delay_cnt);
    void print_info(int stat_period);
};

class func_stat
{
public:
    func_stat(string file_name, MODULE_TYPE base_mod_name);
    void input_comm_stat_func(s_pkt_desc &pkt_stat);
    void output_comm_stat_func(s_pkt_desc &pkt_stat);
    void drop_comm_stat_func(s_pkt_desc &pkt_stat);
    void record_comm_latency_func(s_pkt_desc &pkt_stat, int delay_cnt);
    void check_enable_level(s_pkt_desc &pkt_stat);
    void print_info(int stat_period);

public:
    func_stat_base *fid_stat;
    func_stat_base *que_stat;
    func_stat_base *pri_stat;
    func_stat_base *sport_stat;
    func_stat_base *dport_stat;
    int fid_enable_flag;
    int que_enable_flag;
    int pri_enable_flag;
    int sport_enable_flag;
    int dport_enable_flag;
    int que_size;
    int fid_size;
    int port_size;
    int pri_size;
    MODULE_TYPE mod_name;
    static func_stat *global_init_stat;
};

//校验错误汇总
struct s_err_list
{
    int err_var_sum;    // 下列错误中计数非0的种类总数
    int inport_err_cnt; // 收到报文和dport值不对应的报文计数
    int fid_err_cnt;    // fid 超预期的报文计数
    int type_err_cnt;   // type 不为0的报文计数
    int fsn_err_cnt;    // fsn 不合理的报文计数
    int hash_err_cnt;   // sid/did/pri与 fid 不符合的计数
    int len_err_cnt;    // 长度不符合流表的报文计数
    int sport_err_cnt;  // sport 不符合流表的报文计数
    int dport_err_cnt;  // dport 不符合流表的报文计数
    int qid_err_cnt;    // qid 不符合流表的计数

    //default value setting for s_err_list constructor
    s_err_list()
    {
        err_var_sum = 0;
        inport_err_cnt = 0;
        fid_err_cnt = 0;
        type_err_cnt = 0;
        fsn_err_cnt = 0;
        hash_err_cnt = 0;
        len_err_cnt = 0;
        sport_err_cnt = 0;
        dport_err_cnt = 0;
        qid_err_cnt = 0;
    }

    inline bool operator==(const s_err_list &T_class) const
    {
        return ((T_class.err_var_sum == err_var_sum)
                && (T_class.inport_err_cnt == inport_err_cnt)
                && (T_class.fid_err_cnt == fid_err_cnt)
                && (T_class.type_err_cnt == type_err_cnt)
                && (T_class.fsn_err_cnt == fsn_err_cnt)
                && (T_class.hash_err_cnt == hash_err_cnt)
                && (T_class.len_err_cnt == len_err_cnt)
                && (T_class.sport_err_cnt == sport_err_cnt)
                && (T_class.dport_err_cnt == dport_err_cnt)
                && (T_class.qid_err_cnt == qid_err_cnt));
    }
};

inline ostream &operator<<(ostream &out, const s_err_list &T_class)
{
    out << "@" << sc_time_stamp()
        << " [evs:" << (T_class.err_var_sum)
        << ", iec:" << (T_class.inport_err_cnt)
        << ", fec:" << (T_class.fid_err_cnt)
        << ", tec:" << (T_class.type_err_cnt)
        << ", nec:" << (T_class.fsn_err_cnt)
        << ", hec:" << (T_class.hash_err_cnt)
        << ", lec:" << (T_class.len_err_cnt)
        << ", sec:" << (T_class.sport_err_cnt)
        << ", dec:" << (T_class.dport_err_cnt)
        << ", qec:" << (T_class.qid_err_cnt)
        << "] " << endl;
    return out;
}

inline void
#if defined(SC_API_VERSION_STRING)
sc_trace(sc_trace_file *tf, const s_err_list &T_class, const std::string &name)
#else
sc_trace(sc_trace_file *tf, const s_err_list &T_class, const sc_string &name)
#endif
{
    sc_trace(tf, T_class.err_var_sum, name + ".evs");
    sc_trace(tf, T_class.inport_err_cnt, name + ".iec");
    sc_trace(tf, T_class.fid_err_cnt, name + ".fec");
    sc_trace(tf, T_class.type_err_cnt, name + ".tec");
    sc_trace(tf, T_class.fsn_err_cnt, name + ".nec");
    sc_trace(tf, T_class.hash_err_cnt, name + ".hec");
    sc_trace(tf, T_class.len_err_cnt, name + ".lec");
    sc_trace(tf, T_class.sport_err_cnt, name + ".sec");
    sc_trace(tf, T_class.dport_err_cnt, name + ".dec");
    sc_trace(tf, T_class.qid_err_cnt, name + ".qec");
}

#define ASSERT(A) (assert(A))

class mod_logger: public logger::console_logger
{
public:
    using logger::console_logger::console_logger;

    logger::log_stream operator()(sc_module *mod, logger::level lv)
    {
        logger::log_stream ls(*this, lv);
        ls << "[cycle@" << g_cycle_cnt << "][" << mod->name() << "]: ";
        return ls;
    }
};

#define MOD_LOG__(lv) mod_lg_inst(this, lv)
#define MOD_LOG_DEBUG MOD_LOG__(logger::level::DEBUG)
#define MOD_LOG_INFO MOD_LOG__(logger::level::INFO)
#define MOD_LOG_WARNING MOD_LOG__(logger::level::WARNING)
#define MOD_LOG_ERROR MOD_LOG__(logger::level::ERROR)
#define MOD_LOG_FATAL MOD_LOG__(logger::level::FATAL)
#define MOD_LOG MOD_LOG_INFO
extern mod_logger mod_lg_inst;

#endif //__COMM_DEF_H__
