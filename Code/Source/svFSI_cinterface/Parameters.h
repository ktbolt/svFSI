
#ifndef PARAMETERS_H 
#define PARAMETERS_H 

#include <string>
#include <tuple>
#include <vector>

#include "tinyxml2.h"

//-----------------------------
// BoundaryConditionParameters
//-----------------------------
//
class BoundaryConditionParameters
{
  const char* PROFILE = "profile";
  std::string TIME_DEP = "time_dependence";
  std::string TYPE = "type";
  std::string VALUE = "value";
  std::string ZERO_OUT_PERIM = "zero_out_perimeter";

  public:
      BoundaryConditionParameters() {};
      void set_parameters(tinyxml2::XMLElement* bc_params);
      std::string type_;
      std::string time_dependence_;
      int value_;
      int zero_out_perimeter_;
};

//--------------------
// EquationParameters
//--------------------
//
class EquationParameters
{
  std::string COUPLED = "coupled";
  std::string BACKFLOW_COEF = "backflow_stabilization_coefficient";
  const char* BOUNDARY_COND = "BoundaryCondition";
  std::string PHYSICS = "physics";

  public:
      EquationParameters(){};
      void set_parameters(tinyxml2::XMLElement* equation_params);

      std::string physics_;
      float backflow_stab_coef_;
      int coupled_;
      std::vector<BoundaryConditionParameters> boundary_conditions_;
};

//-------------------
// GeneralParameters
//-------------------
//
class GeneralParameters
{
  std::string DEBUG = "debug";
  std::string INCREMENT_IN_SAVING_RESTART_FILES= "increment_in_saving_restart_files";
  std::string NAME_PREFIX_OF_SAVED_VTK_FILES= "name_prefix_of_saved_vtk_files";
  std::string SAVE_AVERAGED_RESULTS = "save_averaged_results";
  std::string SAVE_RESULTS_IN_FOLDER = "save_results_in_folder";
  std::string SAVE_RESULTS_TO_VTK_FORMAT = "save_results_to_vtk_format";
  std::string SEARCHED_FILE_NAME_TO_TRIGGER_STOP = "searched_file_name_to_trigger_stop";
  std::string SIMULATION_REQUIRES_REMESHING = "simulation_requires_remeshing";
  std::string START_SAVING_AFTER_TIME_STEP = "start_saving_after_time_step";
  std::string VERBOSE = "verbose";
  std::string WARNING = "warning";

  std::string GENERAL_PARAMS = "GeneralParameters";

  public:
    GeneralParameters();

    void set_parameters(tinyxml2::XMLElement* fsi_file);

    bool debug_;
    bool verbose_;
    bool warning_;
    int restart_save_increment_;
    std::string save_results_dir_;
    bool save_averaged_results_;
    bool save_results_using_vtk_format_;
    bool simulation_requires_remeshing_;
    std::string vtk_file_name_prefix_;
    std::string trigger_stop_file_name_;
    int start_saving_time_step_;
};

//----------------
// MeshParameters
//----------------

// Define a tuple for storing face information. 
typedef std::tuple<std::string, std::string> MeshFace;

class MeshParameters
{
  std::string DOMAIN = "domain";
  const char* FACE = "Face";
  const char* FACE_FILE = "file";
  const char* FACE_TYPE = "type";
  std::string MESH_FILE = "mesh_file";
  std::string NAME = "name";
  std::string PHYSICS = "physics";

  public:
      MeshParameters() {};

      void set_parameters(tinyxml2::XMLElement* fsi_file);

      int domain_;
      std::vector<MeshFace> faces_;
      std::string mesh_file_;
      std::string name_;
      std::string physics_;
};

//--------------------
// SolutionParameters
//--------------------
//
class SolutionParameters
{
  std::string SOLUTION_PARAMS = "SolutionParameters";

  std::string CONT_PREV_SIM = "continue_previous_simulation";
  std::string NUM_DIMS = "number_of_spatial_dimensions";
  std::string NUM_TIME_STEPS = "number_of_time_steps";
  std::string NUM_INIT_TIME_STEPS = "number_of_initialization_time_steps";
  std::string SPECTRAL_RADIUS_OF_INFINITE_TIME_STEP = "spectral_radius_of_infinite_time_step";
  std::string TIME_STEP = "time_step_size";

  public:
    SolutionParameters(); 
    ~SolutionParameters();

    void set_parameters(tinyxml2::XMLElement* fsi_file);

    int cont_prev_sim_;
    int num_dims_;
    int num_time_steps_;
    int num_init_time_steps_;
    double spectral_radius_inf_time_step_;
    double time_step_;
};

//------------
// Parameters
//------------
// The Parameters class stores parameter values read in from a solver input file.
//
class Parameters {

  std::string MESH_PARAMS = "Mesh";
  const char* EQUATION_PARAMS = "Equation";
  std::string FSI_FILE = "svFSIFile";

  public:
      Parameters();
      ~Parameters();

      void set_parameters(std::string file_name);
      void get_logging_levels(int& verbose, int& warning, int& debug);

      GeneralParameters general_parameters_;
      SolutionParameters solution_parameters_;
      std::vector<MeshParameters> mesh_parameters_;
      std::vector<EquationParameters> equation_parameters_;

};

#endif

