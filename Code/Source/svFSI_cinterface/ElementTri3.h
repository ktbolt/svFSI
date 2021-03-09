
#ifndef ELEMENT_TRI3_H 
#define ELEMENT_TRI3_H 

#include "Element2d.h"

#include <array>
#include <vector>

//-------------
// ElementTri3
//-------------
//
class ElementTri3 : public Element2d {

  static const int NUM_EDGES = 3;
  static const int NUM_GAUSS_POINTS = 4;
  static const int NUM_ELEM_NODES = 3;
  static const int NUM_SPACE_DIMS = 3;

  public:
    ElementTri3();
    ~ElementTri3();

    void add_element_conn(const int elem_id, const std::vector<int>& conn);
    void define_shape_functions();
    void define_essential_bcs();

    int num_edges_;

    std::array< std::array<double,NUM_ELEM_NODES>, NUM_GAUSS_POINTS> shape_functions_;
    std::array< std::array<std::array<double,NUM_ELEM_NODES>,NUM_SPACE_DIMS>, NUM_GAUSS_POINTS> shape_functions_grad_;

    std::array< std::array<double,2>, NUM_SPACE_DIMS> gauss_points_bounds_;

    std::array< std::array<double,2>, NUM_ELEM_NODES> shape_functions_bounds_;

    std::vector< std::array<int,NUM_ELEM_NODES> > connectivity_;

    // Stores element information for essential BCs.
    //
    // shape: nodes_per_elem+1, num_elems
    //
    std::vector< std::array<int,NUM_ELEM_NODES+1> > essential_bcs_;

};

#endif

