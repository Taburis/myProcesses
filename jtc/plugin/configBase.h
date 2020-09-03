
#ifndef configBase_H
#define configBase_H

namespace configuration{
	template<typename PSet, typename Cuts, typename Weight>
		class configBase{
			public :
				configBase(){}
				~configBase(){}
				PSet pset;
				Cuts cuts;
				Weight weight;
				
				bool isMC =1, isHI = 1;
				TString jetSetName = "";
		};
}
#endif
