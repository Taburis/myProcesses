
import py_fig_collector

a = py_fig_collector.copyer()


eos = '/eos/user/w/wangx/www/AN20-029/bjtc_c2bin_50mix_nomianl'
f = '.pdf'
#a.add(eos,'Results/figure_js_ratio_b2Inclusive'+f, 'Results/figure_js_ratio_b2Inclusive'+f)

f = '.png'
a.add(eos,'production/fullClosure_pTsummed'+f, 'production/closureTest_fullTest'+f) 
a.add(eos,'production/closure_pTcombined_decont_step'+f, 'production/closureTest_decontamination'+f) 
a.add(eos,'production/closure_pTcombined_trk_step'+f, 'production/closureTest_tracking'+f) 
a.add(eos,'production/closure_pTcombined_bias_step'+f, 'production/closureTest_bias'+f) 

a.add(eos,'corrections/recoJet_tagBias'+f, 'corrections/recoJet_tagBias'+f) 
a.add(eos,'corrections/SpillOver_trueB_spillCorr'+f, 'corrections/SpillOverCorrection_trueB'+f) 
a.add(eos,'corrections/SpillOver_incl_spillCorr'+f, 'corrections/SpillOverCorrection_incl'+f) 
a.add(eos,'corrections/JFF_incl_sube0_JffCorr'+f, 'corrections/JFFCorrection_incl'+f) 
a.add(eos,'corrections/JFF_trueB_sube0_JffCorr'+f, 'corrections/JFFCorrection_trueB'+f) 

#signal check part
a.add(eos,'correlations_HIHardProbe_trigMerge80and100/canvas_sbCheck_incl_RecoJet_RecoTrk.jpg', 'correlation/canvas_sbCheck_incl_RecoJet_RecoTrk.jpg') 
a.add(eos,'correlations_HIHardProbe_trigMerge80and100/canvas_sbCheck_tagged_RecoJet_RecoTrk.jpg', 'correlation/canvas_sbCheck_tagged_RecoJet_RecoTrk.jpg') 
a.add(eos,'correlations_HIHardProbe_trigMerge80and100/canvas_sigCheck_incl_RecoJet_RecoTrk.jpg', 'correlation/canvas_sigCheck_incl_RecoJet_RecoTrk.jpg') 
a.add(eos,'correlations_HIHardProbe_trigMerge80and100/canvas_sigCheck_tagged_RecoJet_RecoTrk.jpg', 'correlation/canvas_sigCheck_tagged_RecoJet_RecoTrk.jpg') 
a.add(eos,'correlations_HIHardProbe_trigMerge80and100/proc_sbcorr_incl_RecoJet_RecoTrk.jpg', 'correlation/proc_sbcorr_incl_RecoJet_RecoTrk.jpg') 
a.add(eos,'correlations_HIHardProbe_trigMerge80and100/proc_sbcorr_tagged_RecoJet_RecoTrk.jpg', 'correlation/proc_sbcorr_tagged_RecoJet_RecoTrk.jpg') 

#tracking part
f='.png'
a.add(eos,'corrections/tracking_incl_trkEff'+f, 'corrections/trackingEff_inclusive'+f) 
a.add(eos,'corrections/tracking_tagged_trkEff'+f, 'corrections/trackingEff_tagged'+f) 
a.add(eos,'corrections/correction_contBias'+f, 'corrections/contBias'+f) 

# tagging QAs
eos = '/eos/user/w/wangx/www/btagger/AA2018bTagger_MCptWeighted_QAs'
f='.png'
a.add(eos,'jtpt'+f,'jetreco/jtpt'+f)
a.add(eos,'jteta'+f,'jetreco/jteta'+f)
a.add(eos,'jtphi'+f,'jetreco/jtphi'+f)
a.add(eos,'QAsvtxdls'+f,'jetreco/QAsvtxdls'+f)
a.add(eos,'QAsvtmass'+f,'jetreco/QAsvtmass'+f)
a.add(eos,'QAtrk3dIP'+f,'jetreco/QAtrk3dIP'+f)
a.add(eos,'QAtrk3dIPSig'+f,'jetreco/QAtrk3dIPSig'+f)
a.add(eos,'QAtrkPtRel'+f,'jetreco/QAtrkPtRel'+f)

eos = '/eos/user/w/wangx/www/AN20-029/bTaggingQAs'
f='.png'
a.add(eos,'bJetType_Pt_weighted'+f,'syst/bJetType_Pt'+f)
a.add(eos,'bJetType_Pt_GSPweighted'+f,'syst/bJetType_Pt_GSPweighted'+f)
a.add(eos,'scaleFactor.png','jetreco/scaleFactor.png')

#event reweight
path = '/eos/user/w/wangx/www/AN20-029/evtQAPlot/'
a.add(path,'evtQA_vz_cent'+f,'mc/eventReweight'+f)

#JER 
eos = '/eos/user/w/wangx/www/AN20-029/qaScan/'
f='.png'
a.add(eos,'JER'+f,'syst/JER'+f)
eos = '/eos/user/w/wangx/www/AN20-029/jetQAPlot'
a.add(eos,'/jetQA_JEC.png','/jetreco/JEC.png')
a.add(eos,'/jetQA_JER.png','/jetreco/JER.png')

#syst 
eos = '/eos/user/w/wangx/www/AN20-029/bjtc_c2bin_50mix_nomianl'
a.add(eos,'production/systUncert_bkg_ME.png', 'syst/systUncert_bkg_ME.png') 
a.add(eos,'production/systUncert_taggingBias.png', 'syst/systUncert_taggingBias.png') 


#tigger bias
eos = '/eos/user/w/wangx/www/AN20-029/trigger/jetQAPlot'
a.add(eos,'jetQA_jetKinematics.png', 'jetreco/jetSpectra_jet60vsjet80and100.png') 
eos = '/eos/user/w/wangx/www/AN20-029/bjtc_c2bin_50mix_systematic/systematic'
a.add(eos,'systUncert_incl_trigger.png', 'syst/systUncert_incl_trigger.png') 
a.add(eos,'systUncert_tagged_trigger.png', 'syst/systUncert_tagged_trigger.png') 
a.add(eos,'/systUncert_JEC_tagged.png', 'syst/systUncert_tagged_JEC.png') 
a.add(eos,'/systUncert_JEC_incl.png', 'syst/systUncert_incl_JEC.png') 

eos = '/eos/user/w/wangx/www/AN20-029/bjtc_c2bin_50mix_nomianl/production'
a.add(eos,'systUncert_trigger_incl.png'  , 'syst/systUncert_incl_triggerJet80.png') 
a.add(eos,'systUncert_trigger_tagged.png', 'syst/systUncert_tagged_triggerJet80.png') 

#result
eos = '/eos/user/w/wangx/www/AN20-029/bjtc_c2bin_50mix_nomianl'
a.add(eos,'Results/figure_nominal_js_overlay.pdf', 'Results/Figure01.pdf')
a.add(eos,'Results/figure_nominal_js_ratio_overlay.pdf', 'Results/Figure02.pdf')
a.add(eos,'Results/figure_nominal_js_ratio_b2Inclusive.pdf', 'Results/Figure03.pdf')

a.copy()
a.tarball()
