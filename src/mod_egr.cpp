#include"mod_egr.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_egr
// Description: mod_egr 出口egr模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_egr::mod_egr(sc_module_name name):sc_module(name)
{
    out_port.resize(g_m_inter_num);
    for(int i =0; i < g_m_inter_num; i++)
    {
        out_port[i] = new sc_out<pkt_desc>();
    }

}
