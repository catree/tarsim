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
#ifndef vtk_m_internal__ExportMacros_h
#define vtk_m_internal__ExportMacros_h

#include <vtkm/internal/Configure.h>

/*!
  * Export macros for various parts of the VTKm library.
  */

#ifdef VTKM_CUDA
#define VTKM_EXEC __device__ __host__
#define VTKM_EXEC_CONT __device__ __host__
#if __CUDAVER__ >= 75000
#define VTKM_SUPPRESS_EXEC_WARNINGS #pragma nv_exec_check_disable
#else
#define VTKM_SUPPRESS_EXEC_WARNINGS #pragma hd_warning_disable
#endif
#define VTKM_EXEC_CONSTANT __device__ __constant__
#else
#define VTKM_EXEC
#define VTKM_EXEC_CONT
#define VTKM_SUPPRESS_EXEC_WARNINGS
#define VTKM_EXEC_CONSTANT
#endif

#define VTKM_CONT

// Background:
// VTK-m libraries are built with the hidden symbol visibility by default.
// This means that all template classes that are defined in VTK-m headers are
// defined with hidden visibility when we are building VTK-m. But when those
// headers are included by a third-party which has differing visibility controls
// causing link time warnings ( external vs private ).
//
// Issue:
// Dynamic cast is not just based on the name of the class, but also the
// combined visibility of the class on OSX. When building the hash_code of
// an object the symbol visibility controls of the type are taken into
// consideration ( including symbol vis of template parameters ).
// Therefore if a class has a component with private/hidden vis than it
// can't be passed across library boundaries.
//
//  Example is PolymorphicArrayHandleContainer<T,S> whose visibility affected
//  by the visibility of both T and S.
//
// Solution:
// The solution is fairly simple, but annoying. You need to mark
//
// TL;DR:
// This markup is used when we want to make sure:
//  - The class can be compiled into multiple libraries and at runtime will
//    resolve to a single type instance
//  - Be a type ( or component of a types signature ) that can be passed between
//    dynamic libraries and requires RTTI support ( dynamic_cast ).
#if defined(VTKM_MSVC)
#define VTKM_ALWAYS_EXPORT
#define VTKM_NEVER_EXPORT
#else
#define VTKM_ALWAYS_EXPORT __attribute__((visibility("default")))
#define VTKM_NEVER_EXPORT __attribute__((visibility("hidden")))
#endif

// constexpr support was added to VisualStudio 2015 and above. So this makes
// sure when that we gracefully fall back to just const when using 2013
#if defined(VTKM_MSVC) && _MSC_VER < 1900
#define VTKM_CONSTEXPR const
#define VTKM_NOEXCEPT
#else
#define VTKM_CONSTEXPR constexpr
#define VTKM_NOEXCEPT noexcept
#endif

#define VTKM_OVERRIDE override

// Clang will warn about weak vtables (-Wweak-vtables) on exception classes,
// but there's no good way to eliminate them in this case because MSVC (See
// http://stackoverflow.com/questions/24511376). These macros will silence the
// warning for classes defined within them.
#ifdef VTKM_CLANG
#define VTKM_SILENCE_WEAK_VTABLE_WARNING_START                                                     \
  _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wweak-vtables\"")
#define VTKM_SILENCE_WEAK_VTABLE_WARNING_END _Pragma("clang diagnostic pop")
#else // VTKM_CLANG
#define VTKM_SILENCE_WEAK_VTABLE_WARNING_START
#define VTKM_SILENCE_WEAK_VTABLE_WARNING_END
#endif // VTKM_CLANG

/// Simple macro to identify a parameter as unused. This allows you to name a
/// parameter that is not used. There are several instances where you might
/// want to do this. For example, when using a parameter to overload or
/// template a function but do not actually use the parameter. Another example
/// is providing a specialization that does not need that parameter.
#define vtkmNotUsed(parameter_name)

#endif //vtk_m_internal__ExportMacros_h
