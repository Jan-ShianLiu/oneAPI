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

#ifndef SDLT_SIMD_ELEMENT_H
#define SDLT_SIMD_ELEMENT_H


#include "../../internal/simd/root_access.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward declarations
template <typename StructT>
class primitive;

namespace internal {
namespace simd {

// proxy objects for use with sdlt::primitive<T>::simd_type
// The simd_type can be accessed with array subscript with a simd lane index:
// T data = simdData[simdlane_index];
// and return an const_element or element proxy object which can provide
// import export functionality for that particular data lane.
// And like the other Accessors, also provides an * operator to export T
// without doing a static cast.  Can be combined with "auto" keyword.  IE:
// auto data = *simdData[simdlane_index];
// Without the *, the type of data would have been the proxy itself,
// rather than the desired T.
template<typename SimdDataT>
class const_element
{
public:
    typedef typename SimdDataT::primitive_type primitive_type;
    static const int simdlane_count = SimdDataT::simdlane_count;

    SDLT_INLINE
    const_element(const SimdDataT &a_simd_data, const int a_simdlane_index)
    : m_simdlane_index(a_simdlane_index)
    , m_simd_data(a_simd_data)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_element(const const_element &other)
    : m_simdlane_index(other.m_simdlane_index)
    , m_simd_data(other.m_simd_data)
    {
    }

    // NOTE: const PrimitiveType type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE \
    operator primitive_type const () const {
        primitive_type value;
        primitive<primitive_type>::template simd_extract<SimdDataT, typename root_access<SimdDataT>::type>(m_simd_data, m_simdlane_index, value);
        return value;
    }

    // NOTE: const PrimitiveType type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE friend
    primitive_type const unproxy(const const_element &self)
    {
        return self.operator primitive_type const();
    }


protected:
    const int m_simdlane_index;
    const SimdDataT & m_simd_data;
};

template<typename SimdDataT>
class element
{
public:
    typedef typename SimdDataT::primitive_type primitive_type;
    static const int simdlane_count = SimdDataT::simdlane_count;

    SDLT_INLINE
    element(SimdDataT &a_simd_data, const int a_simdlane_index)
    : m_simdlane_index(a_simdlane_index)
    , m_simd_data(a_simd_data)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    element(const element &other)
    : m_simdlane_index(other.m_simdlane_index)
    , m_simd_data(other.m_simd_data)
    {
    }

    // NOTE:  the return value is constant, intent is to allow chained
    // assignments, not modification of return value.  As we can't ever
    // provide direct access to the underlying object for all containers,
    // just import export of its values.
    // We could have returned *this, but in the case of chaining wanted
    // to avoid re-exporting the primitive when we already have it
    // represented as a_value
    SDLT_INLINE
    const primitive_type &
    operator = (const primitive_type &a_value) {
        primitive<primitive_type>::template simd_import<SimdDataT, typename root_access<SimdDataT>::type>(m_simd_data, m_simdlane_index, a_value);
        return a_value;
    }

    // NOTE: const primitive_type type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator primitive_type const () const {
        primitive_type value;
        primitive<primitive_type>::template simd_extract<SimdDataT, typename root_access<SimdDataT>::type>(m_simd_data, m_simdlane_index, value);
        return value;
    }

    // NOTE: const primitive_type type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE friend
    primitive_type const unproxy(const element &self)
    {
        return self.operator primitive_type const();
    }


protected:
    const int m_simdlane_index;
    SimdDataT & m_simd_data;
};

} // namespace simd
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SIMD_ELEMENT_H
