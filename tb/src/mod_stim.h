#ifndef __STIM_H__
#define __STIM_H__

#include "systemc.h"
#include "../../src/comm_def.h"
#include <string>
#include <iostream>
#define SEND_FILE_NUM 64
#define FLOW_RULE_TAB_SIZE 16

struct mod_stim: sc_module
{
<<<<<<< HEAD
  //端口
  sc_in<int> in_clk_cnt;
  std::array<sc_out<s_pkt_desc>, G_INTER_NUM> out_pkt_stim;  
//  char* pkt_sender_filename;
   char *pkt_sender_filename = (char*)"pkt_sender_file.log";

  //信号


  SC_CTOR(mod_stim)
  {
    SC_THREAD(stim_prc);
    sensitive << in_clk_cnt;
  }
  void stim_prc();
=======
    //端口
    sc_in<int> in_clk_cnt;
    std::array<sc_out<s_pkt_desc>, G_INTER_NUM> out_pkt_stim;
    //  int   pkt_inprt;
    //  char* pkt_sender_filename;
    char *pkt_sender_filename = (char *)"pkt_sender_file.log";

    //信号
    //  std::array<s_pkt_desc, G_INTER_NUM> pkt_desc_tmp;
    s_pkt_desc pkt_desc_tmp;
    s_flow_rule a;

    //std::array<array<s_pkt_desc, FLOW_RULE_TAB_SIZE>,G_INTER_NUM> fifo_regs;
    //std::array<bool,G_INTER_NUM> fifo_full;
    //std::array<bool,G_INTER_NUM> fifo_empty;
    //std::array<int,G_INTER_NUM>  fifo_pntr;

    SC_CTOR(mod_stim)
    {
        pkt_desc_tmp.type = -1;
        pkt_desc_tmp.fid = -1;
        pkt_desc_tmp.sid = -1;
        pkt_desc_tmp.did = -1;
        pkt_desc_tmp.fsn = -1;
        pkt_desc_tmp.len = -1;
        pkt_desc_tmp.pri = -1;
        pkt_desc_tmp.sport = -1;
        pkt_desc_tmp.dport = -1;
        pkt_desc_tmp.qid = -1;
        pkt_desc_tmp.vldl = -1;
        pkt_desc_tmp.csn = -1;
        pkt_desc_tmp.sop = false;
        pkt_desc_tmp.eop = false;

        ////vector<s_flow_rule>  g_flow_rule_tab;
        //for(int fid=0; fid<FLOW_RULE_TAB_SIZE; fid++)
        //{
        //    a.sid       = fid;
        //    a.did       = 0;
        //    a.len       = 64;
        //    a.pri       = 0;
        //    a.sport     = fid % G_INTER_NUM;
        //    a.dport     = 0;
        //    a.qid       = 0;
        //    a.len2add   = 1;
        //    a.flow_speed= 64;
        //
        //    g_flow_rule_tab.push_back(a);
        //}

        SC_THREAD(stim_prc);
        sensitive << in_clk_cnt;
    }
    void stim_prc();
>>>>>>> d40213ec89555ce24d08f16df3df7a037a83b47b
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

<<<<<<< HEAD
  void pkt_in(const s_pkt_desc& data_pkt);
  s_pkt_desc pkt_out();
  s_pkt_desc pkt_pre_val();
=======
    void pkt_in(const s_pkt_desc &data_pkt);

    s_pkt_desc pkt_out();

    s_pkt_desc pkt_pre_val();
>>>>>>> d40213ec89555ce24d08f16df3df7a037a83b47b
};

struct token_bucket
{
    int token;

    token_bucket()
    {
        token = 0;
    }

<<<<<<< HEAD
  void add_token(const int& add_token_val);
  void sub_token(const int& sub_token_val);
  int read_token();
=======
    void add_token(const int &add_token_val);

    void sub_token(const int &sub_token_val);

    int read_token();
>>>>>>> d40213ec89555ce24d08f16df3df7a037a83b47b
};

#endif // _STIM_H_