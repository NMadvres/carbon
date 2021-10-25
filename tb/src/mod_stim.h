#ifndef __STIM_H__
#define __STIM_H__

#include "systemc.h"
#include "comm_def.h" 
#include <string>
#include <iostream>
#define SEND_FILE_NUM 64
#define FLOW_RULE_TAB_SIZE 16

struct mod_stim: sc_module
{
  //端口
  sc_in<int> in_clk_cnt;
  std::array<sc_out<s_pkt_desc>, G_INTER_NUM> out_pkt_stim;  
//  char* pkt_sender_filename;
   char *pkt_sender_filename = (char*)"pkt_sender_file.log";

  //信号
    ofstream pkt_sender_file;

  SC_CTOR(mod_stim)
  {
//    pkt_sender_file.open("pkt_sender_file.log");
    pkt_sender_file.open(pkt_sender_filename);

    SC_THREAD(stim_prc);
    sensitive << in_clk_cnt;
  }

   ~mod_stim()
	{
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
  int  pntr;

  port_fifo()
  {
    full = false;
    empty = true;
    pntr = 0;
  }

  void pkt_in(const s_pkt_desc& data_pkt);
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

  void add_token(const int& add_token_val);
  void sub_token(const int& sub_token_val);
  int read_token();
};

#endif // _STIM_H_