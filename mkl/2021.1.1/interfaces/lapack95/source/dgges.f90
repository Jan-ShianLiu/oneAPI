!===============================================================================
! Copyright 2005-2020 Intel Corporation.
!
! This software and the related documents are Intel copyrighted  materials,  and
! your use of  them is  governed by the  express license  under which  they were
! provided to you (License).  Unless the License provides otherwise, you may not
! use, modify, copy, publish, distribute,  disclose or transmit this software or
! the related documents without Intel's prior written permission.
!
! This software and the related documents  are provided as  is,  with no express
! or implied  warranties,  other  than those  that are  expressly stated  in the
! License.
!===============================================================================

!  Content:
!      F95 interface for LAPACK routines
!*******************************************************************************
! This file was generated automatically!
!*******************************************************************************

PURE SUBROUTINE DGGES_F95(A,B,ALPHAR,ALPHAI,BETA,VSL,VSR,SELECT,SDIM,   &
     &                                                             INFO)
    ! Fortran77 call:
    ! DGGES(JOBVSL,JOBVSR,SORT,SELECT,N,A,LDA,B,LDB,SDIM,ALPHAR,ALPHAI,
    !   BETA,VSL,LDVSL,VSR,LDVSR,WORK,LWORK,BWORK,INFO)
    ! <<< Use statements >>>
    USE F77_LAPACK, ONLY: F77_GGES, F77_XERBLA
    ! <<< Implicit statement >>>
    IMPLICIT NONE
    ! <<< Kind parameter >>>
    INTEGER, PARAMETER :: WP = KIND(1.0D0)
    ! <<< Scalar arguments >>>
    INTEGER, INTENT(OUT), OPTIONAL :: SDIM
    INTEGER, INTENT(OUT), OPTIONAL :: INFO
    ! <<< Array arguments >>>
    REAL(WP), INTENT(INOUT) :: A(:,:)
    REAL(WP), INTENT(INOUT) :: B(:,:)
    REAL(WP), INTENT(OUT) :: ALPHAR(:)
    REAL(WP), INTENT(OUT) :: ALPHAI(:)
    REAL(WP), INTENT(OUT) :: BETA(:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: VSL(:,:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: VSR(:,:)
    ! <<< Function arguments >>>
    INTERFACE
        PURE LOGICAL FUNCTION SELECT(ALPHAR,ALPHAI,BETA)
            INTEGER, PARAMETER :: WP = KIND(1.0D0)
            REAL(WP), INTENT(IN) :: ALPHAR,ALPHAI,BETA
        END FUNCTION SELECT
    END INTERFACE
    OPTIONAL :: SELECT
    ! <<< Local declarations >>>
    ! <<< Parameters >>>
    CHARACTER(LEN=4), PARAMETER :: SRNAME = 'GGES'
    ! <<< Local scalars >>>
    INTEGER :: O_SDIM
    INTEGER :: O_INFO
    CHARACTER(LEN=1) :: JOBVSL
    CHARACTER(LEN=1) :: JOBVSR
    CHARACTER(LEN=1) :: SORT
    INTEGER :: N
    INTEGER :: LDA
    INTEGER :: LDB
    INTEGER :: LDVSL
    INTEGER :: LDVSR
    INTEGER :: LWORK
    INTEGER :: L_STAT_ALLOC, L_STAT_DEALLOC
    ! <<< Local arrays >>>
    REAL(WP), POINTER :: O_VSL(:,:)
    REAL(WP), POINTER :: O_VSR(:,:)
    REAL(WP), POINTER :: WORK(:)
    LOGICAL, POINTER :: BWORK(:)
    ! <<< Arrays to request optimal sizes >>>
    REAL(WP) :: S_WORK(1)
    ! <<< Stubs to "allocate" optional arrays >>>
    LOGICAL, TARGET :: L_A1_LOGI(1)
    REAL(WP), TARGET :: L_A2_REAL(1,1)
    ! <<< Intrinsic functions >>>
    INTRINSIC MAX, PRESENT, SIZE
    ! <<< Executable statements >>>
    ! <<< Init optional and skipped scalars >>>
    IF(PRESENT(VSL)) THEN
        JOBVSL = 'V'
    ELSE
        JOBVSL = 'N'
    ENDIF
    IF(PRESENT(VSR)) THEN
        JOBVSR = 'V'
    ELSE
        JOBVSR = 'N'
    ENDIF
    LDA = MAX(1,SIZE(A,1))
    LDB = MAX(1,SIZE(B,1))
    IF(PRESENT(VSL)) THEN
        LDVSL = MAX(1,SIZE(VSL,1))
    ELSE
        LDVSL = 1
    ENDIF
    IF(PRESENT(VSR)) THEN
        LDVSR = MAX(1,SIZE(VSR,1))
    ELSE
        LDVSR = 1
    ENDIF
    N = SIZE(A,2)
    IF(PRESENT(SELECT)) THEN
        SORT = 'S'
    ELSE
        SORT = 'N'
    ENDIF
    ! <<< Init allocate status >>>
    L_STAT_ALLOC = 0
    ! <<< Allocate local and work arrays >>>
    IF(PRESENT(VSL)) THEN
        O_VSL => VSL
    ELSE
        O_VSL => L_A2_REAL
    ENDIF
    IF(PRESENT(VSR)) THEN
        O_VSR => VSR
    ELSE
        O_VSR => L_A2_REAL
    ENDIF
    IF(.NOT.PRESENT(SELECT)) THEN
        BWORK => L_A1_LOGI
    ELSE
        ALLOCATE(BWORK(N), STAT=L_STAT_ALLOC)
    ENDIF
    ! <<< Request work array(s) size >>>
    LWORK = -1
    CALL F77_GGES(JOBVSL,JOBVSR,SORT,SELECT,N,A,LDA,B,LDB,O_SDIM,ALPHAR,&
     &    ALPHAI,BETA,O_VSL,LDVSL,O_VSR,LDVSR,S_WORK,LWORK,BWORK,O_INFO)
    ! <<< Exit if error: bad parameters >>>
    IF(O_INFO /= 0) THEN
        GOTO 200
    ENDIF
    LWORK = S_WORK(1)
    ! <<< Allocate work arrays with requested sizes >>>
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(WORK(LWORK), STAT=L_STAT_ALLOC)
    ENDIF
    ! <<< Call lapack77 routine >>>
    IF(L_STAT_ALLOC==0) THEN
        CALL F77_GGES(JOBVSL,JOBVSR,SORT,SELECT,N,A,LDA,B,LDB,O_SDIM,   &
     &     ALPHAR,ALPHAI,BETA,O_VSL,LDVSL,O_VSR,LDVSR,WORK,LWORK,BWORK, &
     &                                                           O_INFO)
    ELSE; O_INFO = -1000
    ENDIF
    ! <<< Set output optional scalar parameters >>>
    IF(PRESENT(SDIM)) THEN
        SDIM = O_SDIM
    ENDIF
    ! <<< Deallocate work arrays with requested sizes >>>
    DEALLOCATE(WORK, STAT=L_STAT_DEALLOC)
200    CONTINUE
    ! <<< Deallocate local and work arrays >>>
    IF(PRESENT(SELECT)) THEN
        DEALLOCATE(BWORK, STAT=L_STAT_DEALLOC)
    ENDIF
    ! <<< Error handler >>>
    IF(PRESENT(INFO)) THEN
        INFO = O_INFO
    ELSEIF(O_INFO <= -1000) THEN
        CALL F77_XERBLA(SRNAME,-O_INFO)
    ENDIF
END SUBROUTINE DGGES_F95
