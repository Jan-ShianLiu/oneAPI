//
// Copyright (C) 2014-2016 Intel Corporation. All Rights Reserved.
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

#ifndef __SDLT_XMACRO_OPERATOR
#error must define __SDLT_XMACRO_OPERATOR to number before including this header
#endif

#ifndef __SDLT_XMACRO_SELF
#error must define __SDLT_XMACRO_SELF to number before including this header
#endif

    // proxy operators are templated to allow deferred instantiation such that
    // we don't get build errors if the underlying primitive does not support the operator.
    template<typename OtherT>
    SDLT_INLINE
    __SDLT_XMACRO_SELF & operator __SDLT_XMACRO_OPERATOR (const OtherT &other)
    {
        value_type value = unproxy(*this);
        // If OtherT happens to be a proxy object, that is fine.
        // Because if the operator exists on the value_type, then
        // implicit conversion operator on the proxy should extract
        // a value_type
        value __SDLT_XMACRO_OPERATOR other;
        this->operator=(value);
        return *this;
    }

#undef __SDLT_XMACRO_OPERATOR
