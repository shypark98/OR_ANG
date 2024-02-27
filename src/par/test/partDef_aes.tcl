source "helpers.tcl"
source flow_helpers.tcl

read_liberty "Nangate45/Nangate45_typ.lib"
read_lef Nangate45/Nangate45.lef
read_def /home/shyeon/research/ANG/bench/ng45/aes_cipher_top/def/util_0.50_ar_1.0_rmax_10_pdn_sparse_clk_0/preRoute.def
#read_verilog /home/shyeon/research/ANG/bench/ng45/aes_cipher_top/enc/util_0.50_ar_1.0_rmax_10_pdn_sparse_clk_0/aes.v
link_design aes_cipher_top

read_sdc /home/shyeon/research/ANG/bench/ng45/aes_cipher_top/netlist/aes_cipher_top.sdc

######

set part_file [make_result_file partition_aes_t_2.part]
set graph_file [make_result_file partition_aes_t_2.graph]
set paths_file [make_result_file partition_aes_t_2.paths]
set part_v [make_result_file partition_aes_t_2.v]

triton_part_design -solution_file $part_file -num_parts 4 \
    -placement_flag true
#    -hypergraph_file $graph_file 
#    -paths_file $paths_file

write_partition_verilog $part_v

#diff_files partition_gcd.graphok $graph_file
#diff_files partition_gcd.pathsok $paths_file
#diff_files partition_aes.partok $part_file
#diff_files partition_aes.vok $part_v
exit

