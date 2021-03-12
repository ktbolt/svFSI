
#include "Simulation.h"

#include "Mesh.h"
#include "Face.h"
#include "Physics.h"

#include <iostream>
 
//------------
// Simulation
//------------
//
Simulation::Simulation() 
{
  std::cout << "+ + + + +  Simulation ctor + + + + + " << std::endl;
}

//-------------
// ~Simulation
//-------------
//
Simulation::~Simulation() 
{
}

//-----------------
// create_aux_data
//-----------------
//
void Simulation::create_aux_data()
{
  std::cout << "========== Simulation::create_aux_data ==========" << std::endl;
  create_bcs_aux_data();
  std::cout << "========== Simulation::create_aux_data Finish ==========" << std::endl;
}

//---------------------
// create_bcs_aux_data
//---------------------
//
void Simulation::create_bcs_aux_data()
{
  std::cout << "========== Simulation::create_bcs_aux_data ==========" << std::endl;
  for (auto const& mesh : meshes_) { 
    for (auto const& face : mesh->faces_) { 
      face->create_bcs_aux_data();
    }
  }

  std::cout << "========== Simulation::create_bcs_aux_data Finish ==========" << std::endl;
}

//-----------
// load_mesh
//-----------
//
void Simulation::load_mesh()
{
  std::cout << "========== Simulation::load_mesh ==========" << std::endl;
  std::cout << "[Simulation::load_mesh] Number of meshes: " << parameters_.mesh_parameters_.size() << std::endl;
  //parameters_.mesh_parameters_;
  int n = 1;
  for (const auto &meshParams : parameters_.mesh_parameters_) { 
    std::cout << "---------- Mesh " << n << " ----------" << std::endl;
    //auto physics = Physics::string_to_type_.at(meshParams.physics_);
    //std::cout << "[Simulation::load_mesh] Physics: " << meshParams.physics_ << std::endl;
    std::cout << "[Simulation::load_mesh] Mesh file: " << meshParams.mesh_file_ << std::endl;

    auto mesh = new Mesh();
    mesh->name_ = meshParams.name_;
    mesh->load_mesh(meshParams.mesh_file_);
    meshes_.emplace_back(mesh);

    /*
    mesh->physics_ = physics;
    if (mesh->physics_ == PhysicsType::fluid) { 
      fluid_mesh_.emplace_back(mesh);
    }
    */

    // Load mesh faces.
    //
    std::cout << "[Simulation::load_mesh] Load face meshes ... " << std::endl;
    int face_num = 1;
    for (auto& mesh_face : meshParams.faces_) {
      auto face_type = std::get<0>(mesh_face);
      auto file_name = std::get<1>(mesh_face);
      std::cout << "[Simulation::load_mesh] ----- Face " << face_num << " -----" << std::endl;
      std::cout << "[Simulation::load_mesh] Type: " << face_type << std::endl;
      std::cout << "[Simulation::load_mesh] File name: " << file_name << std::endl;
      auto face = new Face();
      face->type_ = face_type; 
      face->load_mesh(file_name);
      mesh->faces_.push_back(face); 
      face_num += 1;
    }

    n += 1;
  }
  std::cout << "---------- done Load Mesh ----------" << std::endl;
}

//-----------------
// read_parameters
//-----------------
//
void Simulation::read_parameters(const std::string& file_name)
{
  parameters_.set_parameters(file_name);
}

