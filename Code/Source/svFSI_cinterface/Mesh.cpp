
#include "Mesh.h"

#include "element_utils.h"
#include "vtk_utils.h"

#include <iostream>

//------
// Mesh 
//------
//
Mesh::Mesh() 
{
  static int num_objs = 1;
  id_ = num_objs;
  std::cout << "+ + + ctor Mesh " << id_ << " + + + " << std::endl;
  num_objs += 1;

  nodes_ = new Nodes();
}

Mesh::Mesh(const Mesh& source)
{
  id_ = source.id_;
  std::cout << "===== copy Mesh " << id_ << " ====" << std::endl;
}

//-------------
// ~Mesh
//-------------
//
Mesh::~Mesh() 
{
  std::cout << "- - - dtor Mesh " << id_ << " - - - " << std::endl;
}

//-------------
// add_element
//-------------
//
void Mesh::add_element(const ElementType type, const int elem_id,  const std::vector<int>& conn)
{
  if (element_set_.count(type) == 0) {
    std::cout << "[add_element] Add new element type" << std::endl;
    element_set_[type] = element_utils::create_element(type);
  }
  auto element = element_set_[type];
  element->add_element_conn(elem_id, conn);
}

//-----------
// load_mesh 
//-----------
//
void Mesh::load_mesh(const std::string& file_name)
{
  std::cout << "========== Mesh::load_mesh =========" << std::endl;
  auto file_ext = file_name.substr(file_name.find_last_of(".") + 1);
  std::cout << "[Mesh::load_mesh] File extension: " << file_ext << std::endl;

  load_mesh_from_vtu(file_name, this);
}

