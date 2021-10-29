#ifndef __STIM_H__
#define __STIM_H__

#include "systemc.h"
#include "../../src/comm_def.h"
#include <string>
#include <iostream>
#define SEND_FILE_NUM 64
//#define FLOW_RULE_TAB_SIZE 16
#define TOKEN_MAX_BYTE 125 * 100 * 2

struct mod_stim: sc_module
{
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

    SC_CTOR(mod_stim)
    {
        //    pkt_sender_file.open("pkt_sender_file.log");
        pkt_sender_file.open(pkt_sender_filename);

        flow_sent_pkts.resize(g_flow_rule_tab.size());
        flow_sent_bytes.resize(g_flow_rule_tab.size());
        flow_dpd_pkts.resize(g_flow_rule_tab.size());
        flow_dpd_bytes.resize(g_flow_rule_tab.size());
        flow_sent_mbps.resize(g_flow_rule_tab.size());
        port_sent_pkts.resize(G_INTER_NUM);
        port_sent_bytes.resize(G_INTER_NUM);
        port_dpd_pkts.resize(G_INTER_NUM);
        port_dpd_bytes.resize(G_INTER_NUM);
        port_sent_mbps.resize(G_INTER_NUM);

        SC_THREAD(stim_prc);
        sensitive << in_clk_cnt;
    }

    ~mod_stim()
    {
        for (int i = 0; i < (int)g_flow_rule_tab.size(); i++) {
            pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total drop packets:" << flow_dpd_pkts[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total drop bytes  :" << flow_dpd_bytes[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total send packets:" << flow_sent_pkts[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total send bytes  :" << flow_sent_bytes[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] send speed(MBPS)  :" << flow_sent_mbps[i] << endl;
        }
        for (int i = 0; i < G_INTER_NUM; i++) {
            pkt_sender_file << "@" << in_clk_cnt << ":port [" << i << "] total drop packets:" << port_dpd_pkts[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":port [" << i << "] total drop bytes  :" << port_dpd_bytes[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":port [" << i << "] total send packets:" << port_sent_pkts[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":port [" << i << "] total send bytes  :" << port_sent_bytes[i] << endl;
            pkt_sender_file << "@" << in_clk_cnt << ":port [" << i << "] send speed(MBPS)  :" << port_sent_mbps[i] << endl;
        }
        pkt_sender_file.flush();
        pkt_sender_file.close();
    }

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