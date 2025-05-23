///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <cstring>

#include "db.h"
#include "dbCore.h"
#include "dbDatabase.h"
#include "dbProperty.h"
#include "dbTable.h"
#include "utl/Logger.h"

namespace odb {

uint dbObject::getId() const
{
  return getImpl()->getOID();
}

inline uint getOid(const char*& name)
{
  char* end;
  uint id = strtoul(name, &end, 10);
  ZASSERT(name != end);
  name = end;
  return id;
}

dbObjectType dbObject::getObjectType() const
{
  return getImpl()->getType();
}

//
// NOTE: The name declaration order MUST match the enumeration order.
//
static const char* name_tbl[] = {"dbDatabase",

                                 // Design Objects
                                 "dbChip",
                                 "dbBlock",
                                 "dbInstHdr",
                                 "dbInst",
                                 "dbNet",
                                 "dbBTerm",
                                 "dbITerm",
                                 "dbBox",
                                 "dbVia",
                                 "dbTrackGrid",
                                 "dbObstruction",
                                 "dbBlockage",
                                 "dbWire",
                                 "dbSWire",
                                 "dbSBox",
                                 "dbCapNode",
                                 "dbRSeg",
                                 "dbCCSeg",
                                 "dbRow",
                                 "dbFill",
                                 "dbRegion",
                                 "dbHier",
                                 "dbBPin",
                                 // Generator Code Begin ObjectNames
                                 "dbAccessPoint",
                                 "dbGCellGrid",
                                 "dbGlobalConnect",
                                 "dbGroup",
                                 "dbGuide",
                                 "dbIsolation",
                                 "dbLevelShifter",
                                 "dbLogicPort",
                                 "dbMetalWidthViaMap",
                                 "dbModInst",
                                 "dbModule",
                                 "dbNetTrack",
                                 "dbPowerDomain",
                                 "dbPowerSwitch",
                                 "dbScanChain",
                                 "dbScanInst",
                                 "dbScanPartition",
                                 "dbScanPin",
                                 "dbTechLayer",
                                 "dbTechLayerAreaRule",
                                 "dbTechLayerArraySpacingRule",
                                 "dbTechLayerCornerSpacingRule",
                                 "dbTechLayerCutClassRule",
                                 "dbTechLayerCutEnclosureRule",
                                 "dbTechLayerCutSpacingRule",
                                 "dbTechLayerCutSpacingTableDefRule",
                                 "dbTechLayerCutSpacingTableOrthRule",
                                 "dbTechLayerEolExtensionRule",
                                 "dbTechLayerEolKeepOutRule",
                                 "dbTechLayerForbiddenSpacingRule",
                                 "dbTechLayerKeepOutZoneRule",
                                 "dbTechLayerMinCutRule",
                                 "dbTechLayerMinStepRule",
                                 "dbTechLayerSpacingEolRule",
                                 "dbTechLayerSpacingTablePrlRule",
                                 "dbTechLayerWidthTableRule",
                                 "dbTechLayerWrongDirSpacingRule",
                                 // Generator Code End ObjectNames

                                 // Lib Objects
                                 "dbLib",
                                 "dbSite",
                                 "dbMaster",
                                 "dbMPin",
                                 "dbMTerm",
                                 "dbTarget",
                                 "dbTechAntennaPinModel",

                                 // Tech Objects
                                 "dbTech",
                                 "dbTechVia",
                                 "dbTechNonDefaultRule",
                                 "dbTechLayerRule",
                                 "dbTechSameNetRule",
                                 "dbTechLayerSpacingRule",
                                 "dbTechMinCutRule",
                                 "dbTechMinEncRule",
                                 "dbTechV55InfluenceEntry",
                                 "dbTechLayerAntennaRule",
                                 "dbTechViaRule",
                                 "dbTechViaGenerateRule",
                                 "dbTechViaLayerRule",

                                 "dbProperty",
                                 "dbName"};

const char* dbObject::getObjName() const
{
  return name_tbl[getImpl()->getType()];
}

const char* dbObject::getObjName(dbObjectType type)
{
  return name_tbl[type];
}

}  // namespace odb
