#include "artnetgen/artNetGen.h"
#include "odb/db.h"
//#include "opendb/dbSet.h"

#include "sta/Graph.hh"
#include "sta/Sta.hh"
#include "sta/Network.hh"
#include "sta/Liberty.hh"
#include "sta/Sdc.hh"
#include "sta/PortDirection.hh"
#include "sta/Corner.hh"
#include "sta/PathExpanded.hh"
#include "sta/PathEnd.hh"
#include "sta/PathRef.hh"
#include "sta/Search.hh"
#include "sta/Bfs.hh"
#include "utl/Logger.h"

#include "db_sta/dbSta.hh"
#include "db_sta/dbNetwork.hh"
//#include "logger.h"
#include "netlist.h"
#include "node.h"


#include <fstream>
#include <sstream>
#include <math.h>
#include <unordered_map>


// for easier coding with boost

// save point and dbInst* pointer 

using std::max;
using std::min;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::make_pair;
using std::pair;
using std::set;
using namespace odb;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::sqrt;
using std::unordered_map;
using std::to_string;



namespace artnetgen {

using utl::ANG;

unordered_map<string,int> getPortInfo(dbMaster* master) {
    dbSet<dbMTerm> terms = master->getMTerms();
    
    unordered_map<string,int> ioCount;
    ioCount["input_signal"] = 0;
    ioCount["input_clock"] = 0;
    ioCount["input_scan"] = 0;
    ioCount["input_reset"] = 0;
    ioCount["output_signal"] = 0;
    ioCount["output_clock"] = 0;
    ioCount["output_scan"] = 0;
    ioCount["output_reset"] = 0;
    for(auto it = terms.begin(); it != terms.end(); it++) {
        string termName = it->getName();
        if(termName == "si" || termName == "ssb") {
            ioCount["input_scan"]++;
        } else {
            dbSigType sigType = it->getSigType();
            dbIoType ioType = it->getIoType();
            if(sigType.getValue() == dbSigType::SIGNAL) {
                if(ioType.getValue() == dbIoType::INPUT)
                    ioCount["input_signal"]++;
                else if(ioType.getValue() == dbIoType::OUTPUT)
                    ioCount["output_signal"]++;
            } else if(sigType.getValue() == dbSigType::CLOCK) {
                if(ioType.getValue() == dbIoType::INPUT)
                    ioCount["input_clock"]++;
                else if(ioType.getValue() == dbIoType::OUTPUT)
                    ioCount["output_clock"]++;
            } else if(sigType.getValue() == dbSigType::RESET) {
                if(ioType.getValue() == dbIoType::INPUT)
                    ioCount["input_reset"]++;
                else if(ioType.getValue() == dbIoType::OUTPUT)
                    ioCount["output_reset"]++;
            } else if(sigType.getValue() == dbSigType::SCAN) {
                if(ioType.getValue() == dbIoType::INPUT)
                    ioCount["input_scan"]++;
                else if(ioType.getValue() == dbIoType::OUTPUT)
                    ioCount["output_scan"]++;
            } else {

            }
        }
    }

    return ioCount;
}


MasterInfo::MasterInfo( odb::dbMaster* master, double ratio) : 
  master_(master),  ratio_(ratio) {}

MasterInfo::~MasterInfo() {
  master_ = nullptr;
  ratio_ = 0; 
}

void 
MasterInfo::print() {
    std::cout << "  " << master_->getConstName();
    unordered_map<string, int> ioCount = getPortInfo(master_);
    std::cout   << "[in " << ioCount["input_signal"] 
                << " out " << ioCount["output_signal"] 
                << " clk " << ioCount["input_clock"] << "]" << std::endl;
}

ArtNetGen::ArtNetGen() : 
    db_(nullptr), sta_(nullptr), logger_(nullptr),
    instanceCnt_(5000),
    inputPinCnt_(20),
    outputPinCnt_(20),
    avgTopoOrder_(10.0), 
    avgGateDelay_(0.1),
    combRatio_(0.9),
    verbose_(1) {
    specFile_ = "";
    SPLFile_ = ""; // spec log file
    logFile_ = "";
    hierarchy_flag_ = false;
    outFile_ = "";
    topModule_ = "artnet";    
} 

ArtNetGen::~ArtNetGen() {
  clear(); 
}

void
ArtNetGen::clear() {
  db_ = nullptr; 
  sta_ = nullptr;
  instanceCnt_ = 5000;
  inputPinCnt_ = 20;
  outputPinCnt_ = 20;
  avgTopoOrder_ = 10.0;
  avgGateDelay_ = 0.1;
  combRatio_ = 0.9;
  verbose_ = 1;
  specFile_ = "";
  SPLFile_ = "";
  logFile_ = "";
  hierarchy_flag_ = false;
  outFile_ = "";
  topModule_ = "artnet";


  masters_.clear();
  masters_.shrink_to_fit();

  fiDist_.clear();
  foDist_.clear();
  bboxDist_.clear();
  edgeDist_.clear();
}

void 
ArtNetGen::init() {  
  /*
  if( !log_ ) {
    log_ = std::make_shared<Logger>("ANG", verbose_);
  }
  */
  if (specFile_ != "" ) {
      readSpec();
  }
  else {
      // spec file must be specified
      logger_->error(ANG, 200, "Spec file must be specified"); //error
  }
  
  checkMasters();
  printAllVars();
}

void
ArtNetGen::setDontUse(const char* macroName) {
    vector<MasterInfo>::iterator it= masters_.begin();
    while(it!=masters_.end()) {
        if(it->master()->getName() == macroName)
            break;
        it++;
    }

    if(it == masters_.end()) {
        cout << "[WARN] " << macroName << " does not exist in the list" << endl;
    } else {
        masters_.erase(it);
        cout << "[INFO] " << macroName << " has been removed from the list" << endl;
    }
}


void
ArtNetGen::writeVerilog(const char* fileName) {
    
    logger_->info(ANG, 100, "Write Verilog");

    ofstream verilog(fileName);

    if(!verilog.good()) {
        cout << "Cannot open " << fileName << " file" << endl;
        return;
    }


    verilog << "module " << topModule_ << " (" << endl;

    // 1. I/O
    vector<Node*> primIns = artnet_->getPrimaryInputs();
    vector<Node*> primOuts = artnet_->getPrimaryOutputs();
    vector<Net*> nets = artnet_->getNetlist();
    vector<Node*> nodes = artnet_->getNodes();

    for(int i=0; i < primIns.size(); i++) {
        Node* inNode = primIns[i];
        verilog << "\tinput " << inNode->getName() << "," << endl;
    }

    for(int i=0; i < primOuts.size(); i++) {
        Node* outNode = primOuts[i];
        verilog << "\toutput " << outNode->getName();

        if(i == primOuts.size()-1) {
            verilog << endl << ");" << endl;
        } else {
            verilog << "," << endl;
        }
    }

    verilog << endl << endl;

    unordered_map<Node*, unordered_map<string,string>> term2net;
    
    // 2. Wires
    for(int i=0; i < nets.size(); i++) {
        Net* net = nets[i];
        verilog << "wire " << net->getName() << ";" << endl; 

        /*
        vector<pair<Node*, string>> terms = net->getTerms();
        Node* srcNode = terms.begin()->first;
        switch(srcNode->getType()) {
            case NodeType::Combinational:
                verilog << "wire " << net->getName() << ";" << endl; break;
            case NodeType::Sequential:
                verilog << "wire " << net->getName() << ";" << endl; break;
            default:
                break;
        }
        */
        for(auto p : net->getTerms()) {
            Node* node = p.first;
            string termName = p.second;
            
            if(termName == "PIN") 
            //if(node->getType() == NodeType::PrimaryOut)
                term2net[node][termName] = node->getName();
            else
                term2net[node][termName] = net->getName();
        }
    }
  
    // 3. instances
    for(int i=0; i < nodes.size(); i++) {
        Node* node = nodes[i];
        vector<Node*> sinks = node->getSinks();
        vector<Node*> sources = node->getSources();

        if(node->getType() != NodeType::Combinational && node->getType() != NodeType::Sequential)
            continue;

        verilog << node->getDbMaster()->getName() << " " << node->getName() << "(" << endl;

        set<string> terms;

        for(int j=0; j < sources.size(); j++) {
            Node* source = sources[j];
            string termName = node->getTerm(source);
            string netName = term2net[node][termName];
            
            if(terms.find(termName) == terms.end()) {
                terms.insert(termName); 
                if(j!=0)
                    verilog << "," << endl;
                verilog << "." << termName << "(" << netName << ")";
            }
        }

        
        
        //set<string> outTerms;


        for(int j=0; j < sinks.size(); j++) {
        
            
            Node* sink = sinks[j];
            string termName = node->getTerm(sink);
            string netName = term2net[node][termName];
            
            if(terms.find(termName) == terms.end()) {
                terms.insert(termName); 
            //if(outTerms.find(termName) == outTerms.end()) {
            //    outTerms.insert(termName);
                verilog << "," << endl;
                verilog << "." << termName << "(" << netName << ")";
            }
        }

        verilog << endl << ");" << endl << endl;
    }


    verilog << endl << "endmodule" << endl;
    verilog.close();
    logger_->info(ANG, 101, "Write Verilog");
    //log_->procEnd("Write Verilog",1);
}

void
ArtNetGen::writeSdc(const char* fileName) {
    //log_->procBegin("Write Sdc",1);
    logger_->info(ANG, 102, "Start Write Sdc");

    ofstream sdc(fileName);

    if(!sdc.good()) {
        cout << "Cannot open " << fileName << " file" << endl;
        return;
    }



    sdc << "###########################################" << endl;
    sdc << "# Created by NetGen (author: Daeyeon Kim) #" << endl;
    sdc << "###########################################" << endl;
    sdc << endl;

    sdc << "set sdc_version 2.1" << endl;
    sdc << endl;

  
    int resolution = 50;
    int unit = 1000;
    int synClkPeriod = (avgGateDelay_ * avgTopoOrder_) * unit;
    
    synClkPeriod -= synClkPeriod % resolution;
    //double clkRaising = 0;
    //double clkFalling = 0.5 * synClkPeriod;

    // unit --> ns
    double clkPeriod = 1.0 * synClkPeriod; 
    double ioDelay = 0.00 * synClkPeriod;

    //sdc << "set_units -time ns -resistance kOhm -capacitance pF -voltage V -current mA" << endl;
    sdc << "create_clock [get_ports clk] -period " << to_string(clkPeriod) << " -name clk" << endl;
    //"-waveform " << "{" << to_string(clkRaising*unit) << " " << to_string(clkFalling*unit) << "} -name clk" << endl;


    vector<Node*> primIns = artnet_->getPrimaryInputs();
    vector<Node*> primOuts = artnet_->getPrimaryOutputs();

    for(Node* inNode : primIns) {
        if(inNode->getType() == NodeType::ClockIn)
            //|| inNode->getType() == NodeType::ResetIn) 
            continue;
        
        sdc << "set_input_delay -max " << to_string(ioDelay) << " [get_ports " << inNode->getName() << "] -clock clk" << endl;
    }

    for(Node* outNode : primOuts) {
        sdc << "set_output_delay -max " << to_string(ioDelay) << " [get_ports " << outNode->getName() << "] -clock clk" << endl;
    }

    sdc.close();
    //log_->procEnd("Write Sdc",1);
    logger_->info(ANG, 103, "End Write Sdc");
}




void
ArtNetGen::run() {
    artnet_ = new Netlist(); 
    artnet_->setArtNetGen(this);
    artnet_->generate();

}

// parse the spec file
void
ArtNetGen::readSpec() {
    //log_->procBegin("Read Spec",1);
    logger_->info(ANG, 104, "Start Read Spec");

    ifstream ifs(specFile_);
    if( !ifs.good() ) {
        //log_->errorQuit("Cannot open spec file " + specFile_, 3);
        logger_->error(ANG, 201, "Cannot Open Spec File " + specFile_); //error
    }

    string line;

    // modified by dykim
    int maxFi = INT_MIN;
    int maxFo = INT_MIN;
    int maxBbox = INT_MIN;
    int maxWindow = INT_MIN;

    vector<DistributionInfo> fanIns;
    vector<DistributionInfo> fanOuts;
    vector<DistributionInfo> netBboxes;
    vector<DistributionInfo> windows;

    int totInstCnt = 0;

    while(getline(ifs, line)) {
        stringstream ss(line);
        string modeStr;
        ss >> modeStr;

        if( modeStr == "CELL_COUNT" ) {
            int value;
            ss >>  value;
            //instanceCnt_ = value;

        } else if ( modeStr == "COMB_RATIO" ) {
          
            double value;
            ss >>  value;

            combRatio_ = value;

        } else if( modeStr == "ONLY_USE" ) {
            string masterName;
            ss >> masterName;

            dbMaster* master = db_->findMaster( masterName.c_str() ); 
            if( !master ) {
                //log_->errorQuit("Cannot find master cell " + masterName, 4);
                logger_->error(ANG, 202, "Cannot Find Master Cell " + masterName);
            }

            double ratio;
            ss >> ratio;

            masters_.push_back(MasterInfo(master, ratio));
        }
        else if( modeStr == "FANIN" ) { 
            int value, target;
            double ratio;

            ss >> value >> target >> ratio;
            totInstCnt += target;
            maxFi = max(value, maxFi);
            fanIns.push_back(DistributionInfo(value, target, 0, ratio));
        }
        else if( modeStr == "FANOUT" ) {
            int value, target;
            double ratio;

            ss >> value >> target >> ratio;
            maxFo = max(value, maxFo);
            fanOuts.push_back(DistributionInfo(value, target, 0, ratio));
        }
        else if( modeStr == "WINDOW" ) { 
            int value, target;
            double ratio;

            ss >> value >> target >> ratio;
            maxWindow = max(value, maxWindow);
            windows.push_back(DistributionInfo(value, target, 0, ratio));
        }
        else if( modeStr == "NETBBOX" ) { 
            int value, target;
            double ratio;

            ss >> value >> target >> ratio;
            maxBbox = max(value, maxBbox);
            netBboxes.push_back(DistributionInfo(value, target, 0, ratio));
        }
        else if( modeStr == "IPAD_COUNT" ) {
            int value;
            ss >> value;
            setInputPinCnt(value);
        }
        else if( modeStr == "OPAD_COUNT" ) {
            int value;
            ss >> value;
            setOutputPinCnt(value);
        }
        //else if (modeStr == "SYN_CLK_PERIOD" ) {
        //    double value;
        //    ss >> value;
        //    setSynClkPeriod(value); 
        //}
        else if (modeStr == "AVG_GATE_DELAY" ) {
            double value;
            ss >> value;
            setAvgGateDelay(value);
        }
        else if (modeStr == "AVG_TOPO_ORDER" ) {
            double value;
            ss >> value;
            setAvgTopoOrder(value);
        }



    }


    // instance count from SPEC
    instanceCnt_ = totInstCnt;

    // BIN SQRT
    int binCnt = ceil(1.0 * instanceCnt_ / sqrt(instanceCnt_)); 



    fanIns_=fanIns;
    fanOuts_ = fanOuts;
    netBboxes_ = netBboxes;
    edges_ = windows;
    maxFanin_ = maxFi;
    maxFanout_ = maxFo;

  // initialize distribution
  fiDist_.init( 0, maxFi, getInstanceCnt(), fanIns );
  foDist_.init( 0, maxFo, getInstanceCnt(), fanOuts );
  bboxDist_.init( 0, maxBbox, getInstanceCnt(), netBboxes );
  int totalEdgeCnt = ceil(1.0 * getInstanceCnt() * fiDist_.targetAvg()); // 확인 필요
  // avg. net degree = (avg. # fanin)
  // total edge count = (avg. net degree) x (# instances)
  edgeDist_.init( 0, maxWindow, totalEdgeCnt, windows);

  fiDist_.setDescription("fanin distribution");
  foDist_.setDescription("fanout distribution");
  bboxDist_.setDescription("bbox distribution");
  edgeDist_.setDescription("edgelength distribution");


  ifs.close();
  //log_->infoInt("NumMasters", masters_.size());
  logger_->info(ANG, 105, "NumMasters {:d}", masters_.size());
  //log_->procEnd("Read Spec",1);
  logger_->info(ANG, 106, "Read Spec");
}

void 
ArtNetGen::printMasters() {

    cout << "Available Masters" << endl;
    for(auto& master : masters_) {
        cout << " - " << master.master()->getName() << endl;
    }
    cout << endl;
}


void
ArtNetGen::printAllVars() {
  if( verbose_ >= 5) {
    logger_->info(ANG, 107, "Master");
    printMasters();
  }
  logger_->info(ANG, 108, "TargetInstCnt : {:d}", instanceCnt_);
  logger_->info(ANG, 109, "CombinationRatio {:f}", combRatio_);
  logger_->info(ANG, 110, "InputPinCnt {:d}", inputPinCnt_);
  logger_->info(ANG, 111, "OutputPinCnt {:d}", outputPinCnt_);
  logger_->info(ANG, 112, "AvgTopoOrder {:f}", avgTopoOrder_);
  logger_->info(ANG, 113, "AvgGateDelay {:f}", avgGateDelay_);
}


void
ArtNetGen::checkMasters() {
    
    unordered_map<int,vector<MasterInfo>> fi2TotCombMasters;
    unordered_map<int,vector<MasterInfo>> fi2TotSequMasters;
    unordered_map<int,vector<MasterInfo>> fi2OnlyUseCombMasters;
    unordered_map<int,vector<MasterInfo>> fi2OnlyUseSequMasters;

    dbSet<dbLib> libs = db_->getLibs();

    for(auto itl = libs.begin(); itl != libs.end(); itl++) {
        dbSet<dbMaster> masters = itl->getMasters();

        for(auto itm = masters.begin(); itm != masters.end(); itm++) {
            
            string masterName = itm->getName();
            dbMaster* master = db_->findMaster(masterName.c_str());
            unordered_map<string, int> ioCount = getPortInfo(master);

            //cout << " - " << master->getName() 
            //<< " (#in " << ioCount["input_signal"] 
            //<< " #out " << ioCount["output_signal"] <<")";
            //if(ioCount["input_clock"] > 0) 
            //cout << " clock";
            //cout << endl;
            if(ioCount["output_signal"] == 0)
                continue;

            if(ioCount["input_scan"] > 0) {
                cout << "current version does not support scan-chain cell... (" << master->getName() << ")" <<  endl;
                continue;
            }

            if(ioCount["input_clock"] > 0) {
                fi2TotSequMasters[ioCount["input_signal"]].push_back(MasterInfo(master, 1.0));
            } else {
                // combinational cell
                fi2TotCombMasters[ioCount["input_signal"]].push_back(MasterInfo(master, 1.0));
            }
        }
    }

    for(MasterInfo& info : masters_) {
        dbMaster* master = info.master();
        float ratio = info.ratio();
        dbSet<dbMTerm> terms = master->getMTerms();
        
        unordered_map<string, int> ioCount = getPortInfo(master);
        //cout << " - " << master->getName() 
        //<< " (#in " << ioCount["input_signal"] 
        //<< " #out " << ioCount["output_signal"] <<")";
        //if(ioCount["input_clock"] > 0) 
        //cout << " clock";
        //cout << endl;
        if(ioCount["output_signal"] == 0)
            continue;

        if(ioCount["input_scan"] > 0) {
            cout << "current version does not support scan-chain cell... (" << master->getName() << ")" <<  endl;
            continue;
        }

        if(ioCount["input_clock"] > 0) {
            // sequential cell
            fi2OnlyUseSequMasters[ioCount["input_signal"]].push_back(MasterInfo(master,ratio));
        } else {
            // combinational cell
            fi2OnlyUseCombMasters[ioCount["input_signal"]].push_back(MasterInfo(master,ratio));
        } 
    }

    vector<MasterInfo> masterInfos;

    for(int x=1; x <= fiDist_.xMax(); x++) {
        if(fi2OnlyUseCombMasters.find(x) != fi2OnlyUseCombMasters.end()) {
            masterInfos.insert( masterInfos.end(), fi2OnlyUseCombMasters[x].begin(), fi2OnlyUseCombMasters[x].end());
        } else {
            masterInfos.insert( masterInfos.end(), fi2TotCombMasters[x].begin(), fi2TotCombMasters[x].end());
        }

        if(fi2OnlyUseSequMasters.find(x) != fi2OnlyUseSequMasters.end()) {
            masterInfos.insert(masterInfos.end(), fi2OnlyUseSequMasters[x].begin(), fi2OnlyUseSequMasters[x].end());
        } else { 
            masterInfos.insert( masterInfos.end(), fi2TotSequMasters[x].begin(), fi2TotSequMasters[x].end() );
        }

    }

    masters_ = masterInfos;
}

void
ArtNetGen::writeVerilog() {
}

void
ArtNetGen::setDb(odb::dbDatabase* db) {
  db_ = db;
}

void
ArtNetGen::setSta(sta::dbSta* sta) {
  sta_ = sta;
}

void
ArtNetGen::setLogger(utl::Logger* logger) {
  logger_ = logger;
}

void
ArtNetGen::setAvgGateDelay(double avgDelay) {
    cout << "[INFO] avg. gate delay changes from " 
        << avgGateDelay_ << " to " << avgDelay << endl;

    avgGateDelay_ = avgDelay;
}
}
