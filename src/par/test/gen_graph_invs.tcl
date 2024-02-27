# This script was written and developed by ABKGroup students at UCSD. 
# However, the underlying commands and reports are copyrighted by Cadence. 
# We thank Cadence for granting permission to share our research to help 
# promote and foster the next generation of innovators.

### Write out the node details in the node file ###
proc print_node_header { fp } {
  puts $fp "Name,Type,Master,Height,Width, Pins, Nets"
}

proc print_node { fp nPtr } {
  set name [concat {*}[dbget ${nPtr}.name]]
  set type [dbget ${nPtr}.objType]
  set master "NA"
  set height "0.0"
  set width "0.0"
  set pins "NA"
  set nets "NA"
  if { $type == "inst" } {
    set master [dbget ${nPtr}.cell.name]
    set height [dbget ${nPtr}.cell.size_y]
    set width [dbget ${nPtr}.cell.size_x]
    set pins [get_db [get_cell [dbget ${nPtr}.name]] .pins.name]
    set nets [get_db [get_cell [dbget ${nPtr}.name]] .pins.net.name]
  }
  puts $fp "$name,$type,$master,$height,$width"
}

proc print_edge_header { fp } {
  puts $fp "Source,Sink,Weight,Net"
}

proc get_net_fanout { nPtr } {
  set instTerm_count [llength [dbget ${nPtr}.instTerms.isInput 1 -e]]
  set bTerm_count [llength [dbget ${nPtr}.terms.direction output -e]]
  return [expr $instTerm_count + $bTerm_count]
}

proc get_net_source { nPtr {isPin 0} } {
  set term_source [dbget [dbget ${nPtr}.terms.direction input -p ].name -e]
  if { $isPin } {
    set inst_source [dbget [dbget ${nPtr}.instTerms.isOutput 1 -p ].name -e]
  } else {
    set inst_source [dbget [dbget ${nPtr}.instTerms.isOutput 1 -p \
        ].inst.name -e]
  }
  
  if { $term_source != "" } {
    return [concat {*}$term_source]
  } elseif { $inst_source != "" } {
    return [concat {*}$inst_source]
  } else {
    set net_name [dbget ${nPtr}.name]
    puts "Error: Check source of net:$net_name"
  }
  return
}

proc get_net_sinks { nPtr {isPin 0} } {
  set sinks {}
  
  ## Add term sinks ##
  foreach outputTermPtr [dbget ${nPtr}.terms.direction output -p -e] {
    set sink_name [dbget ${outputTermPtr}.name]
    lappend sinks [concat {*}$sink_name]
  }

  ## Add inst sinks ##
  foreach inputIntTermPtr [dbget ${nPtr}.instTerms.isInput 1 -p -e] {
    if { $isPin } {
      set sink_name [dbget ${inputIntTermPtr}.name]
    } else {
      set sink_name [dbget ${inputIntTermPtr}.inst.name]
    }
    lappend sinks [concat {*}$sink_name]
  }
  return [lsort -unique $sinks]
}

proc print_edge { fp nPtr {fanout_threshold 50}} {
  set net_fanout [get_net_fanout $nPtr]
  set net_name [concat {*}[dbget ${nPtr}.name]]
  
  # Ignore power nets or high fanout nets #
  if { [dbget ${nPtr}.isPwrOrGnd] || $net_fanout > $fanout_threshold || 
        $net_fanout == 0 } {
    return
  }

  set edge_weight [expr 1.0/$net_fanout]
  set source_name [get_net_source $nPtr]
  set sinks [get_net_sinks $nPtr]
  if { [llength $sinks] != $net_fanout } {
    puts "Error: Fanout list of $net_name is not unique."
  }

  foreach sink $sinks {
    puts $fp "$source_name,$sink,$edge_weight,$net_name"
  }
}

proc write_graph { {file_name ""} } {
  # This function writes out the graph in the form of nodes and edges in 
  # csv format. The CSV format for the node file is as follows:
  # Name, Type, Master
  # The CSV format for the edge file is as follows:
  # Source, Sink, Weight, Net
  # The output files are <file_name>_nodes.csv and <file_name>_edges.csv
  # Usage: First source the file and the use the below command
  # write_graph <file_name>
  # If file_name is not specified, then the name of the top cell is used.
  
  if {$file_name == ""} {
    set file_name [dbget top.name]
  }

  set node_file "${file_name}_nodes.csv"
  set edge_file "${file_name}_edges.csv"
  set node_fp [open $node_file w]
  set edge_fp [open $edge_file w]

  print_node_header $node_fp
  print_edge_header $edge_fp

  # Write out the instance details as nodes #
  foreach nPtr [dbget top.insts ] {
    print_node $node_fp $nPtr
  }

  # Write out the terminal details as nodes #
  foreach nPtr [dbget top.terms ] {
    print_node $node_fp $nPtr
  }

  # Write out the net details as edges #
  foreach nPtr [dbget top.nets ] {
    print_edge $edge_fp $nPtr
  }

  close $node_fp
  close $edge_fp
}

proc print_hyperedge { fp nPtr {isPin 0}} {
  set net_name [concat {*}[dbget ${nPtr}.name]]
  set source_name [get_net_source $nPtr $isPin]
  set sink_names [get_net_sinks $nPtr $isPin]
  set sinks [join $sink_names " "]
  puts $fp "$net_name $source_name $sinks"
}

proc write_hypergraph { {file_name ""} {isPin 0}} {
  if {$file_name == ""} {
    set file_name [dbget top.name]
  }

  set hgr_file "${file_name}.hgr"
  set hgr_fp [open $hgr_file w]
  foreach nPtr [dbget top.nets] {
    if { [dbget ${nPtr}.isPwrOrGnd] || [get_net_fanout $nPtr] == 0 } {
      continue
    }
    print_hyperedge $hgr_fp $nPtr $isPin
  }
  close $hgr_fp
}

proc write_macro_location { {file_name ""} } {
  if {$file_name == ""} {
    set file_name [dbget top.name]
  }

  set db_unit [dbget head.dbUnits]
  set macro_file "${file_name}_macro.csv"
  set macro_fp [open $macro_file w]
  puts $macro_fp "Name,llx,lly"
  foreach macro_ptr [dbget top.insts.cell.subClass block -p2] {
    set macro_name [concat {*}[dbget ${macro_ptr}.name]]
    set macro_x [expr [dbget ${macro_ptr}.pt_x] * $db_unit]
    set macro_y [expr [dbget ${macro_ptr}.pt_y] * $db_unit]
    # Convert macro_x and macro_y to integer #
    set macro_x [expr int($macro_x)]
    set macro_y [expr int($macro_y)]
    puts $macro_fp "$macro_name,$macro_x,$macro_y"
  }
  close $macro_fp
}

proc highlight_only_clusters { file_name } {
  # This function highlights the clusters in the design.
  # The input file is the cluster file which has the following format:
  # cluster_name, color
  
  # First dehighlight everything #
  dehighlight -all

  if { [file exists $file_name] == 0 } {
    puts "Error: $file_name does not exist."
    return
  }
  set fp [open $file_name r]
  while { [gets $fp line] >= 0 } {
    set cluster_name [lindex $line 0]
    set color [lindex $line 1]
    highlight $cluster_name -color $color
  }
}

proc highlight_clusters { file_name {highlight_io 0} {sortedCluster 0}} {
  # This function highlights the clusters in the design. 
  # The input file is the cluster file which has the following format:
  # Instance / Terminal Name, Cluster ID, Color
  # This function also creates groups for each cluster.
  # Usage: First source the file and the use the below command
  # highlight_clusters <cluster_file> <highlight_io>
  # If highlight_io is set to 1, then the IOs are also highlighted.
  # If highlight_io is set to 0, then only the instances are highlighted.

  # First remove all the existing groups #
  foreach group [dbget top.fplan.groups.name cluster* -e ] {
    deleteInstGroup $group
  }

  # Dehighlight all the instances #
  dehighlight -all

  set fp [open $file_name r]
  set previous_cluster_id -1
  while { [gets $fp line] >= 0 } {
    set inst_name [lindex $line 0]
    set cluster_id [lindex $line 1]
    set color [lindex $line 2]

    if { $sortedCluster == 1 && $cluster_id != $previous_cluster_id } {
      createInstGroup cluster_${cluster_id}
    } elseif { [dbget top.fplan.groups.name cluster_${cluster_id} -e] == "" } {
      createInstGroup cluster_${cluster_id}
    }

    if { [dbget top.insts.name $inst_name -e] != "" } {
      addInstToInstGroup cluster_${cluster_id} $inst_name
      highlight $inst_name -index $color
    }
    
    set previous_cluster_id $cluster_id

    if { $highlight_io == 0 } {
      continue
    }

    highlight $inst_name -color $color
  }

  close $fp
}

proc write_placement_info_helper { fp nPtr } {
  # This is a helper function for the write_placement_info function.
  set name [concat {*}[dbget ${nPtr}.name]]
  set type [dbget ${nPtr}.objType]
  set master "NA"
  if { $type == "inst" } {
    set master [dbget ${nPtr}.cell.name]
  }
  set x [dbget ${nPtr}.pt_x]
  set y [dbget ${nPtr}.pt_y]
  puts $fp "$name,$type,$master,$x,$y"
}

proc write_placement_info { {file_name ""} } {
  # This function writes out the placement information in csv format. 
  # The csv format is as follows:
  # Instance Name, Inst/Term, Cell Master for Instances, X, Y
  # Usage: First source this script then use the below command
  # write_placement_info <file_name>
  # If file_name is not specified, then the name of the top cell is used.

  if {$file_name == ""} {
    set file_name [dbget top.name]
    set file_name "${file_name}_placement.csv"
  }
  
  set fp [open $file_name w]
  puts $fp "Name,Type,Master,X,Y"
  
  foreach instPtr [dbget top.insts ] {
    write_placement_info_helper $fp $instPtr
  }

  foreach termPtr [dbget top.terms] {
    write_placement_info_helper $fp $termPtr
  }

  close $fp
}

proc write_blob_place_exp_info { } {
  # {db_dir "./blo_placement"}
  # exec mkdir $db_dir -p
  # exec cd $db_dir

  # Write out the placement def file #
  set design [dbget top.name]
  deleteBufferTree
  saveNetlist ${design}_buf_removed.v
  defOut -netlist ${design}_placed.def

  # Generate the graph file
  write_graph

  # Generate the hypergraph file
  write_hypergraph

  # Write out the placement file
  write_placement_info

  # exec cd -

  # If the design contains macro then write out the macros
  if { [llength [dbget top.insts.cell.subClass block -p -e]] > 0 } {
    write_macro_location
  }
}

proc extract_clustered_netlist_details { {file_name ""} } {
  # This function extracts the clustered netlist details
  # Writes out three files
  
  #  1. cluster files: <file_name>_cluster.csv
  #                     cluster_name, width, height
  
  #  2. pin file: <file_name>_pin.csv
  #                pin_name, location

  #  3. net file: <file_name>_net.csv
  #                net_name, source, sink1, sink2, ...

  #  4. net weight file: <file_name>_net_weight.csv
  #                       net_name, weight

  if {$file_name == ""} {
    set file_name [dbget top.name]
  }
  
  ## First write out the cluster file ##
  set cluster_file "${file_name}_cluster.csv"
  set cluster_fp [open $cluster_file w]
  puts $cluster_fp "Name,Width,Height,isFixed"
  foreach inst_ptr [dbget top.insts] {
    set inst_name [concat {*}[dbget ${inst_ptr}.name]]
    set width [dbget ${inst_ptr}.box_sizex]
    set height [dbget ${inst_ptr}.box_sizey]
    set pstat [dbget ${inst_ptr}.pStatus]
    set isFixed 0
    if { $pstat == "fixed" } {
      set isFixed 1
    }
    puts $cluster_fp "$inst_name,$width,$height,$isFixed"
  }
  close $cluster_fp

  ## Next write out the pin file ##
  set pin_file "${file_name}_pin.csv"
  set pin_fp [open $pin_file w]
  puts $pin_fp "Name,X,Y"

  foreach term_ptr [dbget top.terms] {
    set term_name [concat {*}[dbget ${term_ptr}.name]]
    set x [dbget ${term_ptr}.pt_x]
    set y [dbget ${term_ptr}.pt_y]
    puts $pin_fp "$term_name,$x,$y"
  }

  close $pin_fp

  ## Next write out the net file ##
  set net_file "${file_name}_net.csv"
  set net_fp [open $net_file w]
  foreach net_ptr [dbget top.nets.isPwrOrGnd 0 -p] {
    set net_name [concat {*}[dbget ${net_ptr}.name]]
    set source [get_net_source $net_ptr]
    set sinks [get_net_sinks $net_ptr]
    set sinks [join $sinks ","]
    if { $source == "" && $sinks == "" } {
      continue
    } elseif { $source == "" } {
      puts $net_fp "$net_name,$sinks"
    } elseif { $sinks == "" } {
      puts $net_fp "$net_name,$source"
    } else {
      puts $net_fp "$net_name,$source,$sinks"
    }
  }
  close $net_fp
}
