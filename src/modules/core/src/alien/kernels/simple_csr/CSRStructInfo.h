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

#include <vector>

#include <algorithm>
#include <alien/kernels/simple_csr/SimpleCSRPrecomp.h>

/*---------------------------------------------------------------------------*/

namespace Alien::SimpleCSRInternal
{

/*---------------------------------------------------------------------------*/

class CSRStructInfo
{
 public:
  enum ColOrdering
  {
    eUndef,
    eOwnAndGhost,
    eFull
  };

 public:
  CSRStructInfo(bool is_variable_block = false)
  : m_is_variable_block(is_variable_block)
  , m_nrow(0)
  , m_col_ordering(eUndef)
  , m_diag_first(true)
  , m_symmetric(true)
  {}

  CSRStructInfo(Integer nrow, bool is_variable_block = false)
  : m_is_variable_block(is_variable_block)
  , m_nrow(nrow)
  , m_col_ordering(eUndef)
  , m_diag_first(true)
  , m_symmetric(true)
  {
    m_row_offset.resize(nrow + 1);
    if (m_is_variable_block)
      m_block_row_offset.resize(nrow + 1);
  }

  CSRStructInfo(Integer nrow, const int* kcol, const int* cols)
  : m_is_variable_block(false)
  , m_nrow(nrow)
  , m_col_ordering(eUndef)
  , m_diag_first(true)
  , m_symmetric(true)
  {
    m_row_offset.resize(nrow + 1);
    std::copy(kcol, kcol + nrow + 1, m_row_offset.data());
    Integer nnz = kcol[nrow];
    m_cols.resize(nnz);
    std::copy(cols, cols + nnz, m_cols.data());
  }

  CSRStructInfo(const CSRStructInfo& src) { copy(src); }

  CSRStructInfo& operator=(const CSRStructInfo& src)
  {
    this->copy(src);
    return *this;
  }

  CSRStructInfo* clone() const { return new CSRStructInfo(*this); }

  void init(Arccore::Integer nrow)
  {
    m_nrow = nrow;
    m_row_offset.resize(nrow + 1);
    if (m_is_variable_block)
      m_block_row_offset.resize(nrow + 1);
  }

  void init(Integer nrows, Integer nnz)
  {
    assert(!m_is_variable_block);
    m_nrow = nrows;
    m_row_offset.resize(nrows + 1);
    m_row_offset.fill(0);
    m_row_offset[nrows] = nnz;
    m_cols.resize(nnz);
  }

  Arccore::Int64 timestamp() const { return m_timestamp; }

  void setTimestamp(Arccore::Int64 value) { m_timestamp = value; }

  bool getSymmetric() const { return m_symmetric; }

  void setSymmetric(bool value) { m_symmetric = value; }

  Integer getNRows() const { return m_nrow; }

  Integer getNRow() const { return m_nrow; }

  Arccore::UniqueArray<Arccore::Integer>& getBlockRowOffset()
  {
    return m_block_row_offset;
  }

  Arccore::ArrayView<Arccore::Integer> getRowOffset() { return m_row_offset.view(); }

  Arccore::ConstArrayView<Arccore::Integer> getRowOffset() const
  {
    return m_row_offset.constView();
  }

  const int* kcol() const { return m_row_offset.data(); }

  int* kcol() { return m_row_offset.data(); }

  ConstArrayView<Integer> getBlockRowOffset() const
  {
    return m_block_row_offset.constView();
  }

  UniqueArray<Integer>& getCols() { return m_cols; }

  UniqueArray<Integer>& getBlockCols() { return m_block_cols; }

  ConstArrayView<Integer> getCols() const { return m_cols.constView(); }

  int* cols() { return m_cols.data(); }

  const int* cols() const { return m_cols.data(); }

  ConstArrayView<Integer> getBlockCols() const { return m_block_cols.constView(); }

  ColOrdering& getColOrdering() { return m_col_ordering; }

  ColOrdering getColOrdering() const { return m_col_ordering; }

  void setDiagFirst(bool val) { m_diag_first = val; }

  bool getDiagFirstOpt() const { return m_diag_first; }

  Integer getRowSize(Integer row) const
  {
    return m_row_offset[row + 1] - m_row_offset[row];
  }

  Integer getBlockRowSize(Integer row) const
  {
    return m_block_row_offset[row + 1] - m_block_row_offset[row];
  }

  Integer getNnz() const { return m_row_offset[m_nrow]; }

  Integer getNElems() const { return m_row_offset[m_nrow]; }

  Integer getBlockNnz() const { return m_block_row_offset[m_nrow]; }

  UniqueArray<Integer>& getUpperDiagIndex() { return m_upper_diag_index; }

  ConstArrayView<Integer> getUpperDiagIndex() const
  {
    return m_upper_diag_index.constView();
  }

  void allocate()
  {
    m_cols.resize(m_row_offset[m_nrow]);
    if (m_is_variable_block)
      m_block_cols.resize(m_row_offset[m_nrow]);
  }

  Integer computeBandeSize() const
  {
    int bande_size = 0;
    for (int irow = 0; irow < m_nrow; ++irow) {
      int min_col = irow;
      int max_col = irow;
      for (int k = m_row_offset[irow]; k < m_row_offset[irow + 1]; ++k) {
        int col = m_cols[k];
        min_col = std::min(col, min_col);
        max_col = std::max(col, max_col);
      }
      bande_size = std::max(bande_size, max_col - min_col);
    }
    return bande_size;
  }

  Integer computeUpperBandeSize() const
  {
    int bande_size = 0;
    for (int irow = 0; irow < m_nrow; ++irow) {
      int max_col = irow;
      for (int k = m_row_offset[irow]; k < m_row_offset[irow + 1]; ++k) {
        int col = m_cols[k];
        max_col = std::max(col, max_col);
      }
      bande_size = std::max(bande_size, max_col - irow);
    }
    return bande_size;
  }

  Integer computeLowerBandeSize() const
  {
    std::vector<int> maxRow(m_nrow);
    for (int col = 0; col < m_nrow; ++col)
      maxRow[col] = col;
    for (int irow = 0; irow < m_nrow; ++irow) {
      for (int k = m_row_offset[irow]; k < m_row_offset[irow + 1]; ++k) {
        int col = m_cols[k];
        maxRow[col] = std::max(maxRow[col], irow);
      }
    }
    int bande_size = 0;
    for (int col = 0; col < m_nrow; ++col)
      bande_size = std::max(bande_size, maxRow[col]);
    return bande_size;
  }

  void copy(const CSRStructInfo& profile)
  {
    const Integer thisNrow = profile.getNRows();
    init(thisNrow);

    m_row_offset.copy(profile.getRowOffset());
    m_block_row_offset.copy(profile.getBlockRowOffset());

    allocate();

    m_cols.copy(profile.getCols());
    m_block_cols.copy(profile.getBlockCols());

    m_col_ordering = profile.getColOrdering();

    setDiagFirst(profile.getDiagFirstOpt());

    m_upper_diag_index.copy(profile.getUpperDiagIndex());

    setSymmetric(profile.getSymmetric());
  }

 protected:
  bool m_is_variable_block = false;
  Arccore::Integer m_nrow = 0;
  Arccore::UniqueArray<Arccore::Integer> m_row_offset;
  Arccore::UniqueArray<Arccore::Integer> m_block_row_offset;
  Arccore::UniqueArray<Arccore::Integer> m_cols;
  Arccore::UniqueArray<Arccore::Integer> m_block_cols;
  ColOrdering m_col_ordering;
  bool m_diag_first = true;
  Arccore::UniqueArray<Arccore::Integer> m_upper_diag_index;
  bool m_symmetric = true;
  Arccore::Int64 m_timestamp = -1;
};

/*---------------------------------------------------------------------------*/

} // namespace Alien::SimpleCSRInternal

/*---------------------------------------------------------------------------*/
