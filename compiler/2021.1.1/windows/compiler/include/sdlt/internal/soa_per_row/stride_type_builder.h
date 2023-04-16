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

#ifndef SDLT_SOA_PER_ROW_STRIDE_TYPE_BUILDER_H
#define SDLT_SOA_PER_ROW_STRIDE_TYPE_BUILDER_H

#include <cstddef>


#include "../../common.h"
#include "../../primitive.h"
#include "../../fixed.h"
#include "../aligned_stride.h"
#include "../fixed_stride.h"
#include "../int_sequence.h"
#include "../stride_list.h"
#include "../tuple.h"
#include "../soa/stride_type_builder.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa_per_row {

// Same as soa for the stride_offset_calculator, share code
using soa::aligned_row_stride_for;
using soa::stride_type_builder;

} // namepace soa_per_row
} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_PER_ROW_STRIDE_TYPE_BUILDER_H
