
#ifndef EVENTMAPBASE_H
#define EVENTMAPBASE_H
// this is a standlone event template for any other analysis

class eventMapBase {
		public : eventMap(){};
				 ~eventMap(){};
				 virtual void nextEvent()=0;
				 virtual void getEvent(Long64_t)=0;
				 
				 virtual bool eventFilter() = 0;
				 virtual double eventWeight() = 0;
};

#endif
