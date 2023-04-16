/*  Copyright 1992-2020 Intel Corporation.                                 */
/*                                                                         */
/*  This software and the related documents are Intel copyrighted          */
/*  materials, and your use of them is governed by the express license     */
/*  under which they were provided to you ("License"). Unless the License  */
/*  provides otherwise, you may not use, modify, copy, publish,            */
/*  distribute, disclose or transmit this software or the related          */
/*  documents without Intel's prior written permission.                    */
/*                                                                         */
/*  This software and the related documents are provided as is, with no    */
/*  express or implied warranties, other than those that are expressly     */
/*  stated in the License.                                                 */

// This file is automatically generated by acl/common_header_gen/global_reset_params_header_gen.pl
//----------------------------------------------------------------------------------------------------------------
// Global constants for the hyper-optimization multi-cycle reset protocol
//----------------------------------------------------------------------------------------------------------------
// Contract between stallable nodes and top-level system
//    S - max cycles between assertion of reset and node raising its safe-state stall    (node's promise to system)
//    P - min number of cycles that reset will be held asserted                          (system's promise to node)
//    D - max cycles between deassertion of reset and node lowering its safe-state stall (node's promise to system)

'define ACL_S10_RESET_OPENCL_P 500
'define ACL_S10_RESET_OPENCL_S 100
'define ACL_S10_RESET_OPENCL_D 100

'define ACL_S10_RESET_HLS_P 60
'define ACL_S10_RESET_HLS_S 60
'define ACL_S10_RESET_HLS_D 60

'define ACL_GLOBAL_RESET_IP_P 50
'define ACL_GLOBAL_RESET_IP_S 50
'define ACL_GLOBAL_RESET_IP_D 50

'define ACL_S10_RESET_SFC_REQUIRED_D 15 
'define ACL_S10_RESET_OPENCL_CMAX 15
'define ACL_S10_RESET_HLS_CMAX 10
'define ACL_S10_BSP_MINIMUM_RESET_DURATION 1024

