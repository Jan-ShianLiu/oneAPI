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

#ifndef SDLT_INDIRECT_ASSERTION_H
#define SDLT_INDIRECT_ASSERTION_H

#include "always_false_type.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

    // To get compiler to emit types of a static_assert value we need a layer of templated indirection
    // so that the call site will contain the types of the assertion value
    //
    // Example Usage:
    // static_assert(indirect_assertion<std::is_same<SomeType, AnotherType>>::value, "MisMatched Types");
    //
    // The nested static_assert inside indirect_assertion on AssertionT::value will have the AssertionT types displayed,
    // but not provide useful line info as it always happens in this file.
    // However the outer static_assert will have useful location info and an additional message for context

    template <typename AssertionT>
    struct indirect_assertion
    {
        static_assert(AssertionT::value, "see [with AssertionT=...] below for internals");
        static bool const value = AssertionT::value;
    };


} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_INDIRECT_ASSERTION_H
