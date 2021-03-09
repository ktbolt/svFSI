
#include "Face.h"

#include "element_utils.h"
#include "vtk_utils.h"

#include <iostream>

//-------------
// Face 
//-------------
//
Face::Face() 
{
  static int num_objs = 1;
  id_ = num_objs;
  std::cout << "+ + + ctor Face " << id_ << " + + + " << std::endl;
  num_objs += 1;

  nodes_ = new Nodes();
}

Face::Face(const Face& source)
{
  id_ = source.id_;
  std::cout << "===== copy Face " << id_ << " ====" << std::endl;
}

//-------------
// ~Face
//-------------
//
Face::~Face() 
{
  std::cout << "- - - dtor Face " << id_ << " - - - " << std::endl;
}

//-------------
// add_element
//-------------
//
void Face::add_element(const ElementType type, const int elem_id, const std::vector<int>& conn)
{
  if (element_set_.count(type) == 0) {
    std::cout << "[add_element] Add new element type" << std::endl;
    element_set_[type] = element_utils::create_element2d(type);
  }
  auto element = element_set_[type];
  element->add_element_conn(elem_id, conn);
}

//---------------------
// create_bcs_aux_data
//---------------------
//
void Face::create_bcs_aux_data()
{
  for (auto entry : element_set_) { 
    entry.second->define_essential_bcs();
  }

}

//-----------
// load_mesh 
//-----------
// Read a surface mesh from a VTK .vtp file store the data. 
//
void Face::load_mesh(const std::string& file_name)
{
  std::cout << std::endl;
  std::cout << "========== Face::load_mesh =========" << std::endl;
  auto file_ext = file_name.substr(file_name.find_last_of(".") + 1);
  std::cout << "[Face::load_mesh] File extension: " << file_ext << std::endl;

  if (file_ext != VtkFileExtentions::VTK_VTP_EXTENSION) {
    throw std::runtime_error("The face file '" + file_name+ "' is not a VTP format.");
  }

  load_mesh_from_vtp(file_name, this);

}

