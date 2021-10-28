#ifndef __STIM_H__
#define __STIM_H__

#include "systemc.h"
#include "../../src/comm_def.h"
#include <string>
#include <iostream>
#define SEND_FILE_NUM 64
#define FLOW_RULE_TAB_SIZE 16
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
    array<int, FLOW_RULE_TAB_SIZE> flow_sent_pkts;
    array<int, FLOW_RULE_TAB_SIZE> flow_sent_bytes;
    array<int, FLOW_RULE_TAB_SIZE> flow_dpd_pkts;
    array<int, FLOW_RULE_TAB_SIZE> flow_dpd_bytes;
    array<int, FLOW_RULE_TAB_SIZE> flow_sent_mbps;
    array<int, G_INTER_NUM> port_sent_pkts;
    array<int, G_INTER_NUM> port_sent_bytes;
    array<int, G_INTER_NUM> port_dpd_pkts;
    array<int, G_INTER_NUM> port_dpd_bytes;
    array<int, G_INTER_NUM> port_sent_mbps;

    SC_CTOR(mod_stim)
    {
        //    pkt_sender_file.open("pkt_sender_file.log");
        pkt_sender_file.open(pkt_sender_filename);

        SC_THREAD(stim_prc);
        sensitive << in_clk_cnt;
    }

    ~mod_stim()
    {
      for (int i = 0; i < FLOW_RULE_TAB_SIZE; i++){
        pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total drop packets:" << flow_dpd_pkts[i] << endl;
        pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total drop bytes  :" << flow_dpd_bytes[i] << endl;
        pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total send packets:" << flow_sent_pkts[i] << endl;
        pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] total send bytes  :" << flow_sent_bytes[i] << endl;
        pkt_sender_file << "@" << in_clk_cnt << ":flow [" << i << "] send speed(MBPS)  :" << flow_sent_mbps[i] << endl;
      }
      for (int i = 0; i < G_INTER_NUM; i++){
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

struct port_fifo
{
    std::array<s_pkt_desc, FLOW_RULE_TAB_SIZE> regs;
    bool full;
    bool empty;
    int pntr;

    port_fifo()
    {
        full = false;
        empty = true;
        pntr = 0;
    }

    void pkt_in(const s_pkt_desc &data_pkt);
    s_pkt_desc pkt_out();
    s_pkt_desc pkt_pre_val();
};

struct token_bucket
{
    int token;

    token_bucket()
    {
        token = 0;
    }

    void add_token(const int &add_token_val);
    void sub_token(const int &sub_token_val);
    int read_token();
};

#endif // _STIM_H_