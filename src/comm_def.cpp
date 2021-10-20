////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   commdef
// Description: 全局定义文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////

#include "comm_def.h"
//用于实现全局cycle_cnt计数
int g_cycle_cnt;
vector<s_flow_rule>  g_flow_rule_tab;
map<s_hash_rule_key, int>  g_hash_rule_tab;
vector<s_tab_que>  g_que_rule_tab;
vector<s_tab_port>  g_port_rule_tab;

mod_testcrtl_c::mod_testcrtl_c(sc_module_name name):sc_module(name)
{
    g_cycle_cnt =0;
    SC_METHOD(calc_cycle_cnt);
    sensitive << clk.pos();
}

void mod_testcrtl_c::calc_cycle_cnt()
{
    g_cycle_cnt ++;
}

//用于实现获取配置，表项填充
glb_cfg_c::glb_cfg_c()
{

}