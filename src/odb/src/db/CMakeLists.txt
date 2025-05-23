add_library(db
    dbBTerm.cpp 
    dbStream.cpp 
    dbBTermItr.cpp 
    dbBPinItr.cpp 
    dbBlock.cpp 
    dbBlockItr.cpp 
    dbBox.cpp 
    dbBoxItr.cpp 
    dbChip.cpp 
    dbDatabase.cpp 
    dbITerm.cpp 
    dbITermItr.cpp 
    dbInst.cpp 
    dbInstHdr.cpp 
    dbLib.cpp 
    dbMPin.cpp 
    dbMPinItr.cpp 
    dbMTerm.cpp 
    dbMaster.cpp 
    dbNet.cpp 
    dbTech.cpp  
    dbTechLayerSpacingRule.cpp 
    dbTechLayerAntennaRule.cpp 
    dbTechMinCutOrAreaRule.cpp 
    dbTechLayerItr.cpp 
    dbTechVia.cpp 
    dbTypes.cpp 
    dbVia.cpp 
    dbWire.cpp 
    dbWireCodec.cpp 
    dbTrackGrid.cpp 
    dbBlockage.cpp 
    dbObstruction.cpp 
    dbObject.cpp 
    dbInstShapeItr.cpp 
    dbTransform.cpp 
    dbITermShapeItr.cpp 
    dbSWire.cpp 
    dbSBox.cpp 
    dbSWireItr.cpp 
    dbSBoxItr.cpp 
    dbDiff.cpp 
    dbSite.cpp 
    dbCCSeg.cpp 
    dbCCSegItr.cpp 
    dbWireShapeItr.cpp 
    dbWirePathItr.cpp 
    dbTarget.cpp 
    dbTargetItr.cpp 
    dbTechNonDefaultRule.cpp 
    dbTechLayerRule.cpp 
    dbCapNode.cpp 
    dbCapNodeItr.cpp 
    dbRSeg.cpp 
    dbRSegItr.cpp 
    dbTechSameNetRule.cpp 
    dbRow.cpp
    dbFill.cpp
    dbShape.cpp 
    dbWireGraph.cpp 
    dbJournal.cpp 
    dbJournalLog.cpp 
    dbBlockCallBackObj.cpp 
    dbRegion.cpp 
    dbRegionInstItr.cpp 
    dbExtControl.cpp 
    dbNullIterator.cpp 
    dbBPin.cpp 
    dbHierInstShapeItr.cpp 
    dbHier.cpp 
    dbTechViaRule.cpp 
    dbTechViaLayerRule.cpp 
    dbTechViaGenerateRule.cpp 
    dbViaParams.cpp 
    dbNameCache.cpp 
    dbProperty.cpp 
    dbPropertyItr.cpp 
    dbUtil.cpp
    gs.cpp
    grids.cpp
    tmg_conn.cpp
    tmg_conn_g.cpp
    tmg_conn_s.cpp
    tmg_conn_w.cpp
    wOrder.cpp
    dbGroupPowerNetItr.cpp
    dbGroupGroundNetItr.cpp
    # Generator Code Begin cpp
    dbAccessPoint.cpp
    dbGCellGrid.cpp
    dbGlobalConnect.cpp
    dbGroup.cpp
    dbGuide.cpp
    dbIsolation.cpp
    dbLevelShifter.cpp
    dbLogicPort.cpp
    dbMetalWidthViaMap.cpp
    dbModInst.cpp
    dbModule.cpp
    dbNetTrack.cpp
    dbPowerDomain.cpp
    dbPowerSwitch.cpp
    dbScanChain.cpp
    dbScanInst.cpp
    dbScanPartition.cpp
    dbScanPin.cpp
    dbTechLayer.cpp
    dbTechLayerAreaRule.cpp
    dbTechLayerArraySpacingRule.cpp
    dbTechLayerCornerSpacingRule.cpp
    dbTechLayerCutClassRule.cpp
    dbTechLayerCutEnclosureRule.cpp
    dbTechLayerCutSpacingRule.cpp
    dbTechLayerCutSpacingTableDefRule.cpp
    dbTechLayerCutSpacingTableOrthRule.cpp
    dbTechLayerEolExtensionRule.cpp
    dbTechLayerEolKeepOutRule.cpp
    dbTechLayerForbiddenSpacingRule.cpp
    dbTechLayerKeepOutZoneRule.cpp
    dbTechLayerMinCutRule.cpp
    dbTechLayerMinStepRule.cpp
    dbTechLayerSpacingEolRule.cpp
    dbTechLayerSpacingTablePrlRule.cpp
    dbTechLayerWidthTableRule.cpp
    dbTechLayerWrongDirSpacingRule.cpp
    dbGroupInstItr.cpp
    dbGroupItr.cpp
    dbGroupModInstItr.cpp
    dbGuideItr.cpp
    dbModuleInstItr.cpp
    dbModuleModInstItr.cpp
    dbNetTrackItr.cpp
    dbRegionGroupItr.cpp
    # Generator Code End cpp
)

target_include_directories(db
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${TCL_INCLUDE_PATH}
)

set_target_properties(db
  PROPERTIES
    # python requirement
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(db
    PUBLIC
        zutil
        utl_lib
        ${TCL_LIBRARY}
)

messages(
  TARGET db
  OUTPUT_DIR ../..
)
