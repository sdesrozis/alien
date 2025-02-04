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
/* IThreadImplementation.h                                     (C) 2000-2019 */
/*                                                                           */
/* Interface d'un service implémentant le support des threads.               */
/*---------------------------------------------------------------------------*/
#ifndef ARCCORE_CONCURRENCY_ITHREADIMPLEMENTATION_H
#define ARCCORE_CONCURRENCY_ITHREADIMPLEMENTATION_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arccore/concurrency/ConcurrencyGlobal.h"
#include "arccore/base/BaseTypes.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arccore
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Interface d'un service implémentant le support des threads.
 *
 * Ce service utilise un compteur de référence et doit être détruit
 * s'il n'y a plus de références dessus. Il ne doit donc en général pas
 * être détruit explicitement.
 */
class ARCCORE_CONCURRENCY_EXPORT IThreadImplementation
{
 public:
  typedef ReferenceCounterTag ReferenceCounterTagType;
 protected:
  virtual ~IThreadImplementation() = default;
 public:
  virtual void addReference() =0;
  virtual void removeReference() =0;
 public:
  virtual void initialize() =0;
 public:
  virtual ThreadImpl* createThread(IFunctor* f) =0;
  virtual void joinThread(ThreadImpl* t) =0;
  virtual void destroyThread(ThreadImpl* t) =0;

  virtual void createSpinLock(Int64* spin_lock_addr) =0;
  virtual void lockSpinLock(Int64* spin_lock_addr,Int64* scoped_spin_lock_addr) =0;
  virtual void unlockSpinLock(Int64* spin_lock_addr,Int64* scoped_spin_lock_addr) =0;

  virtual MutexImpl* createMutex() =0;
  virtual void destroyMutex(MutexImpl*) =0;
  virtual void lockMutex(MutexImpl* mutex) =0;
  virtual void unlockMutex(MutexImpl* mutex) =0;

  virtual Int64 currentThread() =0;

  virtual IThreadBarrier* createBarrier() =0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // End namespace Arccore

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif
