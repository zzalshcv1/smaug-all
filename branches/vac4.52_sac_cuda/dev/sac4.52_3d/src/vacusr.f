!##############################################################################
! module vacusr - sim1 ! setvac -d=22 -g=204,204 -p=hdadiab -u=sim1


!==============================================================================
!
!    THE FOLLOWING SUBROUTINES ADD GRAVITATIONAL SOURCE TERMS, SET GRAVITY
!
!------------------------------------------------------------------------------
!    See vacusr.t.gravity and vacusrpar.t.gravity for an example of usage
!
!    Gravitational force is added to the momentum equation:
!
!    d m_i/dt += rho*eqpar(grav0_+i)
!
!    Gravitational work is added to the energy equation (if present):
!
!    de/dt += Sum_i m_i*eqpar(grav0_+i)
!
!    The eqpar(grav1_),eqpar(grav2_),... coefficients are the components of 
!    the gravitational acceleration in each dimension. Set them to 0 for no
!    gravity in that direction. 
!    The !!! comments show how a grav array could be used for a spatially
!    (and maybe temporally) varying gravitational field.
!    The setgrav subroutine has to be completed then.
!
!============================================================================
subroutine addsource_grav(qdt,ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,&
   ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws,qtC,w,qt,wnew)

! Add gravity source calculated from w to wnew within ixO for all variables 
! in iws. w is at time qtC, wnew is advanced from qt to qt+qdt.

include 'vacdef.f'

integer::          ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,ixOmin1,&
   ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws(niw_)
double precision:: qdt,qtC,qt,w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw),&
   wnew(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)
integer:: iiw,iw,idim
!!! ! For a spatially varying gravity define the common grav array
!!! double precision:: grav(ixG^T,ndim)
!!! common /gravity/ grav

!-----------------------------------------------------------------------------

!!! ! If grav needs to be calculated only once do it for the whole grid
!!! if(it==itmin)call setgrav(w,ixG^L,ixG^L,grav)
!!! ! Otherwise call setgrav in every time step
!!! call setgrav(w,ixI^L,ixO^L,grav)

! add sources from gravity
do iiw=1,iws(niw_); iw=iws(iiw)
   select case(iw)
   case(m1_,m2_,m3_)
      ! dm_i/dt= +rho*g_i
      idim=iw-m0_
      if(abs(eqpar(grav0_+idim))>smalldouble) wnew(ixOmin1:ixOmax1,&
         ixOmin2:ixOmax2,ixOmin3:ixOmax3,m0_+idim)=wnew(ixOmin1:ixOmax1,&
         ixOmin2:ixOmax2,ixOmin3:ixOmax3,m0_+idim)+ qdt*eqpar(grav0_&
         +idim)*(w(ixOmin1:ixOmax1,ixOmin2:ixOmax2,ixOmin3:ixOmax3,rho_))

!          wnew(ixO^S,m0_+idim)=wnew(ixO^S,m0_+idim)+ &
!              qdt*eqpar(grav0_+idim)*(w(ixO^S,rho_)+w(ixO^S,rhob_))

      !!! ! For a spatially varying gravity use instead of the above lines
      !!! wnew(ixO^S,m0_+idim)=wnew(ixO^S,m0_+idim)+ &
      !!!    qdt*grav(ixO^S,idim)*(w(ixO^S,rho_)+w(ixO^S,rhob_))

   case(e_)
      ! de/dt= +g_i*m_i
      do idim=1,ndim
         if(abs(eqpar(grav0_+idim))>smalldouble) wnew(ixOmin1:ixOmax1,&
            ixOmin2:ixOmax2,ixOmin3:ixOmax3,ee_)=wnew(ixOmin1:ixOmax1,&
            ixOmin2:ixOmax2,ixOmin3:ixOmax3,ee_)+ qdt*eqpar(grav0_&
            +idim)*w(ixOmin1:ixOmax1,ixOmin2:ixOmax2,ixOmin3:ixOmax3,rho_)&
            *w(ixOmin1:ixOmax1,ixOmin2:ixOmax2,ixOmin3:ixOmax3,m0_&
            +idim)/(w(ixOmin1:ixOmax1,ixOmin2:ixOmax2,ixOmin3:ixOmax3,rho_)&
            +w(ixOmin1:ixOmax1,ixOmin2:ixOmax2,ixOmin3:ixOmax3,rhob_))

!            wnew(ixO^S,ee_)=wnew(ixO^S,ee_)+ &
!               qdt*eqpar(grav0_+idim)*w(ixO^S,m0_+idim)

         !!! ! For a spatially varying gravity use instead of the above lines
         !!! wnew(ixO^S,ee_)=wnew(ixO^S,ee_)+ &
         !!!    qdt*grav(ixO^S,idim)*w(ixO^S,m0_+idim)

      end do
   end select ! iw
end do        ! iiw

return
end
!=============================================================================
!!! subroutine setgrav(w,ixI^L,ixO^L,grav)

! Set the gravitational acceleration within ixO based on x(ixI,ndim) 
! and/or w(ixI,nw)

!!! include 'vacdef.f'

!!! double precision:: w(ixG^T,nw),grav(ixG^T,ndim)
!!! integer:: ixI^L,ixO^L
!----------------------------------------------------------------------------
!!! return
!!! end
!=============================================================================

subroutine getdt_grav(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3)

include 'vacdef.f'

double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim
double precision:: dtgrav
save dtgrav

!!! ! For spatially varying gravity you need a common grav array
!!! double precision:: grav(ixG^T,ndim)
!!! common/gravity/grav

!----------------------------------------------------------------------------
write(*,*) '*** 1 getdt_grav',dt
oktest=index(teststr,'getdt')>=1

if(it==itmin)then
   ! If gravity is descibed by the equation parameters, use this:
   dtgrav=bigdouble
   do idim=1,ndim
      if(abs(eqpar(grav0_+idim))>zero)dtgrav=min(dtgrav,one&
         /sqrt(maxval(abs(eqpar(grav0_+idim))/dx(ixMmin1:ixMmax1,&
         ixMmin2:ixMmax2,ixMmin3:ixMmax3,1:ndim))))
   enddo
   !!! ! For spatially varying gravity use this instead of the lines above:
   !!! call setgrav(w,ixG^L,ixM^L,grav)
   !!! ! If gravity does not change with time, calculate dtgrav here:
   !!! dtgrav=one/sqrt(maxval(abs(grav(ixM^S,1:ndim))/dx(ixM^S,1:ndim)))
endif

!!! ! If gravity changes with time, calculate dtgrav here:
!!! dtgrav=one/sqrt(maxval(abs(grav(ixM^S,1:ndim))/dx(ixM^S,1:ndim)))



! limit the time step
dt=min(dt,dtgrav)
if(oktest)write(*,*)'Gravity limit for dt:',dtgrav
write(*,*) '*** 2 getdt_grav',dt
return
end

!=============================================================================

!==============================================================================
subroutine addsource_visc(qdt,ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,&
   ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws,qtC,w,qt,wnew)

! Add viscosity source to wnew within ixO 

include 'vacdef.f'

integer::          ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,ixOmin1,&
   ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws(niw_)
double precision:: qdt,qtC,qt,w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw),&
   wnew(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)

integer:: ix,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,idir,jdir,iiw,iw
double precision:: tmp2(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   nushk(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,ndim)


double precision:: tmprhoL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
    tmprhoR(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3), tmprhoC(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3)
double precision:: tmpVL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
    tmpVR(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3), tmpVC(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3)
double precision:: tmpBL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
    tmpBR(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3), tmpBC(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3)

double precision:: tmpL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   tmpR(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3), tmpC(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3)

double precision:: nuL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   nuR(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)

integer:: jxmin1,jxmin2,jxmin3,jxmax1,jxmax2,jxmax3,hxmin1,hxmin2,hxmin3,&
   hxmax1,hxmax2,hxmax3, hxOmin1,hxOmin2,hxOmin3,hxOmax1,hxOmax2,hxOmax3

double precision:: c_ene,c_shk

integer:: i,j,k,l,m,ii0,ii1,t00

double precision:: sB

!-----------------------------------------------------------------------------

! Calculating viscosity sources 
! involves second derivatives, two extra layers
call ensurebound(2,ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,ixOmin1,&
   ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,qtC,w)
ixmin1=ixOmin1-1;ixmin2=ixOmin2-1;ixmin3=ixOmin3-1;ixmax1=ixOmax1+1
ixmax2=ixOmax2+1;ixmax3=ixOmax3+1;
write(*,*) '*** 1 addsource_visc',dt
!sehr wichtig
!call setnushk(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,nushk)

 write(*,'(a,f10.8,a,f10.8,a)')'1 wnew(10,50,50,rho_)  ',wnew(10,50,50,rho_),' wnew(10,50,50,b3_) ',wnew(10,50,50,b3_),'  '  
call setnushk(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,nushk)

do idim=1,ndim
      tmp(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)=w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,rho_)
      call setnu(w,rho_,idim,ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,&
         nuR,nuL)      
      call gradient1L(tmp,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,tmp2)
      tmpL(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         =(nuL(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         +nushk(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,idim))&
         *tmp2(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)         
      call gradient1R(tmp,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,tmp2)
      tmpR(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         =(nuR(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         +nushk(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,idim))&
         *tmp2(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)
      wnew(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,rho_)&
         =wnew(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,rho_)&
         +(tmpR(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         -tmpL(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3))&
         /dx(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,idim)*qdt
enddo
   
!calcvisct1 part 1
do idim=1,ndim
      tmp(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)=w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,e_)-half*((w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,b1_)**2+w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,b2_)**2+w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,b3_)**2)+(w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,m1_)**2+w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,m2_)**2+w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,m3_)**2)/(w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,rho_)+w(ixImin1:ixImax1,&
         ixImin2:ixImax2,ixImin3:ixImax3,rhob_)))
      call setnu(w,173,idim,ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,&
         nuR,nuL)      
      call gradient1L(tmp,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,tmp2)
      tmpL(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         =(nuL(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         +nushk(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,idim))&
         *tmp2(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)      
      call gradient1R(tmp,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,tmp2)
      tmpR(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         =(nuR(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         +nushk(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,idim))&
         *tmp2(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)
      wnew(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,e_)&
         =wnew(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,e_)&
         +(tmpR(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3)&
         -tmpL(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3))&
         /dx(ixImin1:ixImax1,ixImin2:ixImax2,ixImin3:ixImax3,idim)*qdt
enddo




















write(*,'(a,f10.3,a,f10.3,a)')'qdt  ',qdt,' qt ',qt,'  '
write(*,'(a,f10.8,a,f10.8,a)')'2 wnew(10,50,50,rho_)  ',wnew(10,50,50,rho_),' wnew(10,50,50,b3_) ',wnew(10,50,50,b3_),'  '
write(*,*) '*** 2 addsource_visc',dt

return
end

!=============================================================================
subroutine setnu(w,iw,idim,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,nuR,nuL)

! Set the viscosity coefficient nu within ixO based on w(ixI). 

include 'vacdef.f'

double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)
double precision:: d1R(ixGlo1:ixGhi1+1,ixGlo2:ixGhi2+1,ixGlo3:ixGhi3&
   +1),d1L(ixGlo1:ixGhi1+1,ixGlo2:ixGhi2+1,ixGlo3:ixGhi3+1)
double precision:: d3R(ixGlo1:ixGhi1+1,ixGlo2:ixGhi2+1,ixGlo3:ixGhi3&
   +1),d3L(ixGlo1:ixGhi1+1,ixGlo2:ixGhi2+1,ixGlo3:ixGhi3+1)
double precision:: md3R(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   md3L(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
double precision:: md1R(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   md1L(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
double precision:: nuR(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   nuL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)

double precision:: c_tot, c_hyp,cmax(ixGlo1:ixGhi1,ixGlo2:ixGhi2,&
   ixGlo3:ixGhi3), tmp_nu(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim, iw
integer:: kxmin1,kxmin2,kxmin3,kxmax1,kxmax2,kxmax3,jxmin1,jxmin2,jxmin3,&
   jxmax1,jxmax2,jxmax3,hxmin1,hxmin2,hxmin3,hxmax1,hxmax2,hxmax3,gxmin1,&
   gxmin2,gxmin3,gxmax1,gxmax2,gxmax3,ixFFmin1,ixFFmin2,ixFFmin3,ixFFmax1,&
   ixFFmax2,ixFFmax3,jxFFmin1,jxFFmin2,jxFFmin3,jxFFmax1,jxFFmax2,jxFFmax3,&
   hxFFmin1,hxFFmin2,hxFFmin3,hxFFmax1,hxFFmax2,hxFFmax3
integer:: ix_1,ix_2,ix_3

integer:: ixFlo1,ixFlo2,ixFlo3,ixFhi1,ixFhi2,ixFhi3,ixFmin1,ixFmin2,ixFmin3,&
   ixFmax1,ixFmax2,ixFmax3,ixYlo1,ixYlo2,ixYlo3,ixYhi1,ixYhi2,ixYhi3

logical:: new_cmax

double precision:: tmp_nuI(ixGlo1:ixGhi1+2,ixGlo2:ixGhi2+2,ixGlo3:ixGhi3+2)

integer:: k,iwc

integer:: ix,ixe



!----------------------------------------------------------------------------

new_cmax=.true.

call getcmax(new_cmax,w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,cmax)
c_tot=maxval(cmax(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3))





c_hyp=0.4d0 ! 1.4d0 ! 0.6

!if (iw.eq.b^D_|.or.) c_hyp=0.02d0

!if (iw .eq. rho_) c_hyp=0.045d0

!if (iw .eq. 173) c_hyp=0.02d0



if (iw.eq.b1_.or.iw.eq.b2_.or.iw.eq.b3_) c_hyp=0.02d0

if (iw .eq. rho_) c_hyp=0.02d0

if (iw .eq. 173) c_hyp=0.02d0

        
if (iw .ne. 173) then     
        tmp_nu(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)=w(ixGlo1:ixGhi1,&
           ixGlo2:ixGhi2,ixGlo3:ixGhi3,iw)
        if (iw.eq.m1_.or.iw.eq.m2_.or.iw.eq.m3_) tmp_nu(ixGlo1:ixGhi1,&
           ixGlo2:ixGhi2,ixGlo3:ixGhi3)=w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,&
           ixGlo3:ixGhi3,iw)/(w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,&
           rho_)+w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,rhob_))
endif

if (iw .eq. 173) tmp_nu(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)&
   =w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,e_)-half*((w(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3,b1_)**2+w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,&
   ixGlo3:ixGhi3,b2_)**2+w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,b3_)**2)&
   +(w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,m1_)**2+w(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3,m2_)**2+w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,&
   ixGlo3:ixGhi3,m3_)**2)/(w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,rho_)&
   +w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,rhob_)))


ixYlo1=ixmin1-2;ixYlo2=ixmin2-2;ixYlo3=ixmin3-2;ixYhi1=ixmax1+2
ixYhi2=ixmax2+2;ixYhi3=ixmax3+2;

ixFlo1=ixYlo1+1;ixFlo2=ixYlo2+1;ixFlo3=ixYlo3+1;ixFhi1=ixYhi1+1
ixFhi2=ixYhi2+1;ixFhi3=ixYhi3+1;

tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFlo3:ixFhi3)=tmp_nu(ixYlo1:ixYhi1,&
   ixYlo2:ixYhi2,ixYlo3:ixYhi3)





if (iw .eq. 173) then 
   iwc=e_ 
else 
   iwc=iw
endif

do k=0,1  !left-right bc

if (typeB(iwc,2*idim-1+k) .ne. 'mpi') then
      if (upperB(2*idim-1+k)) then

          select case(idim)
               case(1)
                     tmp_nuI(ixFhi1+1,ixFlo2:ixFhi2,ixFlo3:ixFhi3)&
                        =tmp_nuI(ixFhi1-5,ixFlo2:ixFhi2,ixFlo3:ixFhi3)
                case(2)
                     tmp_nuI(ixFlo1:ixFhi1,ixFhi2+1,ixFlo3:ixFhi3)&
                        =tmp_nuI(ixFlo1:ixFhi1,ixFhi2-5,ixFlo3:ixFhi3)
                case(3)
                     tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFhi3&
                        +1)=tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFhi3-5)
            
          end select

      else

          select case(idim)
               case(1)
                     tmp_nuI(ixFlo1-1,ixFlo2:ixFhi2,ixFlo3:ixFhi3)&
                        =tmp_nuI(ixFlo1+5,ixFlo2:ixFhi2,ixFlo3:ixFhi3)
                case(2)
                     tmp_nuI(ixFlo1:ixFhi1,ixFlo2-1,ixFlo3:ixFhi3)&
                        =tmp_nuI(ixFlo1:ixFhi1,ixFlo2+5,ixFlo3:ixFhi3)
                case(3)
                     tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFlo3&
                        -1)=tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFlo3+5)
            
          end select

      endif
endif

enddo 

        ixFmin1=ixFlo1+1;ixFmin2=ixFlo2+1;ixFmin3=ixFlo3+1;ixFmax1=ixFhi1-1
        ixFmax2=ixFhi2-1;ixFmax3=ixFhi3-1; 

        kxmin1=ixFmin1+2*kr(idim,1);kxmin2=ixFmin2+2*kr(idim,2)
        kxmin3=ixFmin3+2*kr(idim,3);kxmax1=ixFmax1+2*kr(idim,1)
        kxmax2=ixFmax2+2*kr(idim,2);kxmax3=ixFmax3+2*kr(idim,3); !5:66
        jxmin1=ixFmin1+kr(idim,1);jxmin2=ixFmin2+kr(idim,2)
        jxmin3=ixFmin3+kr(idim,3);jxmax1=ixFmax1+kr(idim,1)
        jxmax2=ixFmax2+kr(idim,2);jxmax3=ixFmax3+kr(idim,3); !4:65
        hxmin1=ixFmin1-kr(idim,1);hxmin2=ixFmin2-kr(idim,2)
        hxmin3=ixFmin3-kr(idim,3);hxmax1=ixFmax1-kr(idim,1)
        hxmax2=ixFmax2-kr(idim,2);hxmax3=ixFmax3-kr(idim,3); !2:63
        gxmin1=ixFmin1-2*kr(idim,1);gxmin2=ixFmin2-2*kr(idim,2)
        gxmin3=ixFmin3-2*kr(idim,3);gxmax1=ixFmax1-2*kr(idim,1)
        gxmax2=ixFmax2-2*kr(idim,2);gxmax3=ixFmax3-2*kr(idim,3); !1:62

        ixFFmin1=ixFlo1;ixFFmin2=ixFlo2;ixFFmin3=ixFlo3;ixFFmax1=ixFhi1
        ixFFmax2=ixFhi2;ixFFmax3=ixFhi3; !2:65
        jxFFmin1=ixFlo1+kr(idim,1);jxFFmin2=ixFlo2+kr(idim,2)
        jxFFmin3=ixFlo3+kr(idim,3);jxFFmax1=ixFhi1+kr(idim,1)
        jxFFmax2=ixFhi2+kr(idim,2);jxFFmax3=ixFhi3+kr(idim,3); !3:66
        hxFFmin1=ixFlo1-kr(idim,1);hxFFmin2=ixFlo2-kr(idim,2)
        hxFFmin3=ixFlo3-kr(idim,3);hxFFmax1=ixFhi1-kr(idim,1)
        hxFFmax2=ixFhi2-kr(idim,2);hxFFmax3=ixFhi3-kr(idim,3); !1:64
        
        d3R(ixFmin1:ixFmax1,ixFmin2:ixFmax2,ixFmin3:ixFmax3)=abs(3.d0&
           *(tmp_nuI(jxmin1:jxmax1,jxmin2:jxmax2,jxmin3:jxmax3)&
           -tmp_nuI(ixFmin1:ixFmax1,ixFmin2:ixFmax2,ixFmin3:ixFmax3))&
           -(tmp_nuI(kxmin1:kxmax1,kxmin2:kxmax2,kxmin3:kxmax3)&
           -tmp_nuI(hxmin1:hxmax1,hxmin2:hxmax2,hxmin3:hxmax3))) !3:64
        d1R(ixFFmin1:ixFFmax1,ixFFmin2:ixFFmax2,ixFFmin3:ixFFmax3)&
           =abs(tmp_nuI(jxFFmin1:jxFFmax1,jxFFmin2:jxFFmax2,&
           jxFFmin3:jxFFmax3)-tmp_nuI(ixFFmin1:ixFFmax1,ixFFmin2:ixFFmax2,&
           ixFFmin3:ixFFmax3)) !2:65

        do ix_1=ixmin1,ixmax1
        do ix_2=ixmin2,ixmax2
        do ix_3=ixmin3,ixmax3    !3:62  +1=4:63

          md3R(ix_1,ix_2,ix_3)=maxval(d3R(ix_1+1-kr(idim,1):ix_1+1&
             +kr(idim,1),ix_2+1-kr(idim,2):ix_2+1+kr(idim,2),ix_3&
             +1-kr(idim,3):ix_3+1+kr(idim,3)))
          md1R(ix_1,ix_2,ix_3)=maxval(d1R(ix_1+1-2*kr(idim,1):ix_1+1&
             +2*kr(idim,1),ix_2+1-2*kr(idim,2):ix_2+1+2*kr(idim,2),ix_3&
             +1-2*kr(idim,3):ix_3+1+2*kr(idim,3)))

        enddo
        enddo
        enddo

        WHERE (md1R(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3).gt.0.d0)
          nuR(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=c_tot*c_hyp&
             *md3R(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)&
             /md1R(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)&
             *dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)
        ELSEWHERE 
          nuR(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
        END WHERE
        
        maxviscoef=max(maxval(nuR(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)),&
            maxviscoef)


!************

        d3L(ixFmin1:ixFmax1,ixFmin2:ixFmax2,ixFmin3:ixFmax3)=abs(3.d0&
           *(tmp_nuI(ixFmin1:ixFmax1,ixFmin2:ixFmax2,ixFmin3:ixFmax3)&
           -tmp_nuI(hxmin1:hxmax1,hxmin2:hxmax2,hxmin3:hxmax3))&
           -(tmp_nuI(jxmin1:jxmax1,jxmin2:jxmax2,jxmin3:jxmax3)&
           -tmp_nuI(gxmin1:gxmax1,gxmin2:gxmax2,gxmin3:gxmax3)))
        d1L(ixFFmin1:ixFFmax1,ixFFmin2:ixFFmax2,ixFFmin3:ixFFmax3)&
           =abs(tmp_nuI(ixFFmin1:ixFFmax1,ixFFmin2:ixFFmax2,&
           ixFFmin3:ixFFmax3)-tmp_nuI(hxFFmin1:hxFFmax1,hxFFmin2:hxFFmax2,&
           hxFFmin3:hxFFmax3))    

        do ix_1=ixmin1,ixmax1
        do ix_2=ixmin2,ixmax2
        do ix_3=ixmin3,ixmax3

          md3L(ix_1,ix_2,ix_3)=maxval(d3L(ix_1+1-kr(idim,1):ix_1+1&
             +kr(idim,1),ix_2+1-kr(idim,2):ix_2+1+kr(idim,2),ix_3&
             +1-kr(idim,3):ix_3+1+kr(idim,3)))
          md1L(ix_1,ix_2,ix_3)=maxval(d1L(ix_1+1-2*kr(idim,1):ix_1+1&
             +2*kr(idim,1),ix_2+1-2*kr(idim,2):ix_2+1+2*kr(idim,2),ix_3&
             +1-2*kr(idim,3):ix_3+1+2*kr(idim,3)))

        enddo
        enddo
        enddo

        WHERE (md1L(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3).gt.0.d0)
          nuL(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=c_tot*c_hyp&
             *md3L(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)&
             /md1L(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)&
             *dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)
        ELSEWHERE 
          nuL(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0  
        END WHERE

        maxviscoef=max(maxval(nuL(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)),&
            maxviscoef)

        

return
end


!=============================================================================
!=============================================================================
subroutine setnushk(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,nushk)

include 'vacdef.f'

double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw),&
   tmp2(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),nushk(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3,ndim)
double precision:: c_shk

double precision:: tmp3(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)

integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim, iw,i

integer:: ix_1,ix_2

do idim=1,ndim
nushk(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)=0.d0
enddo


go to 100
c_shk=0.5d0

tmp3(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0

!**************************BEGIN shock viscosity*******************************
      do idim=1,ndim
         tmp(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=w(ixmin1:ixmax1,&
            ixmin2:ixmax2,ixmin3:ixmax3,m0_+idim)/(w(ixmin1:ixmax1,&
            ixmin2:ixmax2,ixmin3:ixmax3,rho_)+w(ixmin1:ixmax1,ixmin2:ixmax2,&
            ixmin3:ixmax3,rhob_))
         call gradient1(tmp,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,&
            tmp2)
         tmp3(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=tmp3(ixmin1:ixmax1,&
            ixmin2:ixmax2,ixmin3:ixmax3)+tmp2(ixmin1:ixmax1,ixmin2:ixmax2,&
            ixmin3:ixmax3)
       enddo
      do idim=1,ndim
        nushk(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)&
           =tmp3(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)&
           *(dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)**2.d0)*c_shk
        WHERE (tmp3(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3) .ge. 0.d0)
!	  nushk(ix^S,idim)=0.d0
        END WHERE
        nushk(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)&
           =abs(nushk(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim))
      enddo
!****************************END shock viscosity*******************************

100 continue


return
end



!=============================================================================
subroutine getdt_visc(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3)

! Check diffusion time limit for dt < dtdiffpar * dx**2 / (nu/rho)

! Based on Hirsch volume 2, p.631, eq.23.2.17

include 'vacdef.f'

double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw),dtdiff_visc
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim, ix_1,ix_2

integer:: aa

! For spatially varying nu you need a common nu array
 double precision::tmpdt(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
     nuL(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),nuR(ixGlo1:ixGhi1,&
    ixGlo2:ixGhi2,ixGlo3:ixGhi3), nushk(ixGlo1:ixGhi1,ixGlo2:ixGhi2,&
    ixGlo3:ixGhi3,ndim)
 common/visc/nuL
 common/visc/nuR
!-----------------------------------------------------------------------------
write(*,*) '*** 1 getdt_visc',dt
 call setnushk(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,nushk)

 dtdiffpar=0.25d0

 do idim=1,ndim
   tmpdt(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=(maxviscoef&
      +nushk(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)) !/(w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,rho_)+w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,rhob_))   ! 1/dt
   dtdiff_visc=dtdiffpar/maxval(tmpdt(ixmin1:ixmax1,ixmin2:ixmax2,&
      ixmin3:ixmax3)/(dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)**2))
   
   dt=min(dt,dtdiff_visc)
 end do
write(*,*) '*** 2 getdt_visc',dt 
 maxviscoef=0.d0

return
end


!***** 2-point central finite difference gradient******

subroutine gradient1(q,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,gradq)
include 'vacdef.f'
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim
double precision:: q(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   gradq(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
integer:: hxmin1,hxmin2,hxmin3,hxmax1,hxmax2,hxmax3,kxmin1,kxmin2,kxmin3,&
   kxmax1,kxmax2,kxmax3
integer:: minx11,minx12,minx13,maxx11,maxx12,maxx13,k
!-----------------------------------------------------------------------------

hxmin1=ixmin1-kr(idim,1);hxmin2=ixmin2-kr(idim,2);hxmin3=ixmin3-kr(idim,3)
hxmax1=ixmax1-kr(idim,1);hxmax2=ixmax2-kr(idim,2);hxmax3=ixmax3-kr(idim,3);
kxmin1=ixmin1+kr(idim,1);kxmin2=ixmin2+kr(idim,2);kxmin3=ixmin3+kr(idim,3)
kxmax1=ixmax1+kr(idim,1);kxmax2=ixmax2+kr(idim,2);kxmax3=ixmax3+kr(idim,3);
gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=(q(kxmin1:kxmax1,&
   kxmin2:kxmax2,kxmin3:kxmax3)-q(hxmin1:hxmax1,hxmin2:hxmax2,hxmin3:hxmax3))&
   /dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)/two

 minx11=ixmin1+kr(idim,1);minx12=ixmin2+kr(idim,2);minx13=ixmin3+kr(idim,3);
 maxx11=ixmax1-kr(idim,1);maxx12=ixmax2-kr(idim,2);maxx13=ixmax3-kr(idim,3);
 
 do k=0,1  !left-right bc
 
 if (typeB(1,2*idim-1+k) .ne. 'mpi') then
 if (upperB(2*idim-1+k)) then
 select case(idim)
    case(1)
 gradq(ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
 gradq(maxx11,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
    case(2)
 gradq(ixmin1:ixmax1,ixmax2,ixmin3:ixmax3)=0.d0
 gradq(ixmin1:ixmax1,maxx12,ixmin3:ixmax3)=0.d0
    case(3)
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmax3)=0.d0
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,maxx13)=0.d0

 end select
 else
 select case(idim)
    case(1)
 gradq(ixmin1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
 gradq(minx11,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
    case(2)
 gradq(ixmin1:ixmax1,ixmin2,ixmin3:ixmax3)=0.d0
 gradq(ixmin1:ixmax1,minx12,ixmin3:ixmax3)=0.d0
    case(3)
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3)=0.d0
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,minx13)=0.d0

 end select
 endif
 endif
 enddo


return
end

!=============================================================================


!*****left upwind forward 2-point non-central finite difference gradient******

subroutine gradient1L(q,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,gradq)
include 'vacdef.f'
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim
double precision:: q(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   gradq(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
integer:: hxmin1,hxmin2,hxmin3,hxmax1,hxmax2,hxmax3
integer:: minx11,minx12,minx13,maxx11,maxx12,maxx13,k
!-----------------------------------------------------------------------------

hxmin1=ixmin1-kr(idim,1);hxmin2=ixmin2-kr(idim,2);hxmin3=ixmin3-kr(idim,3)
hxmax1=ixmax1-kr(idim,1);hxmax2=ixmax2-kr(idim,2);hxmax3=ixmax3-kr(idim,3);
gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=(q(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3)-q(hxmin1:hxmax1,hxmin2:hxmax2,hxmin3:hxmax3))&
   /dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)

 minx11=ixmin1+kr(idim,1);minx12=ixmin2+kr(idim,2);minx13=ixmin3+kr(idim,3);
 maxx11=ixmax1-kr(idim,1);maxx12=ixmax2-kr(idim,2);maxx13=ixmax3-kr(idim,3);
 
 do k=0,1  !left-right bc
 
 if (typeB(1,2*idim-1+k) .ne. 'mpi') then
 if (upperB(2*idim-1+k)) then
 select case(idim)
    case(1)
 gradq(ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
 gradq(maxx11,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
    case(2)
 gradq(ixmin1:ixmax1,ixmax2,ixmin3:ixmax3)=0.d0
 gradq(ixmin1:ixmax1,maxx12,ixmin3:ixmax3)=0.d0
    case(3)
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmax3)=0.d0
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,maxx13)=0.d0

 end select
 else
 select case(idim)
    case(1)
 gradq(ixmin1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
 gradq(minx11,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
    case(2)
 gradq(ixmin1:ixmax1,ixmin2,ixmin3:ixmax3)=0.d0
 gradq(ixmin1:ixmax1,minx12,ixmin3:ixmax3)=0.d0
    case(3)
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3)=0.d0
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,minx13)=0.d0

 end select
 endif
 endif
 enddo


return
end

!=============================================================================

!*****right upwind forward 2-point non-central finite difference gradient*****

subroutine gradient1R(q,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim,gradq)
include 'vacdef.f'
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idim
double precision:: q(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   gradq(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
integer:: hxmin1,hxmin2,hxmin3,hxmax1,hxmax2,hxmax3
integer:: minx11,minx12,minx13,maxx11,maxx12,maxx13,k
!-----------------------------------------------------------------------------

hxmin1=ixmin1+kr(idim,1);hxmin2=ixmin2+kr(idim,2);hxmin3=ixmin3+kr(idim,3)
hxmax1=ixmax1+kr(idim,1);hxmax2=ixmax2+kr(idim,2);hxmax3=ixmax3+kr(idim,3);
gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=(q(hxmin1:hxmax1,&
   hxmin2:hxmax2,hxmin3:hxmax3)-q(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3))&
   /dx(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,idim)

 minx11=ixmin1+kr(idim,1);minx12=ixmin2+kr(idim,2);minx13=ixmin3+kr(idim,3);
 maxx11=ixmax1-kr(idim,1);maxx12=ixmax2-kr(idim,2);maxx13=ixmax3-kr(idim,3);
 
 do k=0,1  !left-right bc
 
 if (typeB(1,2*idim-1+k) .ne. 'mpi') then
 if (upperB(2*idim-1+k)) then
 select case(idim)
    case(1)
 gradq(ixmax1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
 gradq(maxx11,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
    case(2)
 gradq(ixmin1:ixmax1,ixmax2,ixmin3:ixmax3)=0.d0
 gradq(ixmin1:ixmax1,maxx12,ixmin3:ixmax3)=0.d0
    case(3)
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmax3)=0.d0
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,maxx13)=0.d0

 end select
 else
 select case(idim)
    case(1)
 gradq(ixmin1,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
 gradq(minx11,ixmin2:ixmax2,ixmin3:ixmax3)=0.d0
    case(2)
 gradq(ixmin1:ixmax1,ixmin2,ixmin3:ixmax3)=0.d0
 gradq(ixmin1:ixmax1,minx12,ixmin3:ixmax3)=0.d0
    case(3)
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3)=0.d0
 gradq(ixmin1:ixmax1,ixmin2:ixmax2,minx13)=0.d0

 end select
 endif
 endif
 enddo


return
end

!=============================================================================
subroutine specialini(ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,w)

include 'vacdef.f'

integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3
double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,1:nw)

double precision:: rhoin,xcent1,xcent2,xcent3,radius
double precision:: inirho,iniene
double precision:: onemor,inix,ddx
double precision:: p_1,p_2

integer:: iii_,iix_1,info,i,j
double precision:: pi,comi,eneu,sum,mode,bmax,l
character*79 atmfilename

integer:: ix_1,ix_2,ix_3

double precision:: ixc_1,ixc_2,ixc_3
double precision:: rfc,a1,a2,a3


double precision:: xc2,xc3,x_max,r_0,r_tr,b_Amp,delta,r2,r3,r_tec

!-----------------------------------------------------------------------------




write(*,*)'param load'
read(unitpar,'(a)') atmfilename
write(*,*) 'from file ',atmfilename
open(33,file=atmfilename,status='old')


!iniene=731191.34d0*8.31e3*(1.1806882e-11)/0.6d0/(eqpar(gamma_)-1.0)

!iniene=731191.34d0*8.31e3*(1.1790001e-11)/0.6d0/(eqpar(gamma_)-1.0)

! 1.6Mm

iniene=6840.d0*8.31e3*(2.3409724e-09)/0.6d0/(eqpar(gamma_)-1.0)

!iniene=6840.d0*8.31e3*(2.2139002e-09)/0.6d0/(eqpar(gamma_)-1.0)

!iniene=731191.34d0*8.31e3*(4.5335481e-12)/0.6d0/(eqpar(gamma_)-1.0)






do ix_1=ixGhi1,ixGlo1,-1
 read(33,*) inix,inirho

 print*,'inix,inirho=',inix,inirho






 do ix_2=ixGlo2,ixGhi2
  do ix_3=ixGlo3,ixGhi3 

   x(ix_1,ix_2,ix_3,1)=inix !*1000.d0
   w(ix_1,ix_2,ix_3,rho_)=inirho
   w(ix_1,ix_2,ix_3,e_)=iniene
   w(ix_1,ix_2,ix_3,m1_)=0.0
   w(ix_1,ix_2,ix_3,m2_)=0.0
   w(ix_1,ix_2,ix_3,m3_)=0.0   

  enddo
 enddo

enddo
 close(33)

 print*,'grav=',eqpar(grav0_),eqpar(grav1_),eqpar(grav2_),eqpar(grav3_)



call primitive(ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,w)

do ix_3=ixGlo3,ixGhi3
 do ix_2=ixGlo2,ixGhi2
  do ix_1=ixGhi1-1,ixGlo1,-1 

comi=-abs(x(ix_1+1,ix_2,ix_3,1)-x(ix_1,ix_2,ix_3,1))

w(ix_1,ix_2,ix_3,p_)=w(ix_1+1,ix_2,ix_3,p_)+w(ix_1,ix_2,ix_3,rho_)*comi*1.d0&
   *eqpar(grav1_)



  enddo
 enddo
enddo

!goto 200
do ix_3=ixGlo3,ixGhi3
 do ix_2=ixGlo2,ixGhi2
  do ix_1=ixGlo1+2,ixGhi1-2
       
       w(ix_1,ix_2,ix_3,rho_)=-(1.D0/eqpar(grav1_))*(1.D0/(12.D0*(x(ix_1&
          +1,ix_2,ix_3,1)-x(ix_1,ix_2,ix_3,1))))*(w(ix_1+2,ix_2,ix_3,p_)&
          -8.D0*w(ix_1+1,ix_2,ix_3,p_)+8.D0*w(ix_1-1,ix_2,ix_3,&
          p_)                         -w(ix_1-2,ix_2,ix_3,p_))
               


     enddo
   enddo
 enddo  



!lower boundary
do ix_1=ixmin1+4,ixmin1+2,-1
  do ix_2=ixmin2,ixmax2
    do ix_3=ixmin3,ixmax3
        p_1=w(ix_1+2,ix_2,ix_3,p_)-8.d0*w(ix_1+1,ix_2,ix_3,p_)&
           +8.d0*w(ix_1-1,ix_2,ix_3,p_)
        p_2=w(ix_1,ix_2,ix_3,rho_)*eqpar(grav1_)
        w(ix_1-2,ix_2,ix_3,p_) = 12.d0*(x(ix_1,ix_2,ix_3,1)-x(ix_1&
           -1,ix_2,ix_3,1))*p_2+p_1

!         p_1=w(ix_1+2,ix_2,p_)-8.d0*w(ix_1+1,ix_2,p_)+8.d0*w(ix_1-1,ix_2,p_)
!         p_2=w(ix_1,ix_2,rho_)*eqpar(grav1_)
!         w(ix_1-2,ix_2,p_) = 12.d0*(x(ix_1,ix_2,1)-x(ix_1-1,ix_2,1))*p_2+p_1

       enddo
    enddo
 enddo


!upper boundary
do ix_1=ixmax1-4,ixmax1-2
   do ix_2=ixmin2,ixmax2
      do ix_3=ixmin3,ixmax3
         
          p_1=w(ix_1-2,ix_2,ix_3,p_)-8.d0*w(ix_1-1,ix_2,ix_3,p_)+8.d0*w(ix_1&
             +1,ix_2,ix_3,p_)
          p_2=w(ix_1,ix_2,ix_3,rho_)*eqpar(grav1_)
          w(ix_1+2,ix_2,ix_3,p_) = -12.d0*(x(ix_1,ix_2,ix_3,1)-x(ix_1&
             -1,ix_2,ix_3,1))*p_2+p_1

!           p_1=w(ix_1-2,ix_2,p_)-8.d0*w(ix_1-1,ix_2,p_)+8.d0*w(ix_1+1,ix_2,p_)
!           p_2=w(ix_1,ix_2,rho_)*eqpar(grav1_)
!           w(ix_1+2,ix_2,p_) = -12.d0*(x(ix_1,ix_2,1)-x(ix_1-1,ix_2,1))*p_2+p_1

      enddo
   enddo
enddo


200 continue



call conserve(ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,w)

w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,eb_)=w(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3,e_)
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,e_)=0.d0

w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,rhob_)=w(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3,rho_)
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,rho_)=0.d0


w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,b1_)=0.d0  !0.4d0



goto 90
! ********************** smooth  tube Bz magnetic field***************
xc2=4.d6  !m
xc3=4.d6  !m

x_max=8.d6 !m

r_0=1.d6
r_tr=0.2d6

b_Amp=0.08d0
delta=400.d0
 
do ix_1=ixGlo1,ixGhi1
 do ix_2=ixGlo2,ixGhi2
  do ix_3=ixGlo3,ixGhi3

    
     
     r2=(x(ix_1,ix_2,ix_3,2)-xc2)
     r3=(x(ix_1,ix_2,ix_3,3)-xc3)     
     
     
     r_tec=sqrt(r2**2.d0+r3**2.d0)         
     
!     if (r_tec .le. r_0+2.0d0*r_tr) w(ix_1,ix_2,ix_3,b1_)=b_Amp
!     if (r_tec .gt. r_0-2.0d0*r_tr) 

     w(ix_1,ix_2,ix_3,b1_)=b_Amp*(one-((atan((r_tec-(r_0+r_tr))/x_max*delta))&
        +Pi/2.d0)/Pi)

  enddo
 enddo
enddo
    
   
! **************************************************************
90 continue


w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,eb_)=w(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3,eb_)-(w(ixmin1:ixmax1,ixmin2:ixmax2,&
   ixmin3:ixmax3,b1_)**2.d0+w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,b2_)&
   **2.d0+w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,b3_)**2.d0)*(1.d0&
   -eqpar(gamma_)/2.d0)/(eqpar(gamma_)-1.d0)

w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,bg1_)=w(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3,b1_)
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,bg2_)=w(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3,b2_)
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,bg3_)=w(ixmin1:ixmax1,&
   ixmin2:ixmax2,ixmin3:ixmax3,b3_)
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,b1_)=0.d0
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,b2_)=0.d0
w(ixmin1:ixmax1,ixmin2:ixmax2,ixmin3:ixmax3,b3_)=0.d0



return
end


!=============================================================================
subroutine specialsource(qdt,ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,&
   ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws,qtC,wCT,qt,w)


include 'vacdef.f'

integer:: ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,ixOmin1,ixOmin2,&
   ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws(niw_)
double precision:: qdt,qtC,qt,wCT(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,&
   nw),w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)
double precision:: fdt,fdthalf2

double precision:: pre(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   tem(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),kapr(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3),so(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   flux(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
double precision:: tau(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   ine(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)

double precision:: preg(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
   pret(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)

integer:: rix_1,i,j
double precision:: mol_0, rrr_

double precision:: fsokr,avgflux

integer:: iw,iiw,iix_1

integer:: ix_1,ix_2,ix_3


double precision:: s_period,xz,xc2,xc3,s_z,delta_r,s_r0,vvv
double precision:: xc1Mm,xc2Mm,xc3Mm
double precision:: xx, yy
double precision:: r(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
    vvx(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3), vvy(ixGlo1:ixGhi1,&
   ixGlo2:ixGhi2,ixGlo3:ixGhi3), vvz(ixGlo1:ixGhi1,ixGlo2:ixGhi2,&
   ixGlo3:ixGhi3)
double precision:: bbx(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3),&
    bby(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3)
double precision:: Vphi, bphi, Vr, tt0, ddt, AA, max_vx, max_vy

!*****************
double precision:: t01,t02,a1,a2,s1,s2,sf,rad,rfc,sdep,tdeps,tdep, tdepc,&
    sigma2
double precision:: s_period120, s_period90, s_period60, s_period30
double precision:: norm, am
!-----------------------------------------------------------------------------

eqpar(eta_)=0.d0

!call addsource_diff(qdt,ixI^L,ixO^L,iws,qtC,wCT,qt,w)

eqpar(nu_)=1.0d0
!eqpar(nu_)=0.d0

call addsource_grav(qdt,ixImin1,ixImin2,ixImin3,ixImax1,ixImax2,ixImax3,&
   ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,ixOmax3,iws,qtC,wCT,qt,w)



if(abs(eqpar(nu_))>smalldouble)call addsource_visc(qdt,ixImin1,ixImin2,&
   ixImin3,ixImax1,ixImax2,ixImax3,ixOmin1,ixOmin2,ixOmin3,ixOmax1,ixOmax2,&
   ixOmax3,iws,qtC,wCT,qt,w)
 
xc1Mm=0.1   !Mm        z axis
xc2Mm=1.0 !0.99d0  !Mm        x axis
!xc2Mm=0.9d0 !0.99d0  !Mm        x axis

xc3Mm=1.0 !0.99d0  !Mm        y axis
!xc3Mm=0.9 !0.99d0  !Mm        y axis

!xc2Mm=0.98333333d0  !Mm        x axis
!xc3Mm=0.98333333d0  !Mm        y axis


xz=xc1Mm*1.0d6  !m        z axis
xc2=xc2Mm*1.0d6  !m        x axis
xc3=xc3Mm*1.0d6  !m        y axis



s_z=16000.d0 !m z axis
delta_r=100000.d0   !m
s_r0=50000.d0       !m


AA=200.d0 ! for s_period 120 s
!AA=150.d0 ! for s_period 200 s
!AA=500.d0

!tt0=10.d0
!ddt=5.d0

s_period=120.d0

s_period120=120.d0
s_period90=90.d0
s_period60=60.d0
s_period30=30.d0





!s_period=200.d0

!tdep=sin(qt*2.d0*pi/s_period) 

!tdeps=sin(qt*2.d0*pi/s_period) 
!tdepc=cos(qt*2.d0*pi/s_period) 

am=2.89d0 
norm=1.d0/am ! normirovka

tdeps=norm*sin(qt*2.d0*pi/s_period120)+sin(qt*2.d0*pi/s_period90)&
   +sin(qt*2.d0*pi/s_period60)+sin(qt*2.d0*pi/s_period30) 
tdepc=norm*cos(qt*2.d0*pi/s_period120)+cos(qt*2.d0*pi/s_period90)&
   +cos(qt*2.d0*pi/s_period60)+cos(qt*2.d0*pi/s_period30) 

Vr=0.0d0

do ix_1=ixImin1,ixImax1
 do ix_2=ixImin2,ixImax2
  do ix_3=ixImin3,ixImax3

     
      xx=x(ix_1,ix_2,ix_3,2)-xc2
      yy=x(ix_1,ix_2,ix_3,3)-xc3
     
      r(ix_1,ix_2,ix_3)=sqrt(xx**2.d0+yy**2.d0)
      
      
!      Vphi=AA*tdep*exp(-(r(ix_1,ix_2,ix_3)-s_r0)**2.d0/delta_r**2.d0)     
      
        vvx(ix_1,ix_2,ix_3)=AA*tdeps*exp(-(r(ix_1,ix_2,ix_3)&
           -s_r0)**2.d0/(delta_r**2.d0))     
        vvy(ix_1,ix_2,ix_3)=-AA*tdepc*exp(-(r(ix_1,ix_2,ix_3)&
           -s_r0)**2.d0/(delta_r**2.d0))     

      
      
!      if (r(ix_1,ix_2,ix_3) .ne. 0.d0) then
         
!        vvx(ix_1,ix_2,ix_3)=(Vr*xx-Vphi*yy)/r(ix_1,ix_2,ix_3)
!        vvy(ix_1,ix_2,ix_3)=(Vr*yy+Vphi*xx)/r(ix_1,ix_2,ix_3)
        

!      else

!        vvx(ix_1,ix_2,ix_3)=0.0d0
!        vvy(ix_1,ix_2,ix_3)=0.0d0

 
!     endif

   enddo
 enddo
enddo



!tdep=exp(-(qt-tt0)**2.d0/ddt**2.d0)



do ix_1=ixImin1,ixImax1
 do ix_2=ixImin2,ixImax2
  do ix_3=ixImin3,ixImax3


 vvv=exp(-(x(ix_1,ix_2,ix_3,1)-xz)**2.d0/s_z**2.d0)

  
 w(ix_1,ix_2,ix_3,m2_)=w(ix_1,ix_2,ix_3,m2_)+(w(ix_1,ix_2,ix_3,rho_)&
    +w(ix_1,ix_2,ix_3,rhob_))*                vvx(ix_1,ix_2,ix_3)*vvv*qdt

 w(ix_1,ix_2,ix_3,m3_)= w(ix_1,ix_2,ix_3,m3_)+(w(ix_1,ix_2,ix_3,rho_)&
    +w(ix_1,ix_2,ix_3,rhob_))*               vvy(ix_1,ix_2,ix_3)*vvv*qdt



! w(ix_1,ix_2,ix_3,b2_)=w(ix_1,ix_2,ix_3,b2_)+bbx(ix_1,ix_2,ix_3)*vvv*qdt

! w(ix_1,ix_2,ix_3,b3_)=w(ix_1,ix_2,ix_3,b3_)+bby(ix_1,ix_2,ix_3)*vvv*qdt


              
 w(ix_1,ix_2,ix_3,e_)=w(ix_1,ix_2,ix_3,e_)+(w(ix_1,ix_2,ix_3,rho_)&
    +w(ix_1,ix_2,ix_3,rhob_))*((vvx(ix_1,ix_2,ix_3)*vvv)**2.d0&
    +(vvy(ix_1,ix_2,ix_3)*vvv)**2.d0)*qdt/2.d0
                      
 
 !Vx[i,j]=Vx[i,j]*G/Vx_m
 !Vy[i,j]=Vy[i,j]*G/Vy_m

   enddo
 enddo
enddo


  
;max_vx=maxval(vvx)
;max_vy=maxval(vvy)
     
     
     
!     tdep=sin(qt*2.d0*pi/s_period)
!
!
!!     vvv=AA*tdep*dexp(-(r1/s_rad1)**2.d0)*dexp(-(r2/s_rad2)**2.d0)*dexp(-(r3/s_rad3)**2.d0)     
!
!     w(ix_1,ix_2,ix_3,e_)=w(ix_1,ix_2,ix_3,e_)+(w(ix_1,ix_2,ix_3,rho_)+w(ix_1,ix_2,ix_3,rhob_))*(vvv**2.d0)*qdt/2.d0
!     w(ix_1,ix_2,ix_3,m1_)=w(ix_1,ix_2,ix_3,m1_)+(w(ix_1,ix_2,ix_3,rho_)+w(ix_1,ix_2,ix_3,rhob_))*vvv*qdt
!
!  enddo
! enddo
!enddo

 write(*,*) '***time=',qt

end



!=============================================================================
subroutine specialbound(qt,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,iw,iB,w)
include 'vacdef.f'

integer:: ix_1,ix_2

integer:: iwmin,iwmax,idimmin,idimmax
double precision:: qt,w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,1:nw)
integer:: ix,ix1,ix2,ix3,ixe,ixf,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,&
   ixpairmin1,ixpairmin2,ixpairmin3,ixpairmax1,ixpairmax2,ixpairmax3,idim,iw,&
   iB
integer:: iwv,jdim

integer:: Ns,i,j
double precision:: ki


double precision:: tmpp1,tmpp2

Ns=1

select case(iB)

case(1)

case(2)


case default
stop 'error iB!'
end select

return
end

!=============================================================================
subroutine getdt_special(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3)

! If the Coriolis force is made very strong it may require time step limiting,
! but this is not implemented here.

include 'vacdef.f'
double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3
!-----------------------------------------------------------------------------

!call getdt_diff(w,ix^L)

if(abs(eqpar(nu_))>smalldouble)call getdt_visc(w,ixmin1,ixmin2,ixmin3,ixmax1,&
   ixmax2,ixmax3)


call getdt_grav(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3)

return
end


subroutine specialeta(w,ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idirmin)
 
include 'vacdef.f'
 
double precision:: w(ixGlo1:ixGhi1,ixGlo2:ixGhi2,ixGlo3:ixGhi3,nw)
integer:: ixmin1,ixmin2,ixmin3,ixmax1,ixmax2,ixmax3,idirmin
!-----------------------------------------------------------------------------
 
stop 'specialeta is not defined'
end

