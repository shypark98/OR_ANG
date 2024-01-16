#include <tcl.h>
#include "sta/StaMain.hh"
#include "ord/OpenRoad.hh"
#include "artnetgen/MakeArtNetGen.h"
#include "artnetgen/artNetGen.h"


namespace sta {
extern const char *artnetgen_tcl_inits[];
}

extern "C" {
extern int Artnetgen_Init(Tcl_Interp* interp);
}

namespace ord {

artnetgen::ArtNetGen * 
makeArtNetGen() 
{
  return new artnetgen::ArtNetGen; 
}

void 
initArtNetGen(OpenRoad *openroad) 
{
  Tcl_Interp* tcl_interp = openroad->tclInterp();
  Artnetgen_Init(tcl_interp);
  sta::evalTclInit(tcl_interp, sta::artnetgen_tcl_inits);
  openroad->getArtNetGen()->init(openroad->getDb(),
                                 openroad->getSta(),
                                 openroad->getLogger());
}

void
deleteArtNetGen(artnetgen::ArtNetGen *artnetgen)
{
  delete artnetgen;
}


}
