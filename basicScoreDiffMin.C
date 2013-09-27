// Copied from $ROOTSYS/tutorials/tree/basic.C 
// Followed instructions from:
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/DisappearingTracksSearch2012Recipes#Make_plots_of_variables_of_inter
//
// Usage:
// /afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/SimTracker/TrackAssociation/test > grep -B 300 "isFirstMissOutNotSim=1" testTrkReco_cfg-bkup20130924.log | grep scoreDiffMin | awk '{print $4}' > scoreDiffMin.dat 


#include "Riostream.h"
void basicScoreDiffMin() {
  //  Read data from an ascii file and create a root file with an histogram and an ntuple.
  //   see a variant of this macro in basic2.C
  //Author: Rene Brun
      

  ifstream in;
  in.open("scoreDiffMin.dat");  
  Float_t x;
  Int_t nlines = 0;
  TH1F *hn = new TH1F("hn",";min score difference;tracks",100,0, 50);
  while (1) {
    in >> x;
    if (!in.good()) break;
    if (nlines < 5) printf("x=%8f\n",x);
    hn->Fill(x);
    nlines++;
  }
  printf(" found %d points\n",nlines);
  in.close();
  TCanvas* c1 = new TCanvas();
  hn->Draw();  

  in.close();

  //  f->Write();
}
