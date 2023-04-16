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

#ifndef SDLT_STATIC_LOOP_H
#define SDLT_STATIC_LOOP_H

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {


template<int FirstT, int LastT, int StepT = 1>
struct loop_unroller
{
    template<typename BodyT>
    static SDLT_INLINE void
    recursivelyLayoutBody(BodyT &iBody)
    {
        iBody(FirstT);
        loop_unroller<FirstT + StepT, LastT>::recursivelyLayoutBody(iBody);
    }

    template<typename FunctorT>
    static SDLT_INLINE void
    recursivelyLayoutFunctor(FunctorT &iFunctor)
    {
        iFunctor.template operator()<FirstT>();
        loop_unroller<FirstT + StepT, LastT>::recursivelyLayoutFunctor(iFunctor);
    }
};

// Specialization for when FirstT == LastT to end the recursion
template<int LastT, int StepT>
struct loop_unroller<LastT, LastT, StepT>
{
    template<typename BodyT>
    static SDLT_INLINE void
    recursivelyLayoutBody(BodyT  &)
    {
    }

    template<typename FunctorT>
    static SDLT_INLINE void
    recursivelyLayoutFunctor(FunctorT &)
    {
    }
};

template<int FirstT, int LastT, int StepT=1, typename FunctorT>
SDLT_INLINE void
static_for(FunctorT &iFunctor)
{
    loop_unroller<FirstT, LastT, StepT>::recursivelyLayoutFunctor(iFunctor);
}


#define __SDLT_STATIC_LOOP_BEGIN(INDEX_NAME, START_AT, END_BEFORE) \
{ \
    const int _sdlt_start_at = (START_AT); \
    const int _sdlt_end_before = (END_BEFORE); \
    auto _sdlt_kernel = [&] (constexpr int INDEX_NAME) -> void


#define __SDLT_STATIC_LOOP_END \
    ; \
    sdlt::__SDLT_IMPL::internal::loop_unroller<_sdlt_start_at, _sdlt_end_before>::recursivelyLayoutBody(_sdlt_kernel); \
}

} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif  /* SDLT_STATIC_LOOP_H */

