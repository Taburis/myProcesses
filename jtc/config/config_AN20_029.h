
#include "myProcesses/jtc/plugin/ParaSet.h"

class eventMap;
namespace config {
	bool evtCut(eventMap* em){
		if(fabs(em->vz)>15) return 1;
		if(em->hiBin>=180) return 1;
		if(em->pthat<30) return 1;
		return 0;
	}
		
	float pthatWeight(eventMap* em){
		return em->weight;
	}
}

