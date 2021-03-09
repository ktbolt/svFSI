
#include "vtk_utils.h" 
#include "Element.h" 

#include <string>

#include "vtkCellData.h"
#include <vtkGenericCell.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLUnstructuredGridReader.h>

// VTK file extensions.
const std::string VtkFileExtentions::VTK_VTU_EXTENSION = "vtu";
const std::string VtkFileExtentions::VTK_VTP_EXTENSION = "vtp";

// Map used to convert VTK cell types to ElementType enum.
static std::map<unsigned char,ElementType> vtk_cell_to_elem {
  {VTK_TETRA, ElementType::tet4},
  {VTK_TRIANGLE, ElementType::tri3}
};

// Names of data arrays store in VTK mesh files.
const static std::string NODE_IDS_NAME("GlobalNodeID");
const static std::string ELEMENT_IDS_NAME("GlobalElementID");

/////////////////////////////////////////////////////////////////
//             I n t e r n a l  U t i l i t i e s              //
/////////////////////////////////////////////////////////////////

//--------------------
// store_element_conn
//--------------------
// Store element connectivity into the Face object.
//
static void store_element_conn(vtkSmartPointer<vtkPolyData> vtk_polydata, Face* face)
{
  auto num_elems = vtk_polydata->GetNumberOfCells();
  auto elem_ids = vtkIntArray::SafeDownCast(vtk_polydata->GetCellData()->GetArray(ELEMENT_IDS_NAME.c_str()));
  if (elem_ids == nullptr) {
    throw std::runtime_error("No '" + ELEMENT_IDS_NAME + "' data found in VTK mesh.");
  }
  int num_elem_ids = elem_ids->GetNumberOfTuples();

  auto cell = vtkGenericCell::New();
  unsigned char cell_type;
  int num_tri = 0;

  for (int i = 0; i < num_elems; i++) {
    int elem_id = elem_ids->GetValue(i);
    vtk_polydata->GetCell(i, cell);
    auto dim = cell->GetCellDimension();
    auto num_cell_pts = cell->GetNumberOfPoints();
    if (num_cell_pts == 3) {
      cell_type = VTK_TRIANGLE;
      num_tri += 1;
    } else if (num_cell_pts == 4) {
      cell_type = VTK_QUAD;
    }
    auto elem_type = vtk_cell_to_elem[cell_type];
    std::vector<int> conn;
    for (int j = 0; j < num_cell_pts; j++) {
      auto id = cell->PointIds->GetId(j);
      conn.push_back(id);
    }

    face->add_element(elem_type, elem_id, conn);
  }
  std::cout << "[store_element_conn] Num tri: " << num_tri << std::endl;
}

//--------------------
// store_element_conn
//--------------------
// Store element connectivity into the Mesh object.
//
static void store_element_conn(vtkSmartPointer<vtkUnstructuredGrid> vtk_ugrid, Mesh* mesh)
{
  auto num_elems = vtk_ugrid->GetNumberOfCells();
  auto elem_ids = vtkIntArray::SafeDownCast(vtk_ugrid->GetCellData()->GetArray(ELEMENT_IDS_NAME.c_str()));
  if (elem_ids == nullptr) {
    throw std::runtime_error("No '" + ELEMENT_IDS_NAME + "' data found in VTK mesh.");
  }
  int num_elem_ids = elem_ids->GetNumberOfTuples();

  vtkSmartPointer<vtkUnsignedCharArray> cell_types = vtk_ugrid->GetCellTypesArray();
  auto cell = vtkGenericCell::New();

  for (int i = 0; i < num_elems; i++) {
    int elem_id = elem_ids->GetValue(i);
    vtk_ugrid->GetCell(i, cell);
    auto dim = cell->GetCellDimension();
    auto num_cell_pts = cell->GetNumberOfPoints();
    auto elem_type = vtk_cell_to_elem[cell_types->GetValue(i)];
    std::vector<int> conn;
    for (int j = 0; j < num_cell_pts; j++) {
      auto id = cell->PointIds->GetId(j);
      conn.push_back(id);
    }

    mesh->add_element(elem_type, elem_id, conn);
  }
}

//-------------------
// store_element_ids
//-------------------
// [TODO] I don't think we need this.
//
static void store_element_ids(vtkSmartPointer<vtkUnstructuredGrid> vtk_ugrid, Mesh* mesh)
{
  auto elem_ids = vtkIntArray::SafeDownCast(vtk_ugrid->GetCellData()->GetArray(ELEMENT_IDS_NAME.c_str()));
  if (elem_ids == nullptr) { 
    throw std::runtime_error("No '" + ELEMENT_IDS_NAME + "' data found in VTK mesh.");
  }
  int num_elem_ids = elem_ids->GetNumberOfTuples();
  for (int i = 0; i < num_elem_ids; i++) { 
  }
}

//--------------------
// store_nodal_coords
//--------------------
//
static void store_nodal_coords(vtkPoints* points, Face* face)
{
  vtkIdType num_nodes = points->GetNumberOfPoints();
  face->nodes_->coordinates_.resize(3*num_nodes);
  double point[3];
  for (int i = 0; i < num_nodes; i++) {
      points->GetPoint(i, point);
      face->nodes_->coordinates_[i] = point[0];
      face->nodes_->coordinates_[i+num_nodes] = point[1];
      face->nodes_->coordinates_[i+2*num_nodes] = point[2];
  }
}

//--------------------
// store_nodal_coords
//--------------------
// Store the nodal coordinates for a mesh.
//
static void store_nodal_coords(vtkPoints* points, Mesh* mesh)
{
  vtkIdType num_nodes = points->GetNumberOfPoints();
  mesh->nodes_->coordinates_.resize(3*num_nodes);
  double point[3];
  for (int i = 0; i < num_nodes; i++) {
      points->GetPoint(i, point);
      mesh->nodes_->coordinates_[i] = point[0];
      mesh->nodes_->coordinates_[i+num_nodes] = point[1];
      mesh->nodes_->coordinates_[i+2*num_nodes] = point[2];
      /*
      if (i < 10) {
        std::cout << i << ": " << point[0] << " " << point[1] << "  " << point[2] << std::endl;
      }
      */
  }

  /*
  for (int i = 0; i < 10; i++) {
    std::cout << i << ": " << mesh->nodes.coordinates_[i] << std::endl;
  }
  */
}

//-----------------
// store_nodal_ids
//-----------------
//
static void store_nodal_ids(vtkSmartPointer<vtkUnstructuredGrid> vtk_ugrid, Mesh* mesh)
{
  vtkIdType num_nodes = vtk_ugrid->GetNumberOfPoints();
  auto node_ids = vtkIntArray::SafeDownCast(vtk_ugrid->GetPointData()->GetArray(NODE_IDS_NAME.c_str()));
  if (node_ids == nullptr) {
    throw std::runtime_error("No '" + NODE_IDS_NAME + "' data found in VTK mesh.");
  }
  mesh->nodes_->ids_.resize(num_nodes);
  for (int i = 0; i < num_nodes; i++) {
    mesh->nodes_->ids_[i] = node_ids->GetValue(i);
    //std::cout << "[store_nodal_ids] nodes[" << i << "]: " << mesh->nodes.ids_[i] << std::endl;
  }
  std::cout << "[store_nodal_ids] stored " << mesh->nodes_->ids_.size() << " node ids." << std::endl;
}

static void store_nodal_ids(vtkSmartPointer<vtkPolyData> vtk_polydata, Face* face)
{
  vtkIdType num_nodes = vtk_polydata->GetNumberOfPoints();
  auto node_ids = vtkIntArray::SafeDownCast(vtk_polydata->GetPointData()->GetArray(NODE_IDS_NAME.c_str()));
  if (node_ids == nullptr) {
    throw std::runtime_error("No '" + NODE_IDS_NAME + "' data found in VTK mesh.");
  }
  face->nodes_->ids_.resize(num_nodes);
  for (int i = 0; i < num_nodes; i++) {
    face->nodes_->ids_[i] = node_ids->GetValue(i);
  }
  std::cout << "[store_nodal_ids] stored " << face->nodes_->ids_.size() << " node ids." << std::endl;
}

/////////////////////////////////////////////////////////////////
//             E x p o s e d    U t i l i t i e s              //
/////////////////////////////////////////////////////////////////

//--------------------
// load_mesh_from_vtp
//--------------------
// Store a surface mesh read from a VTK .vtp file into a Face object.
//
void load_mesh_from_vtp(const std::string& file_name, Face* face)
{
  std::cout << "===== load_mesh_from_vtp ===== " << std::endl;
  auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
  reader->SetFileName(file_name.c_str());
  reader->Update();
  vtkSmartPointer<vtkPolyData> vtk_polydata = reader->GetOutput();

  vtkIdType num_nodes = vtk_polydata->GetNumberOfPoints();
  std::cout << "[load_mesh_from_vtp] Number of nodes: " << num_nodes << std::endl;
  vtkIdType num_elems = vtk_polydata->GetNumberOfCells();
  std::cout << "[load_mesh_from_vtp] Number of elements: " << num_elems << std::endl;

  // Store nodal coordinates.
  auto points = vtk_polydata->GetPoints();
  store_nodal_coords(points, face);

  // Store nodal IDs.
  store_nodal_ids(vtk_polydata, face);

  // Store element connectivity.
  store_element_conn(vtk_polydata, face);

  std::cout << "[load_mesh_from_vtp] Done. " << std::endl;
}

//--------------------
// load_mesh_from_vtu
//--------------------
//
void load_mesh_from_vtu(const std::string& file_name, Mesh* mesh)
{
  std::cout << "===== load_mesh_from_vtu ===== " << std::endl;
  auto reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
  reader->SetFileName(file_name.c_str());
  reader->Update();
  vtkSmartPointer<vtkUnstructuredGrid> vtk_ugrid = reader->GetOutput();

  vtkIdType num_nodes = vtk_ugrid->GetNumberOfPoints();
  std::cout << "[load_mesh_from_vtu] Number of nodes: " << num_nodes << std::endl;
  vtkIdType num_elems = vtk_ugrid->GetNumberOfCells();
  std::cout << "[load_mesh_from_vtu] Number of elements: " << num_elems << std::endl;

  // Store nodal coordinates.
  auto points = vtk_ugrid->GetPoints();
  store_nodal_coords(points, mesh);

  // Store nodal IDs.
  store_nodal_ids(vtk_ugrid, mesh);

  // Store element connectivity.
  store_element_conn(vtk_ugrid, mesh);

}


