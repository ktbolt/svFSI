
#ifndef SIMULATION_H 
#define SIMULATION_H 

#include "Mesh.h"
#include "Parameters.h"

#include <string>

class Simulation {

  public:
    Simulation();
    ~Simulation();

    void create_bcs_aux_data();
    void create_aux_data();
    void load_mesh();
    void read_parameters(const std::string& fileName);

    std::vector<Mesh*> meshes_;
    //std::vector<Mesh*> fluid_mesh_;
    //std::vector<Mesh*> solid_mesh_;

    Parameters parameters_;
};

#endif

