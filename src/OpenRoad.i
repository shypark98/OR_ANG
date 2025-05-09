/////////////////////////////////////////////////////////////////////////////
//
// BSD 3-Clause License
//
// Copyright (c) 2019, James Cherry, Parallax Software, Inc.
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
//
///////////////////////////////////////////////////////////////////////////////

%{

#include "odb/db.h"
#include "odb/lefin.h"
#include "odb/defin.h"
#include "odb/defout.h"
#include "sta/Report.hh"
#include "sta/Network.hh"
#include "db_sta/dbSta.hh"
#include "db_sta/dbNetwork.hh"
#include "db_sta/dbReadVerilog.hh"
#include "ord/Version.hh"
#include "utl/Logger.h"
#include "ord/OpenRoad.hh"

#include <vector>

////////////////////////////////////////////////////////////////
//
// C++ helper functions used by the interface functions.
// These are not visible in the TCL API.
//
////////////////////////////////////////////////////////////////

namespace ord {

using sta::dbSta;
using sta::dbNetwork;
using rsz::Resizer;

using odb::dbDatabase;

OpenRoad *
getOpenRoad()
{
  return OpenRoad::openRoad();
}

utl::Logger *
getLogger()
{
  return OpenRoad::openRoad()->getLogger();
}

dbDatabase *
getDb()
{
  return getOpenRoad()->getDb();
}

// Copied from StaTcl.i because of ordering issues.
class CmdErrorNetworkNotLinked : public sta::Exception
{
public:
  virtual const char *what() const throw()
  { return "Error: no network has been linked."; }
};

void
ensureLinked()
{
  OpenRoad *openroad = getOpenRoad();
  dbNetwork *network = openroad->getDbNetwork();
  if (!network->isLinked())
    throw CmdErrorNetworkNotLinked();
}

dbNetwork *
getDbNetwork()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getDbNetwork();
}

dbSta *
getSta()
{
  return getOpenRoad()->getSta();
}

Resizer *
getResizer()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getResizer();
}

rmp::Restructure *
getRestructure()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getRestructure();
}

cts::TritonCTS *
getTritonCts()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getTritonCts();
}

mpl::MacroPlacer *
getMacroPlacer()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getMacroPlacer();
}

mpl2::MacroPlacer2 *
getMacroPlacer2()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getMacroPlacer2();
}

gpl::Replace*
getReplace()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getReplace();
}

rcx::Ext *
getOpenRCX()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getOpenRCX();
}

triton_route::TritonRoute *
getTritonRoute()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getTritonRoute();
}

psm::PDNSim*
getPDNSim()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getPDNSim();
}

grt::GlobalRouter*
getGlobalRouter()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getGlobalRouter();
}

tap::Tapcell* 
getTapcell()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getTapcell();
}

ppl::IOPlacer*
getIOPlacer()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getIOPlacer();
}

par::PartitionMgr*
getPartitionMgr()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getPartitionMgr();
}

pdn::PdnGen*
getPdnGen()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getPdnGen();
}

pad::ICeWall*
getICeWall()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getICeWall();
}

stt::SteinerTreeBuilder*
getSteinerTreeBuilder()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getSteinerTreeBuilder();
}

artnetgen::ArtNetGen*
getArtNetGen()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->getArtNetGen();
}

} // namespace ord

namespace sta {
std::vector<LibertyCell*> *
tclListSeqLibertyCell(Tcl_Obj *const source,
		      Tcl_Interp *interp);
} // namespace sta


using std::vector;

using sta::LibertyCell;

using ord::OpenRoad;
using ord::getOpenRoad;
using ord::getLogger;
using ord::getDb;
using ord::ensureLinked;

using odb::dbDatabase;
using odb::dbBlock;
using odb::dbTechLayer;
using odb::dbTrackGrid;
using odb::dbTech;

%}

////////////////////////////////////////////////////////////////
//
// C++ functions visible as SWIG functions.
//
////////////////////////////////////////////////////////////////

#ifdef SWIGTCL
%include "Exception.i"

%typemap(in) vector<LibertyCell*> * {
  $1 = sta::tclListSeqLibertyCell($input, interp);
}

%typemap(in) vector<const char*> * {
  int argc;
  Tcl_Obj **argv;

  if (Tcl_ListObjGetElements(interp, $input, &argc, &argv) == TCL_OK) {
    vector<const char*>* seq = new vector<const char*>;
    for (int i = 0; i < argc; i++) {
      int length;
      const char* str = Tcl_GetStringFromObj(argv[i], &length);
      seq->push_back(str);
    }
    $1 = seq;
  }
  else {
    $1 = nullptr;
  }
}

%typemap(in) utl::ToolId {
  int length;
  const char *arg = Tcl_GetStringFromObj($input, &length);
  $1 = utl::Logger::findToolId(arg);
}
#endif

%inline %{

const char *
openroad_version()
{
  return OPENROAD_VERSION;
}

const char *
openroad_git_describe()
{
  return OPENROAD_GIT_DESCRIBE;
}

void
read_lef_cmd(const char *filename,
	     const char *lib_name,
	     const char *tech_name,
	     bool make_tech,
	     bool make_library)
{
  OpenRoad *ord = getOpenRoad();
  ord->readLef(filename, lib_name, tech_name, make_tech, make_library);
}

void
read_def_cmd(const char *filename,
             const char* tech_name,
             bool continue_on_errors,
             bool floorplan_init,
             bool incremental,
             bool child)
{
  OpenRoad *ord = getOpenRoad();
  auto* db = ord->getDb();
  dbTech* tech;
  if (tech_name[0] != '\0') {
    tech = db->findTech(tech_name);
  } else {
    tech = db->getTech();
  }
  if (!tech) {
    auto logger = getLogger();
    logger->error(utl::ORD, 52, "Technology {} not found", tech_name);
  }
  ord->readDef(filename, tech, continue_on_errors,
               floorplan_init, incremental, child);
}

void
write_def_cmd(const char *filename,
	      const char *version)
{
  OpenRoad *ord = getOpenRoad();
  ord->writeDef(filename, version);
}

void
write_lef_cmd(const char *filename)
{
  OpenRoad *ord = getOpenRoad();
  ord->writeLef(filename);
}

void
write_abstract_lef_cmd(const char *filename,
                       int bloat_factor,
                       bool bloat_occupied_layers)
{
  OpenRoad *ord = getOpenRoad();
  ord->writeAbstractLef(filename, bloat_factor, bloat_occupied_layers);
}


void 
write_cdl_cmd(const char *outFilename,
              vector<const char*>* mastersFilenames,
              bool includeFillers)
{
  OpenRoad *ord = getOpenRoad();
  ord->writeCdl(outFilename, *mastersFilenames, includeFillers);
}

void
read_db_cmd(const char *filename)
{
  OpenRoad *ord = getOpenRoad();
  ord->readDb(filename);
}

void
write_db_cmd(const char *filename)
{
  OpenRoad *ord = getOpenRoad();
  ord->writeDb(filename);
}

void
diff_dbs(const char *filename1, const char *filename2, const char* diffs)
{
  OpenRoad *ord = getOpenRoad();
  ord->diffDbs(filename1, filename2, diffs);
}

void
read_verilog_cmd(const char *filename)
{
  OpenRoad *ord = getOpenRoad();
  ord->readVerilog(filename);
}

void
link_design_db_cmd(const char *design_name)
{
  OpenRoad *ord = getOpenRoad();
  ord->linkDesign(design_name);
}

void
ensure_linked()
{
  return ensureLinked();
}

void
set_debug_level(const char* tool_name,
                const char* group,
                int level)
{
  using namespace ord;
  auto logger = getLogger();

  auto id = utl::Logger::findToolId(tool_name);
  if (id == utl::UKN) {
    logger->error(utl::ORD, 15, "Unknown tool name {}", tool_name);
  }
  logger->setDebugLevel(id, group, level);
}

////////////////////////////////////////////////////////////////

odb::dbDatabase *
get_db()
{
  return getDb();
}

odb::dbTech *
get_db_tech()
{
  return getDb()->getTech();
}

bool
db_has_tech()
{
  return !getDb()->getTechs().empty();
}

odb::dbBlock *
get_db_block()
{
  odb::dbDatabase *db = getDb();
  if (db) {
    odb::dbChip *chip = db->getChip();
    if (chip)
      return chip->getBlock();
  }
  return nullptr;
}

odb::Rect
get_db_core()
{
  OpenRoad *ord = getOpenRoad();
  return ord->getCore();
}

double
dbu_to_microns(int dbu)
{
  auto tech = getDb()->getTech();
  if (!tech) {
    auto logger = getLogger();
    logger->error(utl::ORD, 49, "No tech is loaded");
  }
  return static_cast<double>(dbu) / tech->getDbUnitsPerMicron();
}

int
microns_to_dbu(double microns)
{
  auto tech = getDb()->getTech();
  if (!tech) {
    auto logger = getLogger();
    logger->error(utl::ORD, 50, "No tech is loaded");
  }
  return std::round(microns * tech->getDbUnitsPerMicron());
}

// Common check for placement tools.
bool
db_has_rows()
{
  dbDatabase *db = OpenRoad::openRoad()->getDb();
  if (!db->getChip() || !db->getChip()->getBlock()) {
    return false;
  }

  for (odb::dbRow* row : db->getChip()->getBlock()->getRows()) {
    if (row->getSite()->getClass() != odb::dbSiteClass::PAD) {
      return true;
    }
  }

  return false;
}

bool
db_layer_has_tracks(odb::dbTechLayer* layer, bool hor)
{
  if (!layer) {
    return false;
  }
    
  dbDatabase *db = OpenRoad::openRoad()->getDb();
  dbBlock *block = db->getChip()->getBlock();

  dbTrackGrid *trackGrid = block->findTrackGrid(layer);
  if (!trackGrid) {
    return false;
  }

  if (hor) {
    return trackGrid->getNumGridPatternsY() > 0; 
  } else {
    return trackGrid->getNumGridPatternsX() > 0; 
  }
}

bool
db_layer_has_hor_tracks(odb::dbTechLayer* layer)
{
  return db_layer_has_tracks(layer, true);
}

bool
db_layer_has_ver_tracks(odb::dbTechLayer* layer)
{
  return db_layer_has_tracks(layer, false);
}

sta::Sta *
get_sta()
{
  return sta::Sta::sta();
}

// For some bizzare reason this fails without the namespace qualifier for Sta.
void
set_cmd_sta(sta::Sta *sta)
{
  sta::Sta::setSta(sta);
}

bool
units_initialized()
{
  OpenRoad *openroad = getOpenRoad();
  return openroad->unitsInitialized();
}

namespace ord {

void
set_thread_count(int threads)
{
  OpenRoad *ord = getOpenRoad();
  ord->setThreadCount(threads);
}

void
set_thread_count(const char* threads)
{
  OpenRoad *ord = getOpenRoad();
  ord->setThreadCount(threads);
}

int
thread_count()
{
  OpenRoad *ord = getOpenRoad();
  return ord->getThreadCount();
}

void design_created()
{
  OpenRoad *ord = getOpenRoad();
  ord->designCreated();
}

}

%} // inline
