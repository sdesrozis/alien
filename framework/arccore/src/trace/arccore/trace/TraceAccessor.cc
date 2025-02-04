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
/* TraceAccessor.cc                                            (C) 2000-2020 */
/*                                                                           */
/* Accès aux traces.                                                         */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/trace/ITraceMng.h"
#include "arccore/trace/TraceAccessor.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceAccessor::
TraceAccessor(ITraceMng* trace)
: m_local_verbose_level(TraceMessage::DEFAULT_LEVEL)
{
  if (trace)
    m_trace = makeRef(trace);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceAccessor::
TraceAccessor(const TraceAccessor& rhs)
: m_trace(rhs.m_trace)
, m_local_verbose_level(rhs.m_local_verbose_level)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceAccessor& TraceAccessor::
operator=(const TraceAccessor& rhs)
{
  if (&rhs!=this){
    m_trace = rhs.m_trace;
    m_local_verbose_level = rhs.m_local_verbose_level;
  }
  return (*this);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceAccessor::
~TraceAccessor()
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ITraceMng* TraceAccessor::
traceMng() const
{
  return m_trace.get();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef ARCCORE_DEBUG
TraceMessageDbg TraceAccessor::
debug(Trace::eDebugLevel dbg_lvl) const
{
  return m_trace->debug(dbg_lvl);
}
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Trace::eDebugLevel TraceAccessor::
configDbgLevel() const
{
  return m_trace->configDbgLevel();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
info() const
{
  return m_trace->info();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
info(char category) const
{
  return m_trace->info(category);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
info(bool v) const
{
  return m_trace->info(v);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
info(Int32 verbose_level) const
{
  //cout << "LOCAL level=" << verbose_level << " local=" << m_local_verbose_level << '\n';
  return m_trace->info(verbose_level); //m_local_verbose_level>=verbose_level);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
pinfo() const
{
  return m_trace->pinfo();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
pinfo(char category) const
{
  return m_trace->info(category);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
logdate() const
{
  return m_trace->logdate();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
log() const
{
  return m_trace->log();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
plog() const
{
  return m_trace->plog();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
warning() const
{
  return m_trace->warning();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
pwarning() const
{
  return m_trace->pwarning();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
error() const
{
  return m_trace->error();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
perror() const
{
  return m_trace->perror();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
fatal() const
{
  return m_trace->fatal();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

TraceMessage TraceAccessor::
pfatal() const
{
  return m_trace->pfatal();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
