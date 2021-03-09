
#include "Element.h"

const std::map<ElementType,std::string> Element::element_type_string
 {
  {ElementType::hex8, "hex8"},
  {ElementType::hex20, "hex20"},
  {ElementType::hex27, "hex27"},
  {ElementType::line1, "line1"},
  {ElementType::line2, "line2"},
  {ElementType::quad4, "quad4"},
  {ElementType::quad8, "quad8"},
  {ElementType::quad9, "quad9"},
  {ElementType::tet4, "tet4"},
  {ElementType::tet10, "tet10"},
  {ElementType::tri3, "tri3"},
  {ElementType::tri6, "tri6"},
  {ElementType::wedge, "wedge"},
  {ElementType::unknown, "unknown"}
};

