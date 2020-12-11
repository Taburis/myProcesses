
#ifndef EVTCONTENTCONFIG_H
#define EVTCONTENTCONFIG_H

#if defined(event_content_skim)
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#elif defined(event_content_forest)
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#endif

template<typename event, typename pset>
void eventContentInit(event *evt, pset *ps){
	if(typeid(event)==typeid(eventMap)){
cout<<"---------------------detected input event content: eventMap------------------------"<<endl;
		evt->isMC=ps->isMC;
		evt->isHI=ps->isHI;
		evt->init();
		int nfilters = ps->nfilters;
		if(nfilters !=0) evt->regEventFilter(nfilters,ps->evtFilterString);
		int ntrigs = ps->ntrigs;
		if(ntrigs !=0) evt->loadTriggerInfo(ntrigs,ps->trigger);
	}
}

#endif
