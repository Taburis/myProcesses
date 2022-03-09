
#include "myProcesses/jtc/plugin/workflow.h"
//#include "myProcesses/jtc/plugin/Utility.h"
//#include "myProcesses/jtc/plugin/plotLib.h"
#include "myProcesses/jtc/plugin/plotSetting.h"
#include "myProcesses/jtc/plugin/jtcTH1Player.h"
#include "myProcesses/liteFrame/plugin/plots/plotManager.h"

class test_analyzer: public analyzer{
		public:
				test_analyzer(TString name, workflow &b0, ParaSet &ps0 ):analyzer(name,b0, ps0){
						npt = ps0.getPara<int>("npt");
						ncent = ps0.getPara<int>("ncent");
				}
				~test_analyzer(){}
				virtual void analyze();

				TString format = ".jpg", step2fname, step2uncert, systematic;
				int npt, ncent;
				TDirectory* _dir_; //working folder.
				TFile *fstep2, *funcert, *fsystem;
};

//float dr_fine[] = {0.,0.02, 0.04,0.06, 0.08,0.1,0.125,0.15,0.2,0.25,0.3,0.35,0.4,0.5,0.6,0.7,0.8,0.9,1.,1.2, 1.5, 2, 2.5};
//float dr_trk[] = {0.,0.02, 0.04,0.06, 0.08,0.1,0.125,0.15,0.2,0.25,0.3,0.35,0.4,0.5,0.6,0.8,1., 2.5};
//int ndr_trk = 17;
//int ndr_fine = 22;

void test_analyzer::analyze(){
		cout<<"Opening file: "<<output+"/"+step2fname+".root"<<endl;
		fstep2= TFile::Open(output+"/"+step2fname+".root");
		cout<<"Opening file: "<<output+"/"+step2uncert+".root"<<endl;
		funcert= TFile::Open(output+"/"+step2uncert+".root");
		_dir_ = f->mkdir(_name_);
		if(_dir_==0) _dir_=(TDirectory*) f->Get(_name_);
		f->cd(_name_);

		TString folder = "correlations_djetMC_std_c5shift";
		TString sname = "incl";
		TString corr_name = sname+"_trkEff";

		auto rec_p2 = new jtcTH1Player(folder+"/"+sname+reco_tag(1,1)+"_sig_p2_dr_*_*",npt, ncent);
		auto gen_p2 = new jtcTH1Player(folder+"/"+sname+reco_tag(1,0)+"_sig_p2_dr_*_*",npt, ncent);
		rec_p2->autoLoad(fstep2);
		gen_p2->autoLoad(fstep2);
		auto trk_p2 = (jtcTH1Player*) rec_p2->divide(*gen_p2, "B");

		auto rec_p1 = new jtcTH1Player(folder+"/"+sname+reco_tag(1,1)+"_sig_p1_dr_*_*",npt, ncent);
		auto gen_p1 = new jtcTH1Player(folder+"/"+sname+reco_tag(1,0)+"_sig_p1_dr_*_*",npt, ncent);
		rec_p1->autoLoad(fstep2);
		gen_p1->autoLoad(fstep2);
		auto trk_p1 = (jtcTH1Player*) rec_p1->divide(*gen_p1, "B");

		auto rec_p0= new jtcTH1Player(folder+"/"+sname+reco_tag(1,1)+"_sig_p0_dr_*_*",npt, ncent);
		auto gen_p0= new jtcTH1Player(folder+"/"+sname+reco_tag(1,0)+"_sig_p0_dr_*_*",npt, ncent);
		rec_p0->autoLoad(fstep2);
		gen_p0->autoLoad(fstep2);
		auto trkcorr_p0 = (jtcTH1Player*) rec_p0->divide(*gen_p0, "B");


		auto trkcorr_probe = (jtcTH1Player*) trk_p2->clone(sname+"_trkEff_probe");
		auto trk1 = (jtcTH1Player*) rec_p2->divide(*gen_p2, "B");
		auto trkcorr =(jtcTH1Player*)trk1->clone(sname+"_trkEff_p1_smth");
		for(int i=0;i<trk_p1->Nrow(); i++){
				for(int j=0;j<trk_p1->Ncol(); j++){
						TH1D* h = (TH1D*) trkcorr_probe->at(i,j);
						TH1D* h_p0 =(TH1D*) trkcorr_p0->at(i,j);
						TH1D* h_p1 =(TH1D*) trk_p1->at(i,j);
						for(auto k=1; k<h->GetNbinsX()+1; k++){
								float x = h->GetBinLowEdge(k);
								if(x<0.5) continue;
								//if(k!=1 && x<0.5) continue;
								float c = h_p0->GetBinContent(k);
								float e = h_p0->GetBinError(k);
								h->SetBinContent(k, c);
								h->SetBinError(k, e);
								trkcorr_probe->at(i,j)->SetAxisRange(0.1,2.5,"X");
								trkcorr->at(i,j)->SetAxisRange(0.1,2.5,"X");
						}
				}}
		trkcorr_probe->smooth(1, "R");
		trkcorr->smooth(1, "R");
		for(int i=0;i<trkcorr->Nrow(); i++){
				for(int j=0;j<trkcorr->Ncol(); j++){
						trkcorr->at(i,j)->SetAxisRange(0.,2.5,"X");
						trkcorr_probe->at(i,j)->SetAxisRange(0.,2.5,"X");
				}}
		//smoothing_regular(trk1, trkcorr);

		auto rr_sig_p1 = new jtcTH1Player("correlations_djetMC_std_c5shift/incl"+reco_tag(1,1)+"_sig_p1_*_*",npt, ncent);
		auto rr2_sig_p1 = new jtcTH1Player("correlations_djetMC_std2_c5shift/incl"+reco_tag(1,1)+"_sig_p1_*_*",npt, ncent);
		auto rg_sig_p1 = new jtcTH1Player("correlations_djetMC_std_c5shift/incl"+reco_tag(1,0)+"_sig_p1_*_*",npt, ncent);
		rr_sig_p1->autoLoad(fstep2);
		rr2_sig_p1->autoLoad(fstep2);
		rg_sig_p1->autoLoad(fstep2);

		auto rr_as_deta_p1 = rr_sig_p1->projX("rr_deta_AS_p1", 2.64159, 3.64159, "e");
		auto rg_as_deta_p1 = rg_sig_p1->projX("rg_deta_AS_p1", 2.64159, 3.64159, "e");
		auto rr2_as_deta_p1 = rr2_sig_p1->projX("rr2_deta_AS_p1", 2.64159, 3.64159, "e");

		jtcTH1Player rr_sig_p0(folder+"/"+sname+reco_tag(1,1)+"_sig_p0_*_*",npt, ncent);
		jtcTH1Player rg_sig_p0(folder+"/"+sname+reco_tag(1,0)+"_sig_p0_*_*",npt, ncent);
		rr_sig_p0.autoLoad(fstep2);
		rg_sig_p0.autoLoad(fstep2);

		auto rr_as_deta_p0= rr_sig_p0.projX("rr_as_p0_deta", 2.64159, 3.64159, "e");
		auto rg_as_deta_p0= rg_sig_p0.projX("rg_as_p0_deta", 2.64159, 3.64159, "e");
		auto trk_deta_p0 = (jtcTH1Player*) rr_as_deta_p0->divide(*rg_as_deta_p0);
		auto trk_deta_p1 = (jtcTH1Player*) rr_as_deta_p1->divide(*rg_as_deta_p1);

		auto rr_as_dr_p0 = rr_sig_p0.drIntegral(sname+"_sig_dr_rec_as_*_*", 0, 3.14159, ndr_fine, dr_fine, "area");
		auto rg_as_dr_p0 = rg_sig_p0.drIntegral(sname+"_sig_dr_gen_as_*_*", 0, 3.14159, ndr_fine, dr_fine, "area");
		auto rg_as_dr_p1 = rg_sig_p1->drIntegral(sname+"_sig_dr_rg_as_*_*", 0, 3.14159, ndr_fine, dr_fine, "area");
		auto rr_as_dr_p1 = rr_sig_p1->drIntegral(sname+"_sig_dr_rr_as_*_*", 0, 3.14159, ndr_fine, dr_fine, "area");
		auto trk_as_p0 = (jtcTH1Player*) rr_as_dr_p0->divide(*rg_as_dr_p0, "B");
		auto trk_as_p1 = (jtcTH1Player*) rr_as_dr_p1->divide(*rg_as_dr_p1, "B");



		auto rr_sig_deta_p1 = rr_sig_p1->projX("rr_deta_sig_p1", -0.5, 0.5, "e");
		auto rg_sig_deta_p1 = rg_sig_p1->projX("rg_deta_sig_p1", -0.5, 0.5, "e");
		auto rr2_sig_deta_p1 = rr2_sig_p1->projX("rr2_deta_sig_p1", -0.5, 0.5, "e");
		auto c0 = new plotManager();
		c0->initSquarePad("dev_p1_check", "", npt,ncent);
		//c0->addm2TH1(rg_sig_deta_p1, "RG AS");
		c0->addm2TH1(rr_sig_deta_p1, "RR");
		c0->addm2TH1(rr2_sig_deta_p1, "RR2");
		c0->setXrange(-2.5, 2.499);
		c0->draw();
		c0->drawLegend();
		c0->save(fig_output+"/check_sig_deta_p1_check.png");

		auto rr_sig_p2 = new jtcTH1Player("correlations_djetMC_std_c5shift/incl"+reco_tag(1,1)+"_sig_p2_*_*",npt, ncent);
		auto rr2_sig_p2 = new jtcTH1Player("correlations_djetMC_std2_c5shift/incl"+reco_tag(1,1)+"_sig_p2_*_*",npt, ncent);
		auto rg_sig_p2 = new jtcTH1Player("correlations_djetMC_std_c5shift/incl"+reco_tag(1,0)+"_sig_p2_*_*",npt, ncent);
		rr_sig_p2->autoLoad(fstep2);
		rr2_sig_p2->autoLoad(fstep2);
		rg_sig_p2->autoLoad(fstep2);


		auto  rr_sig_deta_p2 = rr_sig_p2->projX("rr_sig_deta_p2" , -0.5, 0.5, "e");
		auto  rg_sig_deta_p2 = rg_sig_p2->projX("rg_sig_deta_p2" , -0.5, 0.5, "e");
		auto rr2_sig_deta_p2 =rr2_sig_p2->projX("rr2_sig_deta_p2", -0.5, 0.5, "e");

		auto c1 = new plotManager();
		c1->initSquarePad("dev_sig_p2_deta_check", "", npt,ncent);
		c1->addm2TH1(rg_sig_deta_p2, "RG");
		c1->addm2TH1(rr_sig_deta_p2, "RR");
		c1->addm2TH1(rr2_sig_deta_p2, "RR2");
		c1->setXrange(-2.5, 2.499);
		c1->draw();
		c1->drawLegend();
		c1->save(fig_output+"/check_sig_p2_check.png");

		auto c11 = new plotManager();
		rg_as_deta_p1->setXtitle("#Delta#eta");
		c11->initOverlayPad("dev_as_p1_check", "", npt,ncent);
		c11->addm2TH1(rg_as_deta_p1, "RG AS");
		c11->addm2TH1(rr_as_deta_p1, "RR AS");
		c11->addm2TH1(rr2_as_deta_p1, "RR2 AS");
		c11->setXrange(-2.5, 2.499);
		c11->draw();
		c11->drawLegend();
		c11->save(fig_output+"/check_AS_p1_check.png");

		auto rr_as_deta_p2 = rr_sig_p2->projX("rr_deta_AS_p2", 2.64159, 3.64159, "e");
		auto rg_as_deta_p2 = rg_sig_p2->projX("rg_deta_AS_p2", 2.64159, 3.64159, "e");
		auto rr2_as_deta_p2 = rr2_sig_p2->projX("rr2_deta_AS_p2", 2.64159, 3.64159, "e");
		auto c01 = new plotManager();
		rg_as_deta_p2->setXtitle("#Delta#eta");
		c01->initOverlayPad("dev_p1_check", "", npt,ncent);
		c01->addm2TH1(rg_as_deta_p2, "RG AS");
		c01->addm2TH1(rr_as_deta_p2, "RR AS");
		c01->addm2TH1(rr2_as_deta_p2, "RR2 AS");
		c01->setXrange(-2.5, 2.499);
		c01->draw();
		c01->drawLegend();
		c01->save(fig_output+"/check_AS_p2_check.png");

		auto rr_as_deta_trk_p1 = (jtcTH1Player*) rr_as_deta_p1->clone("rr_as_deta_trk_p1");
		//auto rr_as_deta_trk_p1 = (jtcTH1Player*) rr_as_deta_p1->divide(*trk_deta_p0);
		auto rr2_as_deta_trk_p1 = (jtcTH1Player*) rr2_as_deta_p1->divide(*trk_deta_p0);
		auto rr_as_deta_trk_p2 = (jtcTH1Player*) rr_as_deta_p2->divide(*trk_deta_p0);
		auto rr2_as_deta_trk_p2 = (jtcTH1Player*) rr2_as_deta_p2->divide(*trk_deta_p1);
		for(auto i=0; i<npt; i++){
				for(auto j=0; j<ncent; j++){
						auto nbin = trk_deta_p1->at(i,j)->FindBin(1.99);
						auto eff = trk_deta_p1->at(i,j)->GetBinContent(nbin);
						auto h = rr_as_deta_trk_p1->at(i,j);
						h->Scale(1.0/eff);
				}
		}


		auto c12 = new plotManager();
		c12->initOverlayPad("dev_as_deta_p1_trk_check", "", npt,ncent);
		c12->addm2TH1(rg_as_deta_p1, "RG AS");
		c12->addm2TH1(rr_as_deta_trk_p1, "RR AS");
		//	c12->addm2TH1(rr2_as_deta_trk_p1, "RR2 AS");
		c12->setXrange(-2.5, 2.499);
		c12->setRatioYrange(0.9, 1.1);
		c12->draw();
		c12->drawLegend();
		c12->save(fig_output+"/check_AS_p1_trk_check.png");

		auto c02 = new plotManager();
		c02->initOverlayPad("dev_as_deta_p2_trk_check", "", npt,ncent);
		c02->addm2TH1(rg_as_deta_p2, "RG AS");
		c02->addm2TH1(rr_as_deta_trk_p2, "RR AS");
		c02->addm2TH1(rr2_as_deta_trk_p2, "RR2 AS");
		c02->setXrange(-2.5, 2.499);
		c02->draw();
		c02->drawLegend();
		c02->save(fig_output+"/check_AS_p2_trk_check.png");

		float x0 = 0, y0 = 3.14159;
		auto rr_as_dr_p2 = rr_sig_p2->drIntegral("incl_rr_as_dr_p2_*_*",   x0, y0, ndr_fine, dr_fine, "area"); 
		auto rr2_as_dr_p2 = rr2_sig_p2->drIntegral("incl_rr2_as_dr_p2_*_*",x0, y0, ndr_fine, dr_fine, "area"); 
		auto rg_as_dr_p2 = rg_sig_p2->drIntegral("incl_rg_as_dr_p2_*_*",   x0, y0, ndr_fine, dr_fine, "area"); 
		auto rr_as_dr_p2_trkcorr = (jtcTH1Player*) rr_as_dr_p2->divide(*trk_as_p0, "B");
		auto rr2_as_dr_p2_trkcorr = (jtcTH1Player*) rr2_as_dr_p2->divide(*trk_as_p0, "B");
		auto c2 = new plotManager();
		//c2->initSquarePad("dev_tracking_"+corr_name, "", npt,ncent);
		c2->initOverlayPad("dev_tracking_"+corr_name, "", npt,ncent);
		c2->addm2TH1(rg_as_dr_p2, "RG AS");
		//c2->addm2TH1(rr_as_dr_p2, "RR AS");
		//c2->addm2TH1(rr2_as_dr_p2, "RR2 AS");
		c2->addm2TH1(rr_as_dr_p2_trkcorr, "RR AS+trk");
		c2->addm2TH1(rr2_as_dr_p2_trkcorr, "RR2 AS+trk");
		c2->setXrange(0,0.99);
		//c2->setYrange(0.4,1);
		c2->draw();
		c2->drawLegend();
		c2->save(fig_output+"/check_AS_mixing_validation.png");

		auto rr_sig_dr_p2 = rr_sig_p2->drIntegral("incl_rr_sig_dr_p2_*_*",   ndr_fine, dr_fine, "area"); 
		auto rr2_sig_dr_p2 = rr2_sig_p2->drIntegral("incl_rr2_sig_dr_p2_*_*",ndr_fine, dr_fine, "area"); 
		auto rg_sig_dr_p2 = rg_sig_p2->drIntegral("incl_rg_as_sig_p2_*_*",   ndr_fine, dr_fine, "area"); 
		auto trk2 = (jtcTH1Player*) rr2_sig_dr_p2->divide(*rg_sig_dr_p2);
		auto c = new plotManager();
		c->initSquarePad("dev_tracking_"+corr_name, "", npt,ncent);
		c->addm2TH1(trkcorr_p0, "Raw");
		c->addm2TH1(trk_as_p0, "A.S.");
		c->addm2TH1(trk_as_p1, "A.S. ME");
		c->addm2TH1(trk_p1, "ME corrected");
		//c->addm2TH1(trk2, "New.");
		c->setXrange(0,0.99);
		c->setYrange(0.4,1);
		c->draw();
		c->drawLegend();
		c->save(fig_output+"/dev_tracking_"+corr_name+".png");

		auto ctrk = new plotManager();
		ctrk->initSquarePad("dev_tracking_"+corr_name, "", npt,ncent);
		ctrk->addm2TH1(trk_deta_p0, "Raw");
		ctrk->addm2TH1(trk_deta_p1, "ME");
		ctrk->setXrange(-2,1.99);
		ctrk->setYrange(0.4,1);
		ctrk->draw();
		ctrk->drawLegend();
		ctrk->save(fig_output+"/dev_tracking_as_deta.png");

//		auto trk_sig_deta_p0 = (jtcTH1Player*) rr_sig_deta_p0->divide(*rg_sig_deta_p0);
		auto trk_sig_deta_p1 = (jtcTH1Player*) rr_sig_deta_p1->divide(*rg_sig_deta_p1, "B");
		auto trk_sig_deta_p2 = (jtcTH1Player*) rr_sig_deta_p2->divide(*rg_sig_deta_p2, "B");
		auto ctrk2 = new plotManager();
		ctrk2->initSquarePad("dev_tracking_deta_sig", "", npt,ncent);
//		ctrk2->addm2TH1(trk_sig_deta_p0, "Raw");
		ctrk2->addm2TH1(trk_sig_deta_p1, "ME");
		ctrk2->addm2TH1(trk_sig_deta_p2, "ME+BKG");
		ctrk2->setXrange(-2,1.99);
		ctrk2->setYrange(0.4,1);
		ctrk2->draw();
		ctrk2->drawLegend();
		ctrk2->save(fig_output+"/dev_tracking_sig_deta.png");

		auto fEScheme = TFile::Open("/eos/user/w/wangx/AN20-029/bjtc_sb_1p6_2p2_fineBin_EScheme/bjtc_step2_output.root");
		auto js_sig_esc = new jtcTH1Player("correlations_djetMC_sube_c5shift/incl_sube0"+reco_tag(0,0)+"_sig_p2_*_*",npt, ncent);
		auto js_sig_wta = new jtcTH1Player("correlations_djetMC_sube_c5shift/incl_sube0"+reco_tag(0,0)+"_sig_p2_*_*",npt, ncent);
		js_sig_esc->autoLoad(fEScheme);
		js_sig_wta->autoLoad(fstep2);
		auto js_sig_dr_esc = (jtcTH1Player*) js_sig_esc->drIntegral("js_sig_dr_esc");
		auto js_sig_dr_wta = (jtcTH1Player*) js_sig_wta->drIntegral("js_sig_dr_wta");
		
		auto cref= new plotManager();
		cref->initOverlayPad("ref"+corr_name, "", npt,ncent);
		cref->addm2TH1(js_sig_dr_esc, "EScheme");
		cref->addm2TH1(js_sig_dr_wta, "WTA");
		cref->setXrange(0,0.99);
		cref->draw();
		cref->drawLegend();
		cref->save(fig_output+"/reference_WTA_VS_EScheme.png");
}

