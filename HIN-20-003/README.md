# HIN-20-003 analysis code

This branch documents the codes after the approval section. The nominal production code is `arc/produce.C`. The production steps are:
* step1: Running the correlation code in the `step1` folder to produce necessary correlation and mixing tables;
* step2: Extract the correlation signal by using ME and BKG methods (calculates the corresponds uncertainties);
* step3: Extract the corrections from MC samples produced from step2;
* step4: Apply the corrections from step3 to the data correlation signal from step2, integrates the 2D table into functions of dr;
* step5: Normalize the 1D tables into jet shapes and draw the analysis result figures (the systematics are evaluated separately).

The step1 codes are in the folder `step1` and the codes of step2 to step5 are in `arc`. The nominal data are stored at lxplus:
* correlations: `/eos/user/w/wangx/AN20-029/bjtc_arc`
* The corrections: `correction/bjtc_step3_output.root`
* step4 output: `arc/bjtc_step4_sm.root`
* step5 output: `arc/bjtc_step5_sm.root`

