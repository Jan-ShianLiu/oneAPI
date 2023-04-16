//
// Copyright (C) 2012-2015 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_IS_SAME_TYPE_H
#define SDLT_IS_SAME_TYPE_H

#include "../internal/false_value.h"
#include "../internal/true_value.h"

namespace sdlt
{

namespace internal {

SDLT_INHERIT_IMPL_BEGIN

template <typename LeftT, typename RightT>
struct is_same_type : public false_value {};

// Specialize for when identical left and right types to derive from true_value
// Note is_same_type<> will cast down to either false_value or true_value
// so it can be used for polymorphic function calls
template<typename SameT> struct is_same_type<SameT, SameT> : public true_value {};

SDLT_INHERIT_IMPL_END

} // namepace internal

} // namespace sdlt

#endif // SDLT_IS_SAME_TYPE_H
