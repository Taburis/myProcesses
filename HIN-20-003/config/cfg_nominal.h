
#include "myProcesses/liteFrame/plugin/toolkit.h"
#include "myProcesses/liteFrame/plugin/liteFrame.h"
#include "myProcesses/jtc/plugin/jtcLib.h"

#if defined(event_content_skim)
#include "myProcesses/hiforest/plugin/eventMap_skim.h"
#elif defined(event_content_forest)
#include "myProcesses/hiforest/plugin/eventMap_hiForest.h"
#endif
#include "myProcesses/HIN-20-003/JEC2018PbPb/JECUncert.h"
//residual JES correction on top of 2018 official JES correction for counting the phi dependence
#include "myProcesses/HIN-20-003/residualJEC2018/JetCorrector.h"

#include "TF1.h"

#ifndef TYPEDEFINE
#define TYPEDEFINE
enum jetType {inclJet, trueBJet, taggedJet, negTagJet, cJet, lightJet, contJet, gspJet};
enum trkType {inclTrk, sube0,suben0};
#endif

namespace config_AN20029{


	class pset_nominalHI_skim{
		public :
			pset_nominalHI_skim():
				centbin {0,20,60, 180},
				ptbin{1, 2, 3,4,8, 12, 300}
			{
				trigger = new std::string[3];
				trigger[0] = "HLT_HIPuAK4CaloJet60Eta5p1_v1";
				trigger[1] = "HLT_HIPuAK4CaloJet80Eta5p1_v1";
				trigger[2] = "HLT_HIPuAK4CaloJet100Eta5p1_v1";
				std::stringstream s1, s2;
				for(int i=0; i<nptbin; i++){
					s1.str(std::string());
					s2.str(std::string());
					s1<<ptbin[i];
					s2<<ptbin[i+1];
					ptLabel[i] =TString("Pt:"+s1.str()+"-"+s2.str());
				}
				centLabel = toolkit::makeCentLabels(ncent, centbin);

			}
			~pset_nominalHI_skim(){}
			TString *centLabel, ptLabel[6];
			const int ncent = 3, nptbin = 6;
			const float centbin[4], ptbin[7];
			TString jetSetName="akFlowPuCs4PFJetAnalyzer";
			bool isMC = 0, isHI=1;
			std::string *evtFilterString=0;
			std::string *trigger=0;
			int ntrigs= 3;
			int nfilters=0;
	};

	class pset_nominalHI_forest{
		public :
			pset_nominalHI_forest():
				centbin {0,20,60, 180},
				ptbin{1, 2, 3,4,8, 12, 300}
			{
				std::stringstream s1, s2;
				for(int i=0; i<nptbin; i++){
					s1.str(std::string());
					s2.str(std::string());
					s1<<ptbin[i];
					s2<<ptbin[i+1];
					ptLabel[i] =TString("Pt:"+s1.str()+"-"+s2.str());
				}
				centLabel = toolkit::makeCentLabels(ncent, centbin);

			}
			~pset_nominalHI_forest(){}
			TString *centLabel, ptLabel[6];
			const int ncent = 3, nptbin = 6;
			const float centbin[4], ptbin[7];
			TString jetSetName="akFlowPuCs4PFJetAnalyzer";
			bool isMC = 0, isHI=1;
			std::string *evtFilterString=0;
			int nfilters=0;
	};


	class selections {	
		public : 
			selections(){
				//JEU.Initialize("myProcesses/HIN-20-003/JEC2018PbPb/Autumn18_HI_V6_DATA_Uncertainty_AK4PF.txt");
			}
			~selections(){}
			void doJECShiftUp(){doJEUUp = 1; doJEUDown = 0;}
			void doJECShiftDown(){doJEUUp = 0; doJEUDown = 1;}
			float jec_shifted_pt(float pt, float eta, float phi){
				JEU.SetJetPT(pt);
				JEU.SetJetEta(eta);
				JEU.SetJetPhi(phi);
				//cout<<JEU.GetUncertainty().first<<" : "<<JEU.GetUncertainty().second<<endl;
				if(doJEUDown) return pt*(1-JEU.GetUncertainty().first);
				else return pt*(1+JEU.GetUncertainty().second);
			}
			xTagger tagRecoJet(eventMap *em, int i){
				xTagger tag;
				//kinematic selection:
				float jetpt = em->jetpt[i];
				if(jeutool.smearSigma >0 ) jetpt = jeutool.smearedPt(jetpt);
				if(useNewJES ){
						double eta = em->jeteta[i];
						double average_rho = em->getUE(eta, 0.4);
						jescorr.SetJetPT(em->jetpt[i]);
						jescorr.SetJetEta(eta);
						jescorr.SetJetPhi(em->jetphi[i]);
						jescorr.SetRho(average_rho);
						jetpt = jescorr.GetCorrectedPT();
				}
				else if(doJEUUp || doJEUDown) 
					jetpt = jec_shifted_pt(em->jetpt[i], 
							em->jeteta[i],
							em->jetphi[i]);
				if( jetpt < 120) return tag;
				if(TMath::Abs(em->jeteta[i]) > 1.6) return tag;

				//other selections
				tag.addTag(jetType::inclJet);
				if(em->ndisc_csvV2[i] > 0.9){
					tag.addTag(jetType::negTagJet);
				}
				if(em->disc_csvV2[i] > 0.9) tag.addTag(jetType::taggedJet);
				if(em->isMC){ 
					if(TMath::Abs(em->matchedHadronFlavor[i]) == 5){
						tag.addTag(jetType::trueBJet);
						if(em->bHadronNumber[i] ==2) 
							tag.addTag(jetType::gspJet);
					}else if(TMath::Abs(em->matchedHadronFlavor[i]) == 4)
						tag.addTag(jetType::cJet);
					else tag.addTag(jetType::lightJet);
					if(TMath::Abs(em->matchedHadronFlavor[i]) != 5 && em->disc_csvV2[i] > 0.9) tag.addTag(jetType::contJet);
				}
				return tag;
			}

			xTagger tagGenJet(eventMap *em, int i){
				xTagger tag;
				if( em->genjetpt[i] < 120 ) return tag;
				if( TMath::Abs(em->genjet_wta_eta[i]) > 1.6 ) return tag;

				//add incl jet tag
				tag.addTag(jetType::inclJet);

				//add true b jet tag
				int index = -1;
				float drmin=1;
				for(int j=0; j<em->nJet(); ++j){
					float dr = toolkit::findDr(em->genjeteta[i],em->genjetphi[i],em->jeteta[j],em->jetphi[j]);
					if(dr > 0.4) continue;
					if(dr < drmin) {
						drmin = dr; index = j;
					}
				}
				//int index = em->genMatchIndex[i];
				if(index > -1){
					if(em->disc_csvV2[index] > 0.9)
						tag.addTag(jetType::taggedJet);
					if(em->ndisc_csvV2[index] > 0.9){
						tag.addTag(jetType::negTagJet);
					}
					if(TMath::Abs(em->matchedHadronFlavor[index]) == 5){
						tag.addTag(jetType::trueBJet);
						if(em->bHadronNumber[i] ==2) 
							tag.addTag(jetType::gspJet);
					}else if(TMath::Abs(em->matchedHadronFlavor[index]) == 4)
						tag.addTag(jetType::cJet);
					else tag.addTag(jetType::lightJet);
					if(TMath::Abs(em->matchedHadronFlavor[index]) != 5 && em->disc_csvV2[index] > 0.9) tag.addTag(jetType::contJet);
				}
				return tag;
			}

			xTagger tagTrk(eventMap *em, int j){
				xTagger tag;
				if(em->trkpt[j] < 1 || em->trkpt[j]>400) return tag;
				if(TMath::Abs(em->trketa[j]) >= 2.4) return tag;
				if(!(em->highPurity[j])) return tag;
				if(em->trknhit[j]< 11) return tag;
				if(em->trkchi2[j]/em->trkndof[j]/em->trknlayer[j] > 0.15) return tag;
				float et = (em->pfEcal[j] + em->pfHcal[j])/TMath::CosH(em->trketa[j]);
				if( !(em->trkpt[j]<20.0 || et>0.5*(em->trkpt[j]))) return tag;

				tag.addTag(trkType::inclTrk);
				//cout<<tag.tag<<endl;
				return tag;
			}

			xTagger tagGenParticle(eventMap *em, int j){
				xTagger tag;

				if(em->gppt(j) < 1.0) return tag;
				if(TMath::Abs(em->gpeta(j)) > 2.4) return tag;
				if(em->gpchg(j)==0) return tag;

				tag.addTag(trkType::inclTrk);
				if(em->gpSube(j) ==0 ) tag.addTag(trkType::sube0);
				else tag.addTag(trkType::suben0);
				return tag;
			}

			bool evtCut(eventMap *em){
				if(em->checkEventFilter()) return 1;
				if(TMath::Abs(em->vz) > 13) return 1;
				if(em->hiBin > 180) return 1;
				if(em->isMC){ if( em->pthat < 50) return 1;
				}else if(!(em->trigFlag[1]) &&!(em->trigFlag[2])){
					return 1;
				}
				return 0;
			}

			jtc::JEUncertTool jeutool;
			JetUncertainty JEU;
			bool doJEUUp = 0, doJEUDown = 0, useNewJES=0;
			JetCorrector jescorr;
	};

	class weight_Hydjet_nominal{
		public :

			weight_Hydjet_nominal(){
				fvzw1  = new TF1("fvzw1", "pol5", -15, 15);
				fvzw2  = new TF1("fvzw2", "pol5", -15, 15);
				fcentw1= new TF1("fcentw1", "pol5",0, 180);
				fcentw2= new TF1("fcentw2", "pol5",0, 180);
				fvzw1->SetParameters(1.00719,-0.0192448,0.000788677,-2.52981e-05,-5.48684e-06, 1.75415e-07);
				fvzw2->SetParameters(0.995013, 0.00130315, 0.00034985 , -5.26669e-05, -2.77803e-06, 2.98957e-07);
				fcentw1->SetParameters(5.06806, -0.131039, 0.00162609, -1.17974e-05, 4.64397e-08, -7.46879e-11);
				fcentw2->SetParameters(0.862714, -0.00162917, 0.000359418, -5.64674e-06, 3.32662e-08, -6.70605e-11);
			}
			~weight_Hydjet_nominal(){}
			//float evtWeight(eventMap* e){return 1;}
			float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin))*(fcentw2->Eval(e->hiBin));}
			//float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin-10));}
			float trkWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float recoJetWeight(eventMap* e, int i, xTagger &tag){
				return 1;
			}
			float genJetWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float genParticleWeight(eventMap* e, int i, xTagger &tag){return 1;}

			TF1 * fvzw1, *fvzw2;
			TF1 * fcentw1, *fcentw2;
	};
	class weight_data_nominal{
		public :

			weight_data_nominal(){
			}
			~weight_data_nominal(){}
			float trkWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float recoJetWeight(eventMap* e, int i, xTagger &tag){
				return 1;
			}
			float genJetWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float genParticleWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float evtWeight(eventMap* e){
				if(mergeTrig) return triggerMergeWeight_jet80and100(e);
				else return 1;
			}
			float triggerMergeWeight_jet80and100(eventMap *e){
				if (e->trigFlag[2] ==1 ) return 1;
				else if (e->trigFlag[1] == 1) return 2.56;
				else return 0;
			}
			bool mergeTrig = 0;
	};
	class weight_Hydjet_gspWeighted{
		public :

			weight_Hydjet_gspWeighted(){
				fvzw1  = new TF1("fvzw1", "pol5", -15, 15);
				fvzw2  = new TF1("fvzw2", "pol5", -15, 15);
				fcentw1= new TF1("fcentw1", "pol5",0, 180);
				fcentw2= new TF1("fcentw2", "pol5",0, 180);
				fvzw1->SetParameters(1.00719,-0.0192448,0.000788677,-2.52981e-05,-5.48684e-06, 1.75415e-07);
				fvzw2->SetParameters(0.995013, 0.00130315, 0.00034985 , -5.26669e-05, -2.77803e-06, 2.98957e-07);
				fcentw1->SetParameters(5.06806, -0.131039, 0.00162609, -1.17974e-05, 4.64397e-08, -7.46879e-11);
				fcentw2->SetParameters(0.862714, -0.00162917, 0.000359418, -5.64674e-06, 3.32662e-08, -6.70605e-11);
			}
			~weight_Hydjet_gspWeighted(){}
			//float evtWeight(eventMap* e){return 1;}
			float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin))*(fcentw2->Eval(e->hiBin));}
			//float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin-10));}
			float trkWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float recoJetWeight(eventMap* e, int i, xTagger &tag){
				if(tag.select(1<<jetType::gspJet)) return 2;
				return 1;
			}
			float genJetWeight(eventMap* e, int i, xTagger &tag){
				if(tag.select(1<<jetType::gspJet)) return 2;
				return 1;
			}
			float genParticleWeight(eventMap* e, int i, xTagger &tag){return 1;}

			TF1 * fvzw1, *fvzw2;
			TF1 * fcentw1, *fcentw2;
	};

	class weight_Hydjet_cJetReweight{
		public :

			weight_Hydjet_cJetReweight(){
				fvzw1  = new TF1("fvzw1", "pol5", -15, 15);
				fvzw2  = new TF1("fvzw2", "pol5", -15, 15);
				fcentw1= new TF1("fcentw1", "pol5",0, 180);
				fcentw2= new TF1("fcentw2", "pol5",0, 180);
				fvzw1->SetParameters(1.00719,-0.0192448,0.000788677,-2.52981e-05,-5.48684e-06, 1.75415e-07);
				fvzw2->SetParameters(0.995013, 0.00130315, 0.00034985 , -5.26669e-05, -2.77803e-06, 2.98957e-07);
				fcentw1->SetParameters(5.06806, -0.131039, 0.00162609, -1.17974e-05, 4.64397e-08, -7.46879e-11);
				fcentw2->SetParameters(0.862714, -0.00162917, 0.000359418, -5.64674e-06, 3.32662e-08, -6.70605e-11);
			}
			~weight_Hydjet_cJetReweight(){}
			//float evtWeight(eventMap* e){return 1;}
			float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin))*(fcentw2->Eval(e->hiBin));}
			//float evtWeight(eventMap* e){return (e->weight)*(fvzw1->Eval(e->vz))*(fcentw1->Eval(e->hiBin-10));}
			float trkWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float recoJetWeight(eventMap* e, int i, xTagger &tag){
				if(tag.select(1<<jetType::cJet)) return 2;
				return 1;
			}
			float genJetWeight(eventMap* e, int i, xTagger &tag){return 1;}
			float genParticleWeight(eventMap* e, int i, xTagger &tag){return 1;}

			TF1 * fvzw1, *fvzw2;
			TF1 * fcentw1, *fcentw2;
	};
}
