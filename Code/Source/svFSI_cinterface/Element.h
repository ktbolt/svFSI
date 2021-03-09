
#ifndef ELEMENT_H 
#define ELEMENT_H 

#include <map>
#include <string>
#include <vector>

//---------------------
// ElementGeometryType
//---------------------
//
enum class ElementGeometryType {
  line,
  planar,
  solid
};

//---------------------
// ElementTopologyType
//---------------------
//
enum class ElementTopologyType {
  hex,
  line,
  quad,
  tet,
  tri,
  wedge,
  unknown
};

//-------------
// ElementType
//-------------
//
enum class ElementType {
  hex8,
  hex20,
  hex27,
  line1,
  line2,
  quad4,
  quad8,
  quad9,
  tet4,
  tet10,
  tri3,
  tri6,
  wedge,
  unknown
};

//---------
// Element
//---------
// The Element class is used to store a list of elements of the same type. 
//
class Element {
  public:
    const static std::map<ElementType,std::string> element_type_string;

    virtual void add_element_conn(const int elem_id, const std::vector<int>& conn) = 0;

    ElementGeometryType geometry_type_;
    ElementType type_;
    int num_nodes_;
    int num_gauss_points_;
    std::vector<int> ids_;
    //std::vector<int> connectivity_;
   
    // [TODO] do we want to define these in the ElementTet4 class using std::array?
    std::vector<double> gauss_weights_;
    std::vector<std::vector<double>> gauss_points_;

};

#endif

