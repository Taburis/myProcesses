
#ifndef ANALYZERBASE_H
#define ANALYZERBASE_H

template <typename config>
class analyzerBase{
	public : 
		analyzerBase(config & cfg0){_cfg = &cfg0;}
		~analyzerBase(){}
		virtual void analyze() = 0;

		config *_cfg;
};

#endif
