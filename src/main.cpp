#include "comm_def.h"
#include "systemc.h"
#include "top_carbon.h"
using namespace std;
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
    //子模块例化
    top_carbon  top_mod("top_carbon");
    //顶层绑定
    vector<sc_signal<pkt_desc> *>  tb_ing_sig;
    vector<sc_signal<pkt_desc> *>  egr_tb_sig;
    tb_ing_sig.resize(g_m_inter_num);
    egr_tb_sig.resize(g_m_inter_num);
    for(int i =0; i < g_m_inter_num; i++)
    {
        tb_ing_sig[i] =new sc_signal<pkt_desc> ();
        egr_tb_sig[i] =new sc_signal<pkt_desc> ();
        (*top_mod.in_ing_port[i])(*tb_ing_sig[i]);
        (*top_mod.out_egr_port[i])(*egr_tb_sig[i]);
    }

    sc_clock clk("clk",10,SC_NS);
    sc_start(1000,SC_NS); //启动仿真
    return 0;
}