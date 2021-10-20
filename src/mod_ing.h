#ifndef __MOD_ING_H__
#define __MOD_ING_H__
#include "comm_def.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: mod_ing 入口ing模块h文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
class mod_ing: public sc_module
{
    public:    
        mod_ing(sc_module_name name);
        SC_HAS_PROCESS(mod_ing);
        void  recv_cell_from_tb();
    public:
        array<sc_in<s_pkt_desc> *,g_inter_num>   in_port;
        sc_out<s_pkt_desc>            out_cell_que;
};
#endif