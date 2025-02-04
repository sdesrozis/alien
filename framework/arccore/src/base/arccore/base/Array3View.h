﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2020 IFPEN-CEA
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* Array3View.h                                                (C) 2000-2018 */
/*                                                                           */
/* Vue d'un tableau 3D.                                                      */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_ARRAY3VIEW_H
#define ARCCORE_BASE_ARRAY3VIEW_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/Array2View.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue pour un tableau 3D.
 *
 * Une vue 3D peut être créée à partir d'un tableau classique (Array)
 * comme suit:
 \code
 UniqueArray<Int32> a(5*7*9);
 Array3View<Int32> view(a.unguardedBasePointer(),5,7,9);
 view[3][4][5] = 2;
 view.setItem(4,5,6, 1); // Met la valeur 1 a l'élément view[4][5][6].
 \endcode
 Pour des raisons de performance, il est préférable d'accéder aux éléments
 via operator()()
 */
template<class DataType>
class Array3View
{
 public:
  Array3View(DataType* ptr,Integer dim1_size,Integer dim2_size,Integer dim3_size)
  : m_ptr(ptr), m_dim1_size(dim1_size), m_dim2_size(dim2_size), m_dim3_size(dim3_size), 
    m_dim23_size(dim2_size*dim3_size)
  {
  }
  Array3View()
  : m_ptr(0), m_dim1_size(0), m_dim2_size(0), m_dim3_size(0), m_dim23_size(0)
  {
  }
 public:
  Integer dim1Size() const { return m_dim1_size; }
  Integer dim2Size() const { return m_dim2_size; }
  Integer dim3Size() const { return m_dim3_size; }
  Integer totalNbElement() const { return m_dim1_size*m_dim23_size; }
 public:
  Array2View<DataType> operator[](Integer i)
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    return Array2View<DataType>(m_ptr + (m_dim23_size*i),m_dim2_size,m_dim3_size);
  }
  ConstArray2View<DataType> operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    return ConstArray2View<DataType>(m_ptr + (m_dim23_size*i),m_dim2_size,m_dim3_size);
  }
  DataType item(Integer i,Integer j,Integer k) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    return m_ptr[(m_dim23_size*i) + m_dim3_size*j + k];
  }
  DataType operator()(Integer i,Integer j,Integer k) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    return m_ptr[(m_dim23_size*i) + m_dim3_size*j + k];
  }
  DataType setItem(Integer i,Integer j,Integer k,const DataType& value)
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    m_ptr[(m_dim23_size*i) + m_dim3_size*j + k] = value;
  }
 public:
  /*!
   * \brief Pointeur sur la mémoire allouée.
   */
  inline DataType* unguardedBasePointer()
  { return m_ptr; }
 private:
  DataType* m_ptr;
  Integer m_dim1_size; //!< Taille de la 1ere dimension
  Integer m_dim2_size; //!< Taille de la 2eme dimension
  Integer m_dim3_size; //!< Taille de la 3eme dimension
  Integer m_dim23_size; //!< dim2 * dim3
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \ingroup Collection
 * \brief Vue pour un tableau 3D constant.
 */
template<class DataType>
class ConstArray3View
{
 public:
  ConstArray3View(const DataType* ptr,Integer dim1_size,Integer dim2_size,Integer dim3_size)
  : m_ptr(ptr), m_dim1_size(dim1_size), m_dim2_size(dim2_size), m_dim3_size(dim3_size), 
    m_dim23_size(dim2_size*dim3_size)
  {
  }
  ConstArray3View()
  : m_ptr(0), m_dim1_size(0), m_dim2_size(0), m_dim3_size(0), m_dim23_size(0)
  {
  }
 public:
  Integer dim1Size() const { return m_dim1_size; }
  Integer dim2Size() const { return m_dim2_size; }
  Integer dim3Size() const { return m_dim3_size; }
  Integer totalNbElement() const { return m_dim1_size*m_dim23_size; }
 public:
  ConstArray2View<DataType> operator[](Integer i) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    return ConstArray2View<DataType>(m_ptr + (m_dim23_size*i),m_dim2_size,m_dim3_size);
  }
  DataType item(Integer i,Integer j,Integer k) const
  {
    ARCCORE_CHECK_AT(i,m_dim1_size);
    ARCCORE_CHECK_AT(j,m_dim2_size);
    ARCCORE_CHECK_AT(k,m_dim3_size);
    return m_ptr[(m_dim23_size*i) + m_dim3_size*j + k];
  }
 public:
  /*!
   * \brief Pointeur sur la mémoire allouée.
   */
  inline const DataType* unguardedBasePointer() const
  { return m_ptr; }
 private:
  const DataType* m_ptr;
  Integer m_dim1_size; //!< Taille de la 1ere dimension
  Integer m_dim2_size; //!< Taille de la 2eme dimension
  Integer m_dim3_size; //!< Taille de la 3eme dimension
  Integer m_dim23_size; //!< dim2 * dim3
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  
