
// Declare prototypes for the functions wrapping c++ Simulation methods.

extern "C" {

class Simulation;

typedef Simulation SimulationType;

static const int STRING_SIZE = 100;

//-----------------------
// CintGeneralParameters
//-----------------------
// Note: the order of the member data must match the type definition in simulation_interface_mod.f90.
//
typedef struct {
  int debug;
  int increment_in_saving_restart_files;
  char name_prefix_of_saved_vtk_files[STRING_SIZE];
  int save_averaged_results;
  char save_results_in_folder[STRING_SIZE];
  int save_results_to_vtk_format;
  char searched_file_name_to_trigger_stop[STRING_SIZE];
  int simulation_requires_remeshing;
  int start_saving_after_time_step;
  int verbose;
  int warning;
} CintGeneralParameters;

//--------------
// CintMeshData 
//--------------
// Note: The order of the member data must match the type 'simulation_mesh_data' definition 
// in simulation_interface_mod.f90.
//
typedef struct {
  int num_coords;
  double* c_coords;
} CintMeshData;

//------------------------
// CintSolutionParameters
//------------------------
// Note: the order of the member data must match the type definition in simulation_interface_mod.f90. 
//
typedef struct {
    int continue_previous_simulation;
    int number_of_initialization_time_steps;
    int number_of_spatial_dimensions;
    int number_of_time_steps;
    double spectral_radius_of_infinite_time_step;
    double time_step_size;
} CintSolutionParameters;

// Constructor
Simulation* create_simulation();

void create_aux_data(Simulation* simulation);

void get_mesh_data(Simulation* simulation, char* mesh_name, CintMeshData* mesh);

void get_nodal_coordinates(Simulation* simulation, int* num_coords, void **coords);

void get_node_ids(Simulation* simulation, int* num_ids, void **node_ids);

void get_num_meshes(Simulation* simulation, int* num_solid_meshes, int* num_fluid_meshes);

void load_mesh(Simulation* simulation);

void read_parameters(Simulation* simulation, const char* file_name);

void set_nodal_coordinates(Simulation* simulation, int num_coords, void *coords);

// Parameters.

void get_general_params(Simulation* simulation, CintGeneralParameters* params); 
void get_solution_params(Simulation* simulation, CintSolutionParameters* params); 

}

