#ifndef __TOP_TB_H__
#define __TOP_TB_H__

#include "../../src/comm_def.h"
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
public: // 例化及互联部分
    mod_stim *stim_mod;
    mod_stat *stat_mod;
    mod_testctrl *testctrl_mod;
    array<sc_in<s_pkt_desc> *, G_INTER_NUM> in_pkt_stat;   //用于透传连线stat的输入端口信号,在顶层连接egr
    array<sc_out<s_pkt_desc> *, G_INTER_NUM> out_pkt_stim; //用于透传连线stim的输出端口信号,在顶层连接ing
    sc_in_clk in_glb_clk;                                  // 用于透传连线testctrl输入信号,在顶层连接testctrl
    sc_out<int> out_clk_cnt;                               // 用于透传连线testctrl输出信号,在顶层连接testctrl
    sc_in<int> in_clk_cnt;                                 // 全局时钟计数，用于互联

public:
    top_tb(sc_module_name name, func_stat *base_top_stat):
        sc_module(name)
    {
        //例化 instantiation for module pointers.
        stim_mod = new mod_stim("stim_mod", base_top_stat);
        stat_mod = new mod_stat("stat_mod");
        testctrl_mod = new mod_testctrl("mod_testctrl");
        for (int i = 0; i < G_INTER_NUM; i++) {
            in_pkt_stat[i] = new sc_in<s_pkt_desc>();
            out_pkt_stim[i] = new sc_out<s_pkt_desc>();
        }

        //PORTING for all modules inside top_tb.
        //[stim module porting]
        //out_pkt_stim is to be link to ing via top_tb 
        for (int i = 0; i < G_INTER_NUM; i++) (stim_mod->out_pkt_stim[i])(*out_pkt_stim[i]);
        stim_mod->in_clk_cnt(in_clk_cnt);
        
        //[testctrl module porting], just to count the clock and feed the globle counter.
        testctrl_mod->in_glb_clk(in_glb_clk);   //透传clk连线
        testctrl_mod->out_clk_cnt(out_clk_cnt); //从 testctrl 模块传出时钟计数

        //[stat moudule porting]
        for (int i = 0; i < G_INTER_NUM; i++) stat_mod->in_pkt_stat[i]->bind(*in_pkt_stat[i]);
        stat_mod->in_clk_cnt(in_clk_cnt);
    }

    SC_HAS_PROCESS(top_tb);
};

#endif // __TOP_TB_H__
