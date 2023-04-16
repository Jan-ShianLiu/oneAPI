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

#ifndef SDLT_UNIFORM_SOA_OVER_1D_H
#define SDLT_UNIFORM_SOA_OVER_1D_H


#include "common.h"
#include "defaults.h"
#include "internal/check_for_primitive_declaration.h"
#include "internal/enable_if_type.h"
#include "internal/simd/root_access.h"
#include "internal/uniform/const_member.h"
#include "internal/uniform/const_simd_accessor.h"
#include "internal/uniform/member.h"
#include "internal/unqualified.h"
#include "primitive.h"

namespace sdlt {
namespace __SDLT_IMPL {

// A UniformOver1d owns uniform data and presents an member_interface to an
// array of infinite length for use with simd_index variables.
//
// Why?  2 reasons:
// 1.  Allows a UniformOver1d to control how the uniform data in modeled and
//     provided to the SIMD loop. For example, uniform_simd_data_over1d will keep
//     arrays of aligned SIMD ready for access by the SIMD loop, avoiding scalar
//     broadcast overhead.
// 2.  Perhaps a templatized SIMD loop will access real array's in some scenarios
//     but in others the data will be uniform.  Rather than write 2 versions of
//     the SIMD loop, an const_accessor from a UniformOver1d maybe used allowing
//     the SIMD loop to access [i] and receive a uniform value back.
//
// The uniform_simd_data_over1d abstracts access to uniform data of type
// PrimitiveT.
//
// The uniform_simd_data_over1d will create an aligned SIMD ready version of the
// PrimitiveT.  Inside a SIMD loop, when accessing data through the const_accessor
// returned from uniform_simd_data_over1d::const_access(), aligned SIMD code can be
// emitted, skipping the loading of scalars then broadcasting them.  Also the
// const_accessor is cheap to copy into a lambda function, versus variables not accessed
// through a uniform_value_over1d, which are copied by value into the lambda function.
// This means for large objects an entire copy is made, then copied back onto
// the stack, and then possibly scalar loaded, broadcast to SIMD registers,
// and then stored back to the stack so there is a SIMD ready version for the
// SIMD loop.  When dealing with short trip counts or large objects this extra
// overhead adds up.
// What the uniform_simd_data_over1d provides is way to directly create(and manipulate)
// a SIMD ready version of the data.  Because the data has already been placed
// inside a SOA data layout which is guaranteed to be aligned, much better code
// generation can be used.
//
// 10-20% (or more) instruction reduction is possible.
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
// uniform_simd_data_over1d<MyStruct, simdlane_count> uniform_data_over1d;
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
// The user should provide the number of SIMD lanes for the SOA.
// Better yet utilize simd_traits<T>::lane_count with an appropriate T.
// It is OK if the SimdLaneCountT is greater than the number of SIMD lanes the
// loop operates at, there would just be some extra overhead involved in
// populating the Uniform container with data that wont be accessed by the
// loop, but the loop code itself is unhindered.
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
// data back onto the stack.  And the uniform_simd_data_over1d pays the price to
// organize its data into SIMD lanes before the loop is even entered, only
// passing a single pointer to its SOA data structure into the lambda function.
// This can be beneficial if the user can manage the uniform data set, only
// updating part (or none) of it before running a SIMD loop

// NOTE:  GCC 4.8.2 had internal errors when trying to compile
// uniform_simd_data_over1d template, so re-factored so that uniform_simd_data_over1d
// class takes a simd_type as a parameter rather than accepting the
// value_type, simdlane_count, and byte_alignment as parameters.
// uniform_simd_data_over1d is just a template alias to instantiate the correct
// uniform_simd_data_over1d
template<
    typename SimdDataT
>
class uniform_simd_data_over1d
: public sdlt::primitive<typename SimdDataT::primitive_type>::template uniform_interface<
        uniform_simd_data_over1d<SimdDataT>,
        SimdDataT,
        typename internal::simd::root_access<SimdDataT>::type,
        internal::uniform::template const_member,
        internal::uniform::template member
>
{
public:
    typedef typename SimdDataT::primitive_type primitive_type;
    typedef primitive_type value_type;

    typedef SimdDataT simd_data_type;
protected:
    typedef typename internal::simd::root_access<simd_data_type>::type root_access_type;
public:
    uniform_simd_data_over1d() = default;

    // disallow copy construction and assignment
    uniform_simd_data_over1d(const uniform_simd_data_over1d &) = delete;
    uniform_simd_data_over1d & operator = (const uniform_simd_data_over1d &) = delete;

    typedef internal::uniform::const_simd_accessor<simd_data_type> const_accessor;

    SDLT_INLINE
    const_accessor
    const_access() const
    {
        return const_accessor(m_simd_data);
    }

    // Support const_access with any offset
    template<typename OffsetT>
    SDLT_INLINE
    const_accessor
    const_access(const OffsetT) const
    {
        return const_accessor(m_simd_data);
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
        SDLT_INLINE_BLOCK
        {
            SDLT_INTEL_PRAGMA("ivdep")
            for(int simdlane_index=0; simdlane_index < simd_data_type::simdlane_count; ++simdlane_index)
            {
                primitive<value_type>::template simd_import<simd_data_type, root_access_type>(m_simd_data, simdlane_index, a_value);
            }
        }
        return a_value;
    }

    SDLT_INLINE
    operator value_type const () const {
        SDLT_INLINE_BLOCK
        {
            value_type data;
            sdlt::primitive<value_type>::template simd_extract<simd_data_type, root_access_type>(m_simd_data, 0, data);
            return data;
        }
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF uniform_simd_data_over1d
    #include "internal/pass_through_land_rvalue_operators_xmacro.h"

private:

    template<
        typename RootSimdDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::member;

    template<
        typename RootSimdDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::const_member;

    const simd_data_type & root_simd_data() const { return m_simd_data; }
    simd_data_type & root_simd_data() { return m_simd_data; }
    simd_data_type m_simd_data;
};

template<
    typename PrimitiveT,
    int SimdLaneCountT,
    int ByteAlignmentT = defaults<PrimitiveT>::byte_alignment
>
using uniform_soa_over1d = sdlt::__SDLT_IMPL::uniform_simd_data_over1d< typename primitive<PrimitiveT>::template simd_type<SimdLaneCountT, ByteAlignmentT> >;

} // namepace __SDLT_IMPL
using __SDLT_IMPL::uniform_soa_over1d;
} // namespace sdlt

#endif // SDLT_UNIFORM_SOA_OVER_1D_H
