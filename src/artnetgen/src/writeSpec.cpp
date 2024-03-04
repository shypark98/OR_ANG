#include "artnetgen/artNetGen.h"
#include "ord/OpenRoad.hh"
#include "odb/db.h"
#include "utl/Logger.h"
//#include "db_sta/dbSta.hh"
//#include "db_sta/dbNetwork.hh"
//#include "logger.h"
//#include "netlist.h"
//#include "node.h"

#include <fstream>
#include <sstream>
#include <math.h>
#include <unordered_map>
#include <iostream>
#include <random>
#include <queue>

namespace artnetgen {

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
using std::default_random_engine;
using std::geometric_distribution;
using std::exponential_distribution;
using std::pow;
using std::exp;
using std::queue;

int getPartitionCoreWidthHeight(vector<dbInst*> insts) {
    int lx = INT_MAX;
    int ly = INT_MAX;
    int ux = 0;
    int uy = 0;
    int inst_num = 0;

    for (auto inst_itr = insts.begin(); inst_itr != insts.end(); ++inst_itr) {
        int cur_x, cur_y;
        inst_itr->getLocation(&cur_x, &cur_y);
        // If Macro is added, need to consider cell width, height;
        lx = min(cur_x, lx);
        ly = min(cur_y, ly);
        ux = max(cur_x, ux);
        uy = max(cur_x, uy);
        inst_num++;
    }
    int coreWidth = ux - lx;
    int coreHeight = uy - ly;
    return coreWidth, coreHeight;
}


int getMaxFanout(vector<dbInst*> insts) {
    
    maxFanout = 0;
    for(auto inst_itr = insts.begin(); inst_itr != insts.end(); ++inst_itr) {
        //dbInst* inst = *inst_itr;
        dbSet<dbITerm> inst_iterms = inst->getITerms();
        numFanouts = 0;
        
        for(auto inst_iterm_itr = inst_iterms.begin(); inst_iterm_itr != inst_iterms.end();
            ++inst_iterm_itr) {
            dbITerm* inst_iterm = *inst_iterm_itr;
            if(inst_iterm->getIoType() == dbIoType::OUTPUT) {
                dbNet* net = inst_iterm->getNet();
                dbSet<dbITerm>  net_iterms = net->getITerms();
                for (auto net_iterm_itr = net_iterms.begin(); net_iterm_itr != net_iterms.end();
                        ++net_iterm_itr) {
                    dbITerm* net_iterm = *net_iterm_itr;
                    dbInst*  sink_inst = net_iterm->getInst();
                    if(net_iterm->getIoType() == dbIoType::INPUT) {
                        numFanouts++;
                    }
                }
            }
        }
    maxFanout = max(maxFanout, numFanouts);
    }
    return maxFanout;
}


int getMaxInSigCnt() {
    dbDatabase* db = ord::OpenRoad::openRoad()->getDb();
    dbSet<dbLib> libs = db->getLibs();
    int maxInSigCnt = 0;

    for(auto itl = libs.begin(); itl != libs.end(); itl++) {
        dbSet<dbMaster> masters = itl->getMasters();

        for(auto itm = masters.begin(); itm != masters.end(); itm++) {

            string masterName = itm->getName();

            dbMaster* master = db->findMaster(masterName.c_str());
            dbSet<dbMTerm> terms = master->getMTerms();

            int inSigCnt = 0;
            int inClkCnt = 0;
            int inRstCnt = 0;
            int inScanCnt = 0;
            int outSigCnt = 0;
            int outClkCnt = 0;
            int outRstCnt = 0;
            int outScanCnt = 0;

            for(auto it = terms.begin(); it != terms.end(); it++) {

                string termName = it->getName();

                dbSigType sigType = it->getSigType();
                dbIoType ioType = it->getIoType();

                if(sigType.getValue() == dbSigType::SIGNAL) {
                    if(ioType.getValue() == dbIoType::INPUT) {
                        inSigCnt++;
                    } else if(ioType.getValue() == dbIoType::OUTPUT) {
                        outSigCnt++;
                    }
                    //cout << termName << " " << sigType.getString() << " " << ioType.getString() << endl;
                } else if(sigType.getValue() == dbSigType::CLOCK) {
                    if(ioType.getValue() == dbIoType::INPUT) {
                        inClkCnt++;
                    } else if(ioType.getValue() == dbIoType::OUTPUT) {
                        outClkCnt++;
                    }
                } else if(sigType.getValue() == dbSigType::RESET) {
                    if(ioType.getValue() == dbIoType::INPUT) {
                        inRstCnt++;
                    } else if(ioType.getValue() == dbIoType::OUTPUT) {
                        outRstCnt++;
                    }
                } else if(sigType.getValue() == dbSigType::SCAN) {
                     if(ioType.getValue() == dbIoType::INPUT) {
                        inScanCnt++;
                    } else if(ioType.getValue() == dbIoType::OUTPUT) {
                        outScanCnt++;
                    }
                } else {

                }
            }

            maxInSigCnt = max(maxInSigCnt, inSigCnt);
        }
    }

    return maxInSigCnt;
}


int factorial(int n) {
    int val = 1;
    for(int x=n; x > 0; x--) 
        val += x;
    return val;
}

unordered_map<int,int> makeFoissonDistribution(int minVal, int maxVal, float avgVal, int nrolls) {
    float alpha = 1.0;
    float lambda = 1.5;
    float minProb = 0.001;
    float divider = 0;
    float curAvgVal = 0;

    unordered_map<int,int> samples;
        
    // FANIN DISTRIBUTION
    // [1] ******
    // [2] *********
    // [3] *******
    // [4] **
    // [5] *
    // [6] *

    float p_bias1 = 0.6;
    float p_bias2 = 0.3;
    // avgVal = 1 * p1 + 2 * p2 + 3 * p3 + 4 * p4 + 5 * p5
    while(true) { 
        curAvgVal = 0;
        divider = 0;
        for(int x=0; x <= maxVal-minVal; x++) {
            //divider += (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb); 

            if ( x==1 )
                divider += (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + p_bias1); 
            else if ( x==2 )
                divider += (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + p_bias2); 
            else
                divider += (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb ); 

            //float bias = (x==1) ? 0.6 : 0;
            //divider += (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + bias); 
        }

        for(int x=0; x <= maxVal-minVal; x++) {
            float prob; 
            if ( x==1 )
                prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + p_bias1) / divider;
            else if ( x==2 )
                prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + p_bias2) / divider;
            else
                prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb ) / divider;
            curAvgVal += 1.0 * (x+minVal) * prob;
        }

        float error = curAvgVal - avgVal;
        //cout << "lambda " << lambda << " " << curAvgVal << " " << avgVal << endl;
        
        if( abs(error) < 0.03 ) {
            break;
        } else {
            if( error > 0 )
                lambda -= 0.01;
            else
                lambda += 0.01;
        }
    }

 
    int totSamples = 0;
    float cdf = 0;
    for(int x=0; x <= maxVal-minVal; x++) {
        //float prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb) / divider;
        //float bias = (x==1) ? 0.6 : 0;
        //float prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + bias) / divider;
        float prob; 
        if ( x==1 )
            prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + p_bias1) / divider;
        else if ( x==2 )
            prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb + p_bias2) / divider;
        else
            prob = (alpha * pow(lambda, x) / factorial(x) * exp(-lambda) + minProb ) / divider;
        cdf += prob;


        int count = ceil(1.0 * prob * nrolls);

        if(totSamples + count > nrolls) 
            count = nrolls - totSamples;

        samples[x+minVal] = count;
        
        totSamples += count;
        
        //ceil(1.0* prob * nrolls);
        
        //cout << x + minVal << " : " << 100 * prob << " %" << endl;
    }

    //cout << "Total probability : " << cdf << endl;   

    return samples;
}



unordered_map<int,int> makeExpDistribution(int minVal, int maxVal, float avgVal, int nrolls) {
    float alpha = 0.7;
    float lambda = 0.1;
    float minProb = 0.00001;
    float divider = 0;
    float curAvgVal = 0;
    unordered_map<int,int> samples;

    while(true) {
        curAvgVal = 0;
        divider = 0;

        for(int x=0; x < maxVal-minVal; x++) {
            divider += (alpha * lambda * exp (-lambda * x) + minProb);
        }

        for(int x=0; x < maxVal-minVal; x++){
            float prob = ( alpha * lambda * exp(-lambda *x ) + minProb ) / divider;
            curAvgVal += 1.0 * (x + minVal) * prob; 
        }

        float error = curAvgVal - avgVal;

        //cout << "lambda " << lambda << " " << curAvgVal << " " << avgVal << endl;

        if( abs(error) < 0.05 ) {
            break;
        } else {
            if( error > 0 ) {
                lambda += 0.01;
            } else {
                lambda -= 0.01;
            }
        }
    }

    int totSamples = 0;
    float cdf = 0;
    for(int x=0; x < maxVal-minVal; x++) {
        float prob = ( alpha * lambda * exp(-lambda *x ) + minProb ) / divider;

        cdf += prob;
        //cout << x + minVal << " : " << 100 * prob << " %" << endl;
    
        int count = ceil(1.0 * prob * nrolls);
        if(totSamples + count > nrolls)
            count = nrolls - totSamples;
        samples[x+minVal] = count;
        totSamples += count;
    
    }

    //cout << "Total probability : " << cdf << endl;
    return samples;

}

void
ArtNetGen::createSpec(int numInsts, int numINs, int numOUTs, float combRatio, float avgNetDeg, float avgBbox, float avgTopoOrder, 
        bool hierachy_flag, const char* onlyUseList, const char* fileName) {
    
    cout << "HERE CREATE SPEC" << endl;

    dbDatabase* db = ord::OpenRoad::openRoad()->getDb();
    
    unordered_map<int,int> faninDistribution;
    unordered_map<int,int> fanoutDistribution;
    unordered_map<int,int> bboxDistribution;
    unordered_map<int,int> edgelenDistribution;
    unordered_map<string,int> onlyUseMasters;

    int numNets = numInsts;
    int numInBTerms = numINs;
    int numOutBTerms = numOUTs;
    int numEdges = ceil(1.0 * numInsts * avgNetDeg);

    int dimX =1, dimY =1;
    while( dimX * dimY < ceil(sqrt(numInsts)) ) {
        if(dimX == dimY)
            dimX++;
        else
            dimY++;
    }


    int minFanins = 1;
    int minFanouts = 1;
    int minEdgelen = 0;
    int minBbox = 0;

    int maxFanins = min(6, getMaxInSigCnt());
    int maxFanouts = 50;
    int maxEdgelen = dimX + dimY;
    int maxBbox = dimX + dimY;
   

    faninDistribution = makeFoissonDistribution(minFanins, maxFanins, avgNetDeg, numInsts);
    fanoutDistribution = makeExpDistribution(minFanouts, maxFanouts, avgNetDeg, numInsts);
    bboxDistribution = makeExpDistribution(minBbox, maxBbox, avgBbox, numNets);
    edgelenDistribution = makeExpDistribution(minEdgelen, maxEdgelen, avgBbox, numEdges);


    bool initOnlyUse = false;
    if(onlyUseList != "") {
        ifstream inFile(onlyUseList);
        if(!inFile.good()) {
            cout << "Cannot open onlyUseList file " << onlyUseList << endl;
        } else {
            string line;
            string masterName;
            int count;
            float ratio;
            int totCount = 0;            
            while(getline(inFile, line)) {
                stringstream ss(line);
                ss >> masterName >> count >> ratio;

                if(!db->findMaster(masterName.c_str())) {
                    cout << "Cannot find master cell " << masterName << endl;
                } else {
                    onlyUseMasters[masterName] = count;
                }
            }
        }
    } else {

    }

    ofstream outFile(fileName);

    if(!outFile.good()) {
        cout << "Invalid output file" << endl;
        return;
    }

    //double avgFanouts = 0;
    //double avgFanins = 0;
    //double avgBbox = 0;
    //double avgEdgelen = 0;
    //double combRatio = 1.0 * numCombinational / numInsts;


    outFile << "CELL_COUNT " << numInsts << endl;
    outFile << "COMB_RATIO " << combRatio << endl;
    outFile << "IPAD_COUNT " << numInBTerms << endl;
    outFile << "OPAD_COUNT " << numOutBTerms << endl;
    outFile << "AVG_FANIN " << avgNetDeg << endl;
    outFile << "AVG_FANOUT " << avgNetDeg << endl;
    outFile << "AVG_BBOX " << avgBbox << endl;
    outFile << "AVG_EDGELEN " << avgBbox << endl;
    outFile << "AVG_TOPO_ORDER " << avgTopoOrder << endl;
    outFile << "AVG_GATE_DELAY " << getAvgGateDelay() << endl;

    for(auto itr : onlyUseMasters) {
        outFile << "ONLY_USE " << itr.first << " " << itr.second << " " << 1.0* itr.second / numInsts << endl;
    }

    for(int x=0; x <= maxFanins; x++) {
        outFile << "FANIN " << x << " " << faninDistribution[x] << " " << 1.0 * faninDistribution[x] / numInsts << endl;
    }
    for(int x=0; x <= maxFanouts; x++) {
        outFile << "FANOUT " << x << " " << fanoutDistribution[x] << " " << 1.0 * fanoutDistribution[x] / numInsts << endl;
    }
    for(int x=0; x <= maxBbox; x++) {
        outFile << "NETBBOX " << x << " " << bboxDistribution[x] << " " << 1.0 * bboxDistribution[x] / numInsts << endl;
    }
    for(int x=0; x <= maxEdgelen; x++) {
        outFile << "WINDOW " << x << " " << edgelenDistribution[x] << " " << 1.0 * edgelenDistribution[x] / numEdges << endl;
    }

    outFile.close();
}


double
ArtNetGen::analyzeTopologicalOrder() {
    dbDatabase* db = ord::OpenRoad::openRoad()->getDb();
    dbChip* chip = db->getChip();
    if( chip == nullptr ) {
        cout << "dbChip does not exist! (program terminated)" << endl;
        exit(0);
    }

    dbBlock* block = chip->getBlock();
    
    dbSet<dbInst> insts = block->getInsts();
    dbSet<dbBTerm> bterms = block->getBTerms();

    struct Node {
        odb::dbInst* inst;
        odb::dbBTerm* bterm;
        vector<Node*> sources;
        vector<Node*> sinks;
        
        Node() {
            inst = nullptr;
            bterm = nullptr;
        }
       
        bool isSequential() {
            if(inst == nullptr)
                return false;
            else
                return inst->getMaster()->isSequential();
        }

        bool isPrimaryIn() {
            if(bterm == nullptr)
                return false;
            else
                return (bterm->getIoType() == dbIoType::INPUT);
        }

        bool isPrimaryOut() {
            if(bterm == nullptr)
                return false;
            else
                return (bterm->getIoType() == dbIoType::OUTPUT);
        }
    };


    vector<Node> nodes;
    nodes.reserve(insts.size() + bterms.size());
    unordered_map<dbInst*, Node*> inst2node;
    unordered_map<dbBTerm*, Node*> bterm2node;

    
    for(auto inst_itr = insts.begin(); inst_itr != insts.end(); ++inst_itr) {
        dbInst* inst = *inst_itr;
    
        Node node;
        node.inst = inst;
        nodes.push_back(node);
        inst2node[inst] = &nodes.back();
    }

    for(auto bterm_itr = bterms.begin(); bterm_itr != bterms.end(); ++bterm_itr) {
 
        dbBTerm* bterm = *bterm_itr;


        if(bterm->getSigType() != dbSigType::SIGNAL) 
            continue;

        Node node;
        node.bterm = bterm;
        nodes.push_back(node);
        bterm2node[bterm] = &nodes.back();

    }

    for(auto inst_itr = insts.begin(); inst_itr != insts.end(); ++inst_itr) {
        
        dbInst* source_inst = *inst_itr;
        dbSet<dbITerm> inst_iterms = source_inst->getITerms();
        for(auto inst_iterm_itr = inst_iterms.begin(); inst_iterm_itr != inst_iterms.end(); ++inst_iterm_itr) {

            dbITerm* inst_iterm = *inst_iterm_itr;

            if(inst_iterm->getNet() == NULL) 
                continue;

            if(inst_iterm->getSigType() != dbSigType::SIGNAL)
                continue;

            dbNet* net = inst_iterm->getNet();
            if(inst_iterm->getIoType() == dbIoType::OUTPUT) {
                dbSet<dbITerm> net_iterms = net->getITerms();
                dbSet<dbBTerm> net_bterms = net->getBTerms();
                    
                for(auto net_iterm_itr = net_iterms.begin(); net_iterm_itr != net_iterms.end(); ++net_iterm_itr) {
                    dbITerm* net_iterm = *net_iterm_itr;
                    if(net_iterm->getIoType() == dbIoType::OUTPUT) {
                        dbInst* source_inst = net_iterm->getInst(); 

                    }

                    if(net_iterm->getIoType() == dbIoType::INPUT) {
                        dbInst* sink_inst = net_iterm->getInst();

                        //cout << sink_inst->getName() << endl;
                        Node* sink = inst2node[sink_inst];
                        Node* source = inst2node[source_inst];
                        sink->sources.push_back(source);
                        source->sinks.push_back(sink);
                    }
                }

                for(auto net_bterm_itr = net_bterms.begin(); net_bterm_itr != net_bterms.end(); ++net_bterm_itr) {
                    dbBTerm* net_bterm = *net_bterm_itr;
                    if(net_bterm->getIoType() == dbIoType::INPUT) {
                        Node* sink = bterm2node[net_bterm];
                        Node* source = inst2node[source_inst];
                        sink->sources.push_back(source);
                        source->sinks.push_back(sink);
                    }
                }
            }
        }
    }
 
    for(auto bterm_itr = bterms.begin(); bterm_itr != bterms.end(); ++bterm_itr) {
 
        dbBTerm* bterm = *bterm_itr;

        if(bterm->getSigType() != dbSigType::SIGNAL) 
            continue;

        if(bterm->getNet() == NULL)
            continue;

        dbNet* net = bterm->getNet();

        if(bterm->getIoType() == dbIoType::OUTPUT) {
            dbSet<dbITerm> net_iterms = net->getITerms();
            dbSet<dbBTerm> net_bterms = net->getBTerms();

            for(auto net_iterm_itr = net_iterms.begin(); net_iterm_itr != net_iterms.end(); ++net_iterm_itr) {
                dbITerm* net_iterm = *net_iterm_itr;
                if(net_iterm->getIoType() == dbIoType::OUTPUT) {
                }

                if(net_iterm->getIoType() == dbIoType::INPUT) {
                    dbInst* sink_inst = net_iterm->getInst();
                    Node* sink = inst2node[sink_inst];
                    Node* source = bterm2node[bterm];
                    sink->sources.push_back(source);
                    source->sinks.push_back(sink);
                }
            }

            for(auto net_bterm_itr = net_bterms.begin(); net_bterm_itr != net_bterms.end(); ++net_bterm_itr) {
                dbBTerm* net_bterm = *net_bterm_itr;
                if(net_bterm->getIoType() == dbIoType::INPUT) {
                    Node* sink = bterm2node[net_bterm];
                    Node* source = bterm2node[bterm];
                    sink->sources.push_back(source);
                    source->sinks.push_back(sink);
                }
            }
        }          
    }
    

    queue<Node*> Q;
    unordered_map<Node*, int> inDegree;
    unordered_map<Node*, int> topoOrder;
    for(int i=0; i < nodes.size(); i++) {
        Node* node = &nodes[i];
        
        topoOrder[node] = -1;
        
        if(node->isSequential() || node->isPrimaryIn()) {
            inDegree[node] = 0;
        } else {
            inDegree[node] = node->sources.size();
        }

        if(inDegree[node] == 0) {
            topoOrder[node] = 0;
            Q.push(node);
        }
    }

    while(!Q.empty()) {
        Node* n1 = Q.front();
        Q.pop();

        for(Node* n2 : n1->sinks) {
            if( inDegree[n2] == 0 )
                continue;
            if( --inDegree[n2] == 0 ) {
                topoOrder[n2] = topoOrder[n1] + 1;
                Q.push(n2);
            }
        }
    }

    double avgTopoOrder = 0;
    int maxTopoOrder = 0;
    
    for(int i=0; i < nodes.size(); i++) {
        Node* node = &nodes[i];
        
        /*
        if (node->inst != nullptr) {
            dbInst* inst = node->inst;
            dbMaster* master = inst->getMaster();
            cout << inst->getName() << " (" << master->getName() << ") : " << topoOrder[node] << endl;
        } else if (node->bterm != nullptr) {
            dbBTerm* bterm = node->bterm;
            cout << bterm->getName() << "(BTERM) : " << topoOrder[node] << endl;
        }
        */
        
        maxTopoOrder = max(maxTopoOrder, topoOrder[node]);
        avgTopoOrder += topoOrder[node];
    }

    avgTopoOrder /= nodes.size();

    cout << "Max topo. order : " << maxTopoOrder << endl;
    cout << "Avg topo. order : " << avgTopoOrder << endl;

    return avgTopoOrder;
}

void
ArtNetGen::writeSpec(const char* fileName) {
    
    dbDatabase* db = ord::OpenRoad::openRoad()->getDb();
    dbChip* chip = db->getChip();

    if (chip == nullptr) {
        cout << "dbChip does not exist!" << endl;
        return;
    }

    dbBlock* block = chip->getBlock();
    
    if (hierarchy_flag_) {
    
        std::map<int, vector<dbInst*>> instance_map;
        // build partition instance map
        for (dbInst* inst : block->getInsts()) {
            dbIntProperty* prop_id = dbIntProperty::find(inst, "partition_id");
            if (!prop_id) {
                logger_->warn(ANG, 15,
                "Property 'partition_id' not found for inst {}.",
                inst->getName());
            } else {
                const int partition = prop_id->getValue();
                instance_map[partition].push_back(inst);
            }
        }

        for (auto it ->instance_map.begin(); it != instance_map.end(); ++it) {
            vector<dbInst*> insts = it->second;
        }
        /*
        std::set<Instance> insts;
        for (auto it = instance_map.begin(); it != instance_map.end(); ++it) {
            for (Instance* inst_ptr : it->second) {
                insts.insert(*inst_ptr);
            }
        }
        */
        coreWidth, coreHeight = getCoreWidthHeight();
        writeSpec(fileName, insts, coreWidth, coreHeight);
    }

    else {
        Rect coreArea = block->getCoreArea();
        Point coreLL = coreArea.ll();
        Point coreUR = coreArea.ur();

        int coreWidth = coreUR.x() - coreLL.x();
        int coreHeight = coreUR.y() - coreLL.y();
        dbSet<dbInst> insts = block->getInsts();
        writeSpec(fileName, insts, coreWidth, coreHeight)
    }
}


void 
ArtNetGen::writeSpec(const char* fileName, vector<dbInst*> insts, int coreWidth, int coreHeight) {
    
    string partition = "";
    
    ofstream spl(SPLFile_);
    /*
    dbDatabase* db = ord::OpenRoad::openRoad()->getDb();

    dbChip* chip = db->getChip();
    if( chip == nullptr ) {
        cout << "dbChip does not exist!" << endl;
        return;
    }
    

    dbBlock* block = chip->getBlock();

    Rect coreArea = block->getCoreArea();
    Point coreLL = coreArea.ll();
    Point coreUR = coreArea.ur();


    int coreWidth = coreUR.x() - coreLL.x();
    int coreHeight = coreUR.y() - coreLL.y();
    
    */
    unordered_map<int,int> faninDistribution;
    unordered_map<int,int> fanoutDistribution;
    unordered_map<int,int> bboxDistribution;
    unordered_map<int,int> edgelenDistribution;
    unordered_map<string,int> onlyUseMasters;



    //dbSet<dbInst> insts = block->getInsts();

    int numInsts = insts.size();
    int numCombinational = 0;
    int numSequential = 0;
    int numInBTerms = 0;
    int numOutBTerms = 0;
    int numEdges = 0;

   
    int dimX =1, dimY =1;
    while( dimX * dimY < ceil(sqrt(numInsts)) ) {
        if(dimX == dimY)
            dimX++;
        else
            dimY++;
    }

    // 초기화 다시 설정할 필요?
    int maxFanins = min(6, getMaxInSigCnt()); 
    int maxFanouts = getMaxFanout(insts);
    int maxEdgelen = dimX + dimY;
    int maxBbox = dimX + dimY;
    

    for(int x=0; x <= maxFanins; x++)
        faninDistribution[x] = 0;
    for(int x=0; x <= maxFanouts; x++)
        fanoutDistribution[x] = 0;
    for(int x=0; x <= maxBbox; x++)
        bboxDistribution[x] = 0;
    for(int x=0; x <= maxEdgelen; x++)
        edgelenDistribution[x] = 0;
    
    if (!hierarchy_flag_) {
        dbSet<dbBTerm> bterms = block->getBTerms(); // get port terminals
        // partition된 애들 port 어캐 처리하는지 참조 필요

        for(auto bterm_itr = bterms.begin(); bterm_itr != bterms.end(); ++bterm_itr) {
     
            dbBTerm* bterm = *bterm_itr;

            if(bterm->getSigType() == dbSigType::SIGNAL) {
                if(bterm->getIoType() == dbIoType::INPUT) 
                    numInBTerms++;
                if(bterm->getIoType() == dbIoType::OUTPUT)
                    numOutBTerms++;
            }
        }
    }
    //BTERM 중에 clk, rst는 구분해야함
    //CreateSpec으로 hierarchy 만들려면 어캐하노?

    int numBins = dimX * dimY;
    int avgInsts = ceil( 1.0 * numInsts / numBins );
    int binSize = (coreWidth + coreHeight) / (dimX + dimY);
    
    else {
        
        
    }
    for(auto inst_itr = insts.begin(); inst_itr != insts.end(); ++inst_itr) {
    
        //dbInst* inst = *inst_itr;
        
        dbSet<dbITerm> inst_iterms = inst->getITerms();

        dbMaster* master = inst->getMaster();
        
        if(master->isSequential())
            numSequential++;
        else
            numCombinational++;

        if(onlyUseMasters.find(master->getName()) == onlyUseMasters.end()) //해당 cell의 개수를 파악
            onlyUseMasters[master->getName()] = 0;
        onlyUseMasters[master->getName()]++;

        int numFanins   = 0;
        int numFanouts  = 0;
        int bboxSize = 0;

        int sourceX, sourceY;
        inst->getLocation(sourceX, sourceY); //instance X Y 좌표 void dbInst::getLocation(int& x, int& y) const
        
        int lx, ly, ux, uy;
        lx = sourceX, ly = sourceY;
        ux = sourceX, uy = sourceY; 
        
        for(auto inst_iterm_itr = inst_iterms.begin(); inst_iterm_itr != inst_iterms.end();
                ++inst_iterm_itr) {
            dbITerm* inst_iterm = *inst_iterm_itr;

            if(inst_iterm->getNet() == NULL) 
                continue;

            if(inst_iterm->getSigType() != dbSigType::SIGNAL)
                continue;

            if(inst_iterm->getIoType() == dbIoType::INPUT)
                numFanins++;

            if(inst_iterm->getIoType() == dbIoType::OUTPUT) {
                dbNet* net = inst_iterm->getNet();

                dbSet<dbITerm>  net_iterms = net->getITerms(); //output에 연결된 net 구하고 net에 연결된 pin들 다 구함
                //cout << " - " << net->getName() << endl;

                for (auto net_iterm_itr = net_iterms.begin(); net_iterm_itr != net_iterms.end();
                        ++net_iterm_itr) {
                    dbITerm* net_iterm = *net_iterm_itr;
                    dbInst*  sink_inst = net_iterm->getInst(); //net에 연결된 pin들이 어떤 inst껀지

                    int sinkX, sinkY;
                    sink_inst->getLocation(sinkX, sinkY);
                    lx = min(lx, sinkX), ly = min(ly, sinkY);
                    ux = max(ux, sinkX), uy = max(uy, sinkY);
                    
                    
                    if(net_iterm->getIoType() == dbIoType::OUTPUT) {
                        // for debug
                        if(inst_iterm != net_iterm) {
                            cout << "invalid output term!" << endl; // 한 net에 output pin이 2개 이상일 때
                            //exit(0); //ANG에서 이 경우가 발생하는 이유는? distribution을 만족할 수 있게 cell을 골라서 넣는 방법은?
                        }
                        //cout << "   (out) ";
                    }
                    if(net_iterm->getIoType() == dbIoType::INPUT) {
                        //cout << "   (in) ";
                        numFanouts++;

                        int edgelen = (abs(sourceX - sinkX) + abs(sourceY - sinkY)) / binSize;
                        //if(edgelenDistribution.find(edgelen) == edgelenDistribution.end()) {
                        //    edgelenDistribution[edgelen] = 0;
                        //}
                        edgelenDistribution[edgelen]++;
                        //maxEdgelen = max(maxEdgelen, edgelen);
                        numEdges++;
                    }
                    //cout << sink_inst->getName();
                    if(net_iterm->getMTerm() != NULL) {
                        //cout << " " << net_iterm->getMTerm()->getName();
                    }
                    //cout << endl;
                }

                //cout << endl;
            }
        }
       
        bboxSize = (abs(ux - lx) + abs(uy - ly)) / binSize;
        
        //edgelen은 net에 같이 연결된 모든 cell과 cell 사이의 manhattan distance를 계산한 것
        //bboxSize는 한 net에서 가장 멀리 떨어져있는 output과 input 사이의 거리를 계산한 것
        /////////////////////////////////////////////////////

        //

        spl << " - instance summary (" << inst->getName() << ")" << endl;
        spl << "   # of fanins  : " << numFanins << endl;
        spl << "   # of fanouts : " << numFanouts << endl;
        spl << "   size of bbox : " << bboxSize << endl;
        spl << endl; 

        if(maxFanouts < numFanouts)
            numFanouts = maxFanouts;

        //if(faninDistribution.find(numFanins) == faninDistribution.end())
        //    faninDistribution[numFanins] = 0;
        //if(fanoutDistribution.find(numFanouts) == fanoutDistribution.end())
        //    fanoutDistribution[numFanouts] = 0;
        //if(bboxDistribution.find(bboxSize) == bboxDistribution.end())
        //    bboxDistribution[bboxSize] = 0;

        faninDistribution[numFanins]++;
        fanoutDistribution[numFanouts]++;
        bboxDistribution[bboxSize]++;

        //maxFanins = max(maxFanins, numFanins);
        //maxFanouts = max(maxFanouts, numFanouts);
        //maxBbox = max(maxBbox, bboxSize);
    
    }
    

    spl << " - design summary" << endl;
    spl << "   core area       : (" << coreLL.x() << " " << coreLL.y() << " " << coreUR.x() << " " << coreUR.y() << ")" << endl;
    spl << "   layout dim.     : (" << dimX << " " << dimY << ")" << endl;
    spl << "   avg. bin size   : " << binSize << endl;
    spl << "   total # bins    : " << numBins << endl;
    spl << endl;
    spl << "   # input pins    : " << numInBTerms << endl;
    spl << "   # output pins   : " << numOutBTerms << endl;
    
    spl << "   total # insts   : " << numInsts << endl;
    spl << "   # combinational : " << numCombinational << endl;
    spl << "   # sequential    : " << numSequential << endl;
    spl << endl;
    spl << "   max # fanins    : " << maxFanins << endl;
    spl << "   max # fanouts   : " << maxFanouts << endl;
    spl << endl;
    spl << "   <fanin distribution> " << endl;
    for(int x=0; x <= maxFanins; x++)
    spl << "       - (" << x << ") " << faninDistribution[x] << endl;
    spl << endl;
    spl << "   <fanout distribution> " << endl;
    for(int x=0; x <= maxFanouts; x++)
    spl << "       - (" << x << ") " << fanoutDistribution[x] << endl;
    spl << endl;
    spl << "   <bbox distribution> " << endl;
    for(int x=0; x <= maxBbox; x++)
    spl << "       - (" << x << ") " << bboxDistribution[x] << endl;
    spl << endl;
    spl << "   <edgelen distribution> " << endl;
    for(int x=0; x <= maxEdgelen; x++)
    spl << "       - (" << x << ") " << edgelenDistribution[x] << endl;
    spl << endl;
    spl << "   <only use masters>" << endl;
    for(auto it : onlyUseMasters)
    spl << "       - (" << it.first << ") " << it.second << endl;
    spl << endl;

    spl.close();

    dbSet<dbNet> nets = block->getNets();
    //dbSet<dbNet>::iterator net_itr;

    for(auto net_itr = nets.begin(); net_itr != nets.end(); ++net_itr) {
        dbNet* net = *net_itr;

        if (net == NULL)
            continue;

        if ((net != NULL) && (net->getSigType() != dbSigType::SIGNAL))
            continue;
    }



    ofstream outFile(fileName);

    if(!outFile.good()) {
        cout << "Invalid output file" << endl;
        return;
    }

    double avgFanouts = 0;
    double avgFanins = 0;
    double avgBbox = 0;
    double avgEdgelen = 0;

    double combRatio = 1.0 * numCombinational / numInsts;


    for(auto itr : onlyUseMasters) {
        outFile << "ONLY_USE " << itr.first << " " << itr.second << " " << 1.0* itr.second / numInsts << endl;
    }

    int devider = 0;

    for(int x=0; x <= maxFanins; x++) {
        outFile << "FANIN " << x << " " << faninDistribution[x] << " " << 1.0 * faninDistribution[x] / numInsts << endl;
        avgFanins += x * faninDistribution[x];
        devider += faninDistribution[x];
    }

    avgFanins /= devider;
    devider=0;
    
    for(int x=0; x <= maxFanouts; x++) {
        outFile << "FANOUT " << x << " " << fanoutDistribution[x] << " " << 1.0 * fanoutDistribution[x] / numInsts << endl;
        avgFanouts += x * fanoutDistribution[x];
        devider += fanoutDistribution[x];
    }

    avgFanouts /= devider;
    devider=0;

    for(int x=0; x <= maxBbox; x++) {
        outFile << "NETBBOX " << x << " " << bboxDistribution[x] << " " << 1.0 * bboxDistribution[x] / numInsts << endl;
        avgBbox += x * bboxDistribution[x];
        devider += bboxDistribution[x];
    }

    avgBbox /= devider;
    devider = 0;

    for(int x=0; x <= maxEdgelen; x++) {
        outFile << "WINDOW " << x << " " << edgelenDistribution[x] << " " << 1.0 * edgelenDistribution[x] / numEdges << endl;
        avgEdgelen += x * edgelenDistribution[x];
        devider += edgelenDistribution[x];
    }

    avgEdgelen /= devider;
    devider = 0;
    outFile << "CELL_COUNT " << numInsts << endl;
    outFile << "COMB_RATIO " << combRatio << endl;
    outFile << "IPAD_COUNT " << numInBTerms << endl;
    outFile << "OPAD_COUNT " << numOutBTerms << endl;
    outFile << "AVG_FANIN " << avgFanins << endl;
    outFile << "AVG_FANOUT " << avgFanouts << endl;
    outFile << "AVG_BBOX " << avgBbox << endl;
    outFile << "AVG_EDGELEN " << avgEdgelen << endl;
    outFile << "AVG_GATE_DELAY " << getAvgGateDelay() << endl;
    outFile << "AVG_TOPO_ORDER " << analyzeTopologicalOrder() << endl;

    outFile.close();
    //unordered_map<int,int> fanoutDist = makeExpDistribution(1, 50, 2.5, 10000); 
    //unordered_map<int,int> bboxDist = makeExpDistribution(0, 20, 0.3, 10000); 
    //unordered_map<int,int> faninDist = makeFoissonDistribution(1, 6, 2.5, 10000);


    cout << "extract spec finished" << endl;    
    //analyzeTopoOrder();

}


}
