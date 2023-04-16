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

// RESERVED FOR FUTURE USE
// Identify the order dimensions should be processed
// Use to identify the order to layout data
// Use to identify the order blocking should occur
#ifdef __SDLT_RESERVED_FOR_FUTURE_USE

#ifndef SDLT_ORDER_H
#define SDLT_ORDER_H


namespace sdlt {
namespace __SDLT_IMPL {

    // Similiar to int_sequence, 
    // Must contain value for 0 to (sizeof(DimensionListT...)-1)
    // which means repeated dimensions are allow
    template<int ...DimensionListT>
    struct order
    {

    };

    template <int DimenstionCountT>
    struct default_order
    {
        //typedef order<...> type;
    }

} // namepace __SDLT_IMPL
using __SDLT_IMPL::blocked;

} // namespace sdlt

#endif // SDLT_ORDER_H
#endif