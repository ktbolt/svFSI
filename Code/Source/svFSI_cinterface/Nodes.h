
#ifndef NODES_H 
#define NODES_H 

#include <vector>

//-------
// Nodes
//-------
// The Nodes class stores nodal IDs and coordinates.
//
class Nodes {

  public:
    Nodes();
    ~Nodes();

    int num_nodes_;
    std::vector<double> coordinates_;
    std::vector<int> ids_;

};

#endif

