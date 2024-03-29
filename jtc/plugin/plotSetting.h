
#ifndef AN20029_plotSetting
#define AN20029_plotSetting
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"

plotManager * plot_sigRef(TString name,TString path, matrixTH1Ptr* sig, TString lab_sig, matrixTH1Ptr* corr, TString labcorr, float xmin, float xmax){
	// this macro is used for plotting the corection compared with the signal; designed for showing the correction like JFF residual or spillover
	TString format = ".png";
	auto c = new plotManager();
	c->initSquarePad("canvas_"+name, "", sig->Nrow(), sig->Ncol());
	c->addm2TH1(sig, lab_sig, "pfl");
	c->addm2TH1(corr, labcorr, "pfl");
	c->setXrange(xmin, xmax);
	c->draw();
	c->drawHLine(0, 2);
	c->drawLegend("lin2right");
	c->save(path+"/"+name+format);	
	return c;
}

plotManager * plot_overlay_uncert(TString name,TString path, matrixTH1Ptr* j1, TString lab1, matrixTH1Ptr* j2, TString lab2, matrixTH1Ptr* j1e, matrixTH1Ptr* j2e, float xmin, float xmax, bool doLogy = 0){
	TString format = ".png";
	auto c = new plotManager();
	c->initOverlayPad("canvas_"+name, "", j1->Nrow(), j1->Ncol());
	for(int i=0; i<j1->Nrow(); i++){
		for(int j=0; j<j1->Ncol(); j++){
			c->addHist(j1->at(i,j), i, j1->Ncol()-1-j, lab1, "pl");
			c->addHist(j2->at(i,j), i, j1->Ncol()-1-j, lab2, "pl");
			c->at(i,j1->Ncol()-1-j)->doLogy=doLogy;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymin = 0.5;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymax = 1.5;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->addUncertainty(j1e->at(i,j), j2e->at(i,j));
		}
	}
	c->setXrange(xmin, xmax);
	c->setRatioYrange(0.7,1.3);
	//c->setRatioYrange(0, 2);
	c->draw();	
	c->drawLegend();
	c->save(path+"/"+name+format);	
	return c;
}
plotManager * plot_overlay(TString name,TString path, jtcTH1Player* j1, TString lab1, jtcTH1Player* j2, jtcTH1Player *j1e, jtcTH1Player *j2e, TString lab2, float xmin, float xmax, bool doLogy = 0){
	return plot_overlay_uncert(name,path, (matrixTH1Ptr*) j1, lab1, (matrixTH1Ptr*) j2, lab2, j1e, j2e,xmin, xmax, doLogy);
}



plotManager * plot_overlay(TString name,TString path, matrixTH1Ptr* j1, TString lab1, matrixTH1Ptr* j2, TString lab2, float xmin, float xmax, bool doLogy = 0){
cout<<"here "<<endl;
	TString format = ".png";
	auto c = new plotManager();
	c->initOverlayPad("canvas_"+name, "", j1->Nrow(), j1->Ncol());
	for(int i=0; i<j1->Nrow(); i++){
		for(int j=0; j<j1->Ncol(); j++){
cout<<i<<" , "<<j<<endl;
			c->addHist(j1->at(i,j), i, j1->Ncol()-1-j, lab1, "pl");
			c->addHist(j2->at(i,j), i, j1->Ncol()-1-j, lab2, "pl");
			c->at(i,j1->Ncol()-1-j)->doLogy=doLogy;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymin = 0.8;
			((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymax = 1.2;
		}
	}
	c->setXrange(xmin, xmax);
	c->draw();	
	c->drawLegend();
	c->save(path+"/"+name+format);	
	return c;
}

plotManager * plot_overlay(TString name,TString path, jtcTH1Player* j1, TString lab1, jtcTH1Player* j2, TString lab2, float xmin, float xmax, bool doLogy = 0){
	return plot_overlay(name,path, (matrixTH1Ptr*) j1, lab1, (matrixTH1Ptr*) j2, lab2, xmin, xmax, doLogy);
}

plotManager * plot_square(TString name,TString path, matrixTH1Ptr* j1, TString lab1,  float xmin, float xmax, bool doLogy = 0){
	TString format = ".png";
	auto c = new plotManager();
	c->initSquarePad("canvas_"+name, "", j1->Nrow(), j1->Ncol());
	for(int i=0; i<j1->Nrow(); i++){
		for(int j=0; j<j1->Ncol(); j++){
			c->addHist(j1->at(i,j), i, j1->Ncol()-1-j, lab1, "pl");
			c->at(i,j1->Ncol()-1-j)->doLogy=doLogy;
			//((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymin = 0.5;
			//((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymax = 1.5;
		}
	}
	c->setXrange(xmin, xmax);
	c->draw();	
	c->drawLegend();
	c->save(path+"/"+name+format);	
	return c;
}

plotManager * plot_square(TString name,TString path, matrixTH1Ptr* j1, TString lab1, matrixTH1Ptr* j2, TString lab2, float xmin, float xmax, float ymin=1, float ymax=0,  bool doLogy = 0){
	TString format = ".png";
	auto c = new plotManager();
	c->initSquarePad("canvas_"+name, "", j1->Nrow(), j1->Ncol());
	for(int i=0; i<j1->Nrow(); i++){
		for(int j=0; j<j1->Ncol(); j++){
			c->addHist(j1->at(i,j), i, j1->Ncol()-1-j, lab1, "pl");
			c->addHist(j2->at(i,j), i, j1->Ncol()-1-j, lab2, "pl");
			c->at(i,j1->Ncol()-1-j)->doLogy=doLogy;
			//((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymin = 0.5;
			//((overlayPad*)c->at(i,j1->Ncol()-1-j))->rymax = 1.5;
		}
	}
	c->setYrange(ymin, ymax);
	c->setXrange(xmin, xmax);
	c->draw();	
	c->drawLegend();
	c->save(path+"/"+name+format);	
	return c;
}

plotManager * plot_square(TString name,TString path, jtcTH1Player* j1, TString lab1, jtcTH1Player* j2, TString lab2, float xmin, float xmax, float ymin =1, float ymax = 0, bool doLogy = 0){
	return plot_square(name,path, (matrixTH1Ptr*) j1, lab1, (matrixTH1Ptr*) j2, lab2, xmin, xmax, ymin, ymax, doLogy);
}
#endif
