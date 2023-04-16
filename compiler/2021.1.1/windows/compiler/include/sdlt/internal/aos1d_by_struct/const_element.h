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

#ifndef SDLT_AOS_1D_BY_STRUCT_CONST_ELEMENT_H
#define SDLT_AOS_1D_BY_STRUCT_CONST_ELEMENT_H

#include "../../common.h"
#include "../../internal/aos1d_by_struct/const_member.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../disambiguator.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"

namespace sdlt
{
namespace __SDLT_IMPL
{
namespace internal
{
namespace aos1d_by_struct
{

template<
    typename PrimitiveT,
    typename IndexD1T
>
class const_element
: public primitive<PrimitiveT>::template aos_interface< const_element<PrimitiveT,
                                                                    IndexD1T>,
                                                       const_member >
// By inheriting from the primitive's aos_interface,
// methods to access to each individual data member of PrimitiveT
// are generated
{
public:
    typedef PrimitiveT value_type;

    explicit SDLT_INLINE
    const_element(
        PrimitiveT const * const a_data,
        const IndexD1T an_index_d1)
    : m_data(a_data)
    , m_index(an_index_d1)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }

    explicit SDLT_INLINE
    const_element(
        // NOTE: even though the data should be treated as constant,
        // the pointer we are being passed a reference to is a constant pointer
        // to non-const data, and we must accept it correctly for the
        // const_cast to work.  This happens when creating an const_element
        // directly from a Container vs. an const_accessor
        unsigned char * const a_data,
        const IndexD1T an_index_d1,
        disambiguator)
    : m_data(reinterpret_cast<PrimitiveT *>(a_data))
    , m_index(an_index_d1)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_element(const const_element &other)
    : m_data(other.m_data)
    , m_index(other.m_index)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }

    // NOTE: const primitive_type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator PrimitiveT const () const {
        return m_data[value_of(m_index)];
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_element
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"


protected:
    template<typename>
    friend class sdlt::primitive;
public:
    SDLT_INLINE
    const PrimitiveT & ref_data() const
    {
        return m_data[value_of(m_index)];
    }

private:
    PrimitiveT const * const m_data;
    IndexD1T m_index;
};


} // namespace aos1d_by_struct
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_1D_BY_STRUCT_CONST_ELEMENT_H
