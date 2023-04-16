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

#ifndef SDLT_ELEMENT_MOVER_H
#define SDLT_ELEMENT_MOVER_H

#include "../../common.h"
#include "../../defaults.h"
#include "../../linear_index.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace linear {

template <typename ContainerT>
class element_mover
{
public:
    typedef typename ContainerT::primitive_type value_type;
    typedef typename ContainerT::template accessor<> accessor;

    static void
    fill_with(
        const accessor dest,
        const value_type & a_value,
        const int start_at,
        const int end_before)
    {
        SDLT_INLINE_BLOCK
        {
            // because dest could be AOS
            // it may not be profitable to force vectorization
            // let the compiler decide if it is profitable
            SDLT_INTEL_PRAGMA("ivdep")
            SDLT_INTEL_PRAGMA("vector")
            for(int i = start_at; i < end_before; ++i)
            {
                dest[i] = a_value;
            }
        }
    }

    template<typename SourceAccessorT>
    static void
    copy_all_values_from(
        const SourceAccessorT iSource,
        const accessor dest)
    {
        SDLT_INLINE_BLOCK
        {
            const int end_before = dest.get_size_d1();
            // because the iSource or dest could be AOS
            // it may not be profitable to force vectorization
            // let the compiler decide if it is profitable
            SDLT_INTEL_PRAGMA("ivdep")
            SDLT_INTEL_PRAGMA("vector")
            for(int i = 0; i < end_before; ++i)
            {
                dest[i] = iSource[i];
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
        for (int i=start_at; i < end_before; ++i)
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

        // TODO: specialize SOA to perform the shift per member for better memory locality
        SDLT_INLINE_BLOCK
        {
            const int end_before = an_array.get_size_d1() - a_count;

            // because an_array could be AOS
            // it may not be profitable to force vectorization
            // let the compiler decide if it is profitable
            SDLT_INTEL_PRAGMA("ivdep")
            SDLT_INTEL_PRAGMA("vector")
            for(int i = from_index; i < end_before; ++i)
            {
                // NOTE:  forward dependency
                // SIMD code gen can hande this, however
                // any parallel engine would have issues
                // as we couldn't guarantee that i + count hadn't been
                // overwritten yet.
                // A multi-pass approach can work, where boundary conditions
                // are stored off, then after the main loop finishes
                // a final cleanup pass runs
                // wonder if the engine could do this for us?

                // To capture index arithmetic to pass through the accessor
                // we must use a proper sdlt index, not a integer;
                linear_index index(i);
                const value_type value = an_array[index + a_count];
                an_array[index] = value;
            }
        }
    }

    static void
    shift_elements_forwards(accessor an_array, int from_index, int a_count)
    {
        __SDLT_ASSERT(a_count > 0);

        // TODO: specialize SOA to perform the shift per member for better memory locality

        int dest_index = an_array.get_size_d1() - 1;
        int source_index = dest_index - a_count;
        __SDLT_ASSERT(source_index < dest_index);
        for(;source_index >= from_index; --source_index, --dest_index)
        {
            const value_type value = an_array[source_index];
            an_array[dest_index] = value;
        }
    }
};

} // namespace linear
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ELEMENT_MOVER_H
