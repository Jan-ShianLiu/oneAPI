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

#ifndef SDLT_AOS_1D_BY_STRUCT_ELEMENT_H
#define SDLT_AOS_1D_BY_STRUCT_ELEMENT_H

#include "../../common.h"
#include "../../internal/aos1d_by_struct/member.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../post_offset_calculator.h"
#include "../rvalue_operator_helper.h"
#include "../unqualified.h"


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
class element
: public primitive<PrimitiveT>::template aos_interface< element<PrimitiveT, IndexD1T>, member >
// By inheriting from the primitive's aos_interface,
// methods to access to each individual data member of PrimitiveT
// are generated
{
public:
    typedef PrimitiveT value_type;

    explicit SDLT_INLINE
    element(
        PrimitiveT * const a_data,
        const IndexD1T an_index_d1)
    : m_data(a_data)
    , m_index(an_index_d1)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }

    explicit SDLT_INLINE
    element(
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
    element(const element &other)
    : m_data(other.m_data)
    , m_index(other.m_index)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }

    // NOTE:  the return value is constant, intent is to allow chained
    // assignments, not modification of return value.  As we can't ever
    // provide direct access to the underlying object for all containers,
    // just import export of its values.
    // We could have returned *this, but in the case of chaining wanted
    // to avoid re-exporting the primitive when we already have it
    // represented as a_value
    SDLT_INLINE
    const value_type &
    operator = (const value_type &a_value) {
        m_data[value_of(m_index)] = a_value;
        return a_value;
    }

    // NOTE: primitive_type const prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator value_type const () const {
        return m_data[value_of(m_index)];
    }

    // can't inherit assignment operator so have to define it here
    SDLT_INLINE
    element &
    operator = (const element &other) {

        PrimitiveT const value = other;
        this->operator = (value);
        return *this;
    }

    // Support Argument Dependent Lookup to specialize for std::swap
    // NOTE:  Iterators return an instance, not the usual reference
    // to the data.  And some algorithms like std::swap_iter
    // will call swap with value, not reference, arguments.
    // For this reason we override the value version of swap,
    // not the reference.
    friend SDLT_INLINE
    void swap(element left, element right)
    {
        // NOTE:  member swap implemented by pass_through_land_rvalue_operators_xmacro
        left.swap(right);
    }

    // By including pass_through_land_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF element
    #include "../../internal/pass_through_land_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    SDLT_INLINE
    PrimitiveT & ref_data() const
    {
        return m_data[value_of(m_index)];
    }

private:
    PrimitiveT * const m_data;
    IndexD1T m_index;
};


} // namespace aos1d_by_struct
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_1D_BY_STRUCT_ELEMENT_H