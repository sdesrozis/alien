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
/* TraceInfo.h                                                 (C) 2000-2018 */
/*                                                                           */
/* Informations de trace.                                                    */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_BASE_TRACEINFO_H
#define ARCCORE_BASE_TRACEINFO_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/base/ArccoreGlobal.h"

#include <iosfwd>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Informations de trace.
 */
class TraceInfo
{
 public:
  TraceInfo()
  : m_file("(None)"), m_name("(None)"), m_line(-1) {}
  TraceInfo(const char* afile,const char* func_name,int aline)
  : m_file(afile), m_name(func_name), m_line(aline), m_print_signature(true) {}
  TraceInfo(const char* afile,const char* func_name,int aline,bool print_signature)
  : m_file(afile), m_name(func_name), m_line(aline), m_print_signature(print_signature) {}
 public:
  const char* file() const { return m_file; }
  int line() const { return m_line; }
  const char* name() const { return m_name; }
  bool printSignature() const { return m_print_signature; }
 private:
  const char* m_file;
  const char* m_name;
  int m_line;
  bool m_print_signature;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C++" ARCCORE_BASE_EXPORT
std::ostream& operator<<(std::ostream& o,const TraceInfo&);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef __GNUG__
#define A_FUNCINFO \
::Arccore::TraceInfo(__FILE__,__PRETTY_FUNCTION__,__LINE__)
#define A_FUNCNAME \
::Arccore::TraceInfo(__FILE__,__PRETTY_FUNCTION__,__LINE__,false)
#else
// Normalement valide uniquement avec extension c99
#ifdef ARCCORE_OS_WIN32
#define A_FUNCINFO \
::Arccore::TraceInfo(__FILE__,__FUNCTION__,__LINE__)
#define A_FUNCNAME \
::Arccore::TraceInfo(__FILE__,__FUNCTION__,__LINE__,false)
#else
#define A_FUNCINFO \
::Arccore::TraceInfo(__FILE__,__func__,__LINE__)
#define A_FUNCNAME \
::Arccore::TraceInfo(__FILE__,__func__,__LINE__,false)
#endif
#endif

#define A_FUNCINFO1(name)\
::Arccore::TraceInfo(__FILE__,name,__LINE__)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif  

