
#ifndef OVERLAY_PAD_H
#define OVERLAY_PAD_H
#include "fast_pad.h"

class overlay_pad : public base_pad{
		public : overlay_pad(){}
				 overlay_pad(TString name,float r = 3.25){
						 uppad  .SetPad(name+"_up", "", 0.0, r, 1, 1);
						 downpad.SetPad(name+"_down", "", 0.0, 0.0, 1, r);
						 uppad  .SetTopMargin(0.05);
						 uppad  .SetLeftMargin(0.18);
						 uppad  .SetRightMargin(0.0001);
						 uppad  .SetTopMargin(0.03);
						 uppad  .SetBottomMargin(0);
						 downpad.SetLeftMargin(0.18);
						 downpad.SetTopMargin(0);
						 downpad.SetRightMargin(0.0001);
						 downpad.SetBottomMargin(0.5);
			//			 uppad  .Draw();
			//			 downpad.Draw();
				 }

				 TPad uppad, downpad;
};

#endif
