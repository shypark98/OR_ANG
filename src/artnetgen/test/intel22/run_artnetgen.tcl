set top_module __TOP_MODULE__
set num_insts __NUM_INSTS__
set num_primary_in __NUM_PRIMARY_IN__
set num_primary_out __NUM_PRIMARY_OUT__
set comb_ratio __COMB_RATIO__
set avg_net_degree __AVG_NET_DEGREE__
set avg_bbox __AVG_BBOX__
set avg_topo_order __AVG_TOPO_ORDER__
set verbose __VERBOSE__
set result_path __RESULT_PATH__
set org_module __ORG_MODULE__

read_lef /home/shyeon/pdk/intel22/tech_lef/intel22.lef
#read_lef /home/shyeon/pdk/intel22/tech_lef/intel22.lef
# NUM_INSTS         :   the number of instances (count only comb/sequ logic gates)
# NUM_PRIMARY_IOS   :   the number of primary inputs and outputs 
# COMB_RATIO        :   the ratio of combinational logic gates over NUM_INSTS
# AVG_NET_DEGREE    :   the average of net degree (~=average fanout)
# AVG_BBOX          :   the average size of net bounding box 
#                       this parameter can be measured after placement
#                       it will be normalized by bin_size 
#                       * bin_size = die_size/bin_count
#                       * bin_count = sqrt(NUM_INSTS)
# CELL_LIST         :   list of standard cell types that you want to use
#                       specify the cell count (refer onlyUseCell.list file)
# SPEC_PATH         :   the path of .spec file to store
# AVG_TOPO_ORDER    :   average topological order of instances in reg2reg / in2reg / reg2out 
#                       this tool estimates the synthesis clock period of top module as avg_topo_order * avg_gate_delay [ns]
# AVG_GATE_DELAY    :   average gate delay for given target technology (ns)
#                       we will delete this input arg, but it is required at this version....
#                       just give an estimated value 


# optional (default : 0.1 [ns])
# artnetgen_set_parameter -avg_gate_delay __AVG_GATE_DELAY__

#artnetgen_create_spec \
    -num_insts 11591 \
    -num_primary_in 53 \
    -num_primary_out 53 \
    -comb_ratio 0.90 \
    -avg_net_degree 2.18 \
    -avg_bbox 0.82 \
    -avg_topo_order 8.33 \
    -cell_list /home/shyeon/research/ANG/bench_spec/${org_module}/onlyUseCell.list \
    -out_file ${result_path}/${top_module}.spec

artnetgen_init  -top_module ${top_module} \
                -spec_file  ${result_path}/${top_module}.spec \
                -verbose 6 
# optional (default : 0.1 [ns])
# artnetgen_set_parameter -avg_gate_delay __AVG_GATE_DELAY__

# set_dont_use clock gate cells (example)
artnetgen_set_parameter -dont_use CLKGATETST_X1
artnetgen_set_parameter -dont_use CLKGATETST_X2
artnetgen_set_parameter -dont_use CLKGATETST_X4
artnetgen_set_parameter -dont_use CLKGATETST_X8
# check the list of available masters
artnetgen_print_masters

# if you finished setting parameters, run
artnetgen_run


artnetgen_write_verilog -out_file ${result_path}/${top_module}.v 
# optional (default : 0.1 [ns])
# artnetgen_set_parameter -avg_gate_delay __AVG_GATE_DELAY__
artnetgen_write_sdc -out_file ${result_path}/${top_module}.sdc
exit
