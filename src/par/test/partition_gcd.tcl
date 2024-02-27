source "helpers.tcl"
source flow_helpers.tcl

read_liberty "Nangate45/Nangate45_typ.lib"
read_lef Nangate45/Nangate45.lef
read_verilog gcd.v
link_design gcd

read_sdc gcd_nangate45.sdc

######

set part_file [make_result_file partition_gcd_t.part]
set graph_file [make_result_file partition_gcd_t.graph]
set paths_file [make_result_file partition_gcd_t.paths]
set part_v [make_result_file partition_gcd_t.v]

triton_part_design -solution_file $part_file -num_parts 4
#    -hypergraph_file $graph_file \
#    -paths_file $paths_file

write_partition_verilog $part_v

#diff_files partition_gcd.graphok $graph_file
#diff_files partition_gcd.pathsok $paths_file
diff_files partition_gcd.partok $part_file
diff_files partition_gcd.vok $part_v
