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

#ifndef SDLT_ALIGNED_H
#define SDLT_ALIGNED_H

#include <cstddef>
#include <iostream>
#include <limits>

#include "common.h"
#include "fixed.h"
#include "internal/disambiguator.h"
#include "internal/zero_offset.h"
#include "internal/enable_if_type.h"
#include "internal/power_of_two.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

    template<typename T, T NumberT>
    struct abs_of
    {
        typedef T value_type;
        static constexpr value_type value  = (NumberT >= 0)?
                NumberT :
                -NumberT;

    };

    template<int NumberT, int IndexAlignmentT>
    struct divisible_by
    {
        static_assert(internal::is_power_of_two<IndexAlignmentT>::value, "IndexAlignmentT must be a power of two");

        static constexpr int power_of_two  = (NumberT%IndexAlignmentT == 0) ?
                                                IndexAlignmentT :
                                                divisible_by<NumberT, IndexAlignmentT/2>::power_of_two;
        static constexpr int is_largest = (NumberT%IndexAlignmentT == 0) && (NumberT%(IndexAlignmentT*2) != 0);
    };

    template<int NumberT>
    struct divisible_by<NumberT, 0>
    {
        static constexpr int power_of_two  = 0;
    };
} // namespace internal


template<int IndexAlignmentT>
class aligned
{
public:
    static_assert(internal::is_power_of_two<IndexAlignmentT>::value, "IndexAlignmentT must be a power of two");

    typedef int value_type;
    typedef int block_type;

    // For performance reasons, allow uninitialized
    //
    aligned()
#if SDLT_DEBUG
    // Encourage an overflow or segfault should someone try and use an uninitialized number
    // We don't want to set it to 0, as it could hide uninitialized use
    : m_aligned_block_count(std::numeric_limits<value_type>::max())
#endif
    {
    }

    static const int index_alignment = IndexAlignmentT;
    explicit SDLT_INLINE
    aligned(const value_type a_aligned_number)
    : m_aligned_block_count(a_aligned_number/index_alignment)
    {
        __SDLT_ASSERT(a_aligned_number%IndexAlignmentT == 0);
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    aligned(const aligned &other)
    : m_aligned_block_count(other.m_aligned_block_count)
    {}

    template<int OtherAlignmentT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT < IndexAlignmentT)>>
    SDLT_INLINE
    explicit aligned(const aligned<OtherAlignmentT> &other)
    : m_aligned_block_count(other.aligned_block_count()/(IndexAlignmentT/OtherAlignmentT))
    {
        // As this is an explicit constructor, we are relying on the programmer
        // to guarantee that the other value is truly a multiple of this IndexAlignmentT
        __SDLT_ASSERT(other.value()%IndexAlignmentT == 0);
        std::cout << "(OtherAlignmentT < IndexAlignmentT)" << std::endl;
    }

    // Implicitly allow higher alignments to downcast into this one
    template<int OtherAlignmentT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT > IndexAlignmentT)>>
    SDLT_INLINE
    aligned(const aligned<OtherAlignmentT> &other, internal::disambiguator = internal::disambiguator())
    : m_aligned_block_count(other.aligned_block_count()*(OtherAlignmentT/IndexAlignmentT))
    {
        std::cout << "(OtherAlignmentT > IndexAlignmentT)" << std::endl;
    }

protected:
    explicit SDLT_INLINE
    aligned(block_type aligned_block_count, internal::disambiguator)
    : m_aligned_block_count(aligned_block_count)
    {
    }
public:

    static SDLT_INLINE 
    aligned from_block_count(block_type aligned_block_count)
    {
        return aligned(aligned_block_count, internal::disambiguator());
    }


    SDLT_INLINE value_type value() const
    {
        return m_aligned_block_count*index_alignment;
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
    std::ostream& operator << (std::ostream& output_stream, const aligned &an_index)
    {
        output_stream << "aligned<" << IndexAlignmentT << ">{m_aligned_block_count=(" << an_index.m_aligned_block_count << "), linear=" << an_index.value() << "}";
        return output_stream;
    }

    SDLT_INLINE
    aligned
    operator *(value_type a_factor) const
    {
        return aligned::from_block_count(aligned_block_count()*a_factor);
    }

    SDLT_INLINE friend
    aligned
    operator *(value_type a_factor, const aligned &a_aligned)
    {
        return aligned::from_block_count(a_factor*a_aligned.aligned_block_count());
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
    SDLT_INLINE
    aligned<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two*IndexAlignmentT>
    operator *(fixed<NumberT>) const
    {
        constexpr int alignment_of_fixed_num = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
        static_assert((NumberT%alignment_of_fixed_num == 0), "logic bug");

        return aligned<alignment_of_fixed_num*IndexAlignmentT>::from_block_count(aligned_block_count()*(NumberT/alignment_of_fixed_num));
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
    friend SDLT_INLINE
    aligned<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two*IndexAlignmentT>
    operator *(fixed<NumberT>, const aligned &a_aligned)
    {
        constexpr int alignment_of_fixed_num = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
        static_assert((NumberT%alignment_of_fixed_num == 0), "logic bug");

        return aligned<alignment_of_fixed_num*IndexAlignmentT>::from_block_count((NumberT/alignment_of_fixed_num)*a_aligned.aligned_block_count());
    }

    template<int OtherAlignmentT>
    SDLT_INLINE
    aligned<IndexAlignmentT*OtherAlignmentT>
    operator *(const aligned<OtherAlignmentT> &a_aligned) const
    {
        return aligned<IndexAlignmentT*OtherAlignmentT>::from_block_count(aligned_block_count()*a_aligned.aligned_block_count());
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
    aligned<IndexAlignmentT/internal::abs_of<int, NumberT>::value>
    operator /(fixed<NumberT>) const
    {
        static_assert((internal::abs_of<int, NumberT>::value < IndexAlignmentT), "enable_if logic bug");
        static_assert((IndexAlignmentT%NumberT == 0), "enable_if logic bug");
        // Optimize to avoid any math at all
        return aligned<IndexAlignmentT/internal::abs_of<int, NumberT>::value>(aligned_block_count()*(IndexAlignmentT/NumberT));
    }

    SDLT_INLINE
    aligned operator -() const
    {
        return aligned::from_block_count(-aligned_block_count());
    }

    SDLT_INLINE
    aligned operator -(const aligned &a_other) const
    {
        return aligned::from_block_count(aligned_block_count() - a_other.aligned_block_count());
    }

    template<int OtherAlignmentT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT>IndexAlignmentT)>>
    SDLT_INLINE
    aligned operator -(const aligned<OtherAlignmentT> &a_other) const
    {
        static_assert((OtherAlignmentT%IndexAlignmentT == 0), "Non power of two alignments, not supported");
        constexpr block_type blocksPerOtherBlock = OtherAlignmentT/IndexAlignmentT;
        static_assert((blocksPerOtherBlock > 1), "enable_if logic bug");
        return aligned::from_block_count(aligned_block_count() - (blocksPerOtherBlock*a_other.aligned_block_count()));
    }

    template<int OtherAlignmentT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT<IndexAlignmentT)>>
    SDLT_INLINE
    aligned<OtherAlignmentT> operator -(const aligned<OtherAlignmentT> &a_other) const
    {
        static_assert((IndexAlignmentT%OtherAlignmentT == 0), "Non power of two alignments, not supported");
        constexpr block_type other_blocks_per_block = IndexAlignmentT/OtherAlignmentT;
        static_assert((other_blocks_per_block > 1), "enable_if logic bug");
        return aligned<OtherAlignmentT>::from_block_count((other_blocks_per_block*aligned_block_count()) - a_other.aligned_block_count());
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT == 0) && (NumberT != 0)>>
    SDLT_INLINE
    aligned operator -(const fixed<NumberT> &) const
    {
        static_assert((NumberT%IndexAlignmentT == 0), "Non power of two alignments, not supported, enable_if logic bug");
        return aligned::from_block_count(aligned_block_count() - NumberT/IndexAlignmentT);
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT == 0) && (NumberT != 0)>>
    friend SDLT_INLINE
    aligned operator -(const fixed<NumberT> &, const aligned &a_aligned)
    {
        static_assert((NumberT%IndexAlignmentT == 0), "Non power of two alignments, not supported, enable_if logic bug");
        return aligned::from_block_count((NumberT / IndexAlignmentT) - a_aligned.aligned_block_count());
    }


    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT != 0) && (internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two > 1) && (NumberT != 0)>>
    SDLT_INLINE
    aligned<internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two> operator -(const fixed<NumberT> &) const
    {
        constexpr int demoted_alignment = internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two;
        static_assert(demoted_alignment < IndexAlignmentT, "logic bug");
        static_assert((NumberT%demoted_alignment == 0), "Non power of two alignments, not supported, enable_if logic bug");
        const block_type demoted_blocks_per_block = IndexAlignmentT/demoted_alignment;
        return aligned<demoted_alignment>::from_block_count(demoted_blocks_per_block*aligned_block_count() - NumberT/demoted_alignment);
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT != 0) && (internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two > 1) && (NumberT != 0)>>
    friend SDLT_INLINE
    aligned<internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two> operator -(const fixed<NumberT> &, const aligned &a_aligned)
    {
        constexpr int demoted_alignment = internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two;
        static_assert(demoted_alignment < IndexAlignmentT, "logic bug");
        static_assert((NumberT%demoted_alignment == 0), "Non power of two alignments, not supported, enable_if logic bug");
        const block_type demoted_blocks_per_block = IndexAlignmentT / demoted_alignment;
        return aligned<demoted_alignment>::from_block_count((NumberT / demoted_alignment) - demoted_blocks_per_block*a_aligned.aligned_block_count());
    }

    SDLT_INLINE
    const aligned & operator +() const
    {
        return *this;
    }

    SDLT_INLINE
    aligned operator +(const aligned &a_other) const
    {
        return aligned::from_block_count(aligned_block_count() + a_other.aligned_block_count());
    }

    template<int OtherAlignmentT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT>IndexAlignmentT)>>
    SDLT_INLINE
    aligned operator +(const aligned<OtherAlignmentT> &a_other) const
    {
        static_assert((OtherAlignmentT%IndexAlignmentT == 0), "Non power of two alignments, not supported");
        constexpr block_type blocksPerOtherBlock = OtherAlignmentT/IndexAlignmentT;
        static_assert((blocksPerOtherBlock > 1), "enable_if logic bug");
        return aligned::from_block_count(aligned_block_count() + (blocksPerOtherBlock*a_other.aligned_block_count()));
    }

    template<int OtherAlignmentT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(OtherAlignmentT<IndexAlignmentT)>>
    SDLT_INLINE
    aligned<OtherAlignmentT> operator +(const aligned<OtherAlignmentT> &a_other) const
    {
        static_assert((IndexAlignmentT%OtherAlignmentT == 0), "Non power of two alignments, not supported");
        constexpr block_type other_blocks_per_block = IndexAlignmentT/OtherAlignmentT;
        static_assert((other_blocks_per_block > 1), "enable_if logic bug");
        return aligned<OtherAlignmentT>::from_block_count((other_blocks_per_block*aligned_block_count()) + a_other.aligned_block_count());
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<((NumberT%IndexAlignmentT == 0) && NumberT != 0)>>
    SDLT_INLINE
    aligned operator +(const fixed<NumberT> &) const
    {
        static_assert((NumberT%IndexAlignmentT == 0), "Non power of two alignments, not supported, enable_if logic bug");
        return aligned::from_block_count(aligned_block_count() + NumberT/IndexAlignmentT);
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<((NumberT%IndexAlignmentT == 0) && NumberT != 0)>>
    friend SDLT_INLINE
    aligned operator +(const fixed<NumberT> &, const aligned &a_aligned)
    {
        static_assert((NumberT%IndexAlignmentT == 0), "Non power of two alignments, not supported, enable_if logic bug");
        return aligned::from_block_count(a_aligned.aligned_block_count() + NumberT / IndexAlignmentT);
    }

    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT != 0) && (internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two > 1) && NumberT != 0>>
    SDLT_INLINE
    aligned<internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two>
    operator +(const fixed<NumberT> &) const
    {
        constexpr int demoted_alignment = internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two;
        static_assert(demoted_alignment < IndexAlignmentT, "logic bug");
        static_assert((NumberT%demoted_alignment == 0), "Non power of two alignments, not supported, enable_if logic bug");
        const block_type demoted_blocks_per_block = IndexAlignmentT/demoted_alignment;
        return aligned<demoted_alignment>::from_block_count(demoted_blocks_per_block*aligned_block_count() + NumberT/demoted_alignment);
    }


    template<int NumberT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT%IndexAlignmentT != 0) && (internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two > 1) && NumberT != 0>>
    friend SDLT_INLINE
    aligned<internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two>
    operator +(const fixed<NumberT> &, const aligned &a_aligned)
    {
        constexpr int demoted_alignment = internal::divisible_by<NumberT, IndexAlignmentT>::power_of_two;
        static_assert(demoted_alignment < IndexAlignmentT, "logic bug");
        static_assert((NumberT%demoted_alignment == 0), "Non power of two alignments, not supported, enable_if logic bug");
        const block_type demoted_blocks_per_block = IndexAlignmentT/demoted_alignment;
        return aligned<demoted_alignment>::from_block_count(demoted_blocks_per_block*a_aligned.aligned_block_count() + NumberT/demoted_alignment);
    }

    template<typename OtherT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<std::is_same<aligned, decltype(std::declval<aligned>() + std::declval<OtherT>())>::value>>
    SDLT_INLINE
    aligned & operator += (const OtherT &a_other)
    {
        static_assert(std::is_same<aligned, decltype(*this + a_other)>::value, "enable_if logic bug");
        m_aligned_block_count = (*this + a_other).aligned_block_count();
        return *this;
    }

    template<typename OtherT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<std::is_same<aligned, decltype(std::declval<aligned>() - std::declval<OtherT>())>::value>>
    SDLT_INLINE
    aligned & operator -= (const OtherT &a_other)
    {
        static_assert(std::is_same<aligned, decltype(*this - a_other)>::value, "enable_if logic bug");
        m_aligned_block_count = (*this - a_other).aligned_block_count();
        return *this;
    }

    template<typename OtherT>
    SDLT_INLINE
    aligned & operator *= (const OtherT &a_other)
    {
        // explicit cast will downcast any results of higher alignment
        *this = aligned(*this * a_other);
        return *this;
    }

    template<typename OtherT,
             typename = sdlt::__SDLT_IMPL::internal::enable_if_type<std::is_same<aligned, decltype(std::declval<aligned>() / std::declval<OtherT>())>::value>>
    SDLT_INLINE
    aligned & operator /= (const OtherT &a_other)
    {
        static_assert(std::is_same<aligned, decltype(*this / a_other)>::value, "enable_if logic bug");
        m_aligned_block_count = (*this / a_other).aligned_block_count();
        return *this;
    }


private:
    // Copy by value semantics to get local uniform instance
    // embedded inside a lambda closure
    block_type m_aligned_block_count;
};

namespace internal
{
    template<int IndexAlignmentT>
    struct zero_offset< aligned<IndexAlignmentT> >
    {
        static const aligned<IndexAlignmentT> value;
    };

    template<int IndexAlignmentT>
    const aligned<IndexAlignmentT> zero_offset< aligned<IndexAlignmentT> >::value = aligned<IndexAlignmentT>(0);
}


// Many math operations between a fixed<NumberT> and int can result in an aligned<AlignmentT>, as neither of the
// operands are aligned,

// Free functions need to be in same namespaces as their parameters to properly support
// argument dependent lookup

template<int NumberT,
         typename = internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
SDLT_INLINE
aligned<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two>
operator *(int a_factor, fixed<NumberT>)
{
    constexpr int AlignmentT = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
    static_assert((NumberT%AlignmentT == 0), "enable_if logic bug");
    return aligned<AlignmentT>::from_block_count(a_factor*(NumberT/AlignmentT));
}

template<int NumberT,
         typename = internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
SDLT_INLINE
aligned<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two>
operator *(fixed<NumberT>, int a_factor)
{
    constexpr int AlignmentT = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
    static_assert((NumberT%AlignmentT == 0), "enable_if logic bug");
    return aligned<AlignmentT>::from_block_count((NumberT/AlignmentT)*a_factor);
}


template<int NumberT,
         typename = internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
SDLT_INLINE
aligned<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two>
operator *(size_t a_factor, fixed<NumberT>)
{
    constexpr int AlignmentT = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
    static_assert((NumberT%AlignmentT == 0), "enable_if logic bug");
    return aligned<AlignmentT>::from_block_count(a_factor*(NumberT / AlignmentT));
}

template<int NumberT,
         typename = internal::enable_if_type<(internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two > 0) && (NumberT != 0)>>
SDLT_INLINE
aligned<internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two>
operator *(fixed<NumberT>, size_t a_factor)
{
    constexpr int AlignmentT = internal::divisible_by<NumberT, internal::power_of_two_floor<internal::abs_of<int, NumberT>::value>::value>::power_of_two;
    static_assert((NumberT%AlignmentT == 0), "enable_if logic bug");
    return aligned<AlignmentT>::from_block_count((NumberT / AlignmentT)*a_factor);
}


} // namepace __SDLT_IMPL
using __SDLT_IMPL::aligned;

} // namespace sdlt

#endif // SDLT_ALIGNED_H
