with open('results/partition_aes_t-tcl.part', 'r') as file:
        lines = file.readlines()

output_lines = []

for line in lines:
    parts = line.split()
    instance = parts[0]
    cluster_id = parts[1]
    color = int(parts[1]) + 1
    print(instance, cluster_id, color)
    output_lines.append(f"{instance} {cluster_id} {color}\n")

with open('partition_aes-tcl-color_2.part', 'w') as output_file:
    output_file.writelines(output_lines)
