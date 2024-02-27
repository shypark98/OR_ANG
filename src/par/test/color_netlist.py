import re

with open('/home/shyeon/research/ANG/bench/ng45/aes_cipher_top/enc/util_0.50_ar_1.0_rmax_10_pdn_sparse_clk_0/aes_buf_removed.v', 'r') as file:
    lines = file.readlines()

output_lines = []
# 정규 표현식 패턴
pattern_master = r'([A-Z]_[A-Z]\d)'
pattern_module = r'\bmodule\s+(\w+)\s*\('

module_cnt = 0
inst_cnt = 1
current_module = str()

for line in lines:
        
    match_module = re.findall(pattern_module, line)
    match_master = re.findall(pattern_master, line)
    
    if match_module:
        strings = line.split()
        module = strings[1]
        module = module.split("(")[0]
        current_module = module
        module_cnt += 1

    if match_master:
        strings = line.split()
        instance = strings[1]
        instance = instance.split("(")[0]
        inst_cnt += 1
        
        #output_lines.append(f"_{inst_cnt}_ {cluster_cnt} {cluster_cnt}\n")
        output_lines.append(f"{current_module}/{instance} {module_cnt} {module_cnt}\n")
        #print(f"{inst_cnt} {cluster_cnt}")

with open('partition_aes-tcl-netlist.part', 'w') as output_file:
    output_file.writelines(output_lines)
