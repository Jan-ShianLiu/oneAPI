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

#ifndef SDLT_AOS_1D_BY_STRIDE_ELEMENT_H
#define SDLT_AOS_1D_BY_STRIDE_ELEMENT_H

#include "../../common.h"
#include "../../internal/aos1d_by_stride/exporter.h"
#include "../../internal/aos1d_by_stride/importer.h"
#include "../../internal/aos1d_by_stride/member.h"
#include "../../primitive.h"
#include "../disambiguator.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../rvalue_operator_helper.h"
#include "../unqualified.h"


namespace sdlt
{
namespace __SDLT_IMPL
{
namespace internal
{
namespace aos1d_by_stride
{

template<
    typename PrimitiveT,
    typename IndexD1T
>
class element
: public primitive<PrimitiveT>::template member_interface< element<PrimitiveT, IndexD1T>, 0, internal::aos1d_by_stride::member<PrimitiveT, IndexD1T>::template proxy >
// By inheriting from the primitive's aos_interface,
// methods to access to each individual data member of PrimitiveT
// are generated
{
public:
    typedef PrimitiveT value_type;

    explicit SDLT_INLINE
    element(
        unsigned char * const a_data,
        const IndexD1T an_index_d1,
        disambiguator)  // To maintain compatibility with aos1d_by_struct::const_element, although no practical use here
    : m_data(a_data)
    , m_index(an_index_d1)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    element(const element &other)
    : m_data(other.m_data)
    , m_index(other.m_index)
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
    const PrimitiveT &
    operator = (const PrimitiveT &a_value) {
        importer<PrimitiveT, 0, IndexD1T> imp(
            m_data,
            m_index);

        // In order to use the same layout strategy for import and export
        // we have to use non-const parameter,
        // thus we have to remove constness of the value
        primitive<PrimitiveT>::transfer(imp, const_cast<PrimitiveT &>(a_value));
        return a_value;
    }

    // NOTE: primitive_type const prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator PrimitiveT const () const {
        PrimitiveT value;
        exporter<PrimitiveT, 0, IndexD1T> exporter(
            m_data,
            m_index);

        primitive<PrimitiveT>::transfer(exporter, value);
        return value;
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

    template<typename MemberDataT, int OffsetT>
    SDLT_INLINE
    typename internal::aos1d_by_stride::member<PrimitiveT, IndexD1T>::template proxy<MemberDataT, OffsetT> member() const
    {
        return typename internal::aos1d_by_stride::member<PrimitiveT, IndexD1T>::template proxy<MemberDataT, OffsetT>(m_data, m_index);
    }

private:
    unsigned char * const m_data;
    IndexD1T m_index;
};


} // namespace aos1d_by_stride
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_1D_BY_STRIDE_ELEMENT_H
