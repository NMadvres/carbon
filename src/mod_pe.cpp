#include "mod_pe.h"

////////////////////////////////////////////////////////
// Project： SystemC虚拟项目
// Module:   mod_pe
// Description: mod_pe PE模块cpp文件
// Group：预研组
// Author: Newton
// Date: 2021.10.14 第一版
// Hierarchy : 编号，索引公共库
////////////////////////////////////////////////////////
mod_pe::mod_pe(sc_module_name name):
    sc_module(name)
{
    SC_METHOD(on_recv_cell);
    sensitive << in_cell_que;
    dont_initialize();

    SC_METHOD(on_send_pkt);
    sensitive << in_clk_cnt;
    dont_initialize();
}

void mod_pe::on_recv_cell()
{
    const s_pkt_desc &cell = in_cell_que.read();

    if (cell.type != DESC_TYPE_CELL) {
        fprintf(stderr, "%s:%d: cell type error\n", __FUNCTION__, __LINE__);
        return;
    }

    if (pkt_que.empty() || pkt_que.back().type == DESC_TYPE_PKT) {
        if (!cell.sop) {
            fprintf(stderr, "%s:%d: sop error\n", __FUNCTION__, __LINE__);
            return;
        }

        pkt_que.push_back(cell);
        pkt_que.back().len = pkt_que.back().vldl;
    } else {
        // append cell data here
        pkt_que.back().len += pkt_que.back().vldl;
    }

    if (cell.eop) {
        pkt_que.back().type = DESC_TYPE_PKT;
        pkt_que.back().vldl = -1;
        pkt_que.back().csn = -1;
        pkt_que.back().sop = -1;
        pkt_que.back().eop = -1;
    }
}

void mod_pe::on_send_pkt()
{
    // TODO speed limit check

    if (pkt_que.empty())
        return;

    out_cell_que.write(pkt_que.front());
    pkt_que.pop_front();
}
