%module ArtNetGen 

%{
#include "ord/OpenRoad.hh"
#include "artnetgen/artNetGen.h"

namespace ord {

artnetgen::ArtNetGen*
getArtNetGen(); 

odb::dbDatabase*
getDb();

sta::dbSta*
getSta();

utl::Logger*
getLogger();
}

using ord::getArtNetGen;
using ord::getDb;
using ord::getSta;
using ord::getLogger;
using artnetgen::ArtNetGen;

%}

%inline %{

void 
artnetgen_init_cmd()
{
  ArtNetGen* artNetGen= getArtNetGen();
  artNetGen->setDb(getDb());
  artNetGen->setSta(getSta());
  artNetGen->setLogger(getLogger());
  artNetGen->init();
}

void
artnetgen_print_masters_cmd()
{
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->printMasters();
}


void
artnetgen_set_dont_use_cmd(const char* macro_name) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->setDontUse(macro_name);
}




void
artnetgen_set_avg_gate_delay_cmd(double avg_gate_delay) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->setAvgGateDelay(avg_gate_delay);
}



void
artnetgen_set_topmodule_cmd(const char* top_module) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->setTopModule(top_module);
}

void
artnetgen_write_verilog_cmd(const char* file_name) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->writeVerilog(file_name);
}

void
artnetgen_write_sdc_cmd(const char* file_name) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->writeSdc(file_name);
}


void
artnetgen_write_spec_cmd(const char* file_name) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->writeSpec(file_name);
}

void
artnetgen_create_spec_cmd(int num_insts, int num_primary_in, int num_primary_out, float comb_ratio, float avg_net_degree, float avg_bbox, 
        float avg_topo_order, const char* cell_list_file, const char* out_file_name) {
    ArtNetGen* artNetGen = getArtNetGen();
    artNetGen->createSpec(num_insts, num_primary_in, num_primary_out, comb_ratio, avg_net_degree, avg_bbox, avg_topo_order, hierarchy_flag, cell_list_file, out_file_name);
}


void
artnetgen_set_verbose_cmd(int verbose)
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->setVerbose(verbose);
}

void
artnetgen_set_spl_file_cmd(const char* file)
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->setSPLFile(file);
}

void
artnetgen_set_log_file_cmd(const char* file)
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->setLogFile(file);
}

void
artnetgen_set_spec_file_cmd(const char* file)
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->setSpecFile(file);
}

void
artnetgen_set_hierarchy_flag_cmd(bool hierarchy_flag)
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->setHierFlag(hierarchy_flag);
}

void
artnetgen_set_out_file_cmd(const char* file)
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->setOutFile(file);
}

void
artnetgen_run_cmd() 
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->run(); 
}

void
artnetgen_clear_cmd() 
{
  ArtNetGen* artNetGen = getArtNetGen();
  artNetGen->clear();
}

%}
