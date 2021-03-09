
#include "Face.h"
#include "Mesh.h"

#ifndef VTK_UTILS
#define VTK_UTILS

//-------------------
// VtkFileExtentions
//-------------------
//
class VtkFileExtentions {
  public:
    const static std::string VTK_VTU_EXTENSION;
    const static std::string VTK_VTP_EXTENSION;
};

void load_mesh_from_vtp(const std::string& file_name, Face* face);

void load_mesh_from_vtu(const std::string& file_name, Mesh* mesh);

#endif


