
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"
#include "myProcesses/jtc/plugin/workflow.h"
#include "myProcesses/jtc/plugin/jtcUti.h"

class bjtc_xcheck : public analyzer{
	public :
		bjtc_xcheck(TString name, workflow &base0, ParaSet &ps ):analyzer(name,base0, ps){}
		virtual void analyze() override;
		void xcheck(bool doRatio  = false);

		TFile *debugf, *fstep2, *fstep3, *fstep4;
		TFile *fstep2_uncert;
		jtcTH1Player *trkeff, *biaseff, *jffCorr, *spillOver, *trkeff_incl, *jffCorr_incl, *spillOver_incl, *contbias;
		TString format = ".png";
		TFile *fresult;
		TString step2fname, step3fname,step4fname, step2Uncertfname;
};



void bjtc_xcheck::xcheck(bool doRatio){
	int ptbin[] = {1, 2, 3, 4, 8, 12, 16, 20, 300};
	TString cent[] = {"Cent0", "Cent10", "Cent30", "Cent50", "Cent100"};

	TH1D* rs1[8][3];
	TH1D* rspp[8][3];
	auto fref = TFile::Open("../data/HIN16020/Jet_Shapes_pTweighted_HIN_16_020.root");
	for(int i=0; i<8; i++){
		for(int j=0; j<3; j++){
			TString name = TString("JetShape2_Yield_BkgSub_pTweightedInclusive_pp")+Form("_TrkPt%d",ptbin[i])+Form("_TrkPt%d",ptbin[i+1]);
			rspp[i][j]=(TH1D*) fref->Get(name)->Clone(Form("js_pp_HIN160020_%d_%d",i,j));
			name = "JetShape2_Yield_BkgSub_pTweightedInclusive_"+cent[j]+"_"+cent[j+1]+Form("_TrkPt%d",ptbin[i])+Form("_TrkPt%d",ptbin[i+1]);
			rs1[i][j]=(TH1D*) fref->Get(name);
		}
	}
	auto js1 = new jtcTH1Player("js_16020", 6,3);
	auto jspp1 = new jtcTH1Player("js_16020_pp_*_*", 6,3);
	auto js2 = new jtcTH1Player("js_inclusive/js_inclusive_data_*_*", 6,3);
	js2->autoLoad(fstep4);
	for(auto j=0;j<3; j++){
		for(auto i=0;i<6; i++){
			js1->add(rs1[i][j], i,j);	
			rs1[i][j]->GetXaxis()->SetTitle("#Deltar");
			jspp1->add(rspp[i][j],i,j);
		}
		jspp1->at(5, j)->Add(rspp[6][j]);
		jspp1->at(5, j)->Add(rspp[7][j]);
		js1->at(5, j)->Add(rs1[6][j]);
		js1->at(5, j)->Add(rs1[7][j]);
	}

	// Jussi's results as reference 
	auto fjussi = TFile::Open("/afs/cern.ch/work/j/jviinika/public/forXiao/dijetPbPb2018_akFlowPuCs4PFJets_5eveMix_calo100Trigger_JECv6_finalTrack_wtaAxis_allCorrectionsWithCentralityShift_modifiedDeltaR_onlyFinalResults_processed_2019-10-18.root");
	auto jussi = new jtcTH1Player("JetShape_trackJetInclusivePtWeighted", 6,3);
	for(auto i=1; i<7; i++){
		for(auto j=0; j<3; j++){
			TString key = Form("JetShape_trackJetInclusivePtWeighted/JetShape_trackJetInclusivePtWeighted_C%dT%d",j,i);
			auto  h =(TH1D*) fjussi->Get(key);
			//h->SetAxisRange(0.,2.0,"X");
			//cout<<"key: "<<key<<endl;
			//cout<<"Name: "<<h->GetName()<<endl;
			//cout<<"Name: "<<h->GetName()<<endl;
			jussi->add(h, i-1,j);
		}
	}
	jussi->setName("js_incl_jussi_ptweighted_*_*");
	auto jussi_ref = (jtcTH1Player*) js1->clone("jussi_ref");
	for(auto j =0; j<js2->Nrow(); j++){
			for(auto i =0; i<js2->Ncol(); i++){
				auto h = (TH1D*) jussi_ref->at(i,j);
				auto h1 = (TH1D*)jussi->at(i,j);
				for(auto k=1 ; k<15; k++){
					auto c = h1->GetBinContent(k);
					auto e = h1->GetBinError(k);
					h->SetBinContent(k,c);
					h->SetBinError(k,e);
				}
			}
	}


	auto c = new plotManager();
	//c->initSquarePad("canvas_check", "", 6,3);
	c->initOverlayPad("canvas_check", "", 6,3);
//	c->addm2TH1(jspp1, "HIN-16-020");
	c->addm2TH1(js1, "HIN-16-020");
	c->addm2TH1(js2, "HIN-20-003");
	c->addm2TH1(jussi_ref, "Jussi");
	c->setXrange(0,0.99);
	//c->setRatioYrange(0,10);
	c->setRatioYrange(0.5,1.5);
	c->draw();
	c->drawLegend();
	c->save(fig_output+"/compare_with_16_020.png");

	if(!doRatio) return;
	auto fpp = TFile::Open("../data/HIN18020/AN17337Result_new.root");
	auto js_incl_pp = new jtcTH1Player("dr_signal_dijtc_jetShape_step3_*_*",base->npt, 1);
	js_incl_pp->autoLoad(fpp);
	auto js_incl_pp_sum = js_incl_pp->contractX("js_wta_pp_sum");
	js_incl_pp_sum->duplicateY("js_wta_incl_pp_sum_ref", 3);

	auto js_incl_pb_sum = (jtcTH1Player*) js2->contractX("incl_HIN20003_pb_sum");
	auto js_incl_pb_sum_ref = (jtcTH1Player*) js1->contractX("incl_HIN16020_pb_sum");
	auto js_incl_pp_sum_dup = (jtcTH1Player*) jspp1->contractX("incl_HIN16020_pp_sum");
//	js_incl_pp_sum_dup->duplicateX("js_escheme_incl_pp_sum_ref",3);
	auto js_incl_jussi= jussi->contractX("js_incl_jussi");
	auto ratio_ref = (jtcTH1Player*) js_incl_pb_sum_ref->divide(*js_incl_pp_sum_dup);
	auto ratio_wta = (jtcTH1Player*) js_incl_pb_sum->divide(*js_incl_pp_sum);
	auto ratio_jussi = (jtcTH1Player*) js_incl_jussi->divide(*js_incl_pp_sum);


	auto c0 = new plotManager();
	c0->initSquarePad("canvas_check_integrated_incl", "", 1,3);
	c0->addm2TH1(js_incl_pb_sum_ref, "HIN-16-020");
	c0->addm2TH1(js_incl_pb_sum, "WTA");
	c0->addm2TH1(js_incl_jussi, "Jussi");
	c0->setXrange(0,0.99);
	c0->draw();
	c0->drawLegend();
	c0->save(fig_output+"/compare_Ref_HIN16020_integrated.png");

	auto c1 = new plotManager();
	c1->initSquarePad("canvas_check_integrated_incl", "", 1,3);
	//c1->initOverlayPad("canvas_check_integrated_incl", "", 1,3);
	//c1->addm2TH1(js_incl_pp_sum_dup, "HIN-16-020");
	c1->addm2TH1(ratio_ref, "HIN-16-020");
	c1->addm2TH1(ratio_wta, "WTA");
	c1->addm2TH1(ratio_jussi, "Jussi");
	c1->setXrange(0,0.99);
	c1->setYrange(0.5,2.5);
	c1->draw();
	c1->drawLegend();
	c1->save(fig_output+"/compare_WTA_vs_EScheme.png");

}

void bjtc_xcheck::analyze(){
	fstep2 = TFile::Open(output+"/"+step2fname+".root");
	fstep3 = TFile::Open(output+"/"+step3fname+".root");
	fstep4 = TFile::Open(step4fname+".root");
	fstep2_uncert= TFile::Open(output+"/"+step2Uncertfname+".root");
	xcheck(1);
}

