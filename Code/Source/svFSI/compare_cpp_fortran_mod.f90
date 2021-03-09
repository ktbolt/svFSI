
!----------------------
! compare_cpp_fortran 
!----------------------
! The compare_cpp_fortran compares values comupted in Fortran code to that 
! computed in the cpp code. 
!
module compare_cpp_fortran 

  use commod 

  implicit none

  ! Define the module subroutines. 

contains 

  !--------------
  ! compare_mesh
  !--------------
  ! Compare mesh data.
  !
  subroutine compare_mesh()
    implicit none

    integer i

    print *, "[compare_mesh] "
    print *, "[compare_mesh] -------------------------------------------- "
    print *, "[compare_mesh] --------------- compare_mesh --------------- "
    print *, "[compare_mesh] -------------------------------------------- "

    call compare_complete_mesh_coordinates()

  end subroutine 

  !-------------------
  ! compare_mesh_data
  !-------------------
  ! Compare the data stored in the Fortran 'mshType' type.
  !
  ! Compare data for 
  !   - nodal coordinates 
  !   - element ids 
  !   - element connectivity 
  !
  subroutine compare_mesh_data(f_mesh)
    implicit none
    TYPE(mshType), INTENT(INOUT) :: f_mesh

    type(simulation_mesh_data) :: c_mesh

    integer c_num_coords 
    integer f_num_coords 

    print *, "[compare_mesh_data] " 
    print *, "[compare_mesh_data] ----- Compare mesh data for mesh named: ", trim(f_mesh % name), " -----"

    call compare_mesh_nodal_coords(f_mesh)

    print *, "[compare_mesh_data] Compare mesh data for mesh named: ", trim(f_mesh % name), ": pass"

  end subroutine 

  !-----------------------------------
  ! compare_complete_mesh_coordinates 
  !-----------------------------------
  !
  subroutine compare_complete_mesh_coordinates()
    implicit none

    integer f_num_coords, f_num_nodes

    integer c_num_meshes, c_num_solid_meshes, c_num_fluid_meshes 
    integer c_num_coords, c_num_nodes
    real(c_double), pointer :: c_node_coords(:,:)

    integer i
    integer num_diff
    type(mshType) :: mesh
    real diff, coord_diff(3)
    real tol

    print *, "[compare_complete_mesh_coordinates] Compare complete mesh coordinates ..."

    call sim_interface % get_node_coords(c_node_coords)
    c_num_coords = ubound(c_node_coords,2)
    f_num_coords = ubound(x,2)

    if (c_num_coords .ne. f_num_coords) then
      print *, "[compare_complete_mesh_coordinates]   Compare mesh coordinates: fail "
      print*, "[compare_complete_mesh_coordinates]     Number of Fortran nodal coordinates: ", f_num_coords
      print*, "[compare_complete_mesh_coordinates]     Number of C nodal coordinates: ", c_num_coords
      err = "[compare_complete_mesh_coordinates]   Compare mesh coordinates: fail "
    end if

    tol = 1.0e-9
    num_diff = 0
    do i = 1, f_num_coords
      coord_diff = x(:,i) - c_node_coords(:,i)
      !print*, "[compare_mesh]     coord_diff: ", coord_diff 
      diff = sqrt(dot_product(coord_diff, coord_diff))
      if (diff .gt. tol) then
        num_diff = num_diff + 1
      end if
      !if (i .lt. 10) then
      !  print*, "[compare_mesh]     x(:,i): ", x(:,i)
      !  print*, "[compare_mesh]     c_node_coords(:,i): ", c_node_coords(:,i)
      !end if
    end do
   
    if (num_diff .eq. 0) then
      print *, "[compare_complete_mesh_coordinates]   Compare mesh coordinates: pass "
    else
      print *, "[compare_complete_mesh_coordinates]   Compare mesh coordinates: fail "
      print *, "[compare_complete_mesh_coordinates]     Number of different coordinates: ", num_diff
      err = "[compare_complete_mesh_coordinates]   Compare mesh coordinates: fail "
    endif
   
  end subroutine 

  !---------------------------
  ! compare_mesh_nodal_coords
  !---------------------------
  !
  subroutine compare_mesh_nodal_coords(f_mesh)
    implicit none
    TYPE(mshType), INTENT(INOUT) :: f_mesh

    integer f_num_coords, f_num_nodes

    type(simulation_mesh_data) :: c_mesh
    integer c_num_meshes, c_num_solid_meshes, c_num_fluid_meshes 
    integer c_num_coords, c_num_nodes
    !real(c_double), pointer :: c_node_coords(:,:)

    integer i
    integer num_diff
    real diff, coord_diff(3)
    real tol

    print *, "[compare_mesh_nodal_coords] Compare mesh nodal coordinates ..."

    ! Get the C++ mesh data.
    call sim_interface % get_mesh_data(f_mesh % name, c_mesh)

    print *, "[compare_mesh_nodal_coords] ############# print nodal coordinates ##########"
    c_num_coords = c_mesh % num_coords
    !do i = 1, c_num_coords
      !print *, c_node_coords(:,i)
      !print *, "## i", i, c_mesh % coords(:,i)
    !end do 
    
    f_num_coords = ubound(f_mesh % x, 2)

    if (c_num_coords .ne. f_num_coords) then
      print *, "[compare_mesh_nodal_coords] Compare mesh nodal coordinates: fail"
      print *, "[compare_mesh_nodal_coords]     Number of Fortran nodal coordinates: ", f_num_coords
      print *, "[compare_mesh_nodal_coords]     Number of C nodal coordinates: ", c_num_coords
      err = "[compare_mesh_nodal_coords]   Compare mesh coordinates: fail "
    end if

    do i = 1, f_num_coords
      print *, "## i", i, f_mesh % x(:,i)
    end do 

    return

    tol = 1.0e-9
    num_diff = 0
    do i = 1, f_num_coords
      coord_diff = f_mesh % x(:,i) - c_mesh % coords(:,i)
      !print*, "[compare_mesh]     coord_diff: ", coord_diff 
      diff = sqrt(dot_product(coord_diff, coord_diff))
      if (diff .gt. tol) then
        num_diff = num_diff + 1
      end if
      !if (i .lt. 10) then
      !  print*, "[compare_mesh]     x(:,i): ", x(:,i)
      !  print*, "[compare_mesh]     c_node_coords(:,i): ", c_node_coords(:,i)
      !end if
    end do
   
    if (num_diff .eq. 0) then
      print *, "[compare_mesh_nodal_coords] Compare mesh nodal coordinates: pass"
    else
      print *, "[compare_mesh_nodal_coords] Compare mesh nodal coordinates: fail"
      print *, "[compare_mesh_nodal_coords]     Number of different coordinates: ", num_diff
      err = "[compare_mesh_nodal_coords] Compare mesh nodal coordinates: fail"
    endif

  end subroutine 


end module compare_cpp_fortran

