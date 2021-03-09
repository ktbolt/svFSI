
#ifndef ELEMENT_2D_H 
#define ELEMENT_2D_H 

#include "Element.h"

#include <map>
#include <string>
#include <vector>

//-----------
// Element2d
//-----------
// The Element2d class is used to store a list of elements of the same type. 
//
class Element2d {
  public:
    const static std::map<ElementType,std::string> element_type_string;

    virtual void add_element_conn(const int elem_id, const std::vector<int>& conn) = 0;
    virtual void define_essential_bcs() = 0;

    ElementGeometryType geometry_type_;
    ElementType type_;
    int num_nodes_;
    int num_gauss_points_;
    std::vector<int> ids_;
    //std::vector<int> connectivity_;
   
    std::vector<double> gauss_weights_;
    std::vector<std::vector<double>> gauss_points_;

};

#endif

