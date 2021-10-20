#include "comm_def.h"
#include "systemc.h"
#include "top_carbon.h"
#include "top_tb.h"
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
    //全局计数器生成
    sc_clock clk("clk",10,SC_NS); //10ns一个周期，100MHZ
    mod_testcrtl_c *cycle_calc_c = new mod_testcrtl_c("mod_testcrtl_c");
    cycle_calc_c->clk(clk);
   
    //子模块例化
    top_carbon  top_carbon_mod("top_carbon");
    top_tb      top_tb_mod("top_tb");
    //顶层绑定
    array<sc_signal<s_pkt_desc> *, g_inter_num>  tb_ing_sig;
    array<sc_signal<s_pkt_desc> *, g_inter_num>  egr_tb_sig;
    for(int i =0; i < g_inter_num; i++)
    {
        tb_ing_sig[i] =new sc_signal<s_pkt_desc> ();
        egr_tb_sig[i] =new sc_signal<s_pkt_desc> ();
        (*top_carbon_mod.in_ing_port[i])(*tb_ing_sig[i]);
        (*top_carbon_mod.out_egr_port[i])(*egr_tb_sig[i]);
        (*top_tb_mod.in_pkt_ports[i])(*egr_tb_sig[i]);
        (*top_tb_mod.out_pkt_ports[i])(*tb_ing_sig[i]);
    }

    #if 0
    //map测试
    map<int, int>  g_que_rule_tab;
    g_que_rule_tab.insert(make_pair(11,100));
    g_que_rule_tab.insert(make_pair(12,100));
    g_que_rule_tab.insert(make_pair(13,100));
    int num = g_que_rule_tab.size();
    
    map<hash_rule_key_s, int>  g_hash_rule_tab;
    hash_rule_key_s a1;
    a1.sid =10;
    hash_rule_key_s a2;
    a2.sid =11;
    hash_rule_key_s a3;
    a3.sid =12;
    g_hash_rule_tab.insert(make_pair(a1,100));
    g_hash_rule_tab.insert(make_pair(a2,101));
    g_hash_rule_tab.insert(make_pair(a3,102));
    int num1 = g_hash_rule_tab.size();
    #endif 

    
    sc_start(1000,SC_NS); //启动仿真
    return 0;
}