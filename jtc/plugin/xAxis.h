

#ifndef xAxis_H
#define xAxis_H


class xAxis{
	public : xAxis (){
		 };
		 ~xAxis(){
			 delete bins;
		 }
		 xAxis (int nbin, const float *bin);
		 xAxis (int n, float xmin, float xmax){ makeAxis(n,xmin, xmax);}
		 void makeAxis(int n , float xmin, float xmax){
			 bins = new float[n+1];
			 nbin = n;
			 float dx = (xmax-xmin)/n;
			 for(int i=0; i<n; ++i) bins[i]=xmin+i*dx;
			 bins[n]=xmax;
		 }
		 void setAxis(int n, float *bin){
			 bins = new float[n+1];
			 nbin = n;
			 for(int i=0; i<n+1; ++i) bins[i]=bin[i];
		 }
		 int findBin(float x);
		 int find_bin_in_range(float x){
			if(x > xmax || x< xmin) return -1;
			return findBin(x);
		 }
		 template <typename T> long BinarySearch(long n, const T *array, T value)
		 {
			 //  Binary search in an array of n values to locate value.
			 //
			 // Array is supposed  to be sorted prior to this call.
			 // If match is found, function returns position of element.
			 // If no match found, function gives nearest element smaller than value.

			 const T* pind;
			 pind = std::lower_bound(array, array + n, value);
			 if ( (pind != array + n) && (*pind == value) )
				 return (pind - array);
			 else
				 return (pind - array - 1);
		 }

	public : 
		 float * bins=nullptr;
		 int     nbin;
		 float   xmax;
		 float   xmin;
		 char*   name;
};

xAxis::xAxis(int n, const float *bin){
	bins = new float[n+1];
	nbin = n;
	for(int i=0; i<n+1; ++i) bins[i]=bin[i];
}
//xAxis::xAxis(int n, const float *bin){
//	bins = new float[n+1];
//	nbin = n;
//	for(int i=0; i<n+1; ++i) bins[i]=bin[i];
//}

int xAxis::findBin(float x){
	return BinarySearch(nbin+1, bins, float(x));
}

#endif

