#ifndef __MOD_TESTCTRL_H__
#define __MOD_TESTCTRL_H__

#include "../../src/comm_def.h"

class mod_testctrl: public sc_module
{
public:
    mod_testctrl(sc_module_name name);
    SC_HAS_PROCESS(mod_testctrl);
    sc_in_clk in_glb_clk;
    void calc_cycle_cnt();
    sc_out<int> out_clk_cnt;
};

#endif // __MOD_TESTCTRL_H__
