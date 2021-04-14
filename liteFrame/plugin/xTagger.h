
#ifndef XTAGGER_H
#define XTAGGER_H

// this class to encode the enum type of data to binary 
// since it use the int (32bit), it can only encode for 32 tags

class xTagger {
	public : xTagger(){tag=0;}
		xTagger(int n){setTag(n);}
		~xTagger(){}
		xTagger( xTagger &&t):tag(std::move(t.tag)){}
		xTagger( const xTagger &t):tag(t.tag){}
		void addTag(int n)
		// the number n corresponds to the code of your tags;
		{tag+=((1<<n)&(~tag));}
		void addTag(const xTagger &t){
			tag = tag | t.tag;
		}
		void setTag(int n){tag = 1<<n; }
		xTagger &operator = (xTagger &t){
			tag = std::move(t.tag); return (*this);
		}
		bool select(int mk)
			{
			//argument mk should be the selection mask
			//check if the tag included in the selection bits
			//e.g. if selection mask = 0 1 1 and tag = 1 1 1, 
			//then tag include the selection bits
			//return 1 if it passed the selection
				return (mk & tag ) == mk;}
		bool select(xTagger &tg){return select(tg.tag);}

		int tag;
};

#endif
