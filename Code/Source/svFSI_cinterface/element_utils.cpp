
#include "Element.h" 
#include "ElementTet4.h" 
#include "ElementTri3.h" 

#include <map>
#include <string>

namespace element_utils {

using ElementFactoryType = std::map<ElementType, std::function<Element*()>>;
ElementFactoryType ElementFactory = {
  {ElementType::tet4, []() -> Element* { return new ElementTet4(); } },
  //{ElementType::tri3, []() -> Element* { return new ElementTri3(); } }
};

using Element2dFactoryType = std::map<ElementType, std::function<Element2d*()>>;
Element2dFactoryType Element2dFactory = {
  {ElementType::tri3, []() -> Element2d* { return new ElementTri3(); } }
};


//----------------
// create_element
//----------------
//
Element* create_element(const ElementType type)
{

  if (ElementFactory.count(type) == 0) {
    throw std::runtime_error("No create element method for element type '" + Element::element_type_string.at(type) + "'.");
  }

  return ElementFactory[type]();
}

//------------------
// create_element2d
//------------------
//
Element2d * 
create_element2d(const ElementType type)
{

  if (Element2dFactory.count(type) == 0) {
    throw std::runtime_error("No create element method for element type '" + Element::element_type_string.at(type) + "'.");
  }

  return Element2dFactory[type]();
}


}


