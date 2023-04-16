//
// Copyright (C) 2012-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_SIMD_ELEMENT_MOVER_H
#define SDLT_SIMD_ELEMENT_MOVER_H

#include "../../common.h"
#include "../../defaults.h"
#include "iterator_index.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace simd {

// Beside code resuse between dynamic Containers,
// the Containers use the element_mover so the users can
// apply "external polymorphism".  That is, they can provide
// a specialization of the element_mover to implement their own
// policies, like threading.
template <typename ContainerT>
class element_mover
{
public:
    static const int simdlane_count = ContainerT::simdlane_count;
    static const int align_d1_on_index = ContainerT::align_d1_on_index;
    typedef typename ContainerT::primitive_type value_type;
    typedef typename ContainerT::template accessor<> accessor;
protected:
    typedef iterator_index<simdlane_count, align_d1_on_index> iter_index_type;
public:

    static void
    fill_with(
        const accessor dest,
        const value_type & a_value,
        const int start_at,
        const int end_before)
    {
        SDLT_INLINE_BLOCK
        {
            iter_index_type i(start_at);
            iter_index_type end_index(end_before);
            // TODO:  specialize for ASA
            //SDLT_INTEL_PRAGMA("ivdep")

            for(;i < end_index; ++i)
            {
                dest[i] = a_value;
            }
        }
    }

    template<typename SourceAccessorT>
    static void
    copy_all_values_from(
        const SourceAccessorT source,
        const accessor dest)
    {
        SDLT_INLINE_BLOCK
        {
            iter_index_type i(0);
            iter_index_type end_index(dest.get_size_d1());
            //SDLT_INTEL_PRAGMA("ivdep")
            // TODO:  break out peel , simd loop, and remainder loop
            for(;i < end_index; ++i)
            {
                dest[i] = source[i];
            }
        }
    }

    template<typename IteratorT>
    static void
    copy_all_values_from(
        IteratorT a_begin,
        const IteratorT __SDLT_DEBUG_ONLY(an_end),
        const int start_at,
        const int end_before,
        accessor dest)
    {
        __SDLT_ASSERT((end_before - start_at) == std::distance(a_begin, an_end));

        // Purposely scalar and single threaded
        // as its an arbitrary iterator, don't expect vectorization to work
        // and if threaded would need to spin iterator's to the correct index
        // Its possibly to thread, but would need a convincing use case to
        // bother added the necessary code, caller is free to do this themselves
        // as this is just a convenience constructor
        iter_index_type i(start_at);
        iter_index_type end_index(end_before);
        for(;i < end_index; ++i)
        {
            __SDLT_ASSERT(a_begin != an_end);
            dest[i] = *a_begin;
            ++a_begin;
        }
    }

    static void
    shift_elements_backwards(
        accessor an_array,
        const int from_index,
        const int a_count)
    {
        __SDLT_ASSERT(a_count > 0);

        // TODO:  not very efficient vanilla loop
        // could possibly do a vectorized store with some kind of gather or masked
        // load from adjacent SIMD structs.
        // Or perhaps two smaller loops

        SDLT_INLINE_BLOCK
        {
            iter_index_type i(from_index);
            iter_index_type end_index(an_array.get_size_d1() - a_count);

            // TODO: outer loop to iterate over SIMD structs
            // 2 inner loops based on count%simdlane_count,
            // could even have specialized version for each SimdLaneIndex value
            // However an intrinsics version might perform the best
            //SDLT_INTEL_PRAGMA("ivdep")
            for(;i < end_index; ++i)
            {
                // NOTE:  forward dependency

                value_type value = an_array[i + a_count];
                an_array[i] = value;
            }
        }
    }

    static void
    shift_elements_forwards(accessor an_array, const iter_index_type from_index, int a_count)
    {
        __SDLT_ASSERT(a_count > 0);

        // Note use of iter_index_type to avoid divisions/modulus
        iter_index_type dest_index(an_array.get_size_d1() - 1);
        iter_index_type source_index = dest_index - a_count;
        __SDLT_ASSERT(source_index < dest_index);
        for(;source_index >= from_index; --source_index, --dest_index)
        {
            value_type value = an_array[source_index];
            an_array[dest_index] = value;
        }
    }
};

} // namespace simd
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SIMD_ELEMENT_MOVER_H
