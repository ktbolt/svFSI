
!----------------------
! simulation_interface
!----------------------
! The simulation_interface module provides an interface to the c++ Simulation class.
!
! This module stores all data needed by svFSI Fortran code.
!
module simulation_interface 
  use iso_c_binding
  !use simulation_params_interface
  public :: simulation_cpp_interface 

  include "simulation_interface_def.f90"

  type MeshType
    integer mesh_id
  end type 

  ! Declare the module data members and methods. 
  type SimulationClass

    private
    type(c_ptr) :: Simulation_object

    integer, public :: num_meshes

    contains
      procedure :: create_aux_data
      procedure :: get_general_params
      procedure :: get_mesh_data
      procedure :: get_solution_params
      procedure :: get_node_coords
      procedure :: get_node_ids 
      procedure :: get_num_meshes
      procedure :: load_mesh 
      procedure :: read_parameters
      procedure :: set_node_coords

  end type

  !-------------------------------
  ! simulation_general_parameters
  !-------------------------------
  ! Define a type to return general parameters value from c++.
  ! 
  ! String parameters are defined using two variables, one that is 
  ! a fixed length char array used to return a blank padded string 
  ! from c++, the other is a dynamic string without trailing blanks. 
  ! 
  ! Note: the order of the member data must match the struct definition in simulation_wrap.h.
  !
  type :: simulation_general_parameters
  !type, bind(c) :: simulation_general_parameters
    integer(c_int) :: debug 
    integer(c_int) :: increment_in_saving_restart_files
    character(len=100) :: name_prefix_of_saved_vtk_files_p
    !character(c_char) :: name_prefix_of_saved_vtk_files(100)
    integer(c_int) :: save_averaged_results
    character(len=100) :: save_results_in_folder_p
    !character(c_char) :: save_results_in_folder(100)
    integer(c_int) :: save_results_to_vtk_format
    character(len=100) :: searched_file_name_to_trigger_stop_p
    !character(c_char) :: searched_file_name_to_trigger_stop(100)
    integer(c_int) :: simulation_requires_remeshing 
    integer(c_int) :: start_saving_after_time_step 
    integer(c_int) :: verbose
    integer(c_int) :: warning 
    ! Put allocatable arrays at the end.
    character(len=:), allocatable :: name_prefix_of_saved_vtk_files
    character(len=:), allocatable :: save_results_in_folder
    character(len=:), allocatable :: searched_file_name_to_trigger_stop
  end type

  !-----------------
  ! simulation_mesh
  !-----------------
  ! Define a type to return mesh parameters value from c++.
  !
  ! Note: the order of the member data must match the struct definition in simulation_wrap.h.
  !
  type :: simulation_mesh_data
    integer(c_int) :: num_coords;
    type(c_ptr) c_coords
    ! Put allocatable arrays at the end, these are not set in C++.
    real(c_double), pointer :: coords(:,:)
  end type

  !--------------------------------
  ! simulation_solution_parameters
  !--------------------------------
  ! Define a type to return solution parameters value from c++.
  !
  ! Note: the order of the member data must match the struct definition in simulation_wrap.h.
  !
  type, bind(c) :: simulation_solution_parameters
    integer(c_int) :: continue_previous_simulation
    integer(c_int) :: number_of_initialization_time_steps
    integer(c_int) :: number_of_spatial_dimensions
    integer(c_int) :: number_of_time_steps
    real(c_double) :: spectral_radius_of_infinite_time_step
    real(c_double) :: time_step_size
  end type

  ! Simulation class constructor.
  interface simulation 
    procedure create_simulation 
  end interface

  ! Define the module methods. 
contains 

  subroutine create_aux_data(this)
    implicit none
    class(SimulationClass), intent(in) :: this
    call create_aux_data_c(this % Simulation_object)
  end subroutine 

  !-------------------
  ! create_simulation
  !-------------------
  ! Function to create a Simulation object.
  !
  function create_simulation()
    implicit none
    type(SimulationClass) :: create_simulation
    create_simulation % Simulation_object = create_simulation_c()
  end function

  !--------------------
  ! get_general_params
  !--------------------
  !
  subroutine get_general_params(this, params)
    implicit none
    class(SimulationClass), intent(in) :: this
    type(simulation_general_parameters), target :: params
    type(c_ptr) cptr
    cptr = c_loc(params)
    call get_general_params_c(this % Simulation_object, cptr)
    
    ! Create dynamic strings without white space.
    params % name_prefix_of_saved_vtk_files = trim(adjustl(params % name_prefix_of_saved_vtk_files_p))
    params % save_results_in_folder = trim(adjustl(params % save_results_in_folder_p))
    params % searched_file_name_to_trigger_stop = trim(adjustl(params % searched_file_name_to_trigger_stop_p))
  end subroutine 

  !---------------
  ! get_mesh_data
  !---------------
  ! Get the data for the mesh with the given name.
  !
  subroutine get_mesh_data(this, mesh_name, mesh)
    implicit none
    class(SimulationClass), intent(in) :: this
    character(len=*), target, intent(in) :: mesh_name
    type(simulation_mesh_data), target :: mesh

    type(c_ptr) mesh_name_cptr
    type(c_ptr) mesh_cptr
    integer i

    !print *, "[get_mesh_data] " 
    !print *, "[get_mesh_data] ##### get_mesh_data #####" 

    mesh_name_cptr = c_loc(mesh_name)
    mesh_cptr = c_loc(mesh)

    call get_mesh_data_c(this % Simulation_object, mesh_name_cptr, mesh_cptr)

    call c_f_pointer(mesh % c_coords, mesh % coords, [3, mesh % num_coords])

    !do i = 1, mesh % num_coords
    !  print *, "get_mesh_data: ", mesh % coords(:,i)
    !end do

    !print *, "[get_mesh_data] ###### mesh % num_coords: ", mesh % num_coords
  end subroutine 

  !-----------------
  ! get_node_coords
  !-----------------
  ! Get the nodal coordinates from the Simulation object.
  !
  subroutine get_node_coords(this, coords) 
    implicit none
    class(SimulationClass), intent(in) :: this
    integer(c_int) :: num_coords
    real(c_double), pointer :: coords(:,:)

    ! Declare void** cptr.
    type(c_ptr) :: cptr

    call get_node_coords_c(this % Simulation_object, num_coords, cptr) 

    call c_f_pointer(cptr, coords, [3, num_coords])
  end subroutine 

  !--------------
  ! get_node_ids
  !--------------
  ! Get the nodal IDs from the Simulation object.
  !
  subroutine get_node_ids(this, node_ids)
    implicit none
    class(SimulationClass), intent(in) :: this
    integer(c_int), pointer :: node_ids(:)
    integer(c_int) :: num_ids

    ! Declare void** cptr.
    type(c_ptr) :: cptr

    call get_node_ids_c(this % Simulation_object, num_ids, cptr)
    call c_f_pointer(cptr, node_ids, [num_ids])
  end subroutine 

  !----------------
  ! get_num_meshes
  !----------------
  !
  subroutine get_num_meshes(this, num_solid_meshes, num_fluid_meshes)
    implicit none
    class(SimulationClass), intent(in) :: this
    integer, intent(out) :: num_solid_meshes 
    integer, intent(out) :: num_fluid_meshes 
    call get_num_meshes_c(this % Simulation_object, num_solid_meshes, num_fluid_meshes) 
  end subroutine

  !---------------------
  ! get_solution_params
  !---------------------
  !
  subroutine get_solution_params(this, params)
    implicit none
    class(SimulationClass), intent(in) :: this
    type(simulation_solution_parameters), target :: params
    type(c_ptr) cptr
    cptr = c_loc(params)
    call get_solution_params_c(this % Simulation_object, cptr)
  end subroutine 

  !-----------
  ! load_mesh
  !-----------
  !
  subroutine load_mesh(this)
    implicit none
    class(SimulationClass), intent(in) :: this
    call load_mesh_c(this % Simulation_object)
  end subroutine 

  !-----------------
  ! read_parameters
  !-----------------
  !
  subroutine read_parameters(this, file_name)
    implicit none
    class(SimulationClass), intent(in) :: this
    character(len=*), intent(in) :: file_name

    ! Converting Fortran string to C string.
    character(len=1, kind=C_CHAR) :: c_file_name(len_trim(file_name) + 1)
    integer :: N, i
    N = len_trim(file_name)
    do i = 1, N
        c_file_name(i) = file_name(i:i)
    end do
    c_file_name(N + 1) = C_NULL_CHAR

    call read_parameters_c(this % Simulation_object, c_file_name)
  end subroutine 

  !-----------------
  ! set_node_coords
  !-----------------
  !
  subroutine set_node_coords(this, coords) 
    implicit none
    class(SimulationClass), intent(in) :: this
    real(c_double), pointer, intent(in) :: coords(:,:)
    type(c_ptr) :: cptr
    integer num_coords 

    num_coords = ubound(coords,1)
    cptr = c_loc(coords(1,1))
    call set_node_coords_c(this % Simulation_object, num_coords, cptr)

  end subroutine 

end module
