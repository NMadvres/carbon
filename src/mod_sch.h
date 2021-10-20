#ifndef __MOD_SCH_H__
#define __MOD_SCH_H__
#include "comm_def.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_sch
// Description: mod_sch sch模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_sch: public sc_module
{
    public:    
        mod_sch(sc_module_name name);
        SC_HAS_PROCESS(mod_sch);
    public:
        sc_in<s_pkt_desc>             in_cell_que;
        sc_out<s_pkt_desc>            out_cell_que;
        sc_in<int>                    in_clk_cnt;  
};

#endif