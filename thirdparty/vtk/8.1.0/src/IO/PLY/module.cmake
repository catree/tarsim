vtk_module(vtkIOPLY
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkRendering${VTK_RENDERING_BACKEND}
    vtkIOImage
    vtkTestingRendering
    vtkInteractionStyle
  KIT
    vtkIO
  DEPENDS
    vtkCommonCore
    vtkIOCore
  PRIVATE_DEPENDS
    vtkCommonDataModel
    vtkCommonMisc
    vtksys
  )
