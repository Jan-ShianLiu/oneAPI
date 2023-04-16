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

#ifndef SDLT_INT_SEQUENCE_H
#define SDLT_INT_SEQUENCE_H


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {


template<int... IntegerListT>
struct int_sequence
{
};

template<int StartAtT, int EndBeforeT, typename IntSequenceT>
struct int_sequence_generator;

template<int StartAtT, int EndBeforeT, int... IntegerListT>
struct int_sequence_generator<StartAtT, EndBeforeT, int_sequence<IntegerListT...>>
{
    typedef typename int_sequence_generator<StartAtT+1, EndBeforeT, int_sequence<IntegerListT..., StartAtT>>::type type;
};

template<int EndBeforeT, int... IntegerListT>
struct int_sequence_generator<EndBeforeT, EndBeforeT, int_sequence<IntegerListT...>>
{
    typedef int_sequence<IntegerListT...> type;
};

template<int EndBeforeT, int StartAtT=0>
using make_int_sequence = typename int_sequence_generator<StartAtT, EndBeforeT, int_sequence<> >::type;

// Consider overriding std::get<>
template <int IndexT, typename IntSequenceT>
struct int_sequence_element;

template <int HeadT, int... TailListT>
struct int_sequence_element<0, int_sequence<HeadT, TailListT...>>
{
    static constexpr int value = HeadT;
};

template <int IndexT, int HeadT, int... TailListT>
struct int_sequence_element<IndexT, int_sequence<HeadT, TailListT...>>
{
    static constexpr int value = int_sequence_element<IndexT-1, int_sequence<TailListT...> >::value;
};


} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_INT_SEQUENCE_H
