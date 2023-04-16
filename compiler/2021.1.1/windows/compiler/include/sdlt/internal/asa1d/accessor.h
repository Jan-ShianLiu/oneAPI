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

#ifndef SDLT_ASA_1D_ACCESSOR_H
#define SDLT_ASA_1D_ACCESSOR_H


#include "../../common.h"
#include "../../no_offset.h"
#include "../default_offset.h"
#include "../enable_if_type.h"
#include "../is_simd_compatible_index.h"
#include "../simd/indices.h"
#include "element.h"
#include "organization.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace asa1d {

// Forward declaration
template<
    typename SimdDataT,
    int AlignD1OnIndexT,
    typename OffsetT>
class const_accessor;

__SDLT_INHERIT_IMPL_BEGIN

// Expect lambda closures to copy these accessors around
// so treat copied version as a "view" that doesn't own/manage the data
// MUST BE PLAIN OLD DATA, for use with lambda closures, therefore
// no copy constructor, assignment operator, or virtual functions
template<
    typename SimdDataT,
    int AlignD1OnIndexT,
    typename OffsetT = no_offset>
class accessor :
    protected organization<AlignD1OnIndexT>
{
protected:
    template<typename , int, typename > friend class asa1d::const_accessor;
    template<typename , int, typename > friend class asa1d::accessor;

    SimdDataT * m_simd_data;
    OffsetT m_offset;

public:

    typedef typename SimdDataT::primitive_type primitive_type;
    typedef OffsetT offset_type;
    static const int simdlane_count = SimdDataT::simdlane_count;
    static const int align_d1_on_index = AlignD1OnIndexT;

    SDLT_INLINE
    accessor()
    : organization<AlignD1OnIndexT>(0, 0)
    , m_simd_data(nullptr)
    , m_offset(default_offset<OffsetT>::value)
    {}

    // Implicit conversion constructor from un-offset accessor
    // NOTE: when OffsetT == no_offset, it is a copy constructor vs. conversion constructor
    SDLT_INLINE
    accessor(const accessor<SimdDataT, AlignD1OnIndexT, no_offset> &unoffset_accessor)
    : organization<AlignD1OnIndexT>(unoffset_accessor)
    , m_simd_data(unoffset_accessor.m_simd_data)
    , m_offset(zero_offset<OffsetT>::value)
    {
        __SDLT_ASSERT(internal::is_aligned<64>(m_simd_data));
    }

    explicit SDLT_INLINE
    accessor(
        SimdDataT * const a_simd_data,
        const int size_d1,
        const int iSimdBlockStrideD1,
        OffsetT offset = default_offset<OffsetT>::value
    )
    : organization<AlignD1OnIndexT>(size_d1, iSimdBlockStrideD1)
    , m_simd_data(a_simd_data)
    , m_offset(offset)
    {
        __SDLT_ASSERT(internal::is_aligned<64>(m_simd_data));
    }

    // default copy and assignment constructors OK


    SDLT_INLINE
    const int & get_size_d1() const { return this->m_size_d1; }

    SDLT_INLINE
    offset_type get_offset() const { return m_offset; }

    // NOTE: no integer variable base offset is allowed on the Asa
    // because we could not guarantee that only a single structure of fixed
    // sized arrays would be required during iteration

    template<int IndexAlignmentT>
    SDLT_INLINE
    accessor<SimdDataT, AlignD1OnIndexT, aligned<IndexAlignmentT> >
    reaccess(aligned<IndexAlignmentT> offset) const
    {
        static_assert(IndexAlignmentT%simdlane_count == 0, "IndexAlignmentT of aligned<IndexAlignmentT> must be a multiple of the SimdLaneCountT of the asa1d_container");
        return accessor<SimdDataT, AlignD1OnIndexT, aligned<IndexAlignmentT>>(
            m_simd_data,
            this->m_size_d1,
            this->m_simd_block_stride_d1,
            offset);
    }

    template<int FixedOffsetT>
    SDLT_INLINE
    accessor<SimdDataT, AlignD1OnIndexT, fixed<FixedOffsetT> >
    reaccess(fixed<FixedOffsetT>) const
    {
        static_assert(FixedOffsetT%simdlane_count == 0, "OffsetT of fixed<OffsetT> must be a multiple of the SimdLaneCountT of the asa1d_container");
        return accessor<SimdDataT, AlignD1OnIndexT, fixed<FixedOffsetT> >(
            m_simd_data,
            this->m_size_d1,
            this->m_simd_block_stride_d1);
    }

    template<typename IndexD1T, typename = internal::enable_if_type<is_simd_compatible_index<IndexD1T, simdlane_count>::value>>
    SDLT_INLINE
    auto
    operator [] (const IndexD1T an_index_d1) const
    -> element<SimdDataT>
    {
        auto offsetIndex = (m_offset + an_index_d1);
        __SDLT_ASSERT(offsetIndex.template simd_struct_index<simdlane_count>() < this->m_simd_block_stride_d1);
        __SDLT_ASSERT(offsetIndex.simdlane_index() < simdlane_count);

        return element<SimdDataT>(
            m_simd_data[offsetIndex.template simd_struct_index<simdlane_count>()],
            offsetIndex.simdlane_index());
    }

    bool operator == (const accessor &other) const
    {
        return m_simd_data == other.m_simd_data &&
               m_offset == other.m_offset &&
               organization<AlignD1OnIndexT>::operator == (other);
    }
};

__SDLT_INHERIT_IMPL_END

} // namespace asa1d
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ASA_1D_ACCESSOR_H
