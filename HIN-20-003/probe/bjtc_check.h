
#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/Utility.h"
#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"


class bjtc_check: public analyzer{
	public:
		bjtc_check(TString name, workflow &b0, ParaSet &ps0 ):analyzer(name,b0, ps0){
		}
		~bjtc_check(){}
	
};

void bjtc_check::analyze(){
	
}	
