//---------------------------------------------------------------
// File and Version Information:
// $Id: makePdfs.C,v 1.1 2012/03/19 14:23:10 wulsin Exp $
//
// Description:
//    Make pdf of all hists in a specified root file.
//
// Usage:
//     root -l -b -q 'makePdfs.C+' |& tee makePdfs.log
//
//     Or:
//     root [0] .x makePdfs.C  
//
//     Or:
//     root [0] .L makePdfs.C+  
//     root [1] makePdfs
//
//   
// Author List:
//      Wells Wulsin <wulsin.1@osu.edu>
//
// Copyright Information:
//      Copyright (C) 2012         OSU
//
// Revision History
//20120101  Created. 
//---------------------------------------------------------------
#include <iostream>  
using std::cout;
using std::endl;

#include "TCanvas.h" 
#include "TDirectory.h" 
#include "TFile.h" 
#include "TString.h" 
#include "TKey.h"
#include "TList.h"
#include "TH1.h" 
#include "TObject.h" 
#include "TROOT.h"  
#include "TMacro.h"
   


#ifndef __CINT__
#endif

// -------------------------
// -- Execution history   --
// -------------------------

// -------------------------
// -- Global variables    --
// -------------------------
TString _outdir;

// -------------------------
// -- Function prototypes --
// -------------------------
void makePdfs(TString fname,
	      TString outdir="");
void readdir(TDirectory *dir);  


// -------------------------
// -- Functions           --
// -------------------------
void makePdfs(TString fname,
	      TString outdir) {
  cout << "Beginning makePdfs." << endl;

  _outdir = outdir;  
  if (_outdir!="" &&
      !_outdir.EndsWith("/")) _outdir += "/";  

  // copy from http://root.cern.ch/root/html/tutorials/io/loopdir.C.html

  // TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
  //  dir.ReplaceAll("loopdir.C","../hsimple.C");
  //  dir.ReplaceAll("/./","/");
  //  if (!gInterpreter->IsLoaded(dir.Data())) gInterpreter->LoadMacro(dir.Data());
  TFile* f1 = new TFile(fname, "READ");

  readdir(f1);  

  // f1->cd("TrackAssoc");  
  // //   TFile *f1 = (TFile*)gROOT->ProcessLineFast("hsimple(1)");
  // TIter next(f1->GetListOfKeys());
  // TKey *key;
  // TCanvas c1;
  // c1.Print("allPlots.ps[");
  // while ((key = (TKey*)next())) {
  //   cout << "Found key " << key->GetName() << endl;  
  //   if (key->IsFolder()) {
  //     f1->cd(key->GetName()); 
  //   }
  //   TClass *cl = gROOT->GetClass(key->GetClassName());
  //   if (!cl->InheritsFrom("TH1")) continue;
  //   TH1 *h = (TH1*) key->ReadObj();
  //   cout << "Found hist " << h->GetName() << endl;  
  //   h->Draw();
  //   c1.Print("allPlots.ps");
  // }
  // c1.Print("allPlots.ps]");
  


  cout << "Finished makePdfs; wrote files to directory: " << outdir << endl;


  return;

} 



void readdir(TDirectory *dir) {
   TDirectory *dirsav = gDirectory;
   TIter next(dir->GetListOfKeys());
   TKey *key;
   while ((key = (TKey*)next())) {
      if (key->IsFolder()) {
         dir->cd(key->GetName());
         TDirectory *subdir = gDirectory;
         readdir(subdir);
         dirsav->cd();
         continue;
      }
      cout << "Found key " << key->GetName() << endl;  
      TClass *cl = gROOT->GetClass(key->GetClassName());
      if (!cl->InheritsFrom("TH1")) continue;
      TCanvas c1;
      TH1 *h = (TH1*) key->ReadObj();
      cout << "Found hist " << h->GetName() << endl;  
      TString plotName = _outdir;  
      plotName += h->GetName();  
      plotName += ".pdf";  
      plotName.ReplaceAll(" ", "");  
      h->Draw();
      c1.SaveAs(plotName);  
      if (cl->InheritsFrom("TH2")) {
	h->Draw("colz");
	plotName.ReplaceAll(".pdf", "_colz.pdf");  
	c1.SaveAs(plotName);  
      }
   }
}
  
