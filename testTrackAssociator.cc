#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "SimTracker/TrackAssociation/test/testTrackAssociator.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/SiStripDetId/interface/TIBDetId.h"  
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"  
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"  
#include "DataFormats/SiStripDetId/interface/TECDetId.h"  


#include "DataFormats/Math/interface/deltaR.h"

// Include from histoanalyzer:  
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
// End Include from histoanalyzer:  

// Copied from TrackProducer.cc:  
#include "RecoTracker/TrackProducer/plugins/TrackProducer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include <TrackingTools/MeasurementDet/interface/MeasurementDet.h>

#include "RecoTracker/TrackProducer/interface/KfTrackProducerBase.h"
#include "RecoTracker/TrackProducer/interface/TrackProducerAlgorithm.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
// End Copied from TrackProducer.cc:  

#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h" 
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"  

#include <memory>
#include <iostream>
#include <string>

//class TrackAssociator; 
class TrackAssociatorByHits; 
class TrackerHitAssociator; 

using namespace reco;
using namespace std;
using namespace edm;
using namespace edmNew;

testTrackAssociator::testTrackAssociator(edm::ParameterSet const& conf) {
  tracksTag = conf.getParameter< edm::InputTag >("tracksTag");
  tpTag = conf.getParameter< edm::InputTag >("tpTag");
  simtracksTag = conf.getParameter< edm::InputTag >("simtracksTag");
  simvtxTag = conf.getParameter< edm::InputTag >("simvtxTag");

  timer = new TStopwatch();  
  timer->Start();  

  //now do what ever initialization is needed
  edm::Service<TFileService> fs;

  hNTrksAll    = fs->make<TH1D>("hNTrksAll",";Num. trks;events",11, -0.5, 10.5);  
  // hNTrks       = fs->make<TH1D>("numTrks",";Num. trks",11, -0.5, 10.5);  
  //  hNSimHit     = fs->make<TH1D>("numSimHits",";Num. sim hits",30,0,30);  
  // hNSimHitMinusRec = fs->make<TH1D>("numSimHitMinusrec",";(Num. sim hits) - (Num. rec hits)",30,0,10);  
  hNSimHitRec  = fs->make<TH1D>("numSimHitsRec", ";Num. sim hits matched to rechit;tracks",    30,0,30);  
  // hNSimHitNRec = fs->make<TH1D>("numSimHitsNRec",";Num. sim hits not matched to rechit",30,0,30);  
  hNSimHitMissOut = fs->make<TH1D>("numSimHitMissOut",";Num. sim hits that give missing outer hits;tracks",11,-0.5,10.5);  
  hNRecoMissOut   = fs->make<TH1D>("hNRecoMissOut",";Num. missing outer hits from reco;tracks",11,-0.5,10.5);  
  hNRecoMissMid   = fs->make<TH1D>("hNRecoMissMid",";Num. missing middle hits from reco;tracks",11,-0.5,10.5);  
  hNRecoSimDiffMissOut   = fs->make<TH1D>("hNRecoSimDiffMissOut",";Num. missing outer hits from reco - Num from sim counting;tracks",11,-5.5,5.5);  
  hNSimHitMissOutFirst = fs->make<TH1D>("numSimHitMissOutFirst",";Num. sim hits that give the first missing outer hit;tracks",6,-0.5,5.5);  
  hNSimHitMissOutFirstMissPrevSim = fs->make<TH1D>("numSimHitMissOutFirstMissPrevSim",";Num. sim hits that give the first missing outer hit and miss the prev. sim hit;tracks",6,-0.5,5.5);  
  hNSimHitMissMid = fs->make<TH1D>("numSimHitMissMid",";Num. sim hits that give missing middle hits;tracks",30,0,15);  
  hNExtraSimHits = fs->make<TH1D>("hNExtraSimHits",";Num. extra sim hits by layer;tracks",15,0,15);  
  hNSimHitMissMidByLayer = fs->make<TH1D>("numSimHitMissMidByLayer",";Num. sim hits that give missing middle hits, matched by layer;tracks",30,0,30);  
  // hSubDetMid    = fs->make<TH1D>("hSubDetMid",";subdetector of sim hits that give missing middle hits",30,0,30);  
  hNPixHit = fs->make<TH1D>("hNPixHit",";Num. pixel rec hits;tracks",30,0,30);  


  //  hELossRec   = fs->make<TH1D>("hELossRec",";#DeltaE (sim hits with rec hit)",100,0,1e-3);  
  // hELossRecMid   = fs->make<TH1D>("hELossRecMid",";#DeltaE (sim hits that have middle rec hits, matched by module)",100,0,1e-3);  
  // hELossNRecMid  = fs->make<TH1D>("hELossNRecMid",";#DeltaE (sim hits that give missing middle hit, matched by module)",100,0,1e-3);  
  // hELossRecMidLayer   = fs->make<TH1D>("hELossRecMidLayer",";#DeltaE (sim hits that have middle rec hits, matched by layer)",100,0,1e-3);  
  // hELossNRecMidLayer  = fs->make<TH1D>("hELossNRecMidLayer",";#DeltaE (sim hits that give missing middle hit, matched by layer)",100,0,1e-3);  
  hPdgOuterHits = fs->make<TH1D>("hPdgOuterHits",";PDG ID of sim hit that gives missing outer hit;hits",100,-50, 50);  
  // hPdgRecHits = fs->make<TH1D>("hPdgRecHits",";PDG ID of sim hit with associated rec hit",100,-50, 50);  
  hDistXMissOut = fs->make<TH1D>("hDistXMissOut",";x distance to sensor edge (cm);hits",100, -1.5, 1.5);  
  hDistYMissOut = fs->make<TH1D>("hDistYMissOut",";y distance to sensor edge (cm);hits",100, -1.5, 1.5);  
  hDistYMissOut_YPos = fs->make<TH1D>("hDistYMissOut_YPos",";y distance to sensor edge (cm);hits",100, -1.5, 1.5);  
  hDistYMissOut_YNeg = fs->make<TH1D>("hDistYMissOut_YNeg",";y distance to sensor edge (cm);hits",100, -1.5, 1.5);  

  hDistXLayNotMod_PerpNear = fs->make<TH1D>("hDistXLayNotMod_PerpNear",";x local pos. (norm)",100, -1.5, 1.5);  
  hDistYLayNotMod_PerpNear = fs->make<TH1D>("hDistYLayNotMod_PerpNear",";y local pos. (norm)",100, -1.5, 1.5);  
  hDistXLayNotMod_PerpFar  = fs->make<TH1D>("hDistXLayNotMod_PerpFar",";x local pos. (norm)",100, -1.5, 1.5);  
  hDistYLayNotMod_PerpFar  = fs->make<TH1D>("hDistYLayNotMod_PerpFar",";y local pos. (norm)",100, -1.5, 1.5);  
  hDistXYLayNotMod_PerpNear = fs->make<TH2D>("hDistXYLayNotMod_PerpNear",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYLayNotMod_PerpFar  = fs->make<TH2D>("hDistXYLayNotMod_PerpFar",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYLayNotMod_PerpNearTIB = fs->make<TH2D>("hDistXYLayNotMod_PerpNearTIB",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYLayNotMod_PerpFarTIB  = fs->make<TH2D>("hDistXYLayNotMod_PerpFarTIB",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYLayNotMod_PerpNearTOB = fs->make<TH2D>("hDistXYLayNotMod_PerpNearTOB",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYLayNotMod_PerpFarTOB  = fs->make<TH2D>("hDistXYLayNotMod_PerpFarTOB",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYNotLay            = fs->make<TH2D>("hDistXYNotLay",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYMissOut          = fs->make<TH2D>("hDistXYMissOut",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  
  hDistXYLayNotMod_NoRecHitOnMod   = fs->make<TH2D>("hDistXYLayNotMod_NoRecHitOnMod",";x local pos. (norm);y local pos. (norm)",100, -1.5, 1.5, 100, -1.5, 1.5);  



  hPosSimVtx          = fs->make<TH2D>("hPosSimVtx",";z position (sim vertices);radius",100, -300, 300, 1000, 0, 120);    
  hPosAll             = fs->make<TH2D>("hPosAll",   ";z position (sim hits);radius"    ,100, -300, 300, 1000, 0, 120);    
  hPosRecMod          = fs->make<TH2D>("hPosRecMod", ";z position (sim hits);radius"    ,100, -300, 300, 1000, 0, 120);    
  //  hPosAll             = fs->make<TH2D>("hPosAll",   ";z position (sim hits);radius", 500, -150, 150, 500, 105, 115);    
  hPosAllXY           = fs->make<TH2D>("hPosAllXY", ";x position (sim hits);y position" ,500, -120, 120, 500, -120, 120);  
  hPosRecModXY        = fs->make<TH2D>("hPosRecModXY", ";x position (sim hits);y position" ,500, -120, 120, 500, -120, 120);  
  hPosMissOut         = fs->make<TH2D>("hPosMissOut",";z position (missing outer hits);radius"    ,100, -300, 300, 1000, 0, 120);    
  //  hPosMissOut         = fs->make<TH2D>("hPosMissOut",";z position (missing outer hits);radius", 500, -150, 150, 500, 105, 115);    



  hTrkMissOutPt = fs->make<TH1D>("hTrkMissOutPt",";pt [tracks with #geq 1 miss. outer hit] (GeV)",100, 50, 150); 
  hTrkMissOutEta = fs->make<TH1D>("hTrkMissOutEta",";eta [tracks with #geq 1 miss. outer hit]",100, -3.0, 3.0); 
  hTrkNoOutEta  = fs->make<TH1D>("hTrkNoOutEta",";eta [tracks with 0 miss. outer hit]",100, -3.0, 3.0); 
  hTrkMissOutPhi = fs->make<TH1D>("hTrkMissOutPhi",";phi [tracks with #geq 1 miss. outer hit]",100, -3.5, 3.5);  
  // hLayerMid      = fs->make<TH2D>("hLayerMid",";layer / wheel / disk number;subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  

  //  file = new TFile("outfile.root","recreate");
  // hNSimHit = new TH1D("numSimHits",";Num. sim hits",30,0,30);  
  // hNSimHitRec  = new TH1D("numSimHitsRec", ";Num. sim hits matched to rechit",    30,0,30);  
  // hNSimHitNRec = new TH1D("numSimHitsNRec",";Num. sim hits not matched to rechit",30,0,30);  
  // hNSimHitMissOut = new TH1D("numSimHitMissOut",";Num. sim hits that give missing outer hits",30,0,30);  
  // hELossRec   = new TH1D("hELossRec",";#DeltaE (sim hits with rec hit)",100,0,1e-3);  
  // hELossNRec  = new TH1D("hELossNRec",";#DeltaE (sim hit that gives missing outer hit)",100,0,1e-3);  
  // hPdgOuterHits = new TH1D("hPdgOuterHits",";PDG ID of sim hit that gives missing outer hit",100,-50, 50);  
  // hPdgRecHits = new TH1D("hPdgRecHits",";PDG ID of sim hit with associated rec hit",100,-50, 50);  

  hTrkEta                    = fs->make<TH1D>("hTrkEta",";#eta;tracks",100, -3, 3);  
  hTrkPt                     = fs->make<TH1D>("hTrkPt", ";p_{T} (GeV);tracks",100, 50, 150);  
  hTrkDistRecSimRecMod       = fs->make<TH1D>("hTrkDistRecSimRecMod",      ";dist (sim hit-rec hit), (rec hit on same module) (cm)",100, 0, 6);  
  hTrkDistRecSimRecLay       = fs->make<TH1D>("hTrkDistRecSimRecLay",      ";dist (sim hit-rec hit), (rec hit on same layer) (cm)",100, 0, 6);  
  hTrkDistRecSimRecLayNotMod = fs->make<TH1D>("hTrkDistRecSimRecLayNotMod",";dist (sim hit-rec hit), (rec hit on same layer, not module) (cm)",100, 0, 5);  
  hTrkDistRecSimRecNoLay     = fs->make<TH1D>("hTrkDistRecSimRecNoLay",    ";dist (sim hit-rec hit), (no rec hit on same layer) (cm)",100, 0, 30);  
  hTrkDistRecSimAll          = fs->make<TH1D>("hTrkDistRecSimAll",         ";dist (sim hit-rec hit), (all sim hits) (cm)",100, 0, 6);  
  hDistVtxMissOut          = fs->make<TH1D>("hDistVtxMissOut",         ";dist (sim hit-vtx), (missing outer hits) (cm)",100, 0, 900);  
  


  hTrkDPerpRecSimRecLayNotMod          = fs->make<TH1D>("hTrkDPerpRecSimRecLayNotMod",         ";perp dist (rec hit-sim hit), (rec hit on same layer, not module) (cm)",100, -5, 5);  
  hTrkDPerpRecSimRecLayNotMod_PerpNear = fs->make<TH1D>("hTrkDPerpRecSimRecLayNotMod_PerpNear",";perp dist (rec hit-sim hit), (rec hit on same layer, not module) (cm)",100, -5, 5);  
  hTrkDPerpRecSimRecLayNotMod_PerpFar  = fs->make<TH1D>("hTrkDPerpRecSimRecLayNotMod_PerpFar", ";perp dist (rec hit-sim hit), (rec hit on same layer, not module) (cm)",100, -5, 5);  
  hTrkDPerpRecSimRecLayNotMod_IsGlued  = fs->make<TH1D>("hTrkDPerpRecSimRecLayNotMod_IsGlued", ";perp dist (rec hit-sim hit), (rec hit on same double layer, not module) (cm)",100, -5, 5);  
  hTrkDPerpRecSimRecLayNotMod_NotGlued = fs->make<TH1D>("hTrkDPerpRecSimRecLayNotMod_NotGlued", ";perp dist (rec hit-sim hit), (rec hit on same single layer, not module) (cm)",100, -5, 5);  

  hLayerRecMod       = fs->make<TH2D>("hLayerRecMod      ",";layer (rec hit on same module);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecLay       = fs->make<TH2D>("hLayerRecLay      ",";layer (rec hit on same layer);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecLayNotMod = fs->make<TH2D>("hLayerRecLayNotMod",";layer (rec hit on same layer, not module);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecLayNotMod_IsGlued  = fs->make<TH2D>("hLayerRecLayNotMod_IsGlued",";layer (rec hit on same double layer, not module);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecLayNotMod_NotGlued = fs->make<TH2D>("hLayerRecLayNotMod_NotGlued",";layer (rec hit on same single layer, not module);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecLayNotMod_PerpNear = fs->make<TH2D>("hLayerRecLayNotMod_PerpNear",";layer (rec hit on same layer, not module);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecLayNotMod_PerpFar  = fs->make<TH2D>("hLayerRecLayNotMod_PerpFar", ";layer (rec hit on same layer, not module);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerRecNoLay     = fs->make<TH2D>("hLayerRecNoLay    ",";layer (no rec hit on same layer);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerAll          = fs->make<TH2D>("hLayerRecAll      ",";layer (all sim hits);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerAll_IsGlued          = fs->make<TH2D>("hLayerRecAll_IsGlued      ",";layer (all sim hits from double layers);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerAll_NotGlued         = fs->make<TH2D>("hLayerRecAll_NotGlued     ",";layer (all sim hits from single layers);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerMissOut       = fs->make<TH2D>("hLayerMissOut",";layer (missing outer hit);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerMissOut_YPos  = fs->make<TH2D>("hLayerMissOut_YPos",";layer (missing outer hit);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  
  hLayerMissOut_YNeg  = fs->make<TH2D>("hLayerMissOut_YNeg",";layer (missing outer hit);subdet (TIB=3, TID=4, TOB=5, TEC=6)",10, -0.5, 9.5, 4, 2.5, 6.5);  

  hNSimHitsVsLayerTIB  = fs->make<TH2D>("hNSimHitsVsLayerTIB",";layer (TIB);number of sim hits",10, -0.5, 9.5, 7, -0.5, 6.5);  
  hNSimHitsVsLayerTOB  = fs->make<TH2D>("hNSimHitsVsLayerTOB",";layer (TOB);number of sim hits",10, -0.5, 9.5, 7, -0.5, 6.5);  


  hTestYDir = fs->make<TH2D>("hTestYDir",";z global position;#vec{y}_{local}#bullet#vec{z}_{global}", 100, -150, 150, 50, -2.5, 2.5);





  hELossNRecOut           = fs->make<TH1D>("hELossNRec",";#DeltaE (sim hit that gives first missing outer hit);hits",100,0,1e-3);  
  hELossRecMod            = fs->make<TH1D>("hELossRecMod",      ";#DeltaE (rec hit on same module)",100,0,1e-3);  
  hELossRecRecLay         = fs->make<TH1D>("hELossRecLay      ",";#DeltaE (rec hit on same layer)",100,0,1e-3);  
  hELossRecRecLayNotMod   = fs->make<TH1D>("hELossRecLayNotMod",";#DeltaE (rec hit on same layer, not module)",100,0,1e-3);  
  hELossRecRecNoLay       = fs->make<TH1D>("hELossRecNoLay    ",";#DeltaE (no rec hit on same layer)",100,0,1e-3);  
  hELossRecAll            = fs->make<TH1D>("hELossAll         ",";#DeltaE (all sim hits) (cm)",100,0,1e-3);  


  hNSimRecHitDiff         = fs->make<TH1D>("hNSimRecHitDiff",   ";N^{hits}_{sim} - N^{hits}_{rec}",21,-10.5,10.5);  
  hNSimHit                = fs->make<TH1D>("hNSimHit",   ";N^{hits}_{sim}",31,-0.5,30.5);  
  hNRecHit                = fs->make<TH1D>("hNRecHit",   ";N^{hits}_{rec}",31,-0.5,30.5);  



}

testTrackAssociator::~testTrackAssociator() {
  timer->Print();  
  cout << "Finished testTrackAssociator successfully." << endl;  


}

void testTrackAssociator::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  
  edm::ESHandle<MagneticField> theMF;
  setup.get<IdealMagneticFieldRecord>().get(theMF);
  edm::ESHandle<TrackAssociatorBase> theChiAssociator;
  setup.get<TrackAssociatorRecord>().get("TrackAssociatorByChi2",theChiAssociator);
  associatorByChi2 = (TrackAssociatorBase *) theChiAssociator.product();
  edm::ESHandle<TrackAssociatorBase> theHitsAssociator;  
  setup.get<TrackAssociatorRecord>().get("TrackAssociatorByHits",theHitsAssociator);
  associatorByHits = (TrackAssociatorBase *) theHitsAssociator.product();

  Handle<View<Track> > trackCollectionH;
  event.getByLabel(tracksTag,trackCollectionH);
  const View<Track>  tC = *(trackCollectionH.product()); 
  
  Handle<SimTrackContainer> simTrackCollection;
  event.getByLabel(simtracksTag, simTrackCollection);
  const SimTrackContainer simTC = *(simTrackCollection.product());
  
  Handle<SimVertexContainer> simVertexCollection;
  event.getByLabel(simvtxTag, simVertexCollection);
  const SimVertexContainer simVC = *(simVertexCollection.product());

  edm::Handle<TrackingParticleCollection>  TPCollectionH ;
  event.getByLabel(tpTag,TPCollectionH);
  const TrackingParticleCollection tPC   = *(TPCollectionH.product());

  // std::string measTkName = iConfig_.getParameter<std::string>("MeasurementTracker");
  // setup.get<CkfComponentsRecord>().get(measTkName,theMeasTk);
  edm::ESHandle<MeasurementTracker> theMeasTk;
  setup.get<CkfComponentsRecord>().get("", theMeasTk);  

  // extract tracker geometry
  //
  setup.get<TrackerDigiGeometryRecord>().get(theG);
  
  event.getByLabel("siStripClusters", clusterStrip);

  event.getByLabel("siStripMatchedRecHits", "rphiRecHit",            rphiRecHits);
  event.getByLabel("siStripMatchedRecHits", "rphiRecHitUnmatched",   rphiRecHitsUnmatched);
  event.getByLabel("siStripMatchedRecHits", "stereoRecHit",          stereoRecHits);
  event.getByLabel("siStripMatchedRecHits", "stereoRecHitUnmatched", stereoRecHitsUnmatched);

  // edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "rphiRecHit"      "HLT"     
  //   edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "rphiRecHitUnmatched"   "HLT"     
  //   edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "stereoRecHit"    "HLT"     
  //   edmNew::DetSetVector<SiStripRecHit2D>    "siStripMatchedRecHits"     "stereoRecHitUnmatched"   "HLT"     
    

  cout << "\nEvent ID = "<< event.id() << endl ;

  // ***********************************
  // Loop over clusters 
  // ***********************************

  int nClus = 0;  
  for (edmNew::DetSetVector<SiStripCluster>::const_iterator itreco = clusterStrip->begin(); itreco != clusterStrip->end(); ++itreco) {
    edmNew::DetSet<SiStripCluster> detset = *itreco;        
    //    cout << "Debug:  detId = " << detset.detId() << endl;  
    for (edmNew::DetSet<SiStripCluster>::const_iterator cluster = detset.begin(), endcluster = detset.end(); cluster != endcluster; ++cluster) {
      nClus++;  
    }
  }
  cout << "Found nClus = " << nClus << endl;  


  // //RECOTOSIM 
  // cout << "                      ****************** Reco To Sim ****************** " << endl;
  // cout << "-- Associator by hits --" << endl;  
  // reco::RecoToSimCollection p = 
  //   associatorByHits->associateRecoToSim (trackCollectionH,TPCollectionH,&event );
  // hNTrksAll->Fill(tC.size());  
  // if (tC.size() != 2) return;  // only consider events with exactly two reco'd tracks  
  // hNTrks->Fill(tC.size());  
  // for(View<Track>::size_type i=0; i<tC.size(); ++i) {
  //   RefToBase<Track> track(trackCollectionH, i);
    
  //   // make list of det id's for all rec hits
  //   vector<unsigned int> t1GoodHits;
  //   vector<RecHitInfo> recHits;  
  //   //    trackingRecHit_iterator iter;
  //   for(trackingRecHit_iterator iter = track->recHitsBegin(); 
  // 	iter != track->recHitsEnd(); iter++) {
  //     if (!(*iter)->isValid()) continue;
  //     t1GoodHits.push_back((*iter)->rawId());  
  //     // use as reference:  http://cmslxr.fnal.gov/lxr/source/RecoTracker/TrackProducer/test/TrackAnalyzer.cc#071
  //     //      GlobalPoint globalPt = theG->idToDet((*iter)->geographicalId())->surface().toGlobal((*iter)->localPosition());  
  //     //      Surface surf = theG->idToDet((*iter)->geographicalId())->surface();  
  //     RecHitInfo onehit;
  //     // onehit.eta = globalPt.eta();
  //     // onehit.phi = globalPt.phi();
  //     onehit.eta = theG->idToDet((*iter)->geographicalId())->surface().eta();
  //     onehit.phi = theG->idToDet((*iter)->geographicalId())->surface().phi();
  //     onehit.subdet = DetId((*iter)->rawId()).subdetId();  
  //     onehit.layer = getLayerHit((*iter)->rawId());  
  //     recHits.push_back(onehit);  
  //   }

  //   try{ 
  //     std::vector<std::pair<TrackingParticleRef, double> > tp = p[track];
  // 	cout << "Reco Track pT: "  << setw(6) << track->pt() 
  // 	     <<  " matched to " << tp.size() << " MC Tracks" << std::endl;
  // 	for (std::vector<std::pair<TrackingParticleRef, double> >::const_iterator it = tp.begin(); 
  // 	     it != tp.end(); ++it) {
  // 	  TrackingParticleRef tpr = it->first;
  // 	  double assocChi2 = it->second;
  // 	  cout << "\t\tMCTrack " << setw(2) << tpr.index() 
  // 	       << " pdgId = " <<  tpr->pdgId()  
  // 	       << ", pT: " << setw(6) << tpr->pt()  
  // 	       << ", eta: " << setw(6) << tpr->eta() 
  // 	       << ", phi: " << tpr->phi() 
  // 	       << " NShared: " << assocChi2 << endl;
  // 	  vector<PSimHit> simHits = tpr->trackPSimHit();  
  // 	  cout << "Number of sim hits: " << simHits.size() << endl;  

  // 	  cout << "Printing sim hits (detId,localX,localY):  " << endl;
  // 	  int nHitTrk = 0;  
  // 	  int nHitTrkMatchRec = 0;  
  // 	  int nHitTrkNoMatchRec = 0;  
  // 	  int nHitMissOut = 0;  
  // 	  int nHitMissMid = 0;  
  // 	  int nHitMissMidByLayer = 0;  
  // 	  int nHitNotMissMidByLayer = 0;  
  // 	  int nHitPix = 0;  
  // 	  hNSimHitMinusRec->Fill(simHits.size() - t1GoodHits.size());  
  // 	  for (uint ihit=0; ihit<simHits.size(); ihit++) {  
  // 	    DetId detid = DetId(simHits.at(ihit).detUnitId());  
  // 	    if (detid.det() != DetId::Tracker) continue;  
  // 	    if (detid.subdetId()<3) { 		  
  // 	      nHitPix++;  
  // 	      continue;  // only consider strip hits
  // 	    }
  // 	    if (detid.subdetId()>6) continue;  

  // 	    bool hasMatchedRecHit;  
  // 	    if (find(t1GoodHits.begin(),t1GoodHits.end(),detid.rawId()) != t1GoodHits.end()) hasMatchedRecHit = true;
  // 	    else                                                                           hasMatchedRecHit = false;
  // 	    bool isMissOut = isMissOutHit(simHits, ihit, t1GoodHits);  
  // 	    bool isMatchRecLayer = isMatchedToRecHitbyLayer(simHits.at(ihit), recHits);  
  // 	    SiStripDetId strDetId = SiStripDetId(detid.rawId());  
  // 	    cout << "\t("
  // 		 << simHits.at(ihit).detUnitId() 
  // 		 << "; det = " << detid.det() 
  // 		 << "; subdetId = " << detid.subdetId() << "("  
  // 		 << simHits.at(ihit).localPosition().x() << ","
  // 		 << simHits.at(ihit).localPosition().y() << ")"
  // 		 << "; hasMatchedRecHit=" << hasMatchedRecHit  
  // 		 << "; energyLoss=" << simHits.at(ihit).energyLoss() 
  // 		 << "; particleType=" << simHits.at(ihit).particleType() 
  // 		 << "; processType=" << simHits.at(ihit).processType() 
  // 		 << "; strDetId subdet=" << strDetId.subDetector() 
  // 		 << endl;  

  // 	    if (isMissOut) {
  // 	      nHitMissOut++;  
  // 	      hPdgOuterHits->Fill(simHits.at(ihit).particleType());  
  // 	      hELossNRecOut->Fill(simHits.at(ihit).energyLoss());  

  // 	      LocalPoint HitLocalPos = simHits.at(ihit).localPosition();  
  // 	      // const MeasurementTracker* measTk;  
  // 	      const MeasurementDet* measDet = theMeasTk->idToDet(detid);	
  // 	      double HalfWidth      = measDet->surface().bounds().width()  /2.0;  // in X direction  
  // 	      double HalfLength     = measDet->surface().bounds().length() /2.0;  // in Y direction        
  // 	      const TrapezoidalPlaneBounds* trapezoidalBounds( dynamic_cast<const TrapezoidalPlaneBounds*>(&(measDet->surface().bounds())));   
  // 	      const RectangularPlaneBounds* rectangularBounds( dynamic_cast<const RectangularPlaneBounds*>(&(measDet->surface().bounds())));   
  // 	      if (trapezoidalBounds) {  
  // 		std::vector<float> const & parameters = (*trapezoidalBounds).parameters();
  // 		HalfLength     = parameters[3];  
  // 		double t       = (HalfLength + HitLocalPos.y()) / (2*HalfLength); 
  // 		HalfWidth      = parameters[0] + (parameters[1]-parameters[0]) * t;  
  // 		//      cout << "Warning:  using trapezoidal bounds." << endl;  
  // 	      } else if (!rectangularBounds) {
  // 		cout << "Warning:  neither trapezoidal nor rectangular bounds found!" << endl;
  // 		return; 
  // 	      }       
  // 	      double distEdgeXNorm =  HitLocalPos.x() / HalfWidth;  
  // 	      double distEdgeYNorm =  HitLocalPos.y() / HalfLength;  
  // 	      hDistXMissOut->Fill(distEdgeXNorm);  
  // 	      hDistYMissOut->Fill(distEdgeYNorm);  
  // 	    } else { // if (isMissOut) {  
  // 	      if (!isMatchRecLayer) {	      // It has no matched rec hit, in the same layer
  // 		hELossNRecMidLayer->Fill(simHits.at(ihit).energyLoss());  
  // 		int layer = getLayerHit(detid.rawId());  
  // 		hLayerMid->Fill(layer, strDetId.subDetector());  
  // 		hSubDetMid->Fill(strDetId.subDetector());  
  // 		nHitMissMidByLayer++;  
  // 	      } else {
  // 		hELossRecMidLayer->Fill(simHits.at(ihit).energyLoss());  
  // 		nHitNotMissMidByLayer++;  
  // 	      }
  // 	      if (hasMatchedRecHit) {	      // It has a matched rec hit
  // 		nHitTrkMatchRec++; 
  // 		hELossRecMid->Fill(simHits.at(ihit).energyLoss());  
  // 		hPdgRecHits->Fill(simHits.at(ihit).particleType());  
  // 	      } else {
  // 		nHitTrkNoMatchRec++;  // No matched rec hit
  // 		hELossNRecMid->Fill(simHits.at(ihit).energyLoss());  
  // 		nHitMissMid++;  
  // 	      }  
  // 	    } // if (isMissOut) {
  // 	    nHitTrk++;  
  // 	  }
  // 	  cout << "Num sim tracker hits: " << nHitTrk 
  // 	       << "; num matched to rec hit: " << nHitTrkMatchRec
  // 	       << "; num not matched to rec hit: " << nHitTrkNoMatchRec
  // 	       << "; num miss outer hits: " << nHitMissOut
  // 	       << "; num miss mid hits: " << nHitMissMid 
  // 	       << endl;  
  // 	  hNSimHit->Fill(nHitTrk );
  // 	  hNSimHitRec->Fill(nHitTrkMatchRec);
  // 	  hNSimHitNRec->Fill(nHitTrkNoMatchRec);
  //  	  hNSimHitMissOut->Fill(nHitMissOut); 	  
  // 	  hNSimHitMissMid->Fill(nHitMissMid); 	  
  // 	  hNSimHitMissMidByLayer->Fill(nHitMissMidByLayer);  
  // 	  hNPixHit->Fill(nHitPix);  
  // 	  if (nHitMissOut>0) {
  // 	    hTrkMissOutPt ->Fill(track->pt());  
  // 	    hTrkMissOutEta->Fill(track->eta());  
  // 	    hTrkMissOutPhi->Fill(track->phi());  
  // 	  } else {
  // 	    hTrkNoOutEta->Fill(track->eta());  
  // 	  }  
  // 	}
  //   } catch (Exception event) {
  //     cout << "->   Track pT: " 
  // 	   << setprecision(2) << setw(6) << track->pt() 
  // 	   <<  " matched to 0  MC Tracks" << endl;
  //   }


  // } //   for(View<Track>::size_type i=0; i<tC.size(); ++i) {

  // cout << "-- Associator by chi2 --" << endl;  
  // p = associatorByChi2->associateRecoToSim (trackCollectionH,TPCollectionH,&event );
  // for(View<Track>::size_type i=0; i<tC.size(); ++i) {
  //   RefToBase<Track> track(trackCollectionH, i);
  //   try{ 
  //     std::vector<std::pair<TrackingParticleRef, double> > tp = p[track];
  //     cout << "Reco Track pT: "  << setw(6) << track->pt() 
  // 	   <<  " matched to " << tp.size() << " MC Tracks" << std::endl;
  //     for (std::vector<std::pair<TrackingParticleRef, double> >::const_iterator it = tp.begin(); 
  // 	   it != tp.end(); ++it) {
  // 	TrackingParticleRef tpr = it->first;
  // 	double assocChi2 = it->second;
  // 	cout << "\t\tMCTrack " << setw(2) << tpr.index() << " pT: " << setw(6) << tpr->pt() << 
  // 	  " chi2: " << assocChi2 << endl;
  //     }
  //   } catch (Exception event) {
  //     cout << "->   Track pT: " 
  // 	   << setprecision(2) << setw(6) << track->pt() 
  // 	   <<  " matched to 0  MC Tracks" << endl;
  //   }
  // }


  // First get positions of all the rec hits, and store them in a data structure.  
  vector<TrackAndHits> trkRecHits;  

  Handle<TrajTrackAssociationCollection> trajTrackAssociationHandle;
  event.getByLabel("TrackRefitter", trajTrackAssociationHandle);  
  const TrajTrackAssociationCollection& TrajToTrackMap = *trajTrackAssociationHandle.product();
  Handle<TrackCollection> tracks;
  event.getByLabel(tracksTag,tracks);

  double minTrkPt = 90;  
  double maxTrkEta = 0.8;  
  int j=0;
  TrackCollection::const_iterator itTrack = tracks->begin();
  for(TrajTrackAssociationCollection::const_iterator cit=TrajToTrackMap.begin(); 
      cit!=TrajToTrackMap.end() && itTrack!=tracks->end(); 
      cit++, j++, itTrack++){

    const reco::TrackRef track = cit->val;  
    const edm::Ref<std::vector<Trajectory> > traj = cit->key;

    if (itTrack->pt() < minTrkPt) continue;  
    if (fabs(itTrack->eta()) > maxTrkEta) continue;  

    cout << "***********************************************************" << endl;  

    const vector<TrajectoryMeasurement> & measurements = traj->measurements();

    TrackAndHits newTrk;
    newTrk.pt  = itTrack->pt();
    newTrk.eta = itTrack->eta();
    newTrk.phi = itTrack->phi();

    // Loop over rechits
    for(vector<TrajectoryMeasurement>::const_iterator it = measurements.begin(); it!=measurements.end(); it++){
      TrajectoryStateOnSurface trajState=it->updatedState();
      if( !trajState.isValid()) continue;      
      const TrackingRecHit * recHit=(*it->recHit()).hit();
      if (!recHit) continue;
      //      cout << "Rec hit det id: " << recHit->geographicalId().rawId() << endl;  


      RecHitInfo newhit;
      newhit.detRawId = recHit->geographicalId().rawId();   
      newhit.subdet = getSubDet(newhit.detRawId);  
      if (newhit.subdet < 3) continue;  // ignore pixel hits  
      newhit.globalPt = trajState.globalPosition();  
      newhit.layer = getLayerHit(newhit.detRawId);  
      newhit.eta = newTrk.eta;  // set the hit eta to the track eta 
      newhit.phi = newTrk.phi;  // set the hit phi to the track phi  
      newTrk.rechits.push_back(newhit);        
    }
    trkRecHits.push_back(newTrk);  

  }

  cout << "Printing reco tracks and rec hits." << endl;
  for (uint itrk=0; itrk<trkRecHits.size(); itrk++) {
    cout << "  Track " << itrk  
	 << ": pt=" << trkRecHits.at(itrk).pt
	 << ", eta=" << trkRecHits.at(itrk).eta
	 << ", phi=" << trkRecHits.at(itrk).phi
	 << ", num hits=" << trkRecHits.at(itrk).rechits.size()
	 << endl; 
    for (uint ihit=0; ihit<trkRecHits.at(itrk).rechits.size(); ihit++) {
      cout << "  " << ihit << ": " << trkRecHits.at(itrk).rechits.at(ihit).detRawId
	   << "(" << trkRecHits.at(itrk).rechits.at(ihit).globalPt.x()
	   << "," << trkRecHits.at(itrk).rechits.at(ihit).globalPt.y()
	   << "," << trkRecHits.at(itrk).rechits.at(ihit).globalPt.z()
	   << "), " 
	   << " subdet=" << trkRecHits.at(itrk).rechits.at(ihit).subdet 
	   << ", layer=" << trkRecHits.at(itrk).rechits.at(ihit).layer
	   << endl; 
    }
  }

  // Look at sim vertices
  // for(SimVertexContainer::size_type i=0; i<simVC.size(); ++i){
  // }
  int nSimVert = 0;
  vector<GlobalPoint> simVerts;  
  for (SimVertexContainer::const_iterator it = simVC.begin(); it != simVC.end(); ++it) {
    nSimVert++;  
    //    if (!it->inVolume()) continue; // consider only vertices inside the tracker volume  
    GlobalPoint simPt(it->position().x(),
		      it->position().y(),
		      it->position().z());  
    if (fabs(simPt.z()) > 293.5) continue;  
    if (simPt.perp() > 120.0)    continue;  // ensure vertex is in tracker volume
    if (simPt.perp() > 1) simVerts.push_back(simPt);  
    cout << "Found vertex with position: " << it->position() << endl;  
    hPosSimVtx->Fill(simPt.z(), simPt.perp());  
  }  
  cout << "Found " << nSimVert << " sim vertices" << endl;  

  //SIMTORECO
  cout << "                      ****************** Sim To Reco ****************** " << endl;
  cout << "-- Associator by hits --" << endl;  
  reco::SimToRecoCollection q = 
    associatorByHits->associateSimToReco(trackCollectionH,TPCollectionH,&event );
  hNTrksAll->Fill(tC.size());  
  for(SimTrackContainer::size_type i=0; i<simTC.size(); ++i){
    TrackingParticleRef tp (TPCollectionH,i);
    try{ 
      std::vector<std::pair<RefToBase<Track>, double> > trackV = q[tp];
      if (tp->pt() < minTrkPt) continue;  // consider only 100 GeV tracks  
      if (fabs(tp->eta()) > maxTrkEta) continue;  // consider tracks in barrel  
      //      if (fabs(tp->eta()) > 2.5) continue;  // testing
      if (trackV.size() > 1) continue;  // consider only sim tracks matched to exactly one reco track 
      cout << "Sim Track " << setw(2) << tp.index() 
	   << " pT: "  << setw(6) << tp->pt() 
	   << " eta: " << tp->eta() 
	   << " phi: " << tp->phi() 
	   << " matched to " << trackV.size() << " reco::Tracks" << std::endl;  
      for (std::vector<std::pair<RefToBase<Track>,double> >::const_iterator it=trackV.begin(); it != trackV.end(); ++it) {
	RefToBase<Track> tr = it->first;
	double assocChi2 = it->second;
	cout << "\t\treco::Track pT: " << setw(6) << tr->pt() << 
	  " NShared: " << assocChi2 << endl;

	cout << "  Reco track hits: " << endl;  
	int iRecoHit = 0;  
	for(trackingRecHit_iterator iter = tr->recHitsBegin(); 
	    iter != tr->recHitsEnd(); iter++) {
	  DetId detid = DetId((*iter)->rawId());  
	  if (detid.det() != DetId::Tracker) continue;  
	  if (detid.subdetId()<3 || 
	      detid.subdetId()>6) continue;  // only consider strip hits
	  if (!(*iter)->isValid()) continue;	     
	  cout << "    " << iRecoHit << ": " 
	       << detid.rawId()  
	       << ", subdet=" << detid.subdetId()
	       << ", layer=" << getLayerHit(detid.rawId())  
	       << endl;  
	  iRecoHit++;  
	}

	hTrkEta->Fill(tp->eta());  
	hTrkPt ->Fill(tp->pt());  

	vector<PSimHit> simHits = tp->trackPSimHit();  
	int nHitMissOutReco = tr->trackerExpectedHitsOuter().numberOfHits();  
	int nHitMissMidReco = tr->hitPattern().trackerLayersWithoutMeasurement();   
	int nSimHit = 0;  
	int nHitPix = 0;  
	int nHitMissOut = 0;  
	int nHitMissOutFirst = 0;  
	int nHitMissOutFirstMissPrevSim = 0;  
	int nSimHitMatchMod = 0;  
	int nSimHitMatchLay = 0;  
	int nSimHitMatchLayNotMod = 0;  

	//	int nHitMissMid = 0;  
  // 	  int nHitTrkMatchRec = 0;  
  // 	  int nHitTrkNoMatchRec = 0;  
  // 	  int nHitMissMidByLayer = 0;  
  // 	  int nHitNotMissMidByLayer = 0;  

	vector<HitSubdetLay> simHitSubdetLay;  
	cout << "  Sim hits:  (total number: " << simHits.size() << ")" << endl;  

	for (uint ihit=0; ihit<simHits.size(); ihit++) {  
	  DetId detid = DetId(simHits.at(ihit).detUnitId());  
	  if (detid.det() != DetId::Tracker) continue;  
	  if (detid.subdetId()<3) { 		  
	    nHitPix++;  
	    continue;  // only consider strip hits
	  }
	  if (detid.subdetId()>6) continue;  // only consider strip tracker hits
	  bool isMissOut      = isMissOutHit     (simHits, ihit, (*tr));   
	  bool isMissOutFirst = isMissOutHitFirst(simHits, ihit, (*tr));   
	  bool isMissSimPrev  = isMissSimPrevLay (simHits, ihit);  
	  //	  if (isMissOutHit(simHits, ihit, (*tr))) continue;   // only consider missing middle hits  
	  
	  bool foundMatchMod = false;  	    
	  bool foundMatchLay = false;  	    
	  int layerSim = getLayerHit(detid.rawId());  
	  bool isGlued = getIsGlued(simHits.at(ihit).detUnitId());  
	  bool isClusOnMod   =   isClusOnModule(simHits.at(ihit).detUnitId());  
	  bool isRecHitOnMod = isRecHitOnModule(simHits.at(ihit).detUnitId());  
	  for(trackingRecHit_iterator iter = tr->recHitsBegin(); 
	      iter != tr->recHitsEnd(); iter++) {
	    if (!(*iter)->isValid()) continue;	      
	    //	    int layerRec = getLayerHit((*iter)->rawId());  
	    if (simHits.at(ihit).detUnitId() == (*iter)->rawId()) foundMatchMod = true;  
	  }  
	  //	  if (isMatchedToRecHitbyLayer(simHits.at(ihit), (*tr))) foundMatchLay = true;  // original 
	  if (isMatchedToRecHitbyLayer(simHits.at(ihit), trkRecHits)) foundMatchLay = true;  
	  RecHitInfo closestHit;
	  int nTrkMatch = 0;  
	  for (uint itrk=0; itrk<trkRecHits.size(); itrk++) {
	    if (deltaR(tr->eta(), tr->phi(), trkRecHits.at(itrk).eta, trkRecHits.at(itrk).phi) > 1.0) continue; 
	    int idxMatch = findClosestRecHit(simHits.at(ihit), trkRecHits.at(itrk).rechits);
	    if (idxMatch >= 0) { 
	      closestHit = trkRecHits.at(itrk).rechits.at(idxMatch);  
	      nTrkMatch++;  
	    } else {
	      cout << "ERROR:  idxMatch < 0" 
		   << " for pt=" << tr->pt()
		   << "; eta=" << tr->eta()
		   << endl; 
	    }
	  }
	  if (nTrkMatch!=1) { 
	    cout << "ERROR:  nTrkMatch = " << nTrkMatch << endl; 
	    continue;  
	  } 

	  HitSubdetLay subdetLay;
	  subdetLay.subdet = getSubDet  (detid.rawId());  
	  subdetLay.layer  = getLayerHit(detid.rawId());  
	  simHitSubdetLay.push_back(subdetLay);  
	  cout << "    " << nSimHit << ": " << detid.rawId()  
	       << ", " << getGlobalPos(simHits.at(ihit)) 
	       << ", subdet=" << detid.subdetId()
	       << ", layer=" << getLayerHit(detid.rawId())  
	       << ", isMissOut=" << isMissOut  
	       << ", isClus=" << isClusOnMod
	       << ", isRecHitOnMod=" << isRecHitOnMod  
	       << ", isGlued=" << isGlued  
	       << ", isMissSimPrev=" << isMissSimPrev
	       << endl; 


	  LocalPoint HitLocalPos = simHits.at(ihit).localPosition();  
	  // const MeasurementTracker* measTk;  
	  const MeasurementDet* measDet = theMeasTk->idToDet(detid);	
	  double HalfWidth      = measDet->surface().bounds().width()  /2.0;  // in X direction  
	  double HalfLength     = measDet->surface().bounds().length() /2.0;  // in Y direction        
	  const TrapezoidalPlaneBounds* trapezoidalBounds( dynamic_cast<const TrapezoidalPlaneBounds*>(&(measDet->surface().bounds())));   
	  const RectangularPlaneBounds* rectangularBounds( dynamic_cast<const RectangularPlaneBounds*>(&(measDet->surface().bounds())));   
	  if (trapezoidalBounds) {  
	    std::vector<float> const & parameters = (*trapezoidalBounds).parameters();
	    HalfLength     = parameters[3];  
	    double t       = (HalfLength + HitLocalPos.y()) / (2*HalfLength); 
	    HalfWidth      = parameters[0] + (parameters[1]-parameters[0]) * t;  
	    //      cout << "Warning:  using trapezoidal bounds." << endl;  
	  } else if (!rectangularBounds) {
	    cout << "Warning:  neither trapezoidal nor rectangular bounds found!" << endl;
	    return; 
	  }       
	  double distEdgeXNorm =  HitLocalPos.x() / HalfWidth;  
	  double distEdgeYNorm =  HitLocalPos.y() / HalfLength;  
	  
	  double distSimToRec = getDist(closestHit.globalPt, getGlobalPos(simHits.at(ihit)));  
	  double distPerpSimToRec = getDistPerp(getGlobalPos(simHits.at(ihit)), closestHit.globalPt);  
	  double energyLoss = simHits.at(ihit).energyLoss();  


	  double distToClosestSimVtx = 999;
	  for (uint i=0; i<simVerts.size(); i++) {
	    double distToVtx = (simVerts.at(i) - getGlobalPos(simHits.at(ihit))).mag();
	    if (distToVtx < distToClosestSimVtx) distToClosestSimVtx = distToVtx;  
	  }

	  // ************************************
	  // Now fill histograms
	  // ************************************

	  if (isMissOut) {  
	    nHitMissOut++;  
	    if (isMissOutFirst) {
	    // if (isMissOutFirst && layerSim==5
	    //    	&& fabs(getGlobalPos(simHits.at(ihit)).z())>60) {
	    // if (isMissOutFirst && layerSim==5
	    //    	&& getGlobalPos(simHits.at(ihit)).z()>0) {
	      nHitMissOutFirst++;  
	      if (isMissSimPrev) nHitMissOutFirstMissPrevSim++;    
	      hPdgOuterHits->Fill(simHits.at(ihit).particleType());  
	      hELossNRecOut->Fill(simHits.at(ihit).energyLoss());  
	      hDistXMissOut->Fill(distEdgeXNorm);  
	      hDistYMissOut->Fill(distEdgeYNorm);  
	      hDistXYMissOut->Fill(distEdgeXNorm, distEdgeYNorm);  
	      hDistVtxMissOut->Fill(distToClosestSimVtx);  
	      hLayerMissOut->Fill(layerSim, detid.subdetId());  
	      hPosMissOut->Fill(getGlobalPos(simHits.at(ihit)).z(), 
				getGlobalPos(simHits.at(ihit)).perp());  
	      hTestYDir->Fill(getGlobalPos(simHits.at(ihit)).z(), 
			      getModuleAngle(simHits.at(ihit)));  
	      if (distEdgeYNorm>0) {
		hDistYMissOut_YPos->Fill(distEdgeYNorm);  
		hLayerMissOut_YPos->Fill(layerSim, detid.subdetId());  
	      } else {
		hDistYMissOut_YNeg->Fill(distEdgeYNorm);  
		hLayerMissOut_YNeg->Fill(layerSim, detid.subdetId());  
	      }
	    }
	  } else { // if (isMissOut) {  	    
	    nSimHit++;  
	    hTrkDistRecSimAll->Fill(distSimToRec);  
	    hLayerAll      ->Fill(layerSim, detid.subdetId());  
	    hELossRecAll->Fill(energyLoss);            
	    hPosAll->Fill(getGlobalPos(simHits.at(ihit)).z(), 
			  getGlobalPos(simHits.at(ihit)).perp());  
	    hPosAllXY->Fill(getGlobalPos(simHits.at(ihit)).x(), 
			    getGlobalPos(simHits.at(ihit)).y());  


	    if (isGlued) {
	      hLayerAll_IsGlued ->Fill(layerSim, detid.subdetId());  
	    } else {
	      hLayerAll_NotGlued->Fill(layerSim, detid.subdetId());  
	    }
	  

	    if (foundMatchMod) { 
	      nSimHitMatchMod++;
	      hTrkDistRecSimRecMod->Fill(distSimToRec);  
	      hLayerRecMod      ->Fill(layerSim, detid.subdetId());  
	      hELossRecMod->Fill(energyLoss);        
	      hPosRecMod->Fill(getGlobalPos(simHits.at(ihit)).z(), 
			       getGlobalPos(simHits.at(ihit)).perp());  
	      hPosRecModXY->Fill(getGlobalPos(simHits.at(ihit)).x(), 
				 getGlobalPos(simHits.at(ihit)).y());  
   	    }  
	    if (foundMatchLay) { 
	      nSimHitMatchLay++;  
	      hTrkDistRecSimRecLay->Fill(distSimToRec);  
	      hLayerRecLay      ->Fill(layerSim, detid.subdetId());  
	      hELossRecRecLay->Fill(energyLoss);          
	    } else {
	      hTrkDistRecSimRecNoLay->Fill(distSimToRec);  
	      hLayerRecNoLay   ->Fill(layerSim, detid.subdetId());  
	      hELossRecRecNoLay->Fill(energyLoss);       
	      hDistXYNotLay->Fill(distEdgeXNorm, distEdgeYNorm);  
	      // int countNumAfter = countNumAfter(simHits, ihit, (*tr));  
	      // hNHitsAfter->Fill(countNumAfter);  
	      double dR = deltaR(getGlobalPos(simHits.at(ihit)).eta(), getGlobalPos(simHits.at(ihit)).phi(), closestHit.eta, closestHit.phi);  
	      cout << "     Found sim hit with no rec hit on same layer: " << getGlobalPos(simHits.at(ihit)) 
		   << ", layer=" << layerSim
		   << ", subdet=" << detid.subdetId() 
		   << ", detRawId=" << detid.rawId()  
		   << "; rec hit pos = " << closestHit.globalPt 
		   << ", detRawId=" << closestHit.detRawId  
		   << ", layer=" << closestHit.layer
		   << ", subdet=" << closestHit.subdet
		   << ", deltaR=" << dR
		   << endl;  
	      // if (distSimToRec < 1) {
	      //   cout << "Debug:  distance between sim and rec hits is < 1 cm, but no rec hit is found on the same layer." 
	      // 	   << "  Sim hit pos = " << getGlobalPos(simHits.at(ihit)) 
	      // 	   << ", layer=" << layerSim
	      // 	   << ", subdet=" << detid.subdetId() 
	      // 	   << ", detRawId=" << detid.rawId()  
	      // 	   << "; rec hit pos = " << closestHit.globalPt 
	      // 	   << ", detRawId=" << closestHit.detRawId  
	      // 	   << ", layer=" << closestHit.layer
	      // 	   << ", subdet=" << closestHit.subdet
	      // 	   << endl;  
	      //   bool debugTest = isMatchedToRecHitbyLayer(simHits.at(ihit), (*tr), true); 
	      //   cout << "  debugTest=" << debugTest << endl;  
	      // }
	    }
	    if (foundMatchLay && !foundMatchMod) {
	      nSimHitMatchLayNotMod++;  
	      hTrkDistRecSimRecLayNotMod->Fill(distSimToRec);  
	      hTrkDPerpRecSimRecLayNotMod->Fill(distPerpSimToRec);  
	      hLayerRecLayNotMod->Fill(layerSim, detid.subdetId());  
	      hELossRecRecLayNotMod->Fill(energyLoss);    
	      if (isGlued) {
		hTrkDPerpRecSimRecLayNotMod_IsGlued->Fill(distPerpSimToRec);  
		hLayerRecLayNotMod_IsGlued ->Fill(layerSim, detid.subdetId());  
	      } else {
		hTrkDPerpRecSimRecLayNotMod_NotGlued->Fill(distPerpSimToRec);  
		hLayerRecLayNotMod_NotGlued->Fill(layerSim, detid.subdetId());  
	      }
	      if (!isRecHitOnMod) { 
		hDistXYLayNotMod_NoRecHitOnMod->Fill(distEdgeXNorm, distEdgeYNorm);  
	      }

	      if (fabs(distPerpSimToRec) < 1.75) {
		hTrkDPerpRecSimRecLayNotMod_PerpNear->Fill(distPerpSimToRec);  
		hLayerRecLayNotMod_PerpNear->Fill(layerSim, detid.subdetId());  
		hDistXLayNotMod_PerpNear->Fill(distEdgeXNorm);  
		hDistYLayNotMod_PerpNear->Fill(distEdgeYNorm);  
		hDistXYLayNotMod_PerpNear->Fill(distEdgeXNorm, distEdgeYNorm);  
		if (layerSim==3) hDistXYLayNotMod_PerpNearTIB->Fill(distEdgeXNorm, distEdgeYNorm);  
		if (layerSim==5) hDistXYLayNotMod_PerpNearTOB->Fill(distEdgeXNorm, distEdgeYNorm);  
	      } else {
		hTrkDPerpRecSimRecLayNotMod_PerpFar->Fill(distPerpSimToRec);  
		hLayerRecLayNotMod_PerpFar->Fill(layerSim, detid.subdetId());  
		hDistXLayNotMod_PerpFar->Fill(distEdgeXNorm);  
		hDistYLayNotMod_PerpFar->Fill(distEdgeYNorm);  
		hDistXYLayNotMod_PerpFar->Fill(distEdgeXNorm, distEdgeYNorm);  
		if (layerSim==3) hDistXYLayNotMod_PerpFarTIB->Fill(distEdgeXNorm, distEdgeYNorm);  
		if (layerSim==5) hDistXYLayNotMod_PerpFarTOB->Fill(distEdgeXNorm, distEdgeYNorm);  
	      }
	    }
	  } // end else from (isMissOut)  
	} // loop over hits
	int nRecHit = -1;  
	for (uint itrk=0; itrk<trkRecHits.size(); itrk++) {
	  if (deltaR(tr->eta(), tr->phi(), trkRecHits.at(itrk).eta, trkRecHits.at(itrk).phi) > 1.0) continue; 
	  nRecHit = trkRecHits.at(itrk).rechits.size();  
	}
	cout << "Number of sim hits: " << nSimHit 
	     << "; nSimHit-nRecHit: " << nSimHit - nRecHit 
	     << "; nRecHit: " << nRecHit 
	     << "; with match by mod: " << nSimHitMatchMod
	     << "; with match by lay: " << nSimHitMatchLay
	     << "; with match by lay but not mod: " << nSimHitMatchLayNotMod  
	     << "; no match by lay: " << nSimHit - nSimHitMatchLay  
	     << "; evt: " << event.id()
	     << endl;  
	
	for (int ilay=1; ilay<=4; ilay++) { hNSimHitsVsLayerTIB->Fill(ilay, getCountSubdetLay(simHitSubdetLay, 3, ilay)); }  
	for (int ilay=1; ilay<=6; ilay++) { hNSimHitsVsLayerTOB->Fill(ilay, getCountSubdetLay(simHitSubdetLay, 5, ilay)); }  
	hNSimRecHitDiff->Fill(nSimHit - nRecHit);  

	hNSimHit->Fill(nSimHit);
	hNSimHitRec->Fill(nSimHitMatchMod);
	//	hNSimHitNRec->Fill(nSimHit - nSimHitMatchMod);
	hNSimHitMissOut->Fill(nHitMissOut); 	  
	hNRecoMissOut->Fill(nHitMissOutReco);  
	hNRecoMissMid->Fill(nHitMissMidReco);  
	hNRecoSimDiffMissOut->Fill(nHitMissOutReco - nHitMissOut);  
	hNSimHitMissOutFirst->Fill(nHitMissOutFirst); 	  
	hNSimHitMissOutFirstMissPrevSim->Fill(nHitMissOutFirstMissPrevSim); 	  
	hNSimHitMissMid       ->Fill(nSimHit - nSimHitMatchMod); 	  
	hNSimHitMissMidByLayer->Fill(nSimHit - nSimHitMatchLay);  
	hNExtraSimHits->Fill(getNExtraSimHits(simHitSubdetLay));  
	cout << "Debug:  found nextrahits = " << getNExtraSimHits(simHitSubdetLay) << endl;  
	hNPixHit->Fill(nHitPix);  
	if (nHitMissOut>0) {
	  hTrkMissOutPt ->Fill(tr->pt());  
	  hTrkMissOutEta->Fill(tr->eta());  
	  hTrkMissOutPhi->Fill(tr->phi());  
	} else {
	  hTrkNoOutEta->Fill(tr->eta());  
	}  
	

      } // loop over tracks
    } catch (Exception event) {
      // cout << "->   TrackingParticle " << setw(2) << tp.index() << " pT: " 
      // 	   <<setprecision(2)<<setw(6)<<tp->pt() 
      // 	   <<  " matched to 0  reco::Tracks" << endl;
    }
  }


  // cout << "-- Associator by chi2 --" << endl;  
  // q = associatorByChi2->associateSimToReco(trackCollectionH,TPCollectionH,&event );
  // for(SimTrackContainer::size_type i=0; i<simTC.size(); ++i){
  //   TrackingParticleRef tp (TPCollectionH,i);
  //   try{ 
  //     std::vector<std::pair<RefToBase<Track>, double> > trackV = q[tp];
  //     cout << "Sim Track " << setw(2) << tp.index() << " pT: "  << setw(6) << tp->pt() 
  // 	   <<  " matched to " << trackV.size() << " reco::Tracks" << std::endl;
  //     for (std::vector<std::pair<RefToBase<Track>,double> >::const_iterator it=trackV.begin(); it != trackV.end(); ++it) {
  // 	RefToBase<Track> tr = it->first;
  // 	double assocChi2 = it->second;
  // 	cout << "\t\treco::Track pT: " << setw(6) << tr->pt() << 
  // 	  " chi2: " << assocChi2 << endl;
  //     }
  //   } catch (Exception event) {
  //     cout << "->   TrackingParticle " << setw(2) << tp.index() << " pT: " 
  // 	   <<setprecision(2)<<setw(6)<<tp->pt() 
  // 	   <<  " matched to 0  reco::Tracks" << endl;
  //   }
  // }

}  // void testTrackAssociator::analyze(const edm::Event& event, const edm::EventSetup& setup)

bool testTrackAssociator::isMissOutHitByMod(const vector<PSimHit>& simHits, uint idxhit, const vector<unsigned int>& recHits) {
  // Return whether the given sim hit is a missing outer hit, i.e., there are no more sim hits that have rechits on the same module.  

  if (DetId(simHits.at(idxhit).detUnitId()).det() != DetId::Tracker) return false;  // return false if not a tracker hit

  for (uint ihit=idxhit; ihit<simHits.size(); ihit++) {  
    DetId detid = DetId(simHits.at(ihit).detUnitId());  
    if (detid.det() != DetId::Tracker) continue;  // skip non-tracker hits  
    bool hasMatchedRecHit;  
    if (find(recHits.begin(),recHits.end(),detid.rawId()) != recHits.end()) hasMatchedRecHit = true;
    else                                                                    hasMatchedRecHit = false;
    if (hasMatchedRecHit) return false;  // found a sim hit that has a rec hit
  }

  return true;  
}  


//bool testTrackAssociator::isMissOutHit(const vector<PSimHit>& simHits, uint idxhit, reco::Track* tr) {
bool testTrackAssociator::isMissOutHit(const vector<PSimHit>& simHits, uint idxhit, const reco::Track tr) {
  // Return whether the given sim hit is a missing outer hit, i.e., there are no more sim hits that have rechits on the same layer.  

  if (idxhit>=simHits.size()) return false;  
  for (uint ihit=idxhit; ihit<simHits.size(); ihit++) {  
    DetId detid = DetId(simHits.at(ihit).detUnitId());  
    if (detid.det() != DetId::Tracker) continue;  // skip non-tracker hits  
    for(trackingRecHit_iterator iter = tr.recHitsBegin(); 
	iter != tr.recHitsEnd(); iter++) {
      if (!(*iter)->isValid()) continue;	     
      if (isMatchedToRecHitbyLayer(simHits.at(ihit), iter)) return false;   // found a sim hit with a rec hit in the same layer
    }
  }

  return true;  

}


bool testTrackAssociator::isMissSimPrevLay(const vector<PSimHit>& simHits, uint idxhit) {
  // Return whether there is a missing sim hit in the previous layer.

  int layer = getLayerHit(simHits.at(idxhit).detUnitId());  
  int subdet = getSubDet (simHits.at(idxhit).detUnitId());  
  if (layer!=1) return (!isFoundSim(simHits, subdet, layer-1));  

  // Now treat the first layer cases.  
  if (subdet==3) return false;  // do not count missing pixel hits
  if (subdet==5) return (!isFoundSim(simHits, 3, 4));  // return whether there is a sim hit in the last layer (4) of the TIB (subdet=3)  

  cout << "WARNING [testTrackAssociator::isMissSimPrevLay]: Should not reach this point." << endl;  
  return false;  

}  

bool testTrackAssociator::isFoundSim(const vector<PSimHit>& simHits, int subdet, int layer) {
  // Return true if sim hit is found in the given subdet and layer
  for (uint ihit=0; ihit<simHits.size(); ihit++) {  
    int l = getLayerHit(simHits.at(ihit).detUnitId());  
    int s = getSubDet  (simHits.at(ihit).detUnitId());  
    if (l==layer && s==subdet) return true;
  }
  return false;
}  

bool testTrackAssociator::isMissOutHitFirst(const vector<PSimHit>& simHits, uint idxhit, const reco::Track tr) {
  // Return whether the given sim hit is the first missing outer hit, i.e., it is a missing outer hit and there are no missing outer hits before it.
  bool isMissOut     = isMissOutHit(simHits, idxhit,   tr);
  bool isMissOutPrev = (idxhit!=0 && isMissOutHit(simHits, idxhit-1, tr));  // Is the previous hit a missing outer hit?  
  return isMissOut && !isMissOutPrev;  

}


bool testTrackAssociator::isMatchedToRecHitbyLayer(PSimHit& simHit, const vector<TrackAndHits>& trkRecHits) {
  // Return true if a rec hit is found in same layer as the simHit, within DeltaR<1.0.  
  bool isMatched = false;  
  int nTrksDR = 0;  
  for (uint i=0; i<trkRecHits.size(); i++) {
    if (deltaR(getGlobalPos(simHit).eta(), 
	       getGlobalPos(simHit).phi(), 
	       trkRecHits.at(i).eta, 
	       trkRecHits.at(i).phi) > 1.0) continue; 
    if (trkRecHits.at(i).rechits.size()==0) continue;  
    // cout << "Debug:  checking isMatchedToRecHitbyLayer 000" 
    // 	 << "; eta=" << getGlobalPos(simHit).eta()
    // 	 << "; phi=" << getGlobalPos(simHit).phi()
    // 	 << endl;  
    nTrksDR++;  
    if (isMatchedToRecHitbyLayer(simHit, trkRecHits.at(i).rechits)) isMatched = true; 
  }  
  if (nTrksDR > 1) cout << "Warning:  Found more than one track with compatible eta, phi." << endl;  
  return isMatched;  
}  

bool testTrackAssociator::isMatchedToRecHitbyLayer(PSimHit& simHit, const vector<RecHitInfo>& recHits) {
  // Return true if rec hit is found in same layer as the simHit, within DeltaR<0.5.  

  // DetId detid = DetId(simHit.detUnitId());  
  // int subdetSim = detid.subdetId();  
  // int layerSim = getLayerHit(simHit.detUnitId());  
  // GlobalPoint globalPtSim = theG->idToDet(simHit.detUnitId())->surface().toGlobal(simHit.localPosition());  
  // double etaSim = globalPtSim.eta(); 
  // double phiSim = globalPtSim.phi();  
  bool foundMatch = false; 
  // cout << "Debug:  checking isMatchedToRecHitbyLayer 001" 
  // 	 << "; recHits size = " << recHits.size() << endl;  
  for (uint i=0; i<recHits.size(); i++) {
    if (isMatchedToRecHitbyLayer(simHit, recHits.at(i))) foundMatch = true;  
  }
  return foundMatch;  
}  

bool testTrackAssociator::isMatchedToRecHitbyLayer(PSimHit& simHit, const RecHitInfo& recHit) {
  DetId detid = DetId(simHit.detUnitId());  
  int subdetSim = detid.subdetId();  
  int layerSim = getLayerHit(simHit.detUnitId());  
  GlobalPoint globalPtSim = theG->idToDet(simHit.detUnitId())->surface().toGlobal(simHit.localPosition());  
  double etaSim = globalPtSim.eta(); 
  double phiSim = globalPtSim.phi();  
  if (recHit.subdet != subdetSim) return false; 
  if (recHit.layer != layerSim) return false; 
  if (deltaR(etaSim, phiSim, recHit.eta, recHit.phi) > 1.0) return false; 
  return true; 
}


bool testTrackAssociator::isMatchedToRecHitbyLayer(const PSimHit& simHit, trackingRecHit_iterator& recHit) {
  if (!(*recHit)->isValid()) return false;
  DetId detid = DetId(simHit.detUnitId());  
  int subdetSim = detid.subdetId();  
  int layerSim = getLayerHit(simHit.detUnitId());  
  DetId detidRec = DetId((*recHit)->rawId());  
  if (detidRec.subdetId() != subdetSim) return false; 
  if (getLayerHit(detidRec.rawId()) != layerSim) return false; 
  return true; 
}


bool testTrackAssociator::isMatchedToRecHitbyLayer(PSimHit& simHit, const reco::Track tr, bool debug) {
  // Return true if rec hit is found in same layer as the simHit, within DeltaR<1.0.  

  bool foundMatch = false; 
  if (debug) cout << "Debug:  Checking isMatchedToRecHitbyLayer for simHit with "
		  << "detid=" << simHit.detUnitId()
		  << "; globalPt=" << getGlobalPos(simHit)  
	       // << ", layerSim=" << layerSim
	       // << ", subdetSim=" << subdetSim  
		  << endl;  
  for(trackingRecHit_iterator iter = tr.recHitsBegin(); 
      iter != tr.recHitsEnd(); iter++) {
    if (debug) cout << "  Rec hit: detid=" << (*iter)->rawId() 
		 // << ", subdetRec = " << subdetRec
		 // << ", layerRec = " << layerRec
		    << endl;  
    return isMatchedToRecHitbyLayer(simHit, iter);  
    foundMatch = true;  
  }
  if (debug) cout << "  Found Match = " << foundMatch << endl;  
  return foundMatch;  
}  



//RecHitInfo findClosestRecHit(PSimHit& simHit, const vector<RecHitInfo>& recHits) {
int testTrackAssociator::findClosestRecHit(PSimHit& simHit, const vector<RecHitInfo>& recHits) {
  double distMin = 999999.;
  int idxMin = -1;  
  if (recHits.size()==0) {
    cout << "ERROR: no hits found" << endl;  
  }
  for (uint ihit = 0; ihit<recHits.size(); ihit++) {
    double dist = getDist(recHits.at(ihit).globalPt, getGlobalPos(simHit));  
    if (dist<distMin) {
      distMin = dist;  
      idxMin = ihit;  
    }
  }

  if (idxMin<0) {
    cout << "ERROR: idxMin = " << idxMin << endl;  
    cout << "  Printing distances:  " << flush;
    for (uint ihit = 0; ihit<recHits.size(); ihit++) {
      double dist = getDist(recHits.at(ihit).globalPt, getGlobalPos(simHit));  
      cout << dist << ", " << flush;
    }
  }

  return idxMin;  
  // if (idxMin<0) { 
  //   cout << "ERROR:  findClosestRecHit could not find rec hit." << endl;
  //   return RecHitInfo();  
  // } 

  // return recHits.at(idxMin);
  
}  
 
double testTrackAssociator::getDist(GlobalPoint pt1, GlobalPoint pt2) {
  // Return 3D distance.  
  GlobalVector vecDiff = pt1 - pt2; 
  //  double dist = sqrt(vecDiff * vecDiff);  
  double dist = vecDiff.mag();  
  return dist;  
  
}
 

double testTrackAssociator::getDistPerp(GlobalPoint pt1, GlobalPoint pt2) {
  // Return perpendicular distance between two points.  
  // Return positive number if radius (pt2) > radius (pt1); negative otherwise.  
  return pt2.perp() - pt1.perp();  

  // GlobalVector vecDiff = pt1 - pt2; 
  // double perp = fabs(vecDiff.perp()); 
  // int sign;
  // if (pt2.perp() > pt1.perp()) 
  //   sign = +1; 
  // else 
  //   sign = -1;
  // perp *= sign;  
  // return perp;  
  
}

GlobalPoint testTrackAssociator::getGlobalPos(PSimHit& simHit) {

  GlobalPoint globalPt = theG->idToDet(simHit.detUnitId())->surface().toGlobal(simHit.localPosition());  

  return globalPt;  
}

GlobalPoint testTrackAssociator::getModuleCtr(PSimHit& simHit) {

  GlobalPoint globalPt = theG->idToDet(simHit.detUnitId())->surface().toGlobal(LocalPoint(0,0,0));  

  return globalPt;  
}

double testTrackAssociator::getModuleAngle(PSimHit& simHit) {
  // get angle of y-axis of module w/r/t/ global z-axis   

  GlobalPoint modCtr  = theG->idToDet(simHit.detUnitId())->surface().toGlobal(LocalPoint(0,0,0));  
  GlobalPoint modYDir = theG->idToDet(simHit.detUnitId())->surface().toGlobal(LocalPoint(0,1,0));  

  return (modYDir - modCtr).z();  
}

int testTrackAssociator::getLayerHit(unsigned int rawId) {
  //rawId must be the raw detector id

  SiStripDetId strDetId = SiStripDetId(rawId);  

  int layer = -1;  
  if (strDetId.subDetector() == SiStripDetId::TIB) {  
    TIBDetId tibid = TIBDetId(rawId);  
    layer = tibid.layerNumber();  
  } else if (strDetId.subDetector() == SiStripDetId::TOB) {  
    TOBDetId tobid = TOBDetId(rawId);  
    layer = tobid.layerNumber();  
  } else if (strDetId.subDetector() == SiStripDetId::TID) {  
    TIDDetId tidid = TIDDetId(rawId);  
    layer = tidid.diskNumber();  
  } else if (strDetId.subDetector() == SiStripDetId::TEC) {  
    TECDetId tecid = TECDetId(rawId);  
    layer = tecid.wheelNumber();  
  } 

  return layer;  

}  

bool testTrackAssociator::getIsGlued(unsigned int rawId) {
  SiStripDetId strDetId = SiStripDetId(rawId);  
  bool isGlued = strDetId.glued();
  return isGlued;  
}


int testTrackAssociator::getSubDet(unsigned int rawId) {
  // return the subdetector ID
  DetId detid = DetId(rawId);
  return detid.subdetId();  
}


bool testTrackAssociator::isClusOnModule(unsigned int rawId) {
  for (edmNew::DetSetVector<SiStripCluster>::const_iterator itreco = clusterStrip->begin(); itreco != clusterStrip->end(); ++itreco) {
    // edmNew::DetSet<SiStripCluster> detset = *itreco;        
    // if (detset.detId() == rawId) return true;
    if ((*itreco).detId() == rawId) return true;
  }
  return false;

}  

bool testTrackAssociator::isRecHitOnModule(unsigned int rawId) {
  // Return true if there is a rec hit on the given module  
  for (edmNew::DetSetVector<SiStripRecHit2D>::const_iterator itreco = rphiRecHits->begin();            itreco != rphiRecHits->end();            ++itreco) { if ((*itreco).detId() == rawId) return true; }  
  for (edmNew::DetSetVector<SiStripRecHit2D>::const_iterator itreco = rphiRecHitsUnmatched->begin();   itreco != rphiRecHitsUnmatched->end();   ++itreco) { if ((*itreco).detId() == rawId) return true; }  
  for (edmNew::DetSetVector<SiStripRecHit2D>::const_iterator itreco = stereoRecHits->begin();          itreco != stereoRecHits->end();          ++itreco) { if ((*itreco).detId() == rawId) return true; }  
  for (edmNew::DetSetVector<SiStripRecHit2D>::const_iterator itreco = stereoRecHitsUnmatched->begin(); itreco != stereoRecHitsUnmatched->end(); ++itreco) { if ((*itreco).detId() == rawId) return true; }  
  return false;

}  

int testTrackAssociator::getCountSubdetLay(const vector<HitSubdetLay>& simhits, int subdet, int layer) {
  int nhits = 0;
  for (uint ihit=0; ihit<simhits.size(); ihit++) {
    if (simhits.at(ihit).subdet == subdet &&
	simhits.at(ihit).layer  == layer)
      nhits++;  
  }
  return nhits;
} 


int testTrackAssociator::getNExtraSimHits(const vector<HitSubdetLay>& simhits) {

  int nextrahits = 0;  
  // TIB
  nextrahits += getCountSubdetLay(simhits, 3, 1) - 2;  
  nextrahits += getCountSubdetLay(simhits, 3, 2) - 2;  
  nextrahits += getCountSubdetLay(simhits, 3, 3) - 1;  
  nextrahits += getCountSubdetLay(simhits, 3, 4) - 1;  
  // TOB
  nextrahits += getCountSubdetLay(simhits, 5, 1) - 2;  
  nextrahits += getCountSubdetLay(simhits, 5, 2) - 2;  
  nextrahits += getCountSubdetLay(simhits, 5, 3) - 1;  
  nextrahits += getCountSubdetLay(simhits, 5, 4) - 1;  
  nextrahits += getCountSubdetLay(simhits, 5, 5) - 1;  
  nextrahits += getCountSubdetLay(simhits, 5, 6) - 1;  
  
  return nextrahits;  

}


// ------------ method called once each job just before starting event loop  ------------
void
testTrackAssociator::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
testTrackAssociator::endJob()
{

  // hNSimHit->Write();
  // hNSimHitRec->Write();
  // hNSimHitNRec->Write();
  // hNSimHitMissOut->Write();  
  // hELossRec->Write();
  // hELossNRec->Write();
  // hPdgOuterHits->Write();  
  // hPdgRecHits->Write();  
  // file->Close();

}


#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(testTrackAssociator);



