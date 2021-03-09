
// The functions defined here are used to wrap calls to Simulation object.

#include "simulation_wrap.h"
#include "Simulation.h"

#include <stdlib.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////
//                           Utilities                                      //
//////////////////////////////////////////////////////////////////////////////

//-------------
// copy_string
//-------------
// Copy a string to a char array.
//
void copy_string(const std::string& source, char dest[])
{
  for (int i = 0; i < source.size(); i++) {
    dest[i] = source[i];
  }
  for (int i = source.size(); i < STRING_SIZE; i++) {
    dest[i] = ' ';
  }
}

//////////////////////////////////////////////////////////////////////////////
//                   C++ / Fortran Interface Functions                      //
//////////////////////////////////////////////////////////////////////////////

void create_aux_data(Simulation* simulation)
{
  std::cout << "========== cpp create_aux_data ==========" << std::endl;
  simulation->create_aux_data();
}

//-------------------
// create_simulation
//-------------------
// Create a C++ Simulation object and passes a pointer to it back to Fortran.
//
Simulation* create_simulation()
{
  std::cout << std::endl;
  std::cout << "========== cpp create_simulation ==========" << std::endl;
  return new Simulation();
}

//---------------
// get_mesh_data
//---------------
//
void get_mesh_data(Simulation* simulation, char* mesh_name, CintMeshData* f_mesh)
{
  f_mesh->num_coords = 0;
  std::cout << std::endl;
  std::cout << "========== cpp get_mesh_data ==========" << std::endl;

  // Remove mesh name trailing spaces.
  std::string c_mesh_name(mesh_name);
  std::string::iterator end_pos = std::remove(c_mesh_name.begin(), c_mesh_name.end(), ' ');
  c_mesh_name.erase(end_pos, c_mesh_name.end());
  std::cout << "[cpp get_mesh_data] mesh_name: '" << c_mesh_name << "'" << std::endl;
  Mesh* mesh = nullptr;

  for (auto smesh : simulation->fluid_mesh_) {
    if (smesh->name_ == c_mesh_name) {
      std::cout << "[cpp get_mesh_data] Found mesh: '" << smesh->name_ << "'" << std::endl;
      mesh = smesh;
    }
  }

  auto nodes = mesh->nodes_;
  int num_nodes = nodes->coordinates_.size() / 3;
  std::cout << "[cpp get_mesh_data] num_nodes: " << num_nodes << std::endl;
  f_mesh->num_coords = num_nodes;
  double* mblock = (double*)malloc(sizeof(double) * num_nodes * 3);

  for (int i = 0; i < num_nodes; i++) {
    mblock[3*i] = nodes->coordinates_[i]; 
    mblock[3*i+1] = nodes->coordinates_[i+num_nodes];
    mblock[3*i+2] = nodes->coordinates_[i+2*num_nodes]; 
  }

  f_mesh->c_coords = mblock;
}

//-----------------------
// get_nodal_coordinates
//-----------------------
// Get the nodal coordinates for the complete mesh.
//
void get_nodal_coordinates(Simulation* simulation, int* num_coords, void **coords)
{
  std::cout << "========== cpp get_nodal_coordinates ==========" << std::endl;
  auto fluid_mesh = simulation->fluid_mesh_[0];
  auto nodes = fluid_mesh->nodes_;
  int num_nodes = nodes->coordinates_.size() / 3;
  std::cout << "[cpp get_nodal_coordinates] num coords:  " << num_nodes << std::endl;
  double* mblock = (double*)malloc(sizeof(double) * num_nodes * 3);

  int n = 0;
  for (int i = 0; i < num_nodes; i++) {
    mblock[3*i] = nodes->coordinates_[i]; 
    mblock[3*i+1] = nodes->coordinates_[i+num_nodes];
    mblock[3*i+2] = nodes->coordinates_[i+2*num_nodes]; 
    n += 3;
  }
  *num_coords = num_nodes;
  *coords = mblock;
}

//--------------
// get_node_ids
//--------------
//
void get_node_ids(Simulation* simulation, int* num_ids, void **node_ids)
{
  std::cout << "========== cpp get_node_ids ==========" << std::endl;

  auto fluid_mesh = simulation->fluid_mesh_[0];
  auto nodes = fluid_mesh->nodes_;
  int num_nodes = nodes->ids_.size();
  std::cout << "[cpp get_node_ids] num nodes:  " << num_nodes << std::endl;

  *num_ids = num_nodes;
  *node_ids = nodes->ids_.data();

  /*
  auto mblock = (int*)malloc(sizeof(int) * num_nodes);
  for (int i = 0; i < num_nodes; i++) {
    mblock[i] = nodes.ids_[i]; 
  }
  *node_ids = mblock;
  */
}

//----------------
// get_num_meshes 
//----------------
//
void get_num_meshes(Simulation* simulation, int* num_solid_meshes, int* num_fluid_meshes)
{
  *num_solid_meshes = simulation->solid_mesh_.size();
  *num_fluid_meshes = simulation->fluid_mesh_.size();
}

//-----------
// load_mesh
//-----------
//
void load_mesh(Simulation* simulation)
{
  std::cout << "========== Simulation_wrap load_mesh ==========" << std::endl;
  simulation->load_mesh();
}

//-----------------
// read_parameters
//-----------------
//
void read_parameters(Simulation* simulation, const char* file_name)
{
  std::cout << "========== read_parameters ==========" << std::endl;
  std::cout << "[read_parameters] file_name: " << file_name << std::endl;
  simulation->read_parameters(std::string(file_name));
}

//-----------------------
// set_nodal_coordinates
//-----------------------
//
void set_nodal_coordinates(Simulation* simulation, int num_coords, void *vcoords)
{
  auto coords = (double*)vcoords;
  std::cout << "========== cpp set_nodal_coordinates ==========" << std::endl;
  std::cout << "[cpp set_nodal_coordinates] num_coords: " << num_coords << std::endl;
  for (int i = 0; i < 10; i++) {
    auto x = coords[i];
    auto y = coords[i+num_coords];
    auto z = coords[i+2*num_coords];
    std::cout << "[cpp set_nodal_coordinates] coords[" << i << "]: " << x << " " << y << "  " << z << std::endl;
  }
}

/////////////////////////////////////////////////
//              P a r a m e t e r s            //
/////////////////////////////////////////////////

//--------------------
// get_general_params
//--------------------
//
void get_general_params(Simulation* simulation, CintGeneralParameters* params)
{
  const auto& gen_params = simulation->parameters_.general_parameters_;

  params->debug = gen_params.debug_;
  params->increment_in_saving_restart_files = gen_params.restart_save_increment_;
  copy_string(gen_params.vtk_file_name_prefix_, params->name_prefix_of_saved_vtk_files);
  params->save_averaged_results = gen_params.save_averaged_results_;
  copy_string(gen_params.save_results_dir_, params->save_results_in_folder);
  params->save_results_to_vtk_format = gen_params.save_results_using_vtk_format_;
  copy_string(gen_params.trigger_stop_file_name_, params->searched_file_name_to_trigger_stop);
  params->simulation_requires_remeshing = gen_params.simulation_requires_remeshing_;
  params->start_saving_after_time_step = gen_params.start_saving_time_step_;
  params->verbose = gen_params.verbose_;
  params->warning = gen_params.warning_;
}

//---------------------
// get_solution_params
//---------------------
//
void get_solution_params(Simulation* simulation, CintSolutionParameters* params)
{
  const auto& sol_params = simulation->parameters_.solution_parameters_;
  params->number_of_initialization_time_steps = sol_params.num_init_time_steps_; 
  params->number_of_spatial_dimensions = sol_params.num_dims_; 
  params->number_of_time_steps = sol_params.num_time_steps_;
  params->spectral_radius_of_infinite_time_step = sol_params.spectral_radius_inf_time_step_;
  params->time_step_size = sol_params.time_step_;
  params->continue_previous_simulation = sol_params.cont_prev_sim_;
}

