//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2014 National Technology & Engineering Solutions of Sandia, LLC (NTESS).
//  Copyright 2014 UT-Battelle, LLC.
//  Copyright 2014 Los Alamos National Security.
//
//  Under the terms of Contract DE-NA0003525 with NTESS,
//  the U.S. Government retains certain rights in this software.
//
//  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
//  Laboratory (LANL), the U.S. Government retains certain rights in
//  this software.
//============================================================================

#include <vtkm/worklet/DispatcherMapField.h>

namespace
{

template <typename DeviceAdapter>
class DeduceCellSet
{
  mutable vtkm::worklet::Tetrahedralize Worklet;
  vtkm::cont::CellSetSingleType<>& OutCellSet;

public:
  DeduceCellSet(vtkm::worklet::Tetrahedralize worklet, vtkm::cont::CellSetSingleType<>& outCellSet)
    : Worklet(worklet)
    , OutCellSet(outCellSet)
  {
  }

  template <typename CellSetType>
  void operator()(const CellSetType& cellset) const
  {
    this->OutCellSet = Worklet.Run(cellset, DeviceAdapter());
  }
};
}

namespace vtkm
{
namespace filter
{

//-----------------------------------------------------------------------------
inline VTKM_CONT Tetrahedralize::Tetrahedralize()
  : vtkm::filter::FilterDataSet<Tetrahedralize>()
  , Worklet()
{
}

//-----------------------------------------------------------------------------
template <typename DerivedPolicy, typename DeviceAdapter>
inline VTKM_CONT vtkm::filter::Result Tetrahedralize::DoExecute(
  const vtkm::cont::DataSet& input,
  const vtkm::filter::PolicyBase<DerivedPolicy>& policy,
  const DeviceAdapter&)
{
  const vtkm::cont::DynamicCellSet& cells = input.GetCellSet(this->GetActiveCellSetIndex());

  vtkm::cont::CellSetSingleType<> outCellSet;
  DeduceCellSet<DeviceAdapter> tetrahedralize(this->Worklet, outCellSet);

  vtkm::cont::CastAndCall(vtkm::filter::ApplyPolicy(cells, policy), tetrahedralize);

  // create the output dataset
  vtkm::cont::DataSet output;
  output.AddCellSet(outCellSet);
  output.AddCoordinateSystem(input.GetCoordinateSystem(this->GetActiveCoordinateSystemIndex()));

  return vtkm::filter::Result(output);
}

//-----------------------------------------------------------------------------
template <typename T, typename StorageType, typename DerivedPolicy, typename DeviceAdapter>
inline VTKM_CONT bool Tetrahedralize::DoMapField(
  vtkm::filter::Result& result,
  const vtkm::cont::ArrayHandle<T, StorageType>& input,
  const vtkm::filter::FieldMetadata& fieldMeta,
  const vtkm::filter::PolicyBase<DerivedPolicy>&,
  const DeviceAdapter& device)
{
  // point data is copied as is because it was not collapsed
  if (fieldMeta.IsPointField())
  {
    result.GetDataSet().AddField(fieldMeta.AsField(input));
    return true;
  }

  // cell data must be scattered to the cells created per input cell
  if (fieldMeta.IsCellField())
  {
    vtkm::cont::ArrayHandle<T> output = this->Worklet.ProcessCellField(input, device);

    result.GetDataSet().AddField(fieldMeta.AsField(output));
    return true;
  }

  return false;
}
}
}
