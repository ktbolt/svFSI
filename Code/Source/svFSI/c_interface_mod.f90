
module c_interface_module

  use iso_c_binding

  public

  include "c_interface_def.f90"

  contains 
    subroutine c_interface_init()
      print *, "##### subroutine interface_init() #####"
      call interface_init_c()
    end subroutine 

end module


