# Usage:
# /afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV1/CMSSW_5_3_8_patch1/src/Demo/DemoTrackAnalyzer/test> 
# cmsRun testTrackAssociator_cfg.py 2>&1 | tee testTrackAssociator_cfg.log  

import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("Demo")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1

### Standard Configurations
process.load("Configuration.StandardSequences.Services_cff")
#process.load('Configuration/StandardSequences/GeometryIdeal_cff')
process.load('Configuration/Geometry/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
 

### Conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = 'START53_V27::All'
process.GlobalTag.globaltag = 'DESIGN53_V15::All'



#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))
process.source = cms.Source("PoolSource",
                            #    fileNames = cms.untracked.vstring(
        #                                'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV1/CMSSW_5_3_8_patch1/src/simTestMaskHits/H200ZZ4L_cfi_py_RAW2DIGI_RECO.root'
        #                                'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/dataCopyFromGrid/WJetsLNuElecNoID/WJetsLNu_lumi541512.root'
        #                                'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/simHToZZ/RECO_Ideal_DIGI_L1_DIGI2RAW_RAW2DIGI_L1Reco_RECO.root'
        #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/SingleMuPt100_cfi_py_Ideal_RECO.root'  
        #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/SingleMuPt100_cfi_py_Ideal_RECO_FEVT.root'
        ## 'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_1.root',
        ## 'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_2.root',
        ## 'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_3.root',        
        #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGunSimShift/SingleMuPt100_cfi_py_GEN_SIM_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco_RECO_Shift.root',
        #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGunShift/SingleMuPt100_cfi_py_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco_RECO.root'
        #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_20.root', 
        #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_new.root',
        #        )  
        ## fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_23.root'),  
        ## eventsToProcess = cms.untracked.VEventRange('1:18','1:38','1:57','1:68','1:116','1:133','1:136','1:149','1:154','1:185'),  

        fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_38.root'),  
        eventsToProcess = cms.untracked.VEventRange('1:15', '1:55', '1:63', '1:72', '1:88', '1:89', '1:133', '1:192'),  

    )  


dir = '/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/'
for file in os.listdir(dir):
    if file.find(".root") != -1:  # Skip over files that do not contain .root.  
        process.source.fileNames.extend(cms.untracked.vstring('file:' + dir + file))    


# Turn off duplicate checking for MC:  
process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')

process.load("RecoTracker.TrackProducer.TrackRefitters_cff")

process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")  # as recommended by https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTrackMCTruth  
process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")  


## process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
## process.load("Configuration.Geometry.GeometryIdeal_cff")
## process.load("Configuration.StandardSequences.MagneticField_cff")
## process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
## process.GlobalTag = GlobalTag(process.GlobalTag, 'START53_V27::All', '')



## process.demo = cms.EDAnalyzer("DemoTrackAnalyzer",
##                               tracks = cms.untracked.InputTag("generalTracks")
##                               #                              trajectoryInput = = cms.untracked.InputTag("TrackRefitter")
##     )
## process.demo = cms.EDProducer("DemoTrackAnalyzer",
##                               tracks = cms.untracked.InputTag("generalTracks")
##     )


## # Must put this MessageLogger near the bottom???  Need to check...  
## process.MessageLogger = cms.Service("MessageLogger",
##        destinations   = cms.untracked.vstring(
##            'detailedInfo'
##            ,'critical'
##         ),
##         detailedInfo   = cms.untracked.PSet(
##             threshold  = cms.untracked.string('DEBUG') 
##             ),

##         debugModules = cms.untracked.vstring(
##             #            'TrackRefitter', 
##             'TrackProducer'
##             )
##     )


process.TrackAssoc = cms.EDAnalyzer("testTrackAssociator",
    tracksTag    = cms.InputTag("generalTracks"), 
    #    tpTag        = cms.InputTag("garbage"),
    tpTag        = cms.InputTag("mergedtruth", "MergedTrackTruth"),
    simtracksTag = cms.InputTag("g4SimHits"),
    simvtxTag    = cms.InputTag("g4SimHits"), 
    )

#process.TrackAssociatorByHits = TrackAssociatorByHits  


## # Copied from: http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/MWalker/TrackAnalyzer/test/runTrackAnalyzer.py?revision=1.1&view=markup
process.load("Validation.RecoTrack.cuts_cff")
process.load("Validation.RecoTrack.MultiTrackValidator_cff")
process.load("SimGeneral.TrackingAnalysis.trackingParticles_cfi")
process.load("DQMServices.Components.EDMtoMEConverter_cff")

## ### validation-specific includes
process.load("SimTracker.TrackAssociation.TrackAssociatorByPosition_cfi")
process.load("SimTracker.TrackAssociation.TrackAssociatorByChi2_cfi")
process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")
process.load("Validation.RecoTrack.cuts_cff")
process.load("Validation.RecoTrack.MultiTrackValidator_cff")
process.load("SimGeneral.TrackingAnalysis.trackingParticles_cfi")
process.load("DQMServices.Components.EDMtoMEConverter_cff")

process.load("Validation.Configuration.postValidation_cff")
process.load("RecoTracker.FinalTrackSelectors.selectHighPurity_cfi")

## process.TrackAssociatorByHits.SimToRecoDenominator = cms.string('reco')
## process.TrackAssociatorByHits.Purity_SimToReco = cms.double(0.75)
## process.TrackAssociatorByHits.Cut_RecoToSim = cms.double(0.75)

## process.options = cms.untracked.PSet(
##     wantSummary = cms.untracked.bool(True)
## )
### End of copy from: http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/MWalker/TrackAnalyzer/test/runTrackAnalyzer.py?revision=1.1&view=markup




process.TFileService = cms.Service("TFileService",
    fileName = cms.string('outputDemo.root')
)


process.MessageLogger.categories     = [
             ## "CkfPattern",
             ## "TkStripMeasurementDet",
             ## "CkfTrackCandidateMakerBase",
             "TrackProducer",  
             ]
process.MessageLogger.debugModules   = [ "*" ]
process.MessageLogger.cout = cms.untracked.PSet(
    threshold = cms.untracked.string("DEBUG"),
    INFO = cms.untracked.PSet( limit = cms.untracked.int32( 0 )),
    DEBUG = cms.untracked.PSet( limit = cms.untracked.int32( 0 )),
    ## CkfPattern                 = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),
    ## TkStripMeasurementDet      = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),
    ## CkfTrackCandidateMakerBase = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),  
    TrackProducer              = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),  
)


 

#process.p = cms.Path(process.TrackRefitter + process.demo)
#process.p = cms.Path(process.TrackRefitter + process.demo + process.TrackAssoc)
#process.p = cms.Path(process.TrackRefitter + process.TrackAssociatorByHits) 
#process.p = cms.Path(process.TrackRefitter + process.trackingParticleRecoTrackAsssociation + process.TrackAssoc)
#process.p = cms.Path(process.TrackRefitter)

#process.p = cms.Path( process.selectHighPurity * process.TrackAssoc )  
#process.p = cms.Path( process.selectHighPurity )  

process.p = cms.Path(process.TrackRefitter + process.TrackAssoc)
#process.p = cms.Path( process.TrackAssoc )  

outfile = open('dumpedConfig.py','w'); print >> outfile,process.dumpPython(); outfile.close()

