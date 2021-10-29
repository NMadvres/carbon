#include "comm_def.h"
#include "systemc.h"
#include "top_carbon.h"
#include "../tb/src/top_tb.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   main
// Description: 主函数
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
int sc_main(int argc, char *argv[])
{
    //全局计数器生成
    sc_clock clk("clk", 10, SC_NS); //10ns一个周期，100MHZ
                                    //   glb_cfg_c glb_cfg("temp_testcase.tab");
    glb_cfg_c glb_cfg("../src/temp_testcase.tab");

    //子模块例化
    top_carbon top_carbon_mod("top_carbon");
    top_tb top_tb_mod("top_tb");
    //顶层绑定
    array<sc_signal<s_pkt_desc> *, G_INTER_NUM> tb_ing_sig;
    array<sc_signal<s_pkt_desc> *, G_INTER_NUM> egr_tb_sig;
    for (int i = 0; i < G_INTER_NUM; i++) {
        tb_ing_sig[i] = new sc_signal<s_pkt_desc>();
        egr_tb_sig[i] = new sc_signal<s_pkt_desc>();
        (*top_carbon_mod.in_ing_port[i])(*tb_ing_sig[i]);
        (*top_carbon_mod.out_egr_port[i])(*egr_tb_sig[i]);
        (*top_tb_mod.in_pkt_stat[i])(*egr_tb_sig[i]);
        (*top_tb_mod.out_pkt_stim[i])(*tb_ing_sig[i]);
    }
    //绑定入口主时钟
    top_tb_mod.in_glb_clk(clk);
    //绑定cnt计数
    sc_signal<int> cycle_cnt_sig;
    top_tb_mod.out_clk_cnt(cycle_cnt_sig); //一发多收
    top_tb_mod.in_clk_cnt(cycle_cnt_sig);  //一发多收
    top_carbon_mod.in_clk_cnt(cycle_cnt_sig);

    sc_start(100000, SC_NS); //启动仿真
    top_tb_mod.stim_mod->~mod_stim();
    return 0;
}
