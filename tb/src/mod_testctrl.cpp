#include "mod_testctrl.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_testcrtl_c
// Description: mod_testcrtl_c 模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
int g_cycle_cnt;
mod_testcrtl::mod_testcrtl(sc_module_name name):
    sc_module(name)
{
    g_cycle_cnt = 0;
    SC_METHOD(calc_cycle_cnt);
    sensitive << in_glb_clk.pos();
}

void mod_testcrtl::calc_cycle_cnt()
{
    g_cycle_cnt++;
    out_clk_cnt.write(g_cycle_cnt);
}