#ifndef __MOD_TESTCTRL_H__
#define __MOD_TESTCTRL_H__
#include "comm_def.h"

class mod_testcrtl: public sc_module
{
   public:
   mod_testcrtl(sc_module_name name);
   SC_HAS_PROCESS(mod_testcrtl);
   sc_in_clk     in_glb_clk;
   void calc_cycle_cnt();
   sc_out<int>  out_clk_cnt;
};

#endif
