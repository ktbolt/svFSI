!
! Copyright (c) Stanford University, The Regents of the University of
!               California, and others.
!
! All Rights Reserved.
!
! See Copyright-SimVascular.txt for additional details.
!
! Permission is hereby granted, free of charge, to any person obtaining
! a copy of this software and associated documentation files (the
! "Software"), to deal in the Software without restriction, including
! without limitation the rights to use, copy, modify, merge, publish,
! distribute, sublicense, and/or sell copies of the Software, and to
! permit persons to whom the Software is furnished to do so, subject
! to the following conditions:
!
! The above copyright notice and this permission notice shall be included
! in all copies or substantial portions of the Software.
!
! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
! IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
! TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
! PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
! OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
! EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
! PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
! PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
! LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
! NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
! SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
!
!--------------------------------------------------------------------
!
!     Here, second Piola-Kirchhoff stress tensor and the material
!     stiffness tensors are computed for material constitutive models.
!
!--------------------------------------------------------------------

!     Compute 2nd Piola-Kirchhoff stress and material stiffness tensors
!     including both dilational and isochoric components
      SUBROUTINE GETPK2CC(stM, F, nfd, fl, S, CC)
      USE MATFUN
      USE COMMOD
      IMPLICIT NONE

      TYPE(stModelType), INTENT(IN) :: stM
      INTEGER, INTENT(IN) :: nfd
      REAL(KIND=8), INTENT(IN) :: F(nsd,nsd), fl(nsd,nfd)
      REAL(KIND=8), INTENT(OUT) :: S(nsd,nsd), CC(nsd,nsd,nsd,nsd)

      REAL(KIND=8) :: nd, Kp, J, J2d, trE, p, pl, Inv1, Inv2, Inv4,
     2   Inv6
      REAL(KIND=8) :: IDm(nsd,nsd), C(nsd,nsd), E(nsd,nsd), Ci(nsd,nsd),
     2   Sb(nsd,nsd), CCb(nsd,nsd,nsd,nsd), PP(nsd,nsd,nsd,nsd),
     3   Eff, Ess, kap, Hff(nsd,nsd), Hss(nsd,nsd), Sfs(nsd,nsd,6)
      REAL(KIND=8) :: r1, r2, g1, g2, g3

!     Some preliminaries
      nd   = REAL(nsd, KIND=8)
      Kp   = stM%Kpen
      J    = MAT_DET(F, nsd)
      J2d  = J**(-2D0/nd)

      IDm  = MAT_ID(nsd)
      C    = MATMUL(TRANSPOSE(F), F)
      E    = 5D-1 * (C - IDm)
      Ci   = MAT_INV(C, nsd)

      trE  = MAT_TRACE(E, nsd)
      Inv1 = J2d*MAT_TRACE(C,nsd)
      Inv2 = 5D-1*( Inv1*Inv1 - J2d*J2d*MAT_TRACE(MATMUL(C,C), nsd) )

!     Contribution of dilational penalty terms to S and CC
      p  = 0D0
      pl = 0D0
      IF (.NOT.ISZERO(Kp)) CALL GETSVOLP(stM, J, p, pl)

!     Now, compute isochoric and total stress, elasticity tensors
      SELECT CASE (stM%isoType)
      CASE (stIso_lin)
         g1 = stM%C10            ! mu
         S  = g1*Idm
         RETURN

!     St.Venant-Kirchhoff
      CASE (stIso_stVK)
         g1 = stM%C10            ! lambda
         g2 = stM%C01 * 2D0      ! 2*mu

         S  = g1*trE*IDm + g2*E
         CC = g1*TEN_DYADPROD(IDm, IDm, nsd) + g2*TEN_IDs(nsd)
         RETURN

!     modified St.Venant-Kirchhoff
      CASE (stIso_mStVK)
         g1 = stM%C10 ! kappa
         g2 = stM%C01 ! mu

         S  = g1*LOG(J)*Ci + g2*(C-IDm)
         CC = g1 * ( -2D0*LOG(J)*TEN_SYMMPROD(Ci, Ci, nsd) +
     2      TEN_DYADPROD(Ci, Ci, nsd) ) + 2D0*g2*TEN_IDs(nsd)
         RETURN

!     NeoHookean model
      CASE (stIso_nHook)
         g1 = 2D0 * stM%C10
         Sb = g1*IDm

         r1 = g1*Inv1/nd
         S  = J2d*Sb - r1*Ci
         CC = (-2D0/nd) * ( TEN_DYADPROD(Ci, S, nsd) +
     2                      TEN_DYADPROD(S, Ci, nsd))

         S  = S + p*J*Ci
         CC = CC + 2D0*(r1 - p*J) * TEN_SYMMPROD(Ci, Ci, nsd) +
     2         (pl*J - 2D0*r1/nd) * TEN_DYADPROD(Ci, Ci, nsd)
         RETURN

!     Mooney-Rivlin model
      CASE (stIso_MR)
         g1  = 2D0 * (stM%C10 + Inv1*stM%C01)
         g2  = -2D0 * stM%C01
         Sb  = g1*IDm + g2*J2d*C

         g1  = 4D0*J2d*J2d* stM%C01
         CCb = g1 * (TEN_DYADPROD(IDm, IDm, nsd) - TEN_IDs(nsd))

         r1  = J2d*MAT_DDOT(C, Sb, nsd) / nd
         S   = J2d*Sb - r1*Ci

         PP  = TEN_IDs(nsd) - (1D0/nd) * TEN_DYADPROD(Ci, C, nsd)
         CC  = TEN_DDOT(CCb, PP, nsd)
         CC  = TEN_TRANSPOSE(CC, nsd)
         CC  = TEN_DDOT(PP, CC, nsd)
         CC  = CC - (2D0/nd) * ( TEN_DYADPROD(Ci, S, nsd) +
     2                           TEN_DYADPROD(S, Ci, nsd) )

         S   = S + p*J*Ci
         CC  = CC + 2D0*(r1 - p*J) * TEN_SYMMPROD(Ci, Ci, nsd) +
     2          (pl*J - 2D0*r1/nd) * TEN_DYADPROD(Ci, Ci, nsd)
         RETURN

!     HGO (Holzapfel-Gasser-Ogden) model with additive splitting of
!     the anisotropic fiber-based strain-energy terms
      CASE (stIso_HGO)

         kap  = stM%kap
         Inv4 = J2d*NORM(fl(:,1), MATMUL(C, fl(:,1)))
         Inv6 = J2d*NORM(fl(:,2), MATMUL(C, fl(:,2)))

         Eff  = kap*Inv1 + (1.0D0-3.0D0*kap)*Inv4 - 1.0D0
         Ess  = kap*Inv1 + (1.0D0-3.0D0*kap)*Inv6 - 1.0D0

         Hff  = MAT_DYADPROD(fl(:,1), fl(:,1), nsd)
         Hff  = kap*IDm + (1.0D0-3.0D0*kap)*Hff
         Hss  = MAT_DYADPROD(fl(:,2), fl(:,2), nsd)
         Hss  = kap*IDm + (1.0D0-3.0D0*kap)*Hss

         g1   = stM%C10
         g2   = stM%aff * Eff * EXP(stM%bff*Eff*Eff)
         g3   = stM%ass * Ess * EXP(stM%bss*Ess*Ess)
         Sb   = 2D0*(g1*IDm + g2*Hff + g3*Hss)

         g1   = stM%aff*(1D0 + 2D0*stM%bff*Eff*Eff)*EXP(stM%bff*Eff*Eff)
         g2   = stM%ass*(1D0 + 2D0*stM%bss*Ess*Ess)*EXP(stM%bss*Ess*Ess)
         g1   = 4D0*J2d*J2d * g1
         g2   = 4D0*J2d*J2d * g2

         CCb  = g1 * TEN_DYADPROD(Hff, Hff, nsd) +
     2          g2 * TEN_DYADPROD(Hss, Hss, nsd)

         r1  = J2d*MAT_DDOT(C, Sb, nsd) / nd
         S   = J2d*Sb - r1*Ci

         PP  = TEN_IDs(nsd) - (1D0/nd) * TEN_DYADPROD(Ci, C, nsd)
         CC  = TEN_DDOT(CCb, PP, nsd)
         CC  = TEN_TRANSPOSE(CC, nsd)
         CC  = TEN_DDOT(PP, CC, nsd)
         CC  = CC - (2D0/nd) * ( TEN_DYADPROD(Ci, S, nsd) +
     2                           TEN_DYADPROD(S, Ci, nsd) )

         S   = S + p*J*Ci
         CC  = CC + 2D0*(r1 - p*J) * TEN_SYMMPROD(Ci, Ci, nsd) +
     2          (pl*J - 2D0*r1/nd) * TEN_DYADPROD(Ci, Ci, nsd)
         RETURN

!     Guccione (1995) transversely isotropic model
      CASE (stIso_Gucci)
!        Compute isochoric component of E
         E = 5D-1 * (J2d*C - Idm)

!        Transform into local orthogonal coordinate system
         Hff(1,:) = fl(:,1)
         Hff(2,:) = fl(:,2)
         Hff(3,:) = CROSS(fl)

!        Project E to local orthogocal coordinate system
         E = MATMUL(E, TRANSPOSE(Hff))
         E = MATMUL(Hff, E)

         g1 = stM%bff
         g2 = stM%bss
         g3 = stM%bfs

!        Fiber stiffness contribution to Siso := (dE*_MN / dE_IJ)
!        - Voigt notation
         Sfs(:,:,:) = 0D0
         Sfs(:,:,1) = MAT_DYADPROD(Hff(1,:), Hff(1,:), nsd)
         Sfs(:,:,2) = MAT_DYADPROD(Hff(2,:), Hff(2,:), nsd)
         Sfs(:,:,3) = MAT_DYADPROD(Hff(3,:), Hff(3,:), nsd)
         Sfs(:,:,4) = MAT_SYMMPROD(Hff(1,:), Hff(2,:), nsd)
         Sfs(:,:,5) = MAT_SYMMPROD(Hff(1,:), Hff(3,:), nsd)
         Sfs(:,:,6) = MAT_SYMMPROD(Hff(2,:), Hff(3,:), nsd)

         Eff = g1* E(1,1)*E(1,1) +
     2         g2*(E(2,2)*E(2,2) + E(3,3)*E(3,3)  +
     3             E(2,3)*E(2,3) + E(3,2)*E(3,2)) +
     4         g3*(E(1,2)*E(1,2) + E(2,1)*E(2,1)  +
     5             E(1,3)*E(1,3) + E(3,1)*E(3,1))

         Hss = g1*E(1,1)*Sfs(:,:,1)
         Hss = Hss + g2*(E(2,2)*Sfs(:,:,2) + E(3,3)*Sfs(:,:,3) +
     2      (E(2,3) + E(3,2))*Sfs(:,:,6))
         Hss = Hss + g3*( (E(1,2) + E(2,1))*Sfs(:,:,4) +
     2                    (E(1,3) + E(3,1))*Sfs(:,:,5) )

         r2  = stM%C10 * EXP(Eff)
         Sb  = r2 * Hss

         r1  = J2d*MAT_DDOT(C, Sb, nsd) / nd
         S   = J2d*Sb - r1*Ci

         r2  = r2*J2d*J2d
         CCb = r2 * ( 2D0*TEN_DYADPROD(Hss, Hss, nsd) +
     2      g1* TEN_DYADPROD(Sfs(:,:,1), Sfs(:,:,1), nsd)  +
     3      g2*(TEN_DYADPROD(Sfs(:,:,2), Sfs(:,:,2), nsd)  +
     4          TEN_DYADPROD(Sfs(:,:,3), Sfs(:,:,3), nsd)  +
     5      2D0*TEN_DYADPROD(Sfs(:,:,6), Sfs(:,:,6), nsd)) + 2D0*
     6      g3*(TEN_DYADPROD(Sfs(:,:,4), Sfs(:,:,4), nsd)  +
     7          TEN_DYADPROD(Sfs(:,:,5), Sfs(:,:,5), nsd)) )

         PP  = TEN_IDs(nsd) - (1D0/nd) * TEN_DYADPROD(Ci, C, nsd)
         CC  = TEN_DDOT(CCb, PP, nsd)
         CC  = TEN_TRANSPOSE(CC, nsd)
         CC  = TEN_DDOT(PP, CC, nsd)
         CC  = CC - (2D0/nd) * ( TEN_DYADPROD(Ci, S, nsd) +
     2                           TEN_DYADPROD(S, Ci, nsd) )

         S   = S + p*J*Ci
         CC  = CC + 2D0*(r1 - p*J) * TEN_SYMMPROD(Ci, Ci, nsd) +
     2          (pl*J - 2D0*r1/nd) * TEN_DYADPROD(Ci, Ci, nsd)

      CASE DEFAULT
         err = "Undefined material constitutive model"
      END SELECT

      RETURN
      END SUBROUTINE GETPK2CC
!====================================================================
      SUBROUTINE GETSVOLP(stM, J, p, pl)
      USE COMMOD
      IMPLICIT NONE

      TYPE(stModelType), INTENT(IN) :: stM
      REAL(KIND=8), INTENT(IN) :: J
      REAL(KIND=8), INTENT(INOUT) :: p, pl

      REAL(KIND=8) Kp

      Kp = stM%Kpen
      SELECT CASE (stM%volType)
      CASE (stVol_Quad)
         p  = Kp*(J-1D0)
         pl = Kp*(2D0*J-1D0)

      CASE (stVol_ST91)
         p  = 5D-1*Kp*(J-1D0/J)
         pl = Kp*J

      CASE (stVol_M94)
         p  = Kp*(1D0-1D0/J)
         pl = Kp

      END SELECT

      END SUBROUTINE GETSVOLP
!####################################################################
!     Compute isochoric (deviatoric) component of 2nd Piola-Kirchhoff
!     stress and material stiffness tensors
      SUBROUTINE GETPK2CCdev(stM, F, nfd, fl, S, CC)
      USE MATFUN
      USE COMMOD
      IMPLICIT NONE

      TYPE(stModelType), INTENT(IN) :: stM
      INTEGER, INTENT(IN) :: nfd
      REAL(KIND=8), INTENT(IN) :: F(nsd,nsd), fl(nsd,nfd)
      REAL(KIND=8), INTENT(OUT) :: S(nsd,nsd), CC(nsd,nsd,nsd,nsd)

      REAL(KIND=8) :: nd, J, J2d, trE, Inv1, Inv2, Inv4, Inv6,
     2   IDm(nsd,nsd), C(nsd,nsd), E(nsd,nsd), Ci(nsd,nsd), Sb(nsd,nsd),
     3   CCb(nsd,nsd,nsd,nsd), PP(nsd,nsd,nsd,nsd), kap, Eff, Ess,
     4   Hff(nsd,nsd), Hss(nsd,nsd), Sfs(nsd,nsd,6)
      REAL(KIND=8) :: r1, r2, g1, g2, g3

!     Some preliminaries
      nd   = REAL(nsd, KIND=8)
      J    = MAT_DET(F, nsd)
      J2d  = J**(-2D0/nd)

      IDm  = MAT_ID(nsd)
      C    = MATMUL(TRANSPOSE(F), F)
      E    = 5D-1 * (C - IDm)
      Ci   = MAT_INV(C, nsd)

      trE  = MAT_TRACE(E, nsd)
      Inv1 = J2d*MAT_TRACE(C,nsd)
      Inv2 = 5D-1*( Inv1*Inv1 - J2d*J2d*MAT_TRACE(MATMUL(C,C), nsd) )

!     Isochoric part of 2nd Piola-Kirchhoff and elasticity tensors
      SELECT CASE (stM%isoType)
!     NeoHookean model
      CASE (stIso_nHook)
         g1 = 2D0 * stM%C10
         Sb = g1*IDm

         r1 = g1*Inv1/nd
         S  = J2d*Sb - r1*Ci
         CC = 2D0*r1 * ( TEN_SYMMPROD(Ci, Ci, nsd) -
     2        1D0/nd *   TEN_DYADPROD(Ci, Ci, nsd) )
     3      - 2D0/nd * ( TEN_DYADPROD(Ci, S, nsd) +
     4                   TEN_DYADPROD(S, Ci, nsd) )
         RETURN

!     Mooney-Rivlin model
      CASE (stIso_MR)
         g1  = 2D0 * (stM%C10 + Inv1*stM%C01)
         g2  = -2D0 * stM%C01
         Sb  = g1*IDm + g2*J2d*C

         g1  = 4D0*J2d*J2d* stM%C01
         CCb = g1 * (TEN_DYADPROD(IDm, IDm, nsd) - TEN_IDs(nsd))

         r1  = J2d*MAT_DDOT(C, Sb, nsd) / nd
         S   = J2d*Sb - r1*Ci

         PP  = TEN_IDs(nsd) - (1D0/nd) * TEN_DYADPROD(Ci, C, nsd)
         CC  = TEN_DDOT(CCb, PP, nsd)
         CC  = TEN_TRANSPOSE(CC, nsd)
         CC  = TEN_DDOT(PP, CC, nsd)
         CC  = CC + 2D0*r1 * ( TEN_SYMMPROD(Ci, Ci, nsd) -
     2              1D0/nd *   TEN_DYADPROD(Ci, Ci, nsd) )
     3            - 2D0/nd * ( TEN_DYADPROD(Ci, S, nsd) +
     4                         TEN_DYADPROD(S, Ci, nsd) )
         RETURN

!     HGO (Holzapfel-Gasser-Ogden) model with additive splitting of
!     the anisotropic fiber-based strain-energy terms
      CASE (stIso_HGO)

         kap  = stM%kap
         Inv4 = J2d*NORM(fl(:,1), MATMUL(C, fl(:,1)))
         Inv6 = J2d*NORM(fl(:,2), MATMUL(C, fl(:,2)))

         Eff  = kap*Inv1 + (1.0D0-3.0D0*kap)*Inv4 - 1.0D0
         Ess  = kap*Inv1 + (1.0D0-3.0D0*kap)*Inv6 - 1.0D0

         Hff  = MAT_DYADPROD(fl(:,1), fl(:,1), nsd)
         Hff  = kap*IDm + (1.0D0-3.0D0*kap)*Hff
         Hss  = MAT_DYADPROD(fl(:,2), fl(:,2), nsd)
         Hss  = kap*IDm + (1.0D0-3.0D0*kap)*Hss

         g1   = stM%C10
         g2   = stM%aff * Eff * EXP(stM%bff*Eff*Eff)
         g3   = stM%ass * Ess * EXP(stM%bss*Ess*Ess)
         Sb   = 2D0*(g1*IDm + g2*Hff + g3*Hss)

         g1   = stM%aff*(1D0 + 2D0*stM%bff*Eff*Eff)*EXP(stM%bff*Eff*Eff)
         g2   = stM%ass*(1D0 + 2D0*stM%bss*Ess*Ess)*EXP(stM%bss*Ess*Ess)
         g1   = 4D0*J2d*J2d * g1
         g2   = 4D0*J2d*J2d * g2

         CCb  = g1 * TEN_DYADPROD(Hff, Hff, nsd) +
     2          g2 * TEN_DYADPROD(Hss, Hss, nsd)

         r1  = J2d*MAT_DDOT(C, Sb, nsd) / nd
         S   = J2d*Sb - r1*Ci

         PP  = TEN_IDs(nsd) - (1D0/nd) * TEN_DYADPROD(Ci, C, nsd)
         CC  = TEN_DDOT(CCb, PP, nsd)
         CC  = TEN_TRANSPOSE(CC, nsd)
         CC  = TEN_DDOT(PP, CC, nsd)
         CC  = CC + 2D0*r1 * ( TEN_SYMMPROD(Ci, Ci, nsd) -
     2              1D0/nd *   TEN_DYADPROD(Ci, Ci, nsd) )
     3            - 2D0/nd * ( TEN_DYADPROD(Ci, S, nsd) +
     4                         TEN_DYADPROD(S, Ci, nsd) )
         RETURN

!     Guccione (1995) transversely isotropic model
      CASE (stIso_Gucci)
!        Compute isochoric component of E
         E = 5D-1 * (J2d*C - Idm)

!        Transform into local orthogonal coordinate system
         Hff(1,:) = fl(:,1)
         Hff(2,:) = fl(:,2)
         Hff(3,:) = CROSS(fl)

!        Project E to local orthogocal coordinate system
         E = MATMUL(E, TRANSPOSE(Hff))
         E = MATMUL(Hff, E)

         g1 = stM%bff
         g2 = stM%bss
         g3 = stM%bfs

!        Fiber stiffness contribution to Siso := (dE*_MN / dE_IJ)
!        - Voigt notation
         Sfs(:,:,:) = 0D0
         Sfs(:,:,1) = MAT_DYADPROD(Hff(1,:), Hff(1,:), nsd)
         Sfs(:,:,2) = MAT_DYADPROD(Hff(2,:), Hff(2,:), nsd)
         Sfs(:,:,3) = MAT_DYADPROD(Hff(3,:), Hff(3,:), nsd)
         Sfs(:,:,4) = MAT_SYMMPROD(Hff(1,:), Hff(2,:), nsd)
         Sfs(:,:,5) = MAT_SYMMPROD(Hff(1,:), Hff(3,:), nsd)
         Sfs(:,:,6) = MAT_SYMMPROD(Hff(2,:), Hff(3,:), nsd)

         Eff = g1* E(1,1)*E(1,1) +
     2         g2*(E(2,2)*E(2,2) + E(3,3)*E(3,3)  +
     3             E(2,3)*E(2,3) + E(3,2)*E(3,2)) +
     4         g3*(E(1,2)*E(1,2) + E(2,1)*E(2,1)  +
     5             E(1,3)*E(1,3) + E(3,1)*E(3,1))

         Hss = g1*E(1,1)*Sfs(:,:,1)
         Hss = Hss + g2*(E(2,2)*Sfs(:,:,2) + E(3,3)*Sfs(:,:,3) +
     2      (E(2,3) + E(3,2))*Sfs(:,:,6))
         Hss = Hss + g3*( (E(1,2) + E(2,1))*Sfs(:,:,4) +
     2                    (E(1,3) + E(3,1))*Sfs(:,:,5) )

         r2  = stM%C10 * EXP(Eff)
         Sb  = r2 * Hss

         r1  = J2d*MAT_DDOT(C, Sb, nsd) / nd
         S   = J2d*Sb - r1*Ci

         r2  = r2*J2d*J2d
         CCb = r2 * ( 2D0*TEN_DYADPROD(Hss, Hss, nsd) +
     2      g1* TEN_DYADPROD(Sfs(:,:,1), Sfs(:,:,1), nsd)  +
     3      g2*(TEN_DYADPROD(Sfs(:,:,2), Sfs(:,:,2), nsd)  +
     4          TEN_DYADPROD(Sfs(:,:,3), Sfs(:,:,3), nsd)  +
     5      2D0*TEN_DYADPROD(Sfs(:,:,6), Sfs(:,:,6), nsd)) + 2D0*
     6      g3*(TEN_DYADPROD(Sfs(:,:,4), Sfs(:,:,4), nsd)  +
     7          TEN_DYADPROD(Sfs(:,:,5), Sfs(:,:,5), nsd)) )

         PP  = TEN_IDs(nsd) - (1D0/nd) * TEN_DYADPROD(Ci, C, nsd)
         CC  = TEN_DDOT(CCb, PP, nsd)
         CC  = TEN_TRANSPOSE(CC, nsd)
         CC  = TEN_DDOT(PP, CC, nsd)
         CC  = CC + 2D0*r1 * ( TEN_SYMMPROD(Ci, Ci, nsd) -
     2              1D0/nd *   TEN_DYADPROD(Ci, Ci, nsd) )
     3            - 2D0/nd * ( TEN_DYADPROD(Ci, S, nsd) +
     4                         TEN_DYADPROD(S, Ci, nsd) )

      CASE DEFAULT
         err = "Undefined isochoric material constitutive model"
      END SELECT

      RETURN
      END SUBROUTINE GETPK2CCdev
!====================================================================
!     Compute rho and beta depending on the Gibb's free-energy based
!     volumetric penalty model
      SUBROUTINE GVOLPEN(stM, p, ro, bt, dro, dbt)
      USE MATFUN
      USE COMMOD
      IMPLICIT NONE

      TYPE(stModelType), INTENT(IN) :: stM
      REAL(KIND=8), INTENT(IN) :: p
      REAL(KIND=8), INTENT(OUT) :: ro, bt, dro, dbt

      REAL(KIND=8) :: Kp, nu, r1, r2

      ro  = eq(cEq)%dmn(cDmn)%prop(solid_density)
      nu  = eq(cEq)%dmn(cDmn)%prop(poisson_ratio)
      bt  = 0D0
      dbt = 0D0
      dro = 0D0
      IF (ISZERO(nu-0.5D0)) RETURN

      Kp = stM%Kpen
      SELECT CASE (stM%volType)
      CASE (stVol_Quad)
         r1  = 1.0D0/(Kp - p)

         ro  = ro*Kp*r1
         bt  = r1
         dro = ro*r1
         dbt = r1*r1

      CASE (stVol_ST91)
         r1  = ro/Kp
         r2  = SQRT(p*p + Kp*Kp)

         ro  = r1*(p + r2)
         bt  = 1D0/r2
         dro = ro*bt
         dbt = -bt*p/(p*p + Kp*Kp)

      CASE (stVol_M94)
         r1  = ro/Kp
         r2  = Kp + p

         ro  = r1*r2
         bt  = 1D0/r2
         dro = r1
         dbt = -bt*bt

      CASE DEFAULT
         err = "Undefined volumetric material constitutive model"
      END SELECT

      RETURN
      END SUBROUTINE GVOLPEN
!====================================================================
!     Compute stabilization parameters tauM and tauC
      SUBROUTINE GETTAU(lDmn, Je, tauM, tauC)
      USE MATFUN
      USE COMMOD
      IMPLICIT NONE

      TYPE(dmnType), INTENT(IN) :: lDmn
      REAL(KIND=8), INTENT(IN)  :: Je
      REAL(KIND=8), INTENT(OUT) :: tauM, tauC

      REAL(KIND=8) :: ctM, ctC, he, rho, Em, nu, mu, lam, c

      he  = 5D-1 * Je**(1D0/REAL(nsd,KIND=8))
      rho = lDmn%prop(solid_density)
      Em  = lDmn%prop(elasticity_modulus)
      nu  = lDmn%prop(poisson_ratio)
      ctM = lDmn%prop(ctau_M)
      ctC = lDmn%prop(ctau_C)

      mu  = 5D-1*Em / (1.0D0 + nu)
      IF (ISZERO(nu-0.5D0)) THEN
         c = SQRT(mu / rho)
      ELSE
         lam = 2.0D0*mu*nu / (1.0D0-2.0D0*nu)
         c = SQRT((lam + 2.0D0*mu)/rho)
      END IF

      tauM = ctM * he / c / rho
      tauC = ctC * he * c * rho

      RETURN
      END SUBROUTINE GETTAU
!####################################################################
