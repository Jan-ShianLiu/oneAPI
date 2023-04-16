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

PURE SUBROUTINE SGGEVX_F95(A,B,ALPHAR,ALPHAI,BETA,VL,VR,BALANC,ILO,IHI, &
     &                     LSCALE,RSCALE,ABNRM,BBNRM,RCONDE,RCONDV,INFO)
    ! Fortran77 call:
    ! SGGEVX(BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,B,LDB,ALPHAR,ALPHAI,BETA,
    !   VL,LDVL,VR,LDVR,ILO,IHI,LSCALE,RSCALE,ABNRM,BBNRM,RCONDE,RCONDV,
    !   WORK,LWORK,IWORK,BWORK,INFO)
    ! BALANC='N','B','P'; default: 'N'
    ! <<< Use statements >>>
    USE F77_LAPACK, ONLY: F77_GGEVX, F77_XERBLA
    ! <<< Implicit statement >>>
    IMPLICIT NONE
    ! <<< Kind parameter >>>
    INTEGER, PARAMETER :: WP = KIND(1.0E0)
    ! <<< Scalar arguments >>>
    CHARACTER(LEN=1), INTENT(IN), OPTIONAL :: BALANC
    INTEGER, INTENT(OUT), OPTIONAL :: ILO
    INTEGER, INTENT(OUT), OPTIONAL :: IHI
    REAL(WP), INTENT(OUT), OPTIONAL :: ABNRM
    REAL(WP), INTENT(OUT), OPTIONAL :: BBNRM
    INTEGER, INTENT(OUT), OPTIONAL :: INFO
    ! <<< Array arguments >>>
    REAL(WP), INTENT(INOUT) :: A(:,:)
    REAL(WP), INTENT(INOUT) :: B(:,:)
    REAL(WP), INTENT(OUT) :: ALPHAR(:)
    REAL(WP), INTENT(OUT) :: ALPHAI(:)
    REAL(WP), INTENT(OUT) :: BETA(:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: VL(:,:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: VR(:,:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: LSCALE(:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: RSCALE(:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: RCONDE(:)
    REAL(WP), INTENT(OUT), OPTIONAL, TARGET :: RCONDV(:)
    ! <<< Local declarations >>>
    ! <<< Parameters >>>
    CHARACTER(LEN=5), PARAMETER :: SRNAME = 'GGEVX'
    ! <<< Local scalars >>>
    CHARACTER(LEN=1) :: O_BALANC
    INTEGER :: O_ILO
    INTEGER :: O_IHI
    REAL(WP) :: O_ABNRM
    REAL(WP) :: O_BBNRM
    INTEGER :: O_INFO
    CHARACTER(LEN=1) :: JOBVL
    CHARACTER(LEN=1) :: JOBVR
    CHARACTER(LEN=1) :: SENSE
    INTEGER :: N
    INTEGER :: LDA
    INTEGER :: LDB
    INTEGER :: LDVL
    INTEGER :: LDVR
    INTEGER :: LWORK
    INTEGER :: L_STAT_ALLOC, L_STAT_DEALLOC
    ! <<< Local arrays >>>
    REAL(WP), POINTER :: O_VL(:,:)
    REAL(WP), POINTER :: O_VR(:,:)
    REAL(WP), POINTER :: O_LSCALE(:)
    REAL(WP), POINTER :: O_RSCALE(:)
    REAL(WP), POINTER :: O_RCONDE(:)
    REAL(WP), POINTER :: O_RCONDV(:)
    REAL(WP), POINTER :: WORK(:)
    INTEGER, POINTER :: IWORK(:)
    LOGICAL, POINTER :: BWORK(:)
    ! <<< Arrays to request optimal sizes >>>
    REAL(WP) :: S_WORK(1)
    ! <<< Stubs to "allocate" optional arrays >>>
    REAL(WP), TARGET :: L_A1_REAL(1)
    REAL(WP), TARGET :: L_A2_REAL(1,1)
    ! <<< Intrinsic functions >>>
    INTRINSIC MAX, PRESENT, SIZE
    ! <<< Executable statements >>>
    ! <<< Init optional and skipped scalars >>>
    IF(PRESENT(BALANC)) THEN
        O_BALANC = BALANC
    ELSE
        O_BALANC = 'N'
    ENDIF
    IF(PRESENT(VL)) THEN
        JOBVL = 'V'
    ELSE
        JOBVL = 'N'
    ENDIF
    IF(PRESENT(VR)) THEN
        JOBVR = 'V'
    ELSE
        JOBVR = 'N'
    ENDIF
    LDA = MAX(1,SIZE(A,1))
    LDB = MAX(1,SIZE(B,1))
    IF(PRESENT(VL)) THEN
        LDVL = MAX(1,SIZE(VL,1))
    ELSE
        LDVL = 1
    ENDIF
    IF(PRESENT(VR)) THEN
        LDVR = MAX(1,SIZE(VR,1))
    ELSE
        LDVR = 1
    ENDIF
    N = SIZE(A,2)
    IF(PRESENT(RCONDE).AND.PRESENT(RCONDV)) THEN
        SENSE = 'B'
    ELSEIF(PRESENT(RCONDE)) THEN
        SENSE = 'E'
    ELSEIF(PRESENT(RCONDV)) THEN
        SENSE = 'V'
    ELSE
        SENSE = 'N'
    ENDIF
    ! <<< Init allocate status >>>
    L_STAT_ALLOC = 0
    ! <<< Allocate local and work arrays >>>
    IF(PRESENT(LSCALE)) THEN
        O_LSCALE => LSCALE
    ELSE
        ALLOCATE(O_LSCALE(N), STAT=L_STAT_ALLOC)
    ENDIF
    IF(PRESENT(RCONDE)) THEN
        O_RCONDE => RCONDE
    ELSE
        O_RCONDE => L_A1_REAL
    ENDIF
    IF(PRESENT(RCONDV)) THEN
        O_RCONDV => RCONDV
    ELSE
        O_RCONDV => L_A1_REAL
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        IF(PRESENT(RSCALE)) THEN
            O_RSCALE => RSCALE
        ELSE
            ALLOCATE(O_RSCALE(N), STAT=L_STAT_ALLOC)
        ENDIF
    ENDIF
    IF(PRESENT(VL)) THEN
        O_VL => VL
    ELSE
        O_VL => L_A2_REAL
    ENDIF
    IF(PRESENT(VR)) THEN
        O_VR => VR
    ELSE
        O_VR => L_A2_REAL
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(BWORK(N), STAT=L_STAT_ALLOC)
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(IWORK(N+6), STAT=L_STAT_ALLOC)
    ENDIF
    ! <<< Request work array(s) size >>>
    LWORK = -1
    CALL F77_GGEVX(O_BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,B,LDB,ALPHAR,     &
     &   ALPHAI,BETA,O_VL,LDVL,O_VR,LDVR,O_ILO,O_IHI,O_LSCALE,O_RSCALE, &
     &O_ABNRM,O_BBNRM,O_RCONDE,O_RCONDV,S_WORK,LWORK,IWORK,BWORK,O_INFO)
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
        CALL F77_GGEVX(O_BALANC,JOBVL,JOBVR,SENSE,N,A,LDA,B,LDB,ALPHAR, &
     &   ALPHAI,BETA,O_VL,LDVL,O_VR,LDVR,O_ILO,O_IHI,O_LSCALE,O_RSCALE, &
     &  O_ABNRM,O_BBNRM,O_RCONDE,O_RCONDV,WORK,LWORK,IWORK,BWORK,O_INFO)
    ELSE; O_INFO = -1000
    ENDIF
    ! <<< Set output optional scalar parameters >>>
    IF(PRESENT(ABNRM)) THEN
        ABNRM = O_ABNRM
    ENDIF
    IF(PRESENT(BBNRM)) THEN
        BBNRM = O_BBNRM
    ENDIF
    IF(PRESENT(IHI)) THEN
        IHI = O_IHI
    ENDIF
    IF(PRESENT(ILO)) THEN
        ILO = O_ILO
    ENDIF
    ! <<< Deallocate work arrays with requested sizes >>>
    DEALLOCATE(WORK, STAT=L_STAT_DEALLOC)
200    CONTINUE
    ! <<< Deallocate local and work arrays >>>
    IF(.NOT. PRESENT(LSCALE)) THEN
        DEALLOCATE(O_LSCALE, STAT=L_STAT_DEALLOC)
    ENDIF
    IF(.NOT. PRESENT(RSCALE)) THEN
        DEALLOCATE(O_RSCALE, STAT=L_STAT_DEALLOC)
    ENDIF
    DEALLOCATE(BWORK, STAT=L_STAT_DEALLOC)
    DEALLOCATE(IWORK, STAT=L_STAT_DEALLOC)
    ! <<< Error handler >>>
    IF(PRESENT(INFO)) THEN
        INFO = O_INFO
    ELSEIF(O_INFO <= -1000) THEN
        CALL F77_XERBLA(SRNAME,-O_INFO)
    ENDIF
END SUBROUTINE SGGEVX_F95