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

PURE SUBROUTINE CGESVX1_F95(A,B,X,AF,IPIV,FACT,TRANS,EQUED,R,C,FERR,    &
     &                                           BERR,RCOND,RPVGRW,INFO)
    ! Fortran77 call:
    ! CGESVX(FACT,TRANS,N,NRHS,A,LDA,AF,LDAF,IPIV,EQUED,R,C,B,LDB,X,LDX,
    !   RCOND,FERR,BERR,WORK,RWORK,INFO)
    ! FACT='N','E','F'; default: 'N'
    ! TRANS='N','C','T'; default: 'N'
    ! EQUED='N','B','C','R'; default: 'N'
    ! Default R(i)=1.0_WP
    ! Default C(i)=1.0_WP
    ! <<< Use statements >>>
    USE F77_LAPACK1, ONLY: F77_GESVX
    USE F77_LAPACK, ONLY: F77_XERBLA
    ! <<< Implicit statement >>>
    IMPLICIT NONE
    ! <<< Kind parameter >>>
    INTEGER, PARAMETER :: WP = KIND(1.0E0)
    ! <<< Scalar arguments >>>
    CHARACTER(LEN=1), INTENT(IN), OPTIONAL :: FACT
    CHARACTER(LEN=1), INTENT(IN), OPTIONAL :: TRANS
    CHARACTER(LEN=1), INTENT(INOUT), OPTIONAL :: EQUED
    REAL(WP), INTENT(OUT), OPTIONAL :: FERR
    REAL(WP), INTENT(OUT), OPTIONAL :: BERR
    REAL(WP), INTENT(OUT), OPTIONAL :: RCOND
    REAL(WP), INTENT(OUT), OPTIONAL :: RPVGRW
    INTEGER, INTENT(OUT), OPTIONAL :: INFO
    ! <<< Array arguments >>>
    COMPLEX(WP), INTENT(INOUT) :: A(:,:)
    COMPLEX(WP), INTENT(INOUT) :: B(:)
    COMPLEX(WP), INTENT(OUT) :: X(:)
    COMPLEX(WP), INTENT(INOUT), OPTIONAL, TARGET :: AF(:,:)
    INTEGER, INTENT(INOUT), OPTIONAL, TARGET :: IPIV(:)
    REAL(WP), INTENT(INOUT), OPTIONAL, TARGET :: R(:)
    REAL(WP), INTENT(INOUT), OPTIONAL, TARGET :: C(:)
    ! <<< Local declarations >>>
    ! <<< Parameters >>>
    CHARACTER(LEN=5), PARAMETER :: SRNAME = 'GESVX'
    ! <<< Local scalars >>>
    CHARACTER(LEN=1) :: O_FACT
    CHARACTER(LEN=1) :: O_TRANS
    CHARACTER(LEN=1) :: O_EQUED
    REAL(WP) :: O_RCOND
    INTEGER :: O_INFO
    INTEGER :: N
    INTEGER :: NRHS
    INTEGER :: LDA
    INTEGER :: LDAF
    INTEGER :: LDB
    INTEGER :: LDX
    INTEGER :: L_STAT_ALLOC, L_STAT_DEALLOC
    ! <<< Local arrays >>>
    COMPLEX(WP), POINTER :: O_AF(:,:)
    INTEGER, POINTER :: O_IPIV(:)
    REAL(WP), POINTER :: O_R(:)
    REAL(WP), POINTER :: O_C(:)
    COMPLEX(WP), POINTER :: WORK(:)
    REAL(WP), POINTER :: RWORK(:)
    REAL(WP) :: R_BERR(1)
    REAL(WP) :: R_FERR(1)
    ! <<< Intrinsic functions >>>
    INTRINSIC MAX, PRESENT, SIZE
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
    IF(PRESENT(TRANS)) THEN
        O_TRANS = TRANS
    ELSE
        O_TRANS = 'N'
    ENDIF
    LDA = MAX(1,SIZE(A,1))
    LDB = MAX(1,SIZE(B,1))
    LDX = MAX(1,SIZE(X,1))
    N = SIZE(A,2)
    NRHS = 1
    IF(PRESENT(AF)) THEN
        LDAF = MAX(1,SIZE(AF,1))
    ELSE
        LDAF = N
    ENDIF
    ! <<< Init allocate status >>>
    L_STAT_ALLOC = 0
    ! <<< Allocate local and work arrays >>>
    IF(PRESENT(AF)) THEN
        O_AF => AF
    ELSE
        ALLOCATE(O_AF(LDAF,N), STAT=L_STAT_ALLOC)
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        IF(PRESENT(C)) THEN
            O_C => C
        ELSE
            ALLOCATE(O_C(N), STAT=L_STAT_ALLOC); O_C = 1.0_WP
        ENDIF
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        IF(PRESENT(IPIV)) THEN
            O_IPIV => IPIV
        ELSE
            ALLOCATE(O_IPIV(N), STAT=L_STAT_ALLOC)
        ENDIF
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        IF(PRESENT(R)) THEN
            O_R => R
        ELSE
            ALLOCATE(O_R(N), STAT=L_STAT_ALLOC); O_R = 1.0_WP
        ENDIF
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(RWORK(2*N), STAT=L_STAT_ALLOC)
    ENDIF
    IF(L_STAT_ALLOC==0) THEN
        ALLOCATE(WORK(2*N), STAT=L_STAT_ALLOC)
    ENDIF
    ! <<< Call lapack77 routine >>>
    IF(L_STAT_ALLOC==0) THEN
        CALL F77_GESVX(O_FACT,O_TRANS,N,NRHS,A,LDA,O_AF,LDAF,O_IPIV,    &
     &    O_EQUED,O_R,O_C,B,LDB,X,LDX,O_RCOND,R_FERR,R_BERR,WORK,RWORK, &
     &                                                           O_INFO)
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
    IF(PRESENT(RPVGRW)) THEN
        RPVGRW = RWORK(1)
    ENDIF
    ! <<< Deallocate local and work arrays >>>
    IF(.NOT. PRESENT(AF)) THEN
        DEALLOCATE(O_AF, STAT=L_STAT_DEALLOC)
    ENDIF
    IF(.NOT. PRESENT(C)) THEN
        DEALLOCATE(O_C, STAT=L_STAT_DEALLOC)
    ENDIF
    IF(.NOT. PRESENT(IPIV)) THEN
        DEALLOCATE(O_IPIV, STAT=L_STAT_DEALLOC)
    ENDIF
    IF(.NOT. PRESENT(R)) THEN
        DEALLOCATE(O_R, STAT=L_STAT_DEALLOC)
    ENDIF
    DEALLOCATE(RWORK, STAT=L_STAT_DEALLOC)
    DEALLOCATE(WORK, STAT=L_STAT_DEALLOC)
    ! <<< Error handler >>>
    IF(PRESENT(INFO)) THEN
        INFO = O_INFO
    ELSEIF(O_INFO <= -1000) THEN
        CALL F77_XERBLA(SRNAME,-O_INFO)
    ENDIF
END SUBROUTINE CGESVX1_F95