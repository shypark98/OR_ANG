
sta::define_cmd_args "artnetgen_init" {
    [-verbose] [-hierarchy_flag] [-spl_file] [-spec_file] [-out_file] [-top_module]}

sta::define_cmd_args "artnetgen_write_verilog" {
    [-out_file] 
}

sta::define_cmd_args "artnetgen_run" {
    [-log_file]
}

sta::define_cmd_args "artnetgen_write_sdc" {
    [-out_file] [-io_delay] [-uncertainty]
}

sta::define_cmd_args "artnetgen_write_spec" {
    [-out_file] 
}

sta::define_cmd_args "artnetgen_create_spec" {
    [-num_insts] [-num_primary_in] [-num_primary_out] [-comb_ratio] \
    [-avg_net_degree] [-avg_bbox] [-avg_topo_order] \
    [-hierarchy_flag] [-cell_list] [-out_file]
}

sta::define_cmd_args "artnetgen_set_parameter" {
    [-avg_gate_delay] [-cell_mapping_mode] [-dont_use] 
}


proc artnetgen_set_parameter { args } {
   sta::parse_key_args "artnetgen_set_parameter" args \
    keys { -avg_gate_delay -cell_mapping_mode -dont_use} flags {}

   if { [info exists keys(-avg_gate_delay)] } {
        set avg_gate_delay $keys(-avg_gate_delay)
        artnetgen_set_avg_gate_delay_cmd $avg_gate_delay
   }

   if { [info exists keys(-cell_mapping_mode)] } {
        set mode $keys(-cell_mapping_mode)
   }


   if { [info exists keys(-dont_use)] } {
        set dont_use $keys(-dont_use)
        artnetgen_set_dont_use $dont_use
   }
}


# added by DYK
proc artnetgen_write_verilog { args } {
   sta::parse_key_args "artnetgen_write_verilog" args \
    keys { -out_file } flags {}
 
    if { [info exists keys(-out_file)] } {
        set out_file $keys(-out_file)
        artnetgen_write_verilog_cmd $out_file
    }

}
# added by DYK
proc artnetgen_write_sdc { args } {
   sta::parse_key_args "artnetgen_write_sdc" args \
    keys { -out_file -io_delay -uncertainty } flags {}
 
    set io_delay 0.0
    set uncertainty 0.0

    if { [info exists keys(-io_delay)] } {
        set io_delay $keys(-io_delay)
    }

    if { [info exists keys(-uncertainty)] } {
        set uncertainty $keys(-uncertainty)
    }

    if { [info exists keys(-out_file)] } {
        set out_file $keys(-out_file)
        artnetgen_write_sdc_cmd $out_file
    }

}

proc artnetgen_write_spec { args } {
    sta::parse_key_args "artnetgen_write_spec" args \
        keys { -out_file } flags {}

    if { [info exists keys(-out_file)] } {
        set out_file $keys(-out_file)
        artnetgen_write_spec_cmd $out_file
    }
}


proc artnetgen_create_spec { args } {
sta::parse_key_args "artnetgen_create_spec" args \
        keys { -num_insts -num_primary_in -num_primary_out -comb_ratio -avg_net_degree -avg_bbox -avg_topo_order -hierarchy_flag -cell_list -out_file } flags {}
    
    set hierarchy_flag false

    if { ![info exists keys(-num_insts)] } {
    }
    if { ![info exists keys(-num_primary_in)] } {
    }
    if { ![info exists keys(-num_primary_out)] } {
    }
    if { ![info exists keys(-comb_ratio)] } {

    }
    if { ![info exists keys(-avg_net_degree)] } {

    }
    if { ![info exists keys(-avg_bbox)] } {

    }
    if { ![info exists keys(-avg_topo_order)] } {

    }
    if { [info exists keys(-hierarchy_flag)] } {
        set hierachy_flag $keys(-hierarchy_flag)
    }
    if { ![info exists keys(-cell_list)] } {

    }
    if { ![info exists keys(-out_file)] } {

    }

    set num_insts $keys(-num_insts)
    set num_primary_in $keys(-num_primary_in)
    set num_primary_out $keys(-num_primary_out)
    set comb_ratio $keys(-comb_ratio)
    set avg_net_degree $keys(-avg_net_degree)
    set avg_bbox $keys(-avg_bbox)
    set avg_topo_order $keys(-avg_topo_order)
    set hierachy_flag $hierarchy_flag
    set cell_list $keys(-cell_list)
    set out_file $keys(-out_file)

    sta::check_positive_integer "-num_insts" $num_insts
    sta::check_positive_integer "-num_primary_in" $num_primary_in
    sta::check_positive_integer "-num_primary_out" $num_primary_out
    sta::check_positive_float "-comb_ratio" $comb_ratio
    sta::check_positive_float "-avg_bbox" $avg_bbox
    sta::check_positive_float "-avg_topo_order" $avg_topo_order

    artnetgen_create_spec_cmd $num_insts $num_primary_in $num_primary_out $comb_ratio $avg_net_degree $avg_bbox $avg_topo_order $cell_list $out_file
}


proc artnetgen_print_masters { } {
    artnetgen_print_masters_cmd
}

proc artnetgen_init { args } {
    sta::parse_key_args "artnetgen_init" args \
        keys { -verbose -hierarchy_flag -spl_file -spec_file -top_module } flags {}
    
    set spl_file "./writeSpec.log"
    if { [info exists keys(-top_module)] } {
        set top_module $keys(-top_module)
        artnetgen_set_topmodule_cmd $top_module
    }
    if { [info exists keys(-verbose)] } {
        set verbose $keys(-verbose)
        sta::check_positive_integer "-verbose" $verbose
        artnetgen_set_verbose_cmd $verbose
    }

    if { [info exists keys(-spl_file)] } {
        set spl_file $keys(-spl_file)
        artnetgen_set_spl_file_cmd $spl_file
    }

    if { [info exists keys(-spec_file)] } {
        set spec_file $keys(-spec_file)
        artnetgen_set_spec_file_cmd $spec_file
    } 
    
    if { [info exists keys(-hierarchy_flag)] } {
        set spec_file $keys(-hierarchy_flag)
        artnetgen_set_hierarchy_flag_cmd $spec_file
    } 

    artnetgen_init_cmd 
}


proc artnetgen_run { args } {
    # The ArtNetGen will write verilog
    sta::parse_key_args "artnetgen_init" args \
        keys { -log_file } flags {}

    set log_file "./artnetgen.log"
    if { [info exists keys(-log_file)] } {
        set log_file $keys(-log_file)
    }
    artnetgen_set_log_file_cmd $log_file
    artnetgen_run_cmd
    #artnetgen_clear_cmd 
}








