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

#ifndef SDLT_ALIGNED_ACCESS_H
#define SDLT_ALIGNED_ACCESS_H


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<int AlignD1OnIndexT, typename BuiltInT>
struct aligned_access
{
    static_assert(internal::is_builtin<BuiltInT>::value, "aligned_access is only valid for built-in types");

    static const int offset = ((64/static_cast<int>(sizeof(BuiltInT))) - AlignD1OnIndexT)%(64/static_cast<int>(sizeof(BuiltInT)));
};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ALIGNED_ACCESS_H
