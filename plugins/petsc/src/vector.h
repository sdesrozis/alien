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

#include <alien/core/impl/IVectorImpl.h>

#include <petscvec.h>

namespace Alien::PETSc {

    class Vector : public IVectorImpl {
    public:
        explicit Vector(const MultiVectorImpl *multi_impl);

        ~Vector() override;

    public:
        void setProfile(int ilower, int iupper);

        void setValues(Arccore::ConstArrayView<double> values);

        void getValues(Arccore::ArrayView<double> values) const;

        void assemble();

        Vec internal() { return m_vec; }

        Vec internal() const { return m_vec; }

    private:
        Vec m_vec;
        MPI_Comm m_comm;

        Arccore::UniqueArray<Arccore::Integer> m_rows;
    };

} // namespace Alien::PETSc
