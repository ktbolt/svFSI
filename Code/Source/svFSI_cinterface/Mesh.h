
#ifndef MESH_H 
#define MESH_H 

#include "Element.h"
//#include "ElementTet4.h"
#include "Face.h"
#include "Nodes.h"
#include "Physics.h"

#include <map>
#include <string>

//------
// Mesh
//------
// The Mesh class stores the nodes and elements for a volume mesh.
//
class Mesh {

  public:
    Mesh();
    Mesh(const Mesh&);
    ~Mesh();

    void add_element(const ElementType type, const int elem_id, const std::vector<int>& conn);
    void load_mesh(const std::string& file_name);

    int id_;
    std::string name_;
    PhysicsType physics_;

    Nodes* nodes_;
    std::map<ElementType, Element*> element_set_;
    std::vector<Face*> faces_;
};

#endif

