
#include "Physics.h"

const std::map<std::string,PhysicsType> Physics::string_to_type_
{
  {"fluid", PhysicsType::fluid},
  {"solid", PhysicsType::solid}
};

const std::map<PhysicsType, std::string> Physics::type_to_string_
{
  {PhysicsType::fluid, "fluid"},
  {PhysicsType::solid, "solid"},
  {PhysicsType::unknown, "unknown"}
};
 
//---------
// Physics 
//---------
//
Physics::Physics() 
{

}

//----------
// ~Physics
//----------
//
Physics::~Physics() 
{
}


