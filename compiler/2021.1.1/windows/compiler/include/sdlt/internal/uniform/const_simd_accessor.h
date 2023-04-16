//
// Copyright (C) 2014-2016 Intel Corporation. All Rights Reserved.
//
// The source code contained or described herein and all documents related
// to the source code ("Material") are owned by Intel Corporation or its
// suppliers or licensors.  Title to the Material remains with Intel
// Corporation or its suppliers and licensors.  The Material is protected
// by worldwide copyright laws and treaty provisions.  No part of the
// Material may be used, copied, reproduced, modified, published, uploaded,
// posted, transmitted, distributed, or disclosed in any way without
// Intel's prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual
// property right is granted to or conferred upon you by disclosure or
// delivery of the Materials, either expressly, by implication, inducement,
// estoppel or otherwise. Any license under such intellectual property rights
// must be express and approved by Intel in writing.
//
// Unless otherwise agreed by Intel in writing, you may not remove or alter
// this notice or any other notice embedded in Materials by Intel or Intel's
// suppliers or licensors in any way.
//

#ifndef SDLT_UNIFORM_CONST_SIMD_ACCESSOR_H
#define SDLT_UNIFORM_CONST_SIMD_ACCESSOR_H


#include "../../common.h"
#include "../../internal/simd/indices.h"
#include "../../internal/uniform/const_simd_element.h"
#include "../enable_if_type.h"
#include "../is_simd_compatible_index.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace uniform {


template<typename SimdDataT>
class const_simd_accessor
{
public:
    typedef typename SimdDataT::primitive_type primitive_type;
    static const int simdlane_count = SimdDataT::simdlane_count;

    SDLT_INLINE
    explicit const_simd_accessor(const SimdDataT & a_simd_data)
    : m_simd_data(a_simd_data)
    {
    }

    SDLT_INLINE
    const_simd_accessor(const const_simd_accessor & other)
    : m_simd_data(other.m_simd_data)
    {
    }

    template<typename OffsetT>
    SDLT_INLINE
    const const_simd_accessor<SimdDataT> &
    reaccess(const OffsetT) const
    {
        // As the values are uniform
        // no matter what the offset this accessor will suffice
        return *this;
    }

    template<typename IndexD1T, typename = internal::enable_if_type<is_simd_compatible_index<IndexD1T, simdlane_count>::value> >
    SDLT_INLINE
    auto
    operator [] (const IndexD1T an_index_d1) const
    ->const_simd_element<SimdDataT>
    {
        __SDLT_ASSERT(an_index_d1.simdlane_index() >= 0);
        __SDLT_ASSERT(an_index_d1.simdlane_index() < simdlane_count);

        return const_simd_element<SimdDataT>(m_simd_data, an_index_d1.simdlane_index());
    }

protected:

    const SimdDataT & m_simd_data;
};

} // namespace uniform
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_UNIFORM_CONST_SIMD_ACCESSOR_H
