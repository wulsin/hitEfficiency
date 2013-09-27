# Usage:
# /afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV1/CMSSW_5_3_8_patch1/src/Demo/DemoTrackAnalyzer/test> 
# cmsRun testTrkReco_cfg.py 2>&1 | tee testTrkReco_cfg.log  

import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("Demo")
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1

### Standard Configurations
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration/Geometry/GeometryIdeal_cff')
process.load('Configuration/StandardSequences/Reconstruction_cff')
process.load('Configuration/StandardSequences/MagneticField_AutoFromDBCurrent_cff')
 

### Conditions
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = 'DESIGN53_V15::All'
process.GlobalTag.globaltag = 'START53_V26::All'  



#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10000))


process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring()  
    ##     #                                'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV1/CMSSW_5_3_8_patch1/src/simTestMaskHits/H200ZZ4L_cfi_py_RAW2DIGI_RECO.root'
    ##     #                                'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/dataCopyFromGrid/WJetsLNuElecNoID/WJetsLNu_lumi541512.root'
    ##     #                                'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/simHToZZ/RECO_Ideal_DIGI_L1_DIGI2RAW_RAW2DIGI_L1Reco_RECO.root'
    ##     #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/SingleMuPt100_cfi_py_Ideal_RECO.root'  
    ##     #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/SingleMuPt100_cfi_py_Ideal_RECO_FEVT.root'
    ##     ## 'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_1.root',
    ##     ## 'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_2.root',
    ##     ## 'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_3.root',        
    ##     #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGunSimShift/SingleMuPt100_cfi_py_GEN_SIM_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco_RECO_Shift.root',
    ##     #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGunShift/SingleMuPt100_cfi_py_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco_RECO.root'
    ##     #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_20.root', 
    ##     #        'file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_new.root',
    ##     ),       
    ## fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_23.root'),  
    ## eventsToProcess = cms.untracked.VEventRange('1:18','1:38','1:57','1:68','1:116','1:133','1:136','1:149','1:154','1:185'),  

##     fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_38.root'),  
## #    eventsToProcess = cms.untracked.VEventRange('1:15', '1:55', '1:63', '1:72', '1:88', '1:89', '1:133', '1:192'),
## eventsToProcess = cms.untracked.VEventRange('1:63'),  

## fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_33.root'),  
##     ## eventsToProcess = cms.untracked.VEventRange('1:35', '1:49', '1:115', '1:135', '1:168', '1:191'),  
##     eventsToProcess = cms.untracked.VEventRange('1:115'),  

#fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_30.root'),  
##     ## eventsToProcess = cms.untracked.VEventRange('1:2', '1:7', '1:16', '1:17', '1:54', '1:65', '1:98', '1:123' ),  
##     #    eventsToProcess = cms.untracked.VEventRange('1:54', '1:65', '1:98', '1:123' ),  
# eventsToProcess = cms.untracked.VEventRange('1:2' ),  

    ## fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_11.root'),  
    ## eventsToProcess = cms.untracked.VEventRange('1:16', '1:45', '1:112', '1:113', '1:129', '1:139', '1:154', '1:157'),  

    #    eventsToProcess = cms.untracked.VEventRange('1:185')  
    #    firstEvent = cms.untracked.uint32(4), 

    #    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_1.root'),  

    ## fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV6MuPartGun/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT_new/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_0.root'),

    ## fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/SingleMuPt100_cfi_py_Ideal_RECO_FEVT_33.root'),  
    ## eventsToProcess = cms.untracked.VEventRange('1:48'),  

    )  


#dir = '/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV2/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/'
#dir = '/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV6MuPartGun/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT/'  
#dir = '/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV6MuPartGun/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT_StartV26/'
#dir = '/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV6MuPartGun/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT_StartV26_Pt20/'
dir = '/afs/cern.ch/user/w/wulsin/workspace/public/disappTrk/simStudyV6MuPartGun/CMSSW_5_3_3/src/muPartGun/batch/outputFEVT_StartV26_PtRange/'  
for file in os.listdir(dir):
    if file.find(".root") != -1:  # Skip over files that do not contain .root.  
        process.source.fileNames.extend(cms.untracked.vstring('file:' + dir + file))    


# Turn off duplicate checking for MC:  
process.source.duplicateCheckMode = cms.untracked.string('noDuplicateCheck')


#process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.categories     = [
    ## "CkfPattern",
    ## "TkStripMeasurementDet",
    ## "CkfTrackCandidateMakerBase",
    ## "TrackFitters", 
             ]
process.MessageLogger.debugModules   = [ "*" ]
process.MessageLogger.cout = cms.untracked.PSet(
    threshold = cms.untracked.string("DEBUG"),
    INFO = cms.untracked.PSet( limit = cms.untracked.int32( 0 )),
    DEBUG = cms.untracked.PSet( limit = cms.untracked.int32( 0 )),
    ## CkfPattern = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),
    ## TkStripMeasurementDet = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),
    ## CkfTrackCandidateMakerBase = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),
    ## TrackFitters = cms.untracked.PSet( limit = cms.untracked.int32( -1 )),      
)


## # Must put this MessageLogger near the bottom???  Need to check...  
## process.MessageLogger = cms.Service("MessageLogger",
##        destinations   = cms.untracked.vstring(
##            'detailedInfo'
##            #           ,'critical'
##         ),
##         detailedInfo   = cms.untracked.PSet(
##             threshold  = cms.untracked.string('DEBUG') 
##             ),
##         debugModules = cms.untracked.vstring(
##             "CkfPattern",
##             "TkStripMeasurementDet",
##             "CkfTrackCandidateMakerBase",
##             )
##     )


## #  WARNING:  Do not do this normally!!!  
## process.TTRHBuilderAngleAndTemplate.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.myTTRHBuilderWithoutAngle.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.myTTRHBuilderWithoutAngle4MixedPairs.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.myTTRHBuilderWithoutAngle4MixedTriplets.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.myTTRHBuilderWithoutAngle4PixelPairs.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.myTTRHBuilderWithoutAngle4PixelTriplets.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.myTTRHBuilderWithoutAngle4PixelTriplets.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.ttrhbwor.ComputeCoarseLocalPositionFromDisk = cms.bool(True)
## process.ttrhbwr.ComputeCoarseLocalPositionFromDisk = cms.bool(True)


# ADD stuff for the Track Association analyzer module
process.load("SimTracker.TrackAssociation.trackingParticleRecoTrackAsssociation_cfi")  # as recommended by https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTrackMCTruth  
process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")  

## # Copied from: http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/MWalker/TrackAnalyzer/test/runTrackAnalyzer.py?revision=1.1&view=markup
process.load("Validation.RecoTrack.cuts_cff")
process.load("Validation.RecoTrack.MultiTrackValidator_cff")
process.load("SimGeneral.TrackingAnalysis.trackingParticles_cfi")
process.load("DQMServices.Components.EDMtoMEConverter_cff")

## ### validation-specific includes
process.load("SimTracker.TrackAssociation.TrackAssociatorByPosition_cfi")
process.load("SimTracker.TrackAssociation.TrackAssociatorByChi2_cfi")
process.load("SimTracker.TrackAssociation.TrackAssociatorByHits_cfi")
process.load("Validation.Configuration.postValidation_cff")
process.load("RecoTracker.FinalTrackSelectors.selectHighPurity_cfi")

process.TrackAssoc = cms.EDAnalyzer("testTrackAssociator",
                                    #    tracksTag    = cms.InputTag("TrackRefitter"), 
    tracksTag    = cms.InputTag("generalTracks"), 
    #    tpTag        = cms.InputTag("garbage"),
    tpTag        = cms.InputTag("mergedtruth", "MergedTrackTruth"),
    simtracksTag = cms.InputTag("g4SimHits"),
    simvtxTag    = cms.InputTag("g4SimHits"), 
    )
process.TFileService = cms.Service("TFileService",
    fileName = cms.string('outputTrkReco.root')
)
# Finish adding stuff for the Track Association analyzer module




#process.newTrackingSequence = cms.Sequence(process.siPixelRecHits * process.trackingGlobalReco)  
#process.newTrackingSequence = cms.Sequence(process.siPixelRecHits * process.siStripMatchedRecHits * process.trackingGlobalReco)  
process.newTrackingSequence = cms.Sequence(process.siPixelRecHits * process.siStripMatchedRecHits * process.trackingGlobalReco * process.TrackAssoc)  
process.newTrackingPath     = cms.Path(process.newTrackingSequence)  


#process.p = cms.Path(process.TrackRefitter + process.TrackAssoc)

outfile = open('dumpedConfig_testTrkReco_cfg.py','w'); print >> outfile,process.dumpPython(); outfile.close()

