
#ifndef PHYSICS_H 
#define PHYSICS_H 

#include <map>
#include <string>

enum class PhysicsType {
  fluid,
  solid,
  unknown
};

//---------
// Physics
//---------
//
class Physics {

  public:

    Physics();
    ~Physics();

    const static std::map<std::string,PhysicsType> string_to_type_;
    const static std::map<PhysicsType, std::string> type_to_string_;

};

#endif

