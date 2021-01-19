
interface 
  subroutine interface_init_c() bind(C, name="interface_init")
    use iso_c_binding
    implicit none
    !type(c_ptr) :: interface_init 
    !integer(c_int), value :: a
    !integer(c_int), value :: b
  end subroutine 
end interface 

