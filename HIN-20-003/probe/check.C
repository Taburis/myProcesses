
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"

void check(){
	auto f1 = TFile::Open("step5_output_nominal.root");
	auto f2 = TFile::Open("step5_output_sm.root");
	//auto f2 = TFile::Open("step5_output_s2.root");
	auto f3 = TFile::Open("step5_output_s1.root");

	auto bjs0 = new jtcTH1Player("js_bjet_pp_data_*_*", 1, 3);
	auto bjs1 = new jtcTH1Player("js_bjet_Pb_data_*_*", 1, 3);
	auto bjs2 = new jtcTH1Player("js_bjet_Pb_data_*_*", 1, 3);
	auto bjs3 = new jtcTH1Player("js_bjet_Pb_data_*_*", 1, 3);
	bjs0->autoLoad(f1);
	bjs1->autoLoad(f1);
	bjs2->autoLoad(f2);
	bjs3->autoLoad(f3);

	auto ratio0 = new jtcTH1Player("division_js0_bjet_pp_data_*_*", 1, 3);
	auto ratio1 = new jtcTH1Player("division_js_bjet_Pb_data_*_*", 1, 3);
	auto ratio2 = new jtcTH1Player("division_js_bjet_Pb_data_*_*", 1, 3);
	auto ratio3 = new jtcTH1Player("division_js_bjet_Pb_data_*_*", 1, 3);
	ratio1->autoLoad(f1);
	ratio2->autoLoad(f2);
	ratio0->autoLoad(f1);
	ratio3->autoLoad(f3);

	auto c1= new plotManager();
	bjs0->at(0,0)->SetTitle("Cent: 0-10%");
	bjs0->at(0,1)->SetTitle("Cent: 10-30%");
	bjs0->at(0,2)->SetTitle("Cent: 30-90%");
	c1->initSquarePad("c_ratio","", 1,3);
	c1->addm2TH1(bjs0, "pp");	
	//c1->addm2TH1(bjs1, "nominal");	
	c1->addm2TH1(bjs2, "Smooth");	
	c1->addm2TH1(bjs3, "Extend 3");	
	c1->setYrange(0.01, 100);
	c1->doLogy=1;
	c1->setXrange(0., .99);
	c1->draw();
	c1->drawLegend();
	c1->save("/eos/user/w/wangx/www/AN20-029/test/result_overlay.png");

	auto c = new plotManager();
	ratio0->at(0,0)->SetTitle("Cent: 0-10%");
	ratio0->at(0,1)->SetTitle("Cent: 10-30%");
	ratio0->at(0,2)->SetTitle("Cent: 30-90%");
	c->initSquarePad("c_ratio","", 1,3);
	c->addm2TH1(ratio0, "pp");	
	//c->addm2TH1(ratio1, "nominal");	
	c->addm2TH1(ratio2, "Smooth");	
	c->addm2TH1(ratio3, "Extend 3");	
	c->setYrange(0.5, 2.5);
	c->setXrange(0., .99);
	c->draw();
	c->drawLegend();
	c->save("/eos/user/w/wangx/www/AN20-029/test/result_ratio_overlay.png");

	auto fstep4= TFile::Open("step4_output_sm.root");
	auto js_bjet_pb_syst = new jtcTH1Player("js_bjet/js_bjet_data_bkgError2_*_*",1, 3);
	auto js_incl_pb_syst= new jtcTH1Player("js_inclusive/js_inclusive_data_bkgError2_*_*",1, 3);
	js_bjet_pb_syst->autoLoad(fstep4);
	js_incl_pb_syst->autoLoad(fstep4);
	js_bjet_pb_syst->area_normalize(0,1.0, "width");
	js_incl_pb_syst->area_normalize(0,1.0, "width");
	auto ratio_syst=(jtcTH1Player*) js_bjet_pb_syst->divide(*js_incl_pb_syst);

	auto fstep42 = TFile::Open("step4_output_s1.root");
	auto js2_bjet_pb_syst = new jtcTH1Player("js_bjet/js_bjet_data_bkgError2_*_*",1, 3);
	auto js2_incl_pb_syst = new jtcTH1Player("js_inclusive/js_inclusive_data_bkgError2_*_*",1, 3);
	js2_bjet_pb_syst->autoLoad(fstep4);
	js2_incl_pb_syst->autoLoad(fstep4);
	js2_bjet_pb_syst->area_normalize(0,1.0, "width");
	js2_incl_pb_syst->area_normalize(0,1.0, "width");
	auto ratio_b_syst = (jtcTH1Player*) js_bjet_pb_syst->divide(*js2_bjet_pb_syst);

	auto c2= new plotManager();
	bjs2->at(0,0)->SetTitle("Cent: 0-10%");
	bjs2->at(0,1)->SetTitle("Cent: 10-30%");
	bjs2->at(0,2)->SetTitle("Cent: 30-90%");
	c2->initOverlayPad("c_ratio","", 1,3);
	c2->addm2TH1(bjs2, "Smooth");	
	c2->addm2TH1(bjs3, "Extend 3");	
	c2->setYrange(0.01, 100);
	c2->doLogy=1;
	c2->setXrange(0., .99);
	c2->setRatioYrange(0.8, 1.2);
	for(auto i=0;i<3; i++){
		((overlayPad*)c2->at(0,i))->addUncertainty(js_bjet_pb_syst->at(0,i),js2_bjet_pb_syst->at(0,i));
	}
	c2->draw();
	c2->drawLegend();
	c2->save("/eos/user/w/wangx/www/AN20-029/test/result_overlay_syst.png");


	auto c3 = new plotManager();
	ratio2->at(0,0)->SetTitle("Cent: 0-10%");
	ratio2->at(0,1)->SetTitle("Cent: 10-30%");
	ratio2->at(0,2)->SetTitle("Cent: 30-90%");
	c3->initSquarePad("c_syst","", 1,3);
	c3->addm2TH1(ratio2, "Smooth");	
	c3->addm2TH1(ratio3, "Extend 3");	
	c3->addm2TH1(ratio_syst, "Syst", "pf", "e2");	
	c3->setYrange(0.5, 2.5);
	c3->setXrange(0., .99);
	c3->draw();
	c3->drawLegend();
	c3->save("/eos/user/w/wangx/www/AN20-029/test/result_ratio_syst.png");
}
