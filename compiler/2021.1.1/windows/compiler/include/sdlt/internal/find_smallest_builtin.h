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

#ifndef SDLT_FIND_SMALLEST_BUILT_IN_H
#define SDLT_FIND_SMALLEST_BUILT_IN_H

#include <type_traits>

#include "../internal/is_builtin.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<typename BuiltInT, typename... ArgsT>
struct find_smallest_builtin
{
    static_assert(is_builtin<BuiltInT>::value, "find_smallest_builtin is only valid for built-in types");

    typedef typename find_smallest_builtin<ArgsT...>::type SmallestTypeFromArgs;

    typedef typename std::conditional<(sizeof(BuiltInT) < sizeof(SmallestTypeFromArgs)),
            BuiltInT,
            SmallestTypeFromArgs>::type type;
};

template<typename BuiltInT>
struct find_smallest_builtin<BuiltInT>
{
    static_assert(is_builtin<BuiltInT>::value, "find_smallest_builtin is only valid for built-in types");

    typedef BuiltInT type;
};


} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_FIND_SMALLEST_BUILT_IN_H
