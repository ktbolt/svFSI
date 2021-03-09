
#ifndef FACE_H 
#define FACE_H 

#include "Element2d.h"
#include "Nodes.h"

#include <map>
#include <string>
#include <vector>

//------
// Face
//------
// The Face class stores the nodes IDs and element connectivity for a surface face.
//
// A Face is used to define a boundary condition.
//
class Face {

  public:
    Face();
    Face(const Face&);
    ~Face();

    void add_element(const ElementType type, const int elem_id, const std::vector<int>& conn);
    void create_bcs_aux_data();
    void load_mesh(const std::string& file_name);

    int id_;
    std::string type_;

    Nodes* nodes_;
    std::map<ElementType, Element2d*> element_set_;


};

#endif

