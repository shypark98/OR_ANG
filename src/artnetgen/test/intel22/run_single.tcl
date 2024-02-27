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

artnetgen_create_spec \
    -num_insts 23940 \
    -num_primary_in 71 \
    -num_primary_out 514 \
    -comb_ratio 0.907602 \
    -avg_net_degree 2.15815 \
    -avg_bbox 1.42673 \
    -avg_topo_order 14.4512 \
    -cell_list /home/shyeon/research/ANG/OR_ANG/src/artnetgen/test/intel22/spec_files_from_def/onlyUseCell.list \
    -out_file /home/shyeon/research/ANG/OR_ANG/src/artnetgen/test/intel22/spec_files_from_def/ang_cell_keccak_bbox_14.spec

exit

artnetgen_init  -top_module ang_cell_keccak_bbox_14 \
                -spec_file  /home/shyeon/research/ANG/OR_ANG/src/artnetgen/test/intel22/spec_files_from_def/ang_cell_keccak_bbox_14.spec \
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


artnetgen_write_verilog -out_file ./netlist_from_spec_files/ang_cell_keccak_bbox_14.v 
# optional (default : 0.1 [ns])
# artnetgen_set_parameter -avg_gate_delay __AVG_GATE_DELAY__
artnetgen_write_sdc -out_file ./netlist_from_spec_files/ang_cell_keccak_bbox_14.sdc
exit
