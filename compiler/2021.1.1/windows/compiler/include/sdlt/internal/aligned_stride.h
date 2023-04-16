//
// Copyright (c) 2015-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_ALIGNED_STRIDE_H
#define SDLT_ALIGNED_STRIDE_H

#include <cstddef>
#include <iostream>

#include "../common.h"
#include "../aligned.h"
#include "../fixed.h"
#include "fixed_stride.h"
#include "disambiguator.h"
#include "enable_if_type.h"
#include "power_of_two.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<int IndexAlignmentT>
class aligned_stride
{
public:
    static_assert(internal::is_power_of_two<IndexAlignmentT>::value, "IndexAlignmentT must be a power of two");

    typedef std::ptrdiff_t value_type;
    typedef std::ptrdiff_t block_type;

    static const int index_alignment = IndexAlignmentT;
    explicit SDLT_INLINE
    aligned_stride(const value_type a_aligned_unsigned_number)
    : m_aligned_block_count(a_aligned_unsigned_number/index_alignment)
    {
        __SDLT_ASSERT(a_aligned_unsigned_number%IndexAlignmentT == 0);
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    aligned_stride(const aligned_stride &other)
    : m_aligned_block_count(other.m_aligned_block_count)
    {}

    template<int OtherAlignmentT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT < IndexAlignmentT)>>
    SDLT_INLINE
    explicit aligned_stride(const aligned_stride<OtherAlignmentT> &other)
    : m_aligned_block_count(other.aligned_block_count()/(IndexAlignmentT/OtherAlignmentT))
    {
        // As this is an explicit constructor, we are relying on the programmer
        // to guarantee that the other value is truly a multiple of this IndexAlignmentT
        __SDLT_ASSERT(other.value()%IndexAlignmentT == 0);
        std::cout << "(OtherAlignmentT < IndexAlignmentT)" << std::endl;
    }

    // Implicitly allow higher alignments to downcast into this one
    template<int OtherAlignmentT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT > IndexAlignmentT)>>
    SDLT_INLINE
    aligned_stride(const aligned_stride<OtherAlignmentT> &other, internal::disambiguator = internal::disambiguator())
    : m_aligned_block_count(other.aligned_block_count()*(OtherAlignmentT/IndexAlignmentT))
    {
        std::cout << "(OtherAlignmentT > IndexAlignmentT)" << std::endl;
    }

protected:
    explicit SDLT_INLINE
    aligned_stride(const int aligned_block_count, internal::disambiguator)
    : m_aligned_block_count(aligned_block_count)
    {
    }
public:

    static SDLT_INLINE 
    aligned_stride from_block_count(const block_type aligned_block_count)
    {
        return aligned_stride(aligned_block_count, internal::disambiguator());
    }


    SDLT_INLINE value_type value() const
    {
        return static_cast<value_type>(m_aligned_block_count)*index_alignment;
    }

    // For simple use in loop variables
    SDLT_INLINE operator value_type () const
    {
        return value();
    }


    SDLT_INLINE
    block_type aligned_block_count() const
    {
        return m_aligned_block_count;
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const aligned_stride &an_index)
    {
        output_stream << "aligned_stride<" << IndexAlignmentT << ">{m_aligned_block_count=(" << an_index.m_aligned_block_count << "), linear=" << an_index.value() << "}";
        return output_stream;
    }

#if __SDLT_ON_64BIT == 1
    static_assert(std::is_same<std::ptrdiff_t, int>::value == false, "SDLT Logic Bug:  Expected std::ptrdiff_t to be different type from int");

    SDLT_INLINE
    aligned_stride
    operator *(int a_factor) const
    {
        __SDLT_ASSERT(a_factor >= 0);
        return aligned_stride::from_block_count(aligned_block_count()*a_factor);
    }

    SDLT_INLINE friend
    aligned_stride
    operator *(int a_factor, const aligned_stride &a_aligned)
    {
        return aligned_stride::from_block_count(a_factor*a_aligned.aligned_block_count());
    }
#else
    static_assert(std::is_same<std::ptrdiff_t, int>::value, "SDLT Logic Bug:  Expected std::ptrdiff_t to be same type as int");
#endif

    SDLT_INLINE
    aligned_stride
    operator *(value_type a_factor) const
    {
        return aligned_stride::from_block_count(aligned_block_count()*a_factor);
    }

    SDLT_INLINE friend
    aligned_stride
    operator *(value_type a_factor, const aligned_stride &a_aligned)
    {
        return aligned_stride::from_block_count(a_factor*a_aligned.aligned_block_count());
    }

    template<int NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(divisible_by<NumberT, power_of_two_floor<abs_of<int, NumberT>::value>::value>::power_of_two > 0  && (NumberT != 0))>>
    SDLT_INLINE
    aligned_stride<divisible_by<NumberT, power_of_two_floor<abs_of<int, NumberT>::value>::value>::power_of_two*IndexAlignmentT>
    operator *(fixed<NumberT>) const
    {
        constexpr int alignment_of_fixed_num = divisible_by<NumberT, power_of_two_floor<abs_of<int, NumberT>::value>::value>::power_of_two;
        static_assert((NumberT%alignment_of_fixed_num == 0), "logic bug");

        return aligned_stride<alignment_of_fixed_num*IndexAlignmentT>::from_block_count(aligned_block_count()*(NumberT/alignment_of_fixed_num));
    }

    template<int NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(divisible_by<NumberT, power_of_two_floor<abs_of<int, NumberT>::value>::value>::power_of_two > 0  && (NumberT != 0))>>
    friend SDLT_INLINE
    aligned_stride<divisible_by<NumberT, power_of_two_floor<abs_of<int, NumberT>::value>::value>::power_of_two*IndexAlignmentT>
    operator *(fixed<NumberT>, const aligned_stride &a_aligned)
    {
        constexpr int alignment_of_fixed_num = divisible_by<NumberT, power_of_two_floor<abs_of<int, NumberT>::value>::value>::power_of_two;
        static_assert((NumberT%alignment_of_fixed_num == 0), "logic bug");

        return aligned_stride<alignment_of_fixed_num*IndexAlignmentT>::from_block_count((NumberT/alignment_of_fixed_num)*a_aligned.aligned_block_count());
    }

    template<std::ptrdiff_t StrideT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(divisible_by<StrideT, power_of_two_floor<abs_of<std::ptrdiff_t, StrideT>::value>::value>::power_of_two > 0)>>
    SDLT_INLINE
    aligned_stride<divisible_by<StrideT, power_of_two_floor<abs_of<std::ptrdiff_t, StrideT>::value>::value>::power_of_two*IndexAlignmentT>
    operator *(fixed_stride<StrideT>) const
    {
        constexpr int alignment_of_fixed_num = divisible_by<StrideT, power_of_two_floor<abs_of<std::ptrdiff_t, StrideT>::value>::value>::power_of_two;
        static_assert((StrideT%alignment_of_fixed_num == 0), "logic bug");

        return aligned_stride<alignment_of_fixed_num*IndexAlignmentT>::from_block_count(aligned_block_count()*(StrideT/alignment_of_fixed_num));
    }

    template<std::ptrdiff_t StrideT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(divisible_by<StrideT, power_of_two_floor<abs_of<std::ptrdiff_t, StrideT>::value>::value>::power_of_two > 0)>>
    friend SDLT_INLINE
    aligned_stride<divisible_by<StrideT, power_of_two_floor<abs_of<std::ptrdiff_t, StrideT>::value>::value>::power_of_two*IndexAlignmentT>
    operator *(fixed_stride<StrideT>, const aligned_stride &a_aligned)
    {
        constexpr int alignment_of_fixed_num = divisible_by<StrideT, power_of_two_floor<abs_of<std::ptrdiff_t, StrideT>::value>::value>::power_of_two;
        static_assert((StrideT%alignment_of_fixed_num == 0), "logic bug");

        return aligned_stride<alignment_of_fixed_num*IndexAlignmentT>::from_block_count((StrideT/alignment_of_fixed_num)*a_aligned.aligned_block_count());
    }

    template<int OtherAlignmentT>
    SDLT_INLINE
    aligned_stride<IndexAlignmentT*OtherAlignmentT>
    operator *(const aligned_stride<OtherAlignmentT> &a_aligned) const
    {
        return aligned_stride<IndexAlignmentT*OtherAlignmentT>::from_block_count(aligned_block_count()*a_aligned.aligned_block_count());
    }

    SDLT_INLINE
    const aligned_stride & operator +() const
    {
        return *this;
    }

    SDLT_INLINE
    aligned_stride operator +(const aligned_stride &a_other) const
    {
        return aligned_stride::from_block_count(aligned_block_count() + a_other.aligned_block_count());
    }

    template<int OtherAlignmentT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT>IndexAlignmentT)>>
    SDLT_INLINE
    aligned_stride operator +(const aligned_stride<OtherAlignmentT> &a_other) const
    {
        static_assert((OtherAlignmentT%IndexAlignmentT == 0), "Non power of two alignments, not supported");
        constexpr int blocksPerOtherBlock = OtherAlignmentT/IndexAlignmentT;
        static_assert((blocksPerOtherBlock > 1), "enable_if logic bug");
        return aligned_stride::from_block_count(aligned_block_count() + (blocksPerOtherBlock*a_other.aligned_block_count()));
    }

    template<int OtherAlignmentT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT<IndexAlignmentT)>>
    SDLT_INLINE
    aligned_stride<OtherAlignmentT> operator +(const aligned_stride<OtherAlignmentT> &a_other) const
    {
        static_assert((IndexAlignmentT%OtherAlignmentT == 0), "Non power of two alignments, not supported");
        constexpr int other_blocks_per_block = IndexAlignmentT/OtherAlignmentT;
        static_assert((other_blocks_per_block > 1), "enable_if logic bug");
        return aligned_stride<OtherAlignmentT>::from_block_count((other_blocks_per_block*aligned_block_count()) + a_other.aligned_block_count());
    }

    template<std::ptrdiff_t NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT == 0)>>
    SDLT_INLINE
    aligned_stride operator +(const fixed_stride<NumberT> &) const
    {
        static_assert((NumberT%IndexAlignmentT == 0), "Non power of two alignments, not supported, enable_if logic bug");
        return aligned_stride::from_block_count(aligned_block_count() + NumberT/IndexAlignmentT);
    }

    template<std::ptrdiff_t NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT != 0) && (divisible_by<NumberT, IndexAlignmentT>::power_of_two > 1)>>
    SDLT_INLINE
    aligned_stride<divisible_by<NumberT, IndexAlignmentT>::power_of_two>
    operator +(const fixed_stride<NumberT> &) const
    {
        constexpr int demoted_alignment = divisible_by<NumberT, IndexAlignmentT>::power_of_two;
        static_assert(demoted_alignment < IndexAlignmentT, "logic bug");
        static_assert((NumberT%demoted_alignment == 0), "Non power of two alignments, not supported, enable_if logic bug");
        const int demoted_blocks_per_block = IndexAlignmentT/demoted_alignment;
        return aligned_stride<demoted_alignment>::from_block_count(demoted_blocks_per_block*aligned_block_count() + NumberT/demoted_alignment);
    }

    template<std::ptrdiff_t NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT == 0)>>
    friend SDLT_INLINE
        aligned_stride operator +(const fixed_stride<NumberT> &, const aligned_stride &a_aligned)
    {
        static_assert((NumberT%IndexAlignmentT == 0), "Non power of two alignments, not supported, enable_if logic bug");
        return aligned_stride::from_block_count(a_aligned.aligned_block_count() + NumberT / IndexAlignmentT);
    }

    template<std::ptrdiff_t NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT != 0) && (divisible_by<NumberT, IndexAlignmentT>::power_of_two > 1)>>
    friend SDLT_INLINE
        aligned_stride<divisible_by<NumberT, IndexAlignmentT>::power_of_two>
        operator +(const fixed_stride<NumberT> &, const aligned_stride &a_aligned)
    {
        constexpr int demoted_alignment = divisible_by<NumberT, IndexAlignmentT>::power_of_two;
        static_assert(demoted_alignment < IndexAlignmentT, "logic bug");
        static_assert((NumberT%demoted_alignment == 0), "Non power of two alignments, not supported, enable_if logic bug");
        const int demoted_blocks_per_block = IndexAlignmentT / demoted_alignment;
        return aligned_stride<demoted_alignment>::from_block_count(demoted_blocks_per_block*a_aligned.aligned_block_count() + NumberT / demoted_alignment);
    }


    SDLT_INLINE
    value_type
    operator /(fixed_stride<IndexAlignmentT>) const
    {
        // The result of dividing by the alignment is just the block count
        // Optimize to avoid converting from aligned block count to integer
        return aligned_block_count();
    }

    SDLT_INLINE
    value_type
    operator /(fixed_stride<-IndexAlignmentT>) const
    {
        // The result of dividing by the alignment is just the block count
        // Optimize to avoid converting from aligned block count to integer
        return -aligned_block_count();
    }

    SDLT_INLINE
    value_type
    operator /(fixed<IndexAlignmentT>) const
    {
        // The result of dividing by the alignment is just the block count
        // Optimize to avoid converting from aligned block count to integer
        return aligned_block_count();
    }

    SDLT_INLINE
    value_type
    operator /(fixed<-IndexAlignmentT>) const
    {
        // The result of dividing by the alignment is just the block count
        // Optimize to avoid converting from aligned block count to integer
        return -aligned_block_count();
    }

    template<std::ptrdiff_t StrideT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(internal::abs_of<std::ptrdiff_t, StrideT>::value > IndexAlignmentT) && (StrideT%IndexAlignmentT == 0)>>
    SDLT_INLINE
    value_type
    operator /(fixed_stride<StrideT>) const
    {
        static_assert((internal::abs_of<std::ptrdiff_t, StrideT>::value > IndexAlignmentT), "enable_if logic bug");
        static_assert((StrideT%IndexAlignmentT == 0), "enable_if logic bug");
        // Optimize to avoid converting from aligned block count to integer
        return aligned_block_count()/(StrideT/IndexAlignmentT);
    }

    template<std::ptrdiff_t StrideT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<((internal::abs_of<std::ptrdiff_t, StrideT>::value) < IndexAlignmentT) && (IndexAlignmentT%StrideT == 0)>>
    SDLT_INLINE
    aligned_stride<IndexAlignmentT/internal::abs_of<std::ptrdiff_t, StrideT>::value>
    operator /(fixed_stride<StrideT>) const
    {
        static_assert((internal::abs_of<std::ptrdiff_t, StrideT>::value < IndexAlignmentT), "enable_if logic bug");
        static_assert((IndexAlignmentT%StrideT == 0), "enable_if logic bug");
        // Optimize to avoid any math at all
        return aligned_stride<IndexAlignmentT/internal::abs_of<std::ptrdiff_t, StrideT>::value>(aligned_block_count()*(IndexAlignmentT/StrideT));
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(internal::abs_of<int, NumberT>::value > IndexAlignmentT) && (NumberT%IndexAlignmentT == 0)>>
    SDLT_INLINE
    value_type
    operator /(fixed<NumberT>) const
    {
        static_assert((internal::abs_of<int, NumberT>::value > IndexAlignmentT), "enable_if logic bug");
        static_assert((NumberT%IndexAlignmentT == 0), "enable_if logic bug");
        // Optimize to avoid converting from aligned block count to integer
        return aligned_block_count()/(NumberT/IndexAlignmentT);
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<((internal::abs_of<int, NumberT>::value) < IndexAlignmentT) && (IndexAlignmentT%NumberT == 0)>>
    SDLT_INLINE
    aligned_stride<IndexAlignmentT/internal::abs_of<int, NumberT>::value>
    operator /(fixed<NumberT>) const
    {
        static_assert((internal::abs_of<int, NumberT>::value < IndexAlignmentT), "enable_if logic bug");
        static_assert((IndexAlignmentT%NumberT == 0), "enable_if logic bug");
        // Optimize to avoid any math at all
        return aligned_stride<IndexAlignmentT/internal::abs_of<int, NumberT>::value>(aligned_block_count()*(IndexAlignmentT/NumberT));
    }

private:
    // Copy by value semantics to get local uniform instance
    // embedded inside a lambda closure
    block_type m_aligned_block_count;
};

template<typename T>
struct is_aligned_stride
: public std::false_type
{};

template<int AlignmentT>
struct is_aligned_stride<aligned_stride<AlignmentT>>
: public std::true_type
{};

#if __SDLT_ON_64BIT == 1
    static_assert(std::is_same<std::ptrdiff_t, int>::value == false, "SDLT Logic Bug:  Expected std::ptrdiff_t to be different type from int");

    template<std::ptrdiff_t StrideT>
    SDLT_INLINE
    aligned_stride<divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two>
    operator *(int a_factor, fixed_stride<StrideT>)
    {
        constexpr int AlignmentT = divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two;
        static_assert((StrideT%AlignmentT == 0), "enable_if logic bug");
        return aligned_stride<AlignmentT>::from_block_count(a_factor*(StrideT / AlignmentT));
    }

    template<std::ptrdiff_t StrideT, typename = enable_if_type<(divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two > 0)>>
    SDLT_INLINE
        aligned_stride<divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two>
        operator *(fixed_stride<StrideT>, int a_factor)
    {
        constexpr int AlignmentT = divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two;
        static_assert((StrideT%AlignmentT == 0), "enable_if logic bug");
        return aligned_stride<AlignmentT>::from_block_count((StrideT / AlignmentT)*a_factor);
    }
#else
    static_assert(std::is_same<std::ptrdiff_t, int>::value, "SDLT Logic Bug:  Expected std::ptrdiff_t to be same type as int");
#endif


template<std::ptrdiff_t StrideT, typename = enable_if_type<(divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two > 0)>>
SDLT_INLINE
    aligned_stride<divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two>
    operator *(std::ptrdiff_t a_factor, fixed_stride<StrideT>)
{
    constexpr int AlignmentT = divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two;
    static_assert((StrideT%AlignmentT == 0), "enable_if logic bug");
    return aligned_stride<AlignmentT>::from_block_count(a_factor*(StrideT / AlignmentT));
}

template<std::ptrdiff_t StrideT, typename = enable_if_type<(divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two > 0)>>
SDLT_INLINE
    aligned_stride<divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two>
    operator *(fixed_stride<StrideT>, std::ptrdiff_t a_factor)
{
    constexpr int AlignmentT = divisible_by<StrideT, power_of_two_floor<StrideT>::value>::power_of_two;
    static_assert((StrideT%AlignmentT == 0), "enable_if logic bug");
    return aligned_stride<AlignmentT>::from_block_count((StrideT / AlignmentT)*a_factor);
}


} // namespace internal

// Free functions need to be in same namespaces as their parameters to properly support
// argument dependent lookup

#if __SDLT_ON_64BIT == 1
    static_assert(std::is_same<std::ptrdiff_t, int>::value == false, "SDLT Logic Bug:  Expected std::ptrdiff_t to be different type from int");

    // On 32 builds these are already covered by aligned operator *(fixed, int)
    template<int NumberT,
             typename = internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
    SDLT_INLINE
    internal::aligned_stride<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two>
    operator *(fixed<NumberT>, std::ptrdiff_t a_factor)
    {
        constexpr int AlignmentT = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
        static_assert((NumberT%AlignmentT == 0), "enable_if logic bug");
        return internal::aligned_stride<AlignmentT>::from_block_count((NumberT/AlignmentT)*a_factor);
    }

    template<int AlignmentT>
    SDLT_INLINE
    internal::aligned_stride<AlignmentT>
    operator *(aligned<AlignmentT> a_aligned, std::ptrdiff_t a_factor)
    {
        return internal::aligned_stride<AlignmentT>::from_block_count(a_aligned.aligned_block_count()*a_factor);
    }
#else
    static_assert(std::is_same<std::ptrdiff_t, int>::value, "SDLT Logic Bug:  Expected std::ptrdiff_t to be same type as int");
#endif


} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_ALIGNED_STRIDE_H
