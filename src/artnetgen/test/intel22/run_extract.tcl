read_lef /home/shyeon/pdk/intel22/tech_lef/intel22.lef
read_def /home/shyeon/research/ANG/exp_dir/comp_topo/run_dir/fan_keccak_bbox_14/def/util_0.60_ar_1.0_rmax_6_pdn_sparse_clk_0/preRoute.def
#read_def /home/shyeon/research/ANG/comp_topo/run_dir/test_circuit/def/util_0.70_ar_1.0_rmax_6_pdn_sparse_clk_0/preRoute.def

# AVG_TOPO_ORDER:   average topological order of instances in reg2reg / in2reg / reg2out 
#                   this tool estimates the synthesis clock period of top module as avg_topo_order * avg_gate_delay [ns]
# AVG_GATE_DELAY:   average gate delay for given target technology (ns)
#                   we will delete this input arg, but it is required at this version....
#                   just give an estimated value 
# 
# optional (default : 0.1 [ns])
#artnetgen_set_parameter -avg_gate_delay 1.0
artnetgen_write_spec -out_file ./spec_files_from_def/from_ang/fan_keccak_bbox_14.spec
exit

