
#ifndef XTAGGER_H
#define XTAGGER_H

// this class to encode the enum type of data to binary 
// since it use the int (32bit), it can only encode for 32 tags

class xTagger {
	public : xTagger(){tag=0;}
		xTagger(int n): tag(1<<n){}
		~xTagger(){}
		xTagger( xTagger &&t):tag(std::move(t.tag)){}
		xTagger( const xTagger &t):tag(t.tag){}
		void addTag(int n)
		{tag+=((1<<n)&(~tag));}
		void setTag(int n){tag = 1<<n; }
		xTagger &operator = (xTagger &t){
			tag = std::move(t.tag); return (*this);
		}
		bool select(int mk)
			{
			//check if the tag included in the selection bits
			//e.g. if selection = 1 1 1 and tag = 1 0 1, 
			//then it is included in selections
			//return 1 if it passed the selection
				return (mk & tag ) == mk;}
		bool select(xTagger &tg){return select(tg.tag);}

		int tag;
};

#endif
