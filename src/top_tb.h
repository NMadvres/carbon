#ifndef __TOP_TB_H__
#define __TOP_TB_H__
#include "comm_def.h"
#include "mod_stim.h"
#include "mod_stat.h"
#include "mod_testctrl.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   top_tb
// Description: top_tb 顶层互联
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class top_tb: public sc_module
{
  public: //例化及互联部分
    mod_stim  *stim_mod;
    mod_stat  *stat_mod;
    array<sc_in<s_pkt_desc> *, g_inter_num>   in_pkt_ports;  //用于透传连线ing的输入端口信号,在顶层连接发包器
    array<sc_out<s_pkt_desc> *,g_inter_num>   out_pkt_ports; //用于透传连线egr的输出端口信号,在顶层连接stat
  public:
    top_tb(sc_module_name name):sc_module(name)
    {
      //例化
      stim_mod =new mod_stim("mod_ing");
      stat_mod =new mod_stat("mod_sch");
      for(int i =0; i < g_inter_num; i++)
      {
        in_pkt_ports[i]  =  new sc_in<s_pkt_desc>(); 
        out_pkt_ports[i] = new sc_out<s_pkt_desc>();            
      }
      //互联
      //ing的入口和egr的出口，连线透传到顶层，待更高层进行连接
      for(int i =0; i < g_inter_num; i++)
      {
        stim_mod->out_pkt_ports[i]->bind(*out_pkt_ports[i]);
        stat_mod->in_pkt_ports[i]->bind(*in_pkt_ports[i]);
      }
    }

  SC_HAS_PROCESS(top_tb);
};
#endif