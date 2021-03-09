
#ifndef ELEMENT_TET4_H 
#define ELEMENT_TET4_H 

#include "Element.h"

#include <array>
#include <vector>

//-------------
// ElementTet4
//-------------
//
class ElementTet4 : public Element {

  static const int NUM_FACES = 4;
  static const int NUM_GAUSS_POINTS = 4;
  static const int NUM_ELEM_NODES = 4;
  static const int NUM_SPACE_DIMS = 3;

  public:
    ElementTet4();
    ~ElementTet4();

    void add_element_conn(const int elem_id, const std::vector<int>& conn);
    void define_shape_functions();

   std::vector< std::array<int,NUM_ELEM_NODES> > connectivity_;

    int num_faces_;
    std::array< std::array<double,NUM_ELEM_NODES>, NUM_GAUSS_POINTS> shape_functions_;
    std::array< std::array<std::array<double,NUM_ELEM_NODES>,NUM_SPACE_DIMS>, NUM_GAUSS_POINTS> shape_functions_grad_;

    std::array< std::array<double,2>, NUM_SPACE_DIMS> gauss_points_bounds_;

    std::array< std::array<double,2>, NUM_ELEM_NODES> shape_functions_bounds_;

};

#endif

