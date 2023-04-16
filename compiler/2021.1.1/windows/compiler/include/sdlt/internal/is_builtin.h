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

#ifndef SDLT_IS_BUILT_IN_H
#define SDLT_IS_BUILT_IN_H

#include <stdint.h>
#include <type_traits>

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

__SDLT_INHERIT_IMPL_BEGIN

template <typename T>
struct is_builtin : public std::false_type {};

// Specialize for each built in type to derive from std::true_type
// Note is_builtin<> will cast down to either std::false_type or std::true_type
// so it can be used for polymorphic function calls
template<> struct is_builtin<char> : public std::true_type {};
template<> struct is_builtin<uint8_t> : public std::true_type {};
template<> struct is_builtin<int8_t> : public std::true_type {};
template<> struct is_builtin<uint16_t> : public std::true_type {};
template<> struct is_builtin<int16_t> : public std::true_type {};
template<> struct is_builtin<uint32_t> : public std::true_type {};
template<> struct is_builtin<int32_t> : public std::true_type {};
template<> struct is_builtin<uint64_t> : public std::true_type {};
template<> struct is_builtin<int64_t> : public std::true_type {};
#if __SDLT_ON_64BIT == 0 || __SDLT_ON_WINDOWS == 1
    // define these types only for 32bit compiler, because these
    // types have different meanings on 32bit and and 64bit.
    // if define them for 64bit compiler these specializations will collide
    // with specializations above
    template<> struct is_builtin<unsigned long> : public std::true_type {};
    template<> struct is_builtin<signed long> : public std::true_type {};
#endif // !__SDLT_ON_64BIT
template<> struct is_builtin<float> : public std::true_type {};
template<> struct is_builtin<double> : public std::true_type {};


__SDLT_INHERIT_IMPL_END

} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_IS_BUILT_IN_H
