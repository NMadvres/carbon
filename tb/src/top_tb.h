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
    mod_stim *stim_mod;
    mod_stat *stat_mod;
    mod_testcrtl *testcrtl_mod;
    array<sc_in<s_pkt_desc> *, g_inter_num> in_pkt_ports;   //用于透传连线ing的输入端口信号,在顶层连接发包器
    array<sc_out<s_pkt_desc> *, g_inter_num> out_pkt_ports; //用于透传连线egr的输出端口信号,在顶层连接stat
    sc_in_clk in_glb_clk;                                   //用于透传连线testctrl输入信号,在顶层连接testctrl
    sc_out<int> out_clk_cnt;                                //用于透传连线testctrl输出信号,在顶层连接testctrl
    sc_in<int> in_clk_cnt;                                  //全局时钟计数，用于互联
public:
    top_tb(sc_module_name name):
        sc_module(name)
    {
        //例化
        stim_mod = new mod_stim("stim_mod");
        stat_mod = new mod_stat("stat_mod");
        testcrtl_mod = new mod_testcrtl("mod_testcrtl");
        for (int i = 0; i < g_inter_num; i++) {
            in_pkt_ports[i] = new sc_in<s_pkt_desc>();
            out_pkt_ports[i] = new sc_out<s_pkt_desc>();
        }
        //互联
        //ing的入口和egr的出口，连线透传到顶层，待更高层进行连接
        for (int i = 0; i < g_inter_num; i++) {
            stim_mod->out_pkt_ports[i]->bind(*out_pkt_ports[i]);
            stat_mod->in_pkt_ports[i]->bind(*in_pkt_ports[i]);
        }
        testcrtl_mod->in_glb_clk(in_glb_clk); //透传clk连线
        testcrtl_mod->out_clk_cnt(out_clk_cnt);
        //cycle计数信号透传
        stim_mod->in_clk_cnt(in_clk_cnt);
        stat_mod->in_clk_cnt(in_clk_cnt);
    }

    SC_HAS_PROCESS(top_tb);
};
#endif