
#ifndef BASE_PAD_H
#define BASE_PAD_H

namespace default_plot_setup{
		Color_t color [] ={kBlue+1, kRed+1, kGreen+2, kAzure+7, kMagenta+2, kBlack};
}

class base_pad {
		public : base_pad(){ }
				 base_pad(TString name): pname(name){ 
						 //cd();
						 pad= (TPad*)gPad;
				 }
				 virtual void init(){
						 hframe = gPad->DrawFrame(0,0, 1,1);
						 hframe->SetName(pname+"_frame");
				 }
				 ~base_pad(){}
				 void autoYrange(float x1, float x2);
				 void autoYrange();
				 void add(TH1* h){
						 if(hframe == nullptr) hframe = h;
						 hist.push_back(h);
						 if(xmin< xmax) return;
						 xmin = h->GetXaxis()->GetXmin();
						 xmax = h->GetXaxis()->GetXmax();
				 }
				 virtual void setup_frame(){
						 hframe->SetAxisRange(xmin, xmax, "X");
						 autoYrange(xmin, xmax);
//						 hframe->SetAxisRange(ymin, ymax, "Y");
				 }
				 void draw(TString opt){
						 int i=0;
//						 cout<<hframe->GetName()<<endl;
						 if(hframe !=nullptr) setup_frame();
						 for(auto &it : hist){
								 style0(it, default_plot_setup::color[i]);
								 pad->cd();
								 it->Draw(opt+"same");
								 gPad->SetLogy(doLogy);
								 i++;}
				 }
				 virtual void style0(TH1* h, Color_t color){
						 h->SetMarkerStyle(marker);
						 h->SetMarkerSize(markerSize);
						 h->SetMarkerColor(color);
				 }
				 void copy_config(base_pad &p){
						xmin = p.xmin; xmax = p.xmax;
						doLogy = p.doLogy;
						marker = p.marker; markerSize = p.markerSize;
				 }
				virtual void setXrange(float x1, float x2) {xmin = x1; xmax = x2;}

				 float xmin = 0, xmax = -1;
				 float ymin = 0, ymax = -1;
				 bool doLogy = 0;
				 float upMargin = 0.1, downMargin = 0.1;
				 int marker = 20; float markerSize = 0.7;
				 TH1* hframe = nullptr;
				 TString pname;
				 std::vector<TH1*>hist;
				 TString xtitle, ytitle;
				 TPad* pad;
};

void base_pad::autoYrange(float x1, float x2){
		if(hist.size()==0) return;
		float max, min;
		max = get_max_in_range(hist.at(0), x1, x2);
		min = get_min_in_range(hist.at(0), x1, x2);
		for(int i=1; i< hist.size(); ++i){	
				float h = get_max_in_range(hist.at(i), x1, x2);
				float l = get_min_in_range(hist.at(i), x1, x2);
				if(max < h) max =h;
				if(min > l) min =l;
		}
		float marginUp = max+(max-min)*upMargin;
		float marginDown = min-(max-min)*downMargin;
		hframe->SetAxisRange(marginDown, marginUp, "Y");
		hframe->SetAxisRange(x1, x2, "X");
}

void base_pad::autoYrange(){
		auto h = hist.at(0);	
		float xmax = h->GetXaxis()->GetXmax();
		float xmin = h->GetXaxis()->GetXmin();
		autoYrange(xmin, xmax);
}

#endif

