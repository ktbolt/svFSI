interface

  function create_simulation_c() bind(C, name="create_simulation")
    use iso_c_binding
    implicit none
    type(c_ptr) :: create_simulation_c
  end function

  subroutine create_aux_data_c(simulation) bind(C, name="create_aux_data")
    use iso_c_binding
    type(c_ptr), value :: simulation
  end subroutine

  subroutine get_mesh_data_c(simulation, mesh_name, mesh) bind(C, name="get_mesh_data")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
    !character(len=*, kind=C_CHAR), intent(in) :: mesh_name(*)
    type(c_ptr), value :: mesh_name 
    type(c_ptr), value :: mesh 
  end subroutine

  subroutine get_node_coords_c(simulation, num_coords, coords) bind(C, name="get_nodal_coordinates")
    use iso_c_binding
    type(c_ptr), value :: simulation
    integer(c_int), intent(out) :: num_coords
    type(c_ptr) :: coords 
  end subroutine

  !----------------
  ! get_node_ids_c
  !----------------
  !
  subroutine get_node_ids_c(simulation, num_ids, node_ids) bind(C, name="get_node_ids")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
    integer(c_int), intent(out) :: num_ids 
    type(c_ptr) :: node_ids
  end subroutine

  subroutine get_num_meshes_c(simulation, num_solid_meshes, num_fluid_meshes) bind(C, name="get_num_meshes")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
    integer(c_int), intent(out) :: num_solid_meshes
    integer(c_int), intent(out) :: num_fluid_meshes 
  end subroutine

  subroutine get_general_params_c(simulation, params) bind(C, name="get_general_params")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
    type(c_ptr), value :: params
  end subroutine

  subroutine get_solution_params_c(simulation, params) bind(C, name="get_solution_params")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
    type(c_ptr),value :: params
  end subroutine

  subroutine load_mesh_c(simulation) bind(C, name="load_mesh")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
  end subroutine

  subroutine read_parameters_c(simulation, str) bind(C, name="read_parameters")
    use iso_c_binding
    implicit none
    type(c_ptr), value :: simulation
    character(len=1, kind=C_CHAR), intent(in) :: str(*)
  end subroutine

  subroutine set_node_coords_c(simulation, num_coords, coords) bind(C, name="set_nodal_coordinates")
    use iso_c_binding
    type(c_ptr), value :: simulation
    integer(c_int), value :: num_coords
    type(c_ptr), value :: coords 
  end subroutine

end interface
