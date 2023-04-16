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

#ifndef SDLT_UNIFORM_REF_OVER_1D_H
#define SDLT_UNIFORM_REF_OVER_1D_H


#include "common.h"
#include "internal/aligned_base.h"
#include "internal/check_for_primitive_declaration.h"
#include "internal/enable_if_type.h"
#include "internal/scalar/root_access.h"
#include "internal/uniform/const_scalar_member.h"
#include "internal/uniform/const_scalar_ref_accessor.h"
#include "internal/uniform/scalar_member.h"
#include "internal/unqualified.h"
#include "primitive.h"
#include "simd_traits.h"

namespace sdlt {
namespace __SDLT_IMPL {

// A UniformOver1d owns uniform data and presents an member_interface to an
// array of infinite length for use with sdlt Index variables.
//
// Why?  2 reasons:
// 1.  Allows a UniformOver1d to control how the uniform data in modeled and
//     provided to the SIMD loop. For example, uniform_soa_over1d will keep
//     arrays of aligned SIMD ready for access by the SIMD loop, avoiding scalar
//     broadcast overhead.
// 2.  Perhaps a templatized SIMD loop will access real array's in some scenarios
//     but in others the data will be uniform.  Rather than write 2 versions of
//     the SIMD loop, an const_accessor from a UniformOver1d maybe used allowing
//     the SIMD loop to access [i] and receive a uniform value back.
//
// The uniform_ref_over1d abstracts access to uniform data of type
// PrimitiveT.   It's primary purpose is to provide an identical member_interface to
// uniform_soa_over1d, so that this Ref(erence) version can be swapped in
// for comparison purposes.  Also the Soa (SIMD ready) version may not be faster
// for small loops.  When using a ScalarCode strategy, uniform_value_over1d or
// uniform_ref_over1d should be used (avoid uniform_soa_over1d).
//
// Only a const_accessor is provided.
// No non-const accessor is provided, because the data is uniform we don't allow
// individual [i] to be written to.  It is logically impossible.
// Instead, users can import/export a PrimitiveT directly to the UniformOver1d,
// or use data member member_interface to read or write individual members.  This
// differs from Containers which require use of an accessor to read or write.
// This convenience is possible because the data is uniform, no array subscript
// [i] is required.
//
// A UniformOver1d can't be passed inside a lambda function (just like Containers).
// A const_accessor must be retrieved and passed.
// Example usage:
//
// uniform_value_over1d<MyStruct> uniform_data_over1d;
// auto uAccessor = uniform_data_over1d.const_access();
//
// then from inside the body of a SIMD loop access it like it were an array
//
// MyStruct uData = uAccessor[i];
//
// The const_accessor behaves like any other, once an array subscript [i] is
// applied, the primitive can be extracted or the data member member_interface used to
// read individual members
//
// float left = uAccessor[i].left();
// float right = uAccessor[i].right();
//
// NOTE:  The uniform_value_over1d differs from the uniform_ref_over1d in
// that the uniform_ref_over1d only copies a reference to the data into the
// lambda function, whereas the uniform_value_over1d makes a copy of the uniform data into
// the lambda function.
// There is a distinct difference in code generation, the uniform_ref_over1d will
// load and broadcast uniform data to SIMD registers as it is used in each
// iteration of the SIMD loop.
// Whereas the uniform_value_over1d will broadcast uniform data to SIMD registers
// outside the loop, but may require pushing the SIMD version of the uniform
// data back onto the stack.  And the uniform_soa_over1d pays the price to
// organize its data into SIMD lanes before the loop is even entered, only
// passing a single pointer to its SOA data structure into the lambda function.
// This can be beneficial if the user can manage the uniform data set, only
// updating part (or none) of it before running a SIMD loop

__SDLT_INHERIT_IMPL_BEGIN

template<
    typename PrimitiveT,
    int UnusedSimdLaneCountT = 0 // for template alias compatibility
>
struct uniform_ref_over1d
: public internal::aligned_base<64>
, public sdlt::primitive<PrimitiveT>::template scalar_uniform_interface<
    uniform_ref_over1d<PrimitiveT, UnusedSimdLaneCountT>,
    PrimitiveT,
    typename internal::scalar::root_access<PrimitiveT>::type,
    internal::uniform::template const_scalar_member,
    internal::uniform::template scalar_member>
, private internal::check_for_primitive_declaration<PrimitiveT>
{
public:
    typedef PrimitiveT value_type;

protected:
    typedef typename internal::scalar::template root_access<PrimitiveT>::type root_access_type;
public:

    uniform_ref_over1d()
    {}

    __SDLT_INHERIT_IMPL_DESTRUCTOR
    ~uniform_ref_over1d() = default;

    // disallow copy construction and assignment
    uniform_ref_over1d(const uniform_ref_over1d &) = delete;
    uniform_ref_over1d & operator = (const uniform_ref_over1d &) = delete;

    typedef internal::uniform::const_scalar_ref_accessor<value_type> const_accessor;

    SDLT_INLINE
    const_accessor
    const_access() const
    {
        return const_accessor(m_data);
    }

    // Support const_access with any offset
    template<typename OffsetT>
    SDLT_INLINE
    const_accessor
    const_access(const OffsetT) const
    {
        return const_accessor(m_data);
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
        m_data = a_value;
        return a_value;
    }

    SDLT_INLINE
    operator PrimitiveT const () const {
        return m_data;
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF uniform_ref_over1d
    #include "internal/pass_through_land_rvalue_operators_xmacro.h"

private:

    template<
        typename RootScalarDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::scalar_member;

    template<
        typename RootScalarDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::const_scalar_member;

    const value_type & root_scalar_data() const { return m_data; }
    value_type & root_scalar_data() { return m_data; }
    value_type m_data;
};
__SDLT_INHERIT_IMPL_END

} // namepace __SDLT_IMPL
using __SDLT_IMPL::uniform_ref_over1d;
} // namespace sdlt

#endif // SDLT_UNIFORM_REF_OVER_1D_H
