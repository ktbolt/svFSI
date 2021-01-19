include_directories(${MPI_C_INCLUDE_PATH})

if(CMAKE_Fortran_COMPILER_ID MATCHES "GNU")
  set(CMAKE_Fortran_FLAGS "-cpp -pthread")
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -std=legacy")
else()
  # nothing for now
  # may need to reset for intel compiler or others
endif()

set(lib ${SV_LIB_SVFSI_CINTERFACE_NAME})

set(CSRCS interface.cpp)

add_library(${lib} ${SV_LIBRARY_TYPE} ${CSRCS})

target_link_libraries(${lib} ${MPI_LIBRARY} ${MPI_Fortran_LIBRARIES})

# extra MPI libraries only if there are not set to NOT_FOUND or other null 
if(SV_MPI_EXTRA_LIBRARY)
  target_link_libraries(${lib} ${SV_MPI_EXTRA_LIBRARY})
endif()

if(SV_INSTALL_LIBS)
  install(TARGETS ${lib}
    RUNTIME DESTINATION ${SV_INSTALL_RUNTIME_DIR} COMPONENT CoreExecutables
    LIBRARY DESTINATION ${SV_INSTALL_LIBRARY_DIR} COMPONENT CoreLibraries
    ARCHIVE DESTINATION ${SV_INSTALL_ARCHIVE_DIR} COMPONENT CoreLibraries
    )
endif()
if(SV_INSTALL_HEADERS)
  install(FILES ${HDRS}
    DESTINATION ${SV_INSTALL_INCLUDE_DIR}/core COMPONENT CoreHeaders
    )
endif()
