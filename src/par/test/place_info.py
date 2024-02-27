def parse_def_and_write_output(def_path, output_path):
    with open(def_path, 'r') as def_file:
        lines = def_file.readlines()
                        
    # Flag to track whether we are in the COMPONENTS section
    in_components_section = False
    components = []
    
    UX = str()
    UY = str()

    for line in lines:
    # Check if we're entering the COMPONENTS section
        if 'DIEAREA' in line:
            parts = line.split()
            UX = parts[6]
            UY = parts[7]

        if line.startswith("COMPONENTS"):
            in_components_section = True
    # Check if we're leaving the COMPONENTS section
        elif line.startswith("END COMPONENTS"):
            in_components_section = False
    # If we're in the COMPONENTS section,
    # parse the line
        elif in_components_section:
            parts = line.split()
            if len(parts) >= 6:
                instance_name = parts[1]
                x_coord = parts[6]
                y_coord = parts[7]
                norm_x = float(x_coord) / float(UX)
                norm_y = float(y_coord) / float(UY)
                components.append((instance_name, norm_x, norm_y))

    # Write the parsed information to the output file
    with open(output_path, 'w') as output_file:
        for instance_name, x, y in components:
            output_file.write(f"{instance_name}, {x}, {y}\n")

# Usage
parse_def_and_write_output('/home/shyeon/research/ANG/bench/ng45/aes_cipher_top/def/util_0.50_ar_1.0_rmax_10_pdn_sparse_clk_0/preRoute.def', 'placement_flag.txt') 

