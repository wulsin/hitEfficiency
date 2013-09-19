#ifndef testTrackAssociator_h
#define testTrackAssociator_h

#include <memory>


#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"  

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"  
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"  

#include "DataFormats/Common/interface/DetSetVectorNew.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/DetLayers/interface/NavigationSchool.h"


#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TStopwatch.h>

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;
using namespace edmNew;


class TrackAssociatorBase;

class testTrackAssociator : public edm::EDAnalyzer {
  
 public:
  testTrackAssociator(const edm::ParameterSet& conf);
  virtual ~testTrackAssociator();
  //  virtual void beginJob() {}  
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  int getLayerHit(unsigned int rawId);  
  int getSubDet(unsigned int rawId);  

 private:
  TStopwatch* timer;  
  TrackAssociatorBase * associatorByChi2;
  TrackAssociatorBase * associatorByHits;
  edm::InputTag tracksTag, tpTag, simtracksTag, simvtxTag;

  edm::ESHandle<Propagator> thePropagator;  
  edm::ESHandle<NavigationSchool> theSchool;  
  edm::ESHandle<TrackerGeometry> theG;
  edm::Handle<edmNew::DetSetVector<SiStripCluster> >  clusterStrip;
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> >  rphiRecHits;
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> >  rphiRecHitsUnmatched;
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> >  stereoRecHits;
  edm::Handle<edmNew::DetSetVector<SiStripRecHit2D> >  stereoRecHitsUnmatched;

  //  edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "rphiRecHit"      "HLT"     
    /* edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "rphiRecHitUnmatched"   "HLT"      */
    /* edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "stereoRecHit"    "HLT"      */
    /* edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "stereoRecHitUnmatched"   "HLT"      */
    


  virtual void beginJob() ;
  virtual void endJob() ;

  /* struct RecHitInfo { */
  /*   // info about a rec hit */
  /*   int subdet;  // subdetector, TIB=3, TID=4, TOB=5, TEC=6, to match definition in http://cmslxr.fnal.gov/lxr/source/DataFormats/SiStripDetId/interface/SiStripDetId.h */
  /*   int layer;  // or wheel, or disk */
  /*   double eta; */
  /*   double phi;   */
  /* }; */

  struct HitSubdetLay {
    int subdet;
    int layer;  
  };  


  struct RecHitInfo {
    // info about a rec hit
    uint detRawId;  
    double energyLoss;
    double distEdgeXNorm;
    double distEdgeYNorm;
    int subdet;  // subdetector, TIB=3, TID=4, TOB=5, TEC=6, to match definition in http://cmslxr.fnal.gov/lxr/source/DataFormats/SiStripDetId/interface/SiStripDetId.h
    int layer;  // or wheel, or disk
    double eta;
    double phi;  
    GlobalPoint globalPt;  

    RecHitInfo() {  // constructor
      detRawId = -99;  
      energyLoss = -99;
      distEdgeXNorm = -99;
      distEdgeYNorm = -99;
      subdet = -99;  
      layer = -99;
      eta = -99;
      phi = -99;
    }
    
  };


  struct TrackAndHits {
    double pt;
    double eta;
    double phi;
    double posYLocalNormFirstMissOut;  
    vector<RecHitInfo> rechits;
    //    vector<uint> hitDetIds;
  };  

  GlobalPoint getGlobalPos(PSimHit& simHit);  
  GlobalPoint getModuleCtr(PSimHit& simHit);  
  double getModuleAngle(PSimHit& simHit);  
  double getDist(GlobalPoint pt1, GlobalPoint pt2);  
  double getDistPerp(GlobalPoint pt1, GlobalPoint pt2);  


  bool isMissOutHitByMod(const vector<PSimHit>& simHits, uint ihit, const vector<unsigned int>& recHits);  
  bool isMissOutHit     (const vector<PSimHit>& simHits, uint idxhit, const reco::Track tr);
  bool isMissOutHitFirst(const vector<PSimHit>& simHits, uint idxhit, const reco::Track tr);
  bool isFoundSim       (const vector<PSimHit>& simHits, int subdet, int layer);  
  bool isMissSimPrevLay (const vector<PSimHit>& simHits, uint idxhit);  


  bool isMatchedToRecHitbyLayer(PSimHit& simHit, const vector<TrackAndHits>& trkRecHits);  
  bool isMatchedToRecHitbyLayer(PSimHit& simHit, const vector<RecHitInfo>& recHits);  
  bool isMatchedToRecHitbyLayer(PSimHit& simHit, const reco::Track tr, bool debug=false);  
  bool isMatchedToRecHitbyLayer(PSimHit& simHit, const RecHitInfo& recHit);  
  bool isMatchedToRecHitbyLayer(const PSimHit& simHit, trackingRecHit_iterator& recHit); 

  bool getIsGlued(unsigned int rawId);  

  bool isClusOnModule(unsigned int rawId);  
  bool isRecHitOnModule(unsigned int rawId);  
  int getCountSubdetLay(const vector<HitSubdetLay>& simhits, int subdet, int layer);  
  int getNExtraSimHits (const vector<HitSubdetLay>& simhits);  


  int sgn(double x) { if (x >= 0) return 1; else return -1;  }  

  //  RecHitInfo findClosestRecHit(PSimHit& simHit, const vector<RecHitInfo>& recHits);  
  int findClosestRecHit(PSimHit& simHit, const vector<RecHitInfo>& recHits);  

  TFile * file;
  TH1D* hNTrks; 
  TH1D* hNTrksAll;  
  TH1D* hTrkMissOutPt;  
  TH1D* hTrkMissOutEta;  
  TH1D* hTrkMissOutPhi;  
  TH1D* hNSimHit;
  TH1D* hNSimHitRec;
  TH1D* hNSimHitNRec;
  TH1D* hNSimHitMissOut;  
  TH1D* hNRecoMissOut;  
  TH1D* hNRecoMissMid;  
  TH1D* hNRecoSimDiffMissOut;  
  TH1D* hNSimHitMissOutFirst; 
  TH1D* hNSimHitMissOutFirstMissPrevSim;  
  TH1D* hNSimHitMissMid; 
  TH1D* hNExtraSimHits;  
  TH1D* hNPixHit; 
  TH1D* hPdgRecHits;  
  TH1D* hELossRec;
  TH1D* hELossNRecOut;
  TH1D* hPdgOuterHits;  
  TH1D* hDistYMissOut;  
  TH1D* hDistYMissOut_YPos;  
  TH1D* hDistYMissOut_YNeg;  
  TH1D* hDistXMissOut; 
  TH1D* hSubDetMid; 
  TH1D* hTrkNoOutEta;  
  TH1D* hNSimHitMinusRec;
  TH1D* hNSimHitMissMidByLayer;  
  TH1D* hELossRecMid;
  TH1D* hELossNRecMid;
  TH1D* hELossRecMidLayer;
  TH1D* hELossNRecMidLayer;

  TH2D* hLayerMid;  
  
  TH1D* hDistYRecMod;  
  TH1D* hDistYFirstMissOut;  
  TH1D* hDistXLayNotMod_PerpNear;
  TH1D* hDistYLayNotMod_PerpNear;
  TH2D* hDistXYLayNotMod_PerpNear;
  TH1D* hDistXLayNotMod_PerpFar;
  TH1D* hDistYLayNotMod_PerpFar; 
  TH2D* hDistXYLayNotMod_PerpFar; 
  TH2D* hDistXYLayNotMod_PerpNearTIB;
  TH2D* hDistXYLayNotMod_PerpFarTIB; 
  TH2D* hDistXYLayNotMod_PerpNearTOB;
  TH2D* hDistXYLayNotMod_PerpFarTOB; 
  TH2D* hDistXYLayNotMod_NoRecHitOnMod;  

  TH2D* hDistXYNotLay;  
  TH2D* hDistXYMissOut;  

  TH1D* hTrkEta;
  TH1D* hTrkPt;  
  TH1D* hTrkDistRecSimRecMod;
  TH1D* hTrkDistRecSimRecLay;
  TH1D* hTrkDistRecSimRecLayNotMod;
  TH1D* hTrkDistRecSimRecNoLay;
  TH1D* hTrkDistRecSimAll;
  TH1D* hTrkDPerpRecSimRecLayNotMod;  
  TH1D* hDistVtxMissOut;  

  TH1D* hTrkDPerpRecSimRecLayNotMod_PerpNear;
  TH1D* hTrkDPerpRecSimRecLayNotMod_PerpFar;
  TH1D* hTrkDPerpRecSimRecLayNotMod_IsGlued;
  TH1D* hTrkDPerpRecSimRecLayNotMod_NotGlued;  

  TH2D* hLayerRecLayNotMod_PerpNear;
  TH2D* hLayerRecLayNotMod_PerpFar;
  TH2D* hTestYDir;
 
  TH2D* hLayerRecMod;
  TH2D* hLayerRecLay;       
  TH2D* hLayerRecLayNotMod; 
  TH2D* hLayerRecLayNotMod_IsGlued; 
  TH2D* hLayerRecLayNotMod_NotGlued;  

  TH2D* hLayerRecNoLay;       
  TH2D* hLayerAll;       
  TH2D* hLayerAll_IsGlued;
  TH2D* hLayerAll_NotGlued; 
  TH2D* hLayerMissOut;  
  TH2D* hLayerMissOut_YPos;  
  TH2D* hLayerMissOut_YNeg;  

  TH2D* hNSimHitsVsLayerTIB;  
  TH2D* hNSimHitsVsLayerTOB;  

  TH2D* hPosSimVtx;  
  TH2D* hPosAll;  
  TH2D* hPosRecMod;  
  TH2D* hPosAllXY; 
  TH2D* hPosRecModXY; 
  TH2D* hPosMissOut;  

  TH1D* hELossRecMod;          
  TH1D* hELossRecRecLay;         
  TH1D* hELossRecRecLayNotMod;   
  TH1D* hELossRecRecNoLay;      
  TH1D* hELossRecAll;           

  TH1D* hNSimRecHitDiff;  
  TH1D* hNRecHit;

};

#endif
