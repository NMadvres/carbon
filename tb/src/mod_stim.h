#ifndef __STIM_H__
#define __STIM_H__

#include "systemc.h"
#include "../../src/comm_def.h"
#include <string>
#include <iostream>
#define SEND_FILE_NUM 64
//#define FLOW_RULE_TAB_SIZE 16
#define TOKEN_MAX_BYTE 125 * 100 * 2

class mod_stim: public sc_module
{
public:
    //端口
    sc_in<int> in_clk_cnt;
    std::array<sc_out<s_pkt_desc>, G_INTER_NUM> out_pkt_stim;

    //信号
    //  char* pkt_sender_filename;
    char *pkt_sender_filename = (char *)"pkt_sender_file.log";
    ofstream pkt_sender_file;
    vector<int> flow_sent_pkts;
    vector<int> flow_sent_bytes;
    vector<int> flow_dpd_pkts;
    vector<int> flow_dpd_bytes;
    vector<int> flow_sent_mbps;
    vector<int> port_sent_pkts;
    vector<int> port_sent_bytes;
    vector<int> port_dpd_pkts;
    vector<int> port_dpd_bytes;
    vector<int> port_sent_mbps;
    s_flow_rule flow_rule_nomatch;
    int flow_nomatch_sent_pkts;
    int flow_nomatch_sent_bytes;
    int flow_nomatch_dpd_pkts;
    int flow_nomatch_dpd_bytes;
    int flow_nomatch_sent_mbps;

public:
    mod_stim(sc_module_name name);
    SC_HAS_PROCESS(mod_stim);
    ~mod_stim();

public:
    void stim_prc();
};

class port_fifo
{
public:
    vector<s_pkt_desc> regs;
    bool full;
    bool empty;
    int pntr;

    port_fifo()
    {
        regs.resize(g_flow_rule_tab.size());
        full = false;
        empty = true;
        pntr = 0;
    }

public:
    void pkt_in(const s_pkt_desc &data_pkt);
    s_pkt_desc pkt_out();
    s_pkt_desc pkt_pre_val();
};

class token_bucket
{
public:
    int token;

    token_bucket()
    {
        token = 0;
    }

public:
    void add_token(const int &add_token_val);
    void sub_token(const int &sub_token_val);
    int read_token();
};

#endif // _STIM_H_