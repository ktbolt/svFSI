
#include "ElementTri3.h" 

#include <iostream>
#include <math.h>

//-------------
// ElementTri3
//-------------
//
ElementTri3::ElementTri3() 
{
  std::cout << "+ + + ElementTri3 ctor + + + " << std::endl;
  num_nodes_ = NUM_ELEM_NODES;
  num_edges_= NUM_EDGES;
  num_gauss_points_ = NUM_GAUSS_POINTS;
  geometry_type_ = ElementGeometryType::planar;

  // Define Gauss weights.
  double w = 1.0 / 24.0;
  double s = (5.0 + 3.0*sqrt(5.0)) / 20.0;
  double t = (5.0 - sqrt(5.0)) / 20.0;
  gauss_weights_ = {w, w, w, w};

  // Define gauss integration points.
  //
  // NUM_SPACE_DIMS X NUM_GAUSS_POINTS 
  //
  gauss_points_ = { {s, t, t, t}, 
                    {t, s, t, t},
                    {t, t, s, t} };

  std::cout << "[ElementTri3::ElementTri3] " << std::endl;
  std::cout << "[ElementTri3::ElementTri3] ----- gauss points -----" << std::endl;
  std::cout << "[ElementTri3::ElementTri3] s: " << s << std::endl;
  std::cout << "[ElementTri3::ElementTri3] t: " << t << std::endl;
  std::cout << "[ElementTri3::ElementTri3] gauss_points_[0][0]: " << gauss_points_[0][0] << std::endl;
  std::cout << "[ElementTri3::ElementTri3] gauss_points_[0][1]: " << gauss_points_[0][1] << std::endl;
  std::cout << "[ElementTri3::ElementTri3] gauss_points_[0][2]: " << gauss_points_[0][2] << std::endl;
  std::cout << "[ElementTri3::ElementTri3] gauss_points_[0][3]: " << gauss_points_[0][3] << std::endl;

  std::cout << "[ElementTri3::ElementTri3] shape_functions_grad_.size(): " << shape_functions_grad_.size() << std::endl;

  define_shape_functions();

}

ElementTri3::~ElementTri3() 
{
}

//----------------------
// define_essential_bcs
//----------------------
// Create element data for essentail boundary conditions.
//
// This is created in READVTP(VTKXML.f).
//
//   face_mesh % gebc(nodes_per_elem+1, num_elems) - essential boundary conditions array 
//
//       genbc(1,:) = elem IDs
//       genbc(2,:) = IEN(1,:)
//       genbc(3,:) = IEN(2,:)
//       genbc(4,:) = IEN(3,:)
//
//   face_mesh % gebc(1,:) = face_mesh % gE(:)
//   face_mesh % gebc(2:nodes_per_elem+1,:) = face_mesh % IEN(:,:)
//
//   face_mesh % gE(num_elems) - element IDs
//
//   face_mesh % IEN(nodes_per_elem, num_elems) - element connectivity 
//
void ElementTri3::define_essential_bcs()
{
  std::cout << "===== ElementTri3 define_essential_bcs ===== " << std::endl;
  std::array<int,NUM_ELEM_NODES+1> entry;

  for (int i = 0; i < connectivity_.size(); i++) { 
    entry[0] = ids_[i];
    for (int j = 0; j < NUM_ELEM_NODES; j++) { 
      entry[j+1] = connectivity_[i][j];
    }
    essential_bcs_.push_back(entry);
  }
}

//-------------------------
// define__shape_functions
//-------------------------
// Define element shape functions and their gradient.
//
void ElementTri3::define_shape_functions() 
{
  for (int i = 0; i < num_gauss_points_; i++) {
    shape_functions_[i][3] = 1.0; 
    for (int j = 0; j < 3; j++) {
      shape_functions_[i][j] = gauss_points_[j][i]; 
      shape_functions_[i][3] -= gauss_points_[j][i]; 
    }

    shape_functions_grad_[i][0][0] = 1.0;
    shape_functions_grad_[i][1][0] = 0.0;
    shape_functions_grad_[i][2][0] = 0.0;

    shape_functions_grad_[i][0][1] = 0.0;
    shape_functions_grad_[i][1][1] = 1.0;
    shape_functions_grad_[i][2][1] = 0.0;

    shape_functions_grad_[i][0][2] = 0.0;
    shape_functions_grad_[i][1][2] = 0.0;
    shape_functions_grad_[i][2][2] = 1.0;

    shape_functions_grad_[i][0][3] = -1.0;
    shape_functions_grad_[i][1][3] = -1.0;
    shape_functions_grad_[i][2][3] = -1.0;
  }

  for (int i = 0; i < NUM_SPACE_DIMS; i++) {
    gauss_points_bounds_[0][i] = 0.0;
    gauss_points_bounds_[1][i] = 1.0;
  }

  for (int i = 0; i < num_nodes_; i++) {
    shape_functions_bounds_[0][i] = 0.0;
    shape_functions_bounds_[0][i] = 1.0;
  }

}

//------------------
// add_element_conn
//------------------
//
void ElementTri3::add_element_conn(const int elem_id, const std::vector<int>& conn)
{
  //std::cout << "[ElementTri3::add_element] Add element" << std::endl;
  ids_.push_back(elem_id); 

  std::array<int,NUM_ELEM_NODES> entry;
  for (int i = 0; i < NUM_ELEM_NODES; i++) {
    entry[i] = conn[i];
  }

  connectivity_.push_back(entry);

}


