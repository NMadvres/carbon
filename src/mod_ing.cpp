#include"mod_ing.h"
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description: mod_ing 入口ing模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_ing::mod_ing(sc_module_name name):sc_module(name)
{
    for(int i =0; i < g_inter_num; i++)
    {
        in_port[i] = new sc_in<s_pkt_desc>();
    }
    SC_METHOD(recv_cell_from_tb);
}
////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_ing
// Description:接收报文切片
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
void mod_ing::recv_cell_from_tb()
{ 
   

}
