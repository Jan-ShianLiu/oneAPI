//
// Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_DEFERRED_H
#define SDLT_DEFERRED_H

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

// We may want a templated method or function to defer instantiation.
// because its implementation calls methods or accesses data members that may not exist
// for a type.  But to enable this deferred instantiation, we need the method to be
// dependent on a template parameter.  For many cases we already know the type, so
// the compiler will immediately instantiate the method, possible causing build error.
// So only can use the "deferred" template to wrap the desired type along with
// an dependent_arg to avoid immediate instantiation.
// E.G.
//    template <typename DependentT=dependent_arg>
//    auto methodName (const deferred<Self, DependentT>::type &other) const
//    -> decltype(this->doWork(other))
//    {
//        doWork(other);
//    }
//

struct dependent_arg
{};

template <typename T, typename DependentT>
struct deferred
{
    typedef T type;
};


} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_DEFERRED_H
