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

PURE SUBROUTINE CPPSVX1_F95(AP,B,X,UPLO,AFP,FACT,EQUED,S,FERR,BERR,     &
     &                                                       RCOND,INFO)
    ! Fortran77 call:
    ! CPPSVX(FACT,UPLO,N,NRHS,AP,AFP,EQUED,S,B,LDB,X,LDX,RCOND,FERR,
    !   BERR,WORK,RWORK,INFO)
    ! UPLO='U','L'; default: 'U'
    ! FACT='N','E','F'; default: 'N'
    ! EQUED='N','Y'; default: 'N'
    ! Default S(i)=1.0_WP
    ! <<< Use statements >>>
    USE F77_LAPACK1, ONLY: F77_PPSVX
    USE F77_LAPACK, ONLY: F77_XERBLA
    ! <<< Implicit statement >>>
    IMPLICIT NONE
    ! <<< Kind parameter >>>
    INTEGER, PARAMETER :: WP = KIND(1.0E0)
    ! <<< Scalar arguments >>>
    CHARACTER(LEN=1), INTENT(IN), OPTIONAL :: UPLO
    CHARACTER(LEN=1), INTENT(IN), OPTIONAL :: FACT
    CHARACTER(LEN=1), INTENT(INOUT), OPTIONAL :: EQUED
    REAL(WP), INTENT(OUT), OPTIONAL :: FERR
    REAL(WP), INTENT(OUT), OPTIONAL :: BERR
    REAL(WP), INTENT(OUT), OPTIONAL :: RCOND
    INTEGER, INTENT(OUT), OPTIONAL :: INFO
    ! <<< Array arguments >>>
    COMPLEX(WP), INTENT(INOUT) :: AP(:)
    COMPLEX(WP), INTENT(INOUT) :: B(:)
    COMPLEX(WP), INTENT(OUT) :: X(:)
    COMPLEX(WP), INTENT(INOUT), OPTIONAL, TARGET :: AFP(:)
    REAL(WP), INTENT(INOUT), OPTIONAL, TARGET :: S(:)
    ! <<< Local declarations >>>
    ! <<< Parameters >>>
    CHARACTER(LEN=5), PARAMETER :: SRNAME = 'PPSVX'
    ! <<< Local scalars >>>
    CHARACTER(LEN=1) :: O_UPLO
    CHARACTER(LEN=1) :: O_FACT
    CHARACTER(LEN=1) :: O_EQUED
    REAL(WP) :: O_RCOND
    INTEGER :: O_INFO
    INTEGER :: N
    INTEGER :: NRHS
    INTEGER :: LDB
    INTEGER :: LDX
    INTEGER :: L_STAT_ALLOC, L_STAT_DEALLOC
    INTEGER :: L_NN
    ! <<< Local arrays >>>
    COMPLEX(WP), POINTER :: O_AFP(:)
    REAL(WP), POINTER :: O_S(:)
    COMPLEX(WP), POINTER :: WORK(:)
    REAL(WP), POINTER :: RWORK(:)
    REAL(WP) :: R_BERR(1)
    REAL(WP) :: R_FERR(1)
    ! <<< Intrinsic functions >>>
    INTRINSIC INT, MAX, PRESENT, REAL, SIZE, SQRT
    ! <<< Executable statements >>>
    ! <<< Init optional and skipped scalars >>>
    IF(PRESENT(EQUED)) THEN
        O_EQUED = EQUED
    ELSE
        O_EQUED = 'N'
    ENDIF
    IF(PRESENT(FACT)) THEN
        O_FACT = FACT
    ELSE
        O_FACT = 'N'
    ENDIF
    IF(PRESENT(UPLO)) THEN
        O_UPLO = UPLO
    ELSE
        O_UPLO = 'U'
    ENDIF
    LDB = MAX(1,SIZE(B,1))
    LDX = MAX(1,SIZE(X,1))
    L_NN = SIZE(AP)
    IF(L_NN <= 0) THEN
        N = L_NN
    ELSE
        ! Packed matrix "AP(N*(N+1)/2)", so: N=(-1+8*(SIZE(AP)))/2
        N = INT((-1+SQRT(1+8*REAL(L_NN,WP)))*0.5)
    ENDIF
    NRHS = 1
    ! <<< Init allocate status >>>
    L_STAT_ALLOC = 0
    ! <<< Allocate local and work arrays >>>
    IF(PRESENT(AFP)) THEN
        O_AFP => AFP
    ELSE
        ALLOCATE(O_AFP(N*(N+1)/2), STAT=L_STAT_ALLOC)
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        IF(PRESENT(S)) THEN
            O_S => S
        ELSE
            ALLOCATE(O_S(N), STAT=L_STAT_ALLOC); O_S = 1.0_WP
        ENDIF
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(RWORK(N), STAT=L_STAT_ALLOC)
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(WORK(2*N), STAT=L_STAT_ALLOC)
    ENDIF
    ! <<< Call lapack77 routine >>>
    IF(L_STAT_ALLOC==0) THEN
        CALL F77_PPSVX(O_FACT,O_UPLO,N,NRHS,AP,O_AFP,O_EQUED,O_S,B,LDB, &
     &                    X,LDX,O_RCOND,R_FERR,R_BERR,WORK,RWORK,O_INFO)
    ELSE; O_INFO = -1000
    ENDIF
    ! <<< Set output optional scalars for reduced arrays >>>
    IF(PRESENT(BERR)) THEN
        BERR = R_BERR(1)
    ENDIF
    IF(PRESENT(FERR)) THEN
        FERR = R_FERR(1)
    ENDIF
    ! <<< Set output optional scalar parameters >>>
    IF(PRESENT(EQUED)) THEN
        EQUED = O_EQUED
    ENDIF
    IF(PRESENT(RCOND)) THEN
        RCOND = O_RCOND
    ENDIF
    ! <<< Deallocate local and work arrays >>>
    IF(.NOT. PRESENT(AFP)) THEN
        DEALLOCATE(O_AFP, STAT=L_STAT_DEALLOC)
    ENDIF
    IF(.NOT. PRESENT(S)) THEN
        DEALLOCATE(O_S, STAT=L_STAT_DEALLOC)
    ENDIF
    DEALLOCATE(RWORK, STAT=L_STAT_DEALLOC)
    DEALLOCATE(WORK, STAT=L_STAT_DEALLOC)
    ! <<< Error handler >>>
    IF(PRESENT(INFO)) THEN
        INFO = O_INFO
    ELSEIF(O_INFO <= -1000) THEN
        CALL F77_XERBLA(SRNAME,-O_INFO)
    ENDIF
END SUBROUTINE CPPSVX1_F95
