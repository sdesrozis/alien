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

#include <iostream> // FIXME: remove from header
#include <memory>

#include <alien/kernels/dok/DoKLocalMatrixIndexer.h>
#include <alien/kernels/dok/ILocalMatrixIndexer.h>
#include <alien/kernels/dok/IReverseIndexer.h>

namespace Alien
{

template <typename NNZValue>
class DoKLocalMatrixT
{
 public:
  typedef DoKLocalMatrixT<NNZValue> ThisClass;

 public:
  DoKLocalMatrixT()
  : m_indexer(new DoKLocalMatrixIndexer())
  , m_offset(0)
  , m_values()
  {}

  virtual ~DoKLocalMatrixT() {}

  ThisClass& operator=(ThisClass&& src)
  {
    m_indexer = std::move(src.m_indexer);
    m_r_indexer = std::move(src.m_r_indexer);

    if (&m_values != &src.m_values) {
      m_values = src.m_values;
      src.m_values.clear();
    }
    return *this;
  }

  ThisClass& operator=(const ThisClass& src)
  {
    m_indexer.reset(src.m_indexer->clone());
    m_values = src.m_values;
    m_r_indexer.reset(nullptr);

    return *this;
  }

  void setMaxNnz(Integer size) { _reallocate(size); }

  void set(Int32 i, Int32 j, const NNZValue& val)
  {
    auto offset = m_indexer->create(i, j, m_offset);
    if (offset == (Integer)m_values.size()) {
      m_values.add(val);
    }
    else {
      m_values[offset] = val;
    }
  }

  void compact()
  {
    UniqueArray<ILocalMatrixIndexer::Renumbering> perm(m_offset);
    m_r_indexer.reset(m_indexer->sort(perm));
    UniqueArray<NNZValue> old_vals = m_values;
    for (auto curr : perm) {
      m_values[curr.second] = old_vals[curr.first];
    }
  }

  IReverseIndexer* getReverseIndexer() const { return m_r_indexer.get(); }

  ILocalMatrixIndexer* getIndexer() const { return m_indexer.get(); }

  ConstArrayView<NNZValue> getValues() const { return m_values; }

  void dump()
  {
    if (m_r_indexer == nullptr)
      this->compact();

    std::cout << "Number of elements: " << m_values.size() << "\n";
    for (int i = 0; i < m_r_indexer->size(); ++i)
      std::cout
      << "( " << (*m_r_indexer)[i].first << " , " << (*m_r_indexer)[i].second << " ) "
      << " = "
      << m_values[m_indexer->find((*m_r_indexer)[i].first, (*m_r_indexer)[i].second)]
      << "\n";
  }

 private:
  void _reallocate(Integer size = 0)
  {
    if (size || (size < m_offset + 1))
      size = m_offset + 1;
    m_values.resize(size);
  }

 private:
  std::unique_ptr<ILocalMatrixIndexer> m_indexer;
  ILocalMatrixIndexer::Offset m_offset;
  UniqueArray<NNZValue> m_values;
  std::unique_ptr<IReverseIndexer> m_r_indexer;
};

} // namespace Alien
