vtk_module(vtkIOXdmf2
  TCL_NAME vtkIOXdmfII
  GROUPS
  TEST_DEPENDS
    vtkFiltersGeneral
    vtkTestingCore
  KIT
    vtkIO
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkCommonExecutionModel
    vtkIOLegacy
  PRIVATE_DEPENDS
    vtkFiltersExtraction
    vtkIOXMLParser
    vtksys
    vtkxdmf2
  )