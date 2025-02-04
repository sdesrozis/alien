/*
 * Copyright 2020 IFPEN-CEA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <alien/utils/Precomp.h>

namespace Alien
{

class IReverseIndexer
{
 public:
  typedef std::pair<Arccore::Int32, Arccore::Int32> Index;
  typedef Arccore::Integer Offset;

 public:
  virtual ~IReverseIndexer() {}

  virtual Index operator[](Offset off) const = 0;
  virtual void record(Offset off, Index i) = 0;

  virtual Arccore::Int32 size() const = 0;
};

} // namespace Alien
