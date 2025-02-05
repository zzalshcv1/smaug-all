!##############################################################################
! module vacusr - sim1 ! setvac -d=22 -g=204,204 -p=hdadiab -u=sim1


INCLUDE:vacusr.gravity.t
INCLUDE:vacusr.viscosity.t

!=============================================================================
subroutine specialini(ix^L,w)

include 'vacdef.f'

integer:: ix^L
double precision:: w(ixG^T,1:nw)

double precision:: rhoin,xcent^D,radius
double precision:: inirho,iniene
double precision:: onemor,inix,ddx
double precision:: p_1,p_2

integer:: iii_,iix_1,info,i,j
double precision:: pi,comi,eneu,sum,mode,bmax,l
character*79 atmfilename

integer:: ix_1,ix_2

double precision:: ixc_1,ixc_2
double precision:: rfc,a1,a2


double precision:: bz,xc, bmin,bmax

!-----------------------------------------------------------------------------

{^IFMPI if (ipe .ne. 0) read(unitpar,'(a)') atmfilename}

{^IFMPI if (ipe .eq. 0) then}
write(*,*)'param load'
read(unitpar,'(a)') atmfilename
write(*,*) 'from file ',atmfilename
open(33,file=atmfilename,status='old')

!iniene=731191.34d0*8.31e3*(1.1790001e-11)/0.6d0/(eqpar(gamma_)-1.0)

!for VALMc
!iniene=7.89e5*8.31e3*(9.0412855e-13)/0.6d0/(eqpar(gamma_)-1.0)
      
!for Phot
!iniene=7160.d0*8.31e3*(2.7479785e-10)/0.6d0/(eqpar(gamma_)-1.0)


!for Phot 727.2 tube case with 1050 Gauss
!iniene=5500.d0*8.31e3*(5.7413280e-07)/0.6d0/(eqpar(gamma_)-1.0)

!for Photosphere -- TR 2.6 Mm 248 
!iniene=770000.d0*8.31e3*(9.5690001e-12)/1.2d0/(eqpar(gamma_)-1.0)

!for Photosphere -- TR 2.6 Mm 505 
!iniene=770000.d0*8.31e3*(9.5815098e-12)/1.2d0/(eqpar(gamma_)-1.0)

!for Photosphere -- TR 2.6 Mm 252 
!iniene=770000.d0*8.31e3*(9.5815098e-12)/1.2d0/(eqpar(gamma_)-1.0)

!for Photosphere -- TR 2.6 Mm 1023
iniene=770000.d0*8.31e3*(9.5815101e-12)/1.2d0/(eqpar(gamma_)-1.0)

!for Photosphere -- TR 2.6 Mm 1976 
iniene=770000.d0*8.31e3*(7.1818159e-12)/1.2d0/(eqpar(gamma_)-1.0)

{^IFMPI endif}

{^IFMPI call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)}
{^IFMPI call MPI_BCAST(iniene,1,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierrmpi)}

do ix_1=ixGhi1,ixGlo1,-1
{^IFMPI if (ipe .eq. 0)} read(33,*) inix,inirho

{^IFMPI if (ipe .eq. 0)} print*,'inix,inirho=',inix,inirho


{^IFMPI call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)}
{^IFMPI call MPI_BCAST(inix,1,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierrmpi)}
{^IFMPI call MPI_BCAST(inirho,1,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierrmpi)}

 do ix_2=ixGlo2,ixGhi2

   x(ix_1,ix_2,1)=inix !*1000.d0
   w(ix_1,ix_2,rho_)=inirho
   w(ix_1,ix_2,e_)=iniene
   w(ix_1,ix_2,m1_)=0.0
   w(ix_1,ix_2,m2_)=0.0

 enddo


enddo
{^IFMPI if (ipe .eq. 0)} close(33)

{^IFMPI if (ipe .eq. 0) } print*,'grav=',eqpar(grav0_),eqpar(grav1_),eqpar(grav2_)



print*, '###################',ix^L

call primitive(ix^L,w)

do ix_2=ixGlo2,ixGhi2
 do ix_1=ixGhi1-1,ixGlo1,-1 

comi=-abs(x(ix_1+1,ix_2,1)-x(ix_1,ix_2,1))

w(ix_1,ix_2,p_)=w(ix_1+1,ix_2,p_)+w(ix_1,ix_2,rho_)*comi*1.d0*eqpar(grav1_)

if (ix_2 .eq. ixGlo2) print*,'eee=',w(ix_1,ix_2,rho_),w(ix_1,ix_2,p_)

 enddo
enddo


!goto 200

do ix_2=ixGlo2,ixGhi2
 do ix_1=ixGlo1+2,ixGhi1-2
       
       w(ix_1,ix_2,rho_)=-(1.D0/eqpar(grav1_))*(1.D0/(12.D0*(x(ix_1+1,ix_2,1)-x(ix_1,ix_2,1))))*(w(ix_1+2,ix_2,p_)-8.D0*w(ix_1+1,ix_2,p_)+8.D0*w(ix_1-1,ix_2,p_)-w(ix_1-2,ix_2,p_))
               
if (ix_2 .eq. ixGlo2) print*,'eeee=',w(ix_1,ix_2,rho_),w(ix_1,ix_2,p_)

     enddo
   enddo



!lower boundary
do ix_1=ixmin1+4,ixmin1+2,-1
  do ix_2=ixmin2,ixmax2
!    do ix_3=ixmin3,ixmax3
!        p_1=w(ix_1+2,ix_2,ix_3,p_)-8.d0*w(ix_1+1,ix_2,ix_3,p_)+8.d0*w(ix_1-1,ix_2,ix_3,p_)
!        p_2=w(ix_1,ix_2,ix_3,rho_)*eqpar(grav1_)
!        w(ix_1-2,ix_2,ix_3,p_) = 12.d0*(x(ix_1,ix_2,ix_3,1)-x(ix_1-1,ix_2,ix_3,1))*p_2+p_1
         p_1=w(ix_1+2,ix_2,p_)-8.d0*w(ix_1+1,ix_2,p_)+8.d0*w(ix_1-1,ix_2,p_)
         p_2=w(ix_1,ix_2,rho_)*eqpar(grav1_)
         w(ix_1-2,ix_2,p_) = 12.d0*(x(ix_1,ix_2,1)-x(ix_1-1,ix_2,1))*p_2+p_1
!       enddo
    enddo
 enddo


!upper boundary
do ix_1=ixmax1-4,ixmax1-2
   do ix_2=ixmin2,ixmax2
!      do ix_3=ixmin3,ixmax3
         
!          p_1=w(ix_1-2,ix_2,ix_3,p_)-8.d0*w(ix_1-1,ix_2,ix_3,p_)+8.d0*w(ix_1+1,ix_2,ix_3,p_)
!          p_2=w(ix_1,ix_2,ix_3,rho_)*eqpar(grav1_)
!          w(ix_1+2,ix_2,ix_3,p_) = -12.d0*(x(ix_1,ix_2,ix_3,1)-x(ix_1-1,ix_2,ix_3,1))*p_2+p_1
           p_1=w(ix_1-2,ix_2,p_)-8.d0*w(ix_1-1,ix_2,p_)+8.d0*w(ix_1+1,ix_2,p_)
           p_2=w(ix_1,ix_2,rho_)*eqpar(grav1_)
           w(ix_1+2,ix_2,p_) = -12.d0*(x(ix_1,ix_2,1)-x(ix_1-1,ix_2,1))*p_2+p_1
      enddo
   enddo
!enddo


200 continue



call conserve(ix^L,w)




w(ix^S,eb_)=w(ix^S,e_)
w(ix^S,e_)=0.d0

w(ix^S,rhob_)=w(ix^S,rho_)
w(ix^S,rho_)=0.d0


!w(ix^S,b1_)=0.04d0

!magnetic field
goto 50
! ********************** smooth  Bx magnetic field***************
xc=2800000.d0 !x(ixmax1,10,1)/2.0


  do ix_1=ixmin1,ixmax1
   do ix_2=ixmin2,ixmax2

      w(ix_1,ix_2,b2_)=((atan((x(ix_1,10,1)-xc)/x(ixmax1,10,1)*100.d0))+Pi/2.d0)/Pi

   enddo
  enddo

bmax=maxval(w(ix^S,b2_))
bmin=minval(w(ix^S,b2_))

    w(ix^S,b2_)=0.04d0*(w(ix^S,b2_)-bmin)/bmax
    
   
! **************************************************************
50 continue
goto 101
! ********************** Bz(z) magnetic field***************

  do ix_1=ixmin1,ixmax1
   do ix_2=ixmin2,ixmax2

      w(ix_1,ix_2,b1_)=0.4d0*(1.d0-x(ix_1,10,1)/x(ixmax1,10,1))

   enddo
  enddo
    
! **************************************************************
101 continue
goto 100
! ********************** smooth  Bz magnetic field***************
xc=x(10,ixmax2,2)/2.0


  do ix_1=ixmin1,ixmax1
   do ix_2=ixmin2,ixmax2

      w(ix_1,ix_2,b1_)=((atan((x(10,ix_2,2)-xc)/x(10,ixmax2,2)*100.d0))+Pi/2.d0)/Pi

   enddo
  enddo

bmax=maxval(w(ix^S,b1_))
bmin=minval(w(ix^S,b1_))

    w(ix^S,b1_)=0.04*(w(ix^S,b1_)-bmin)/bmax
    
! **************************************************************
100 continue
goto 600
! ********************** smooth  opoz Bz magnetic field***************
xc=x(10,ixmax2,2)/2.0


  do ix_1=ixmin1,ixmax1
   do ix_2=ixmin2,ixmax2

      w(ix_1,ix_2,b1_)=((atan((x(10,ix_2,2)-xc)/x(10,ixmax2,2)*100.d0))+Pi/2.d0)/Pi

   enddo
  enddo

bmax=maxval(w(ix^S,b1_))
bmin=minval(w(ix^S,b1_))

    w(ix^S,b1_)=0.8d0*((w(ix^S,b1_)-bmin)/bmax-0.5d0)
    
! **************************************************************
600 continue
goto 700
! ********************** smooth  tube Bz magnetic field***************
xc=x(10,ixmax2,2)/2.0


  do ix_1=ixmin1,ixmax1
   do ix_2=ixmin2,ixmax2

      tmp(ix_1,ix_2)=((atan((x(10,ix_2,2)-xc)/x(10,ixmax2,2)*100.d0))+Pi/2.d0)/Pi

   enddo
  enddo

bmax=maxval(tmp(ix^S))
bmin=minval(tmp(ix^S))

    tmp(ix^S)=(tmp(ix^S)-bmin)/bmax
       
xc=3.0d0*x(10,ixmax2,2)/4.0


  do ix_1=ixmin1,ixmax1
   do ix_2=ixmin2,ixmax2

      w(ix_1,ix_2,b1_)=((atan((x(10,ix_2,2)-xc)/x(10,ixmax2,2)*100.d0))+Pi/2.d0)/Pi

   enddo
  enddo

bmax=maxval(w(ix^S,b1_))
bmin=minval(w(ix^S,b1_))

    w(ix^S,b1_)=0.4*(tmp(ix^S)-(w(ix^S,b1_)-bmin)/bmax)
    
    
    
! **************************************************************
700 continue


w(ix^S,eb_)=w(ix^S,eb_)-(w(ix^S,b1_)**2.d0+w(ix^S,b2_)**2.d0)*(1.d0-eqpar(gamma_)/2.d0)/(eqpar(gamma_)-1.d0)

w(ix^S,bg1_)=w(ix^S,b1_)
w(ix^S,bg2_)=w(ix^S,b2_)
w(ix^S,b1_)=0.d0
w(ix^S,b2_)=0.d0


300 continue

return
end


!=============================================================================
subroutine specialsource(qdt,ixI^L,ixO^L,iws,qtC,wCT,qt,w)


include 'vacdef.f'

integer:: ixI^L,ixO^L,iws(niw_)
double precision:: qdt,qtC,qt,wCT(ixG^T,nw),w(ixG^T,nw)
double precision:: fdt,fdthalf2

double precision:: pre(ixG^T),tem(ixG^T),kapr(ixG^T),so(ixG^T),flux(ixG^T)
double precision:: tau(ixG^T),ine(ixG^T)

double precision:: preg(ixG^T),pret(ixG^T)

integer:: rix_1,i,j
double precision:: mol_0, rrr_

double precision:: fsokr,avgflux

integer:: iw,iiw,iix_1

integer:: ix_1,ix_2


double precision:: s_period,xc1,xc2,s_rad1,s_rad2,vvv,r1,r2
 

!*****************
double precision:: t01,t02,a1,a2,s1,s2,sf,rad,rfc,sdep,tdep,sigma2
double precision:: xc21,xc22,xc23,xc24,xc25, r21,r22,r23,r24,r25

!-----------------------------------------------------------------------------


eqpar(eta_)=0.d0

!call addsource_diff(qdt,ixI^L,ixO^L,iws,qtC,wCT,qt,w)

eqpar(nu_)=1.0d0
!eqpar(nu_)=0.d0

call addsource_grav(qdt,ixI^L,ixO^L,iws,qtC,wCT,qt,w)



if(abs(eqpar(nu_))>smalldouble)&
   call addsource_visc(qdt,ixI^L,ixO^L,iws,qtC,wCT,qt,w)
 



s_period=30.d0

!s_period=24.d0

!xc1=0.1d6 !m
!xc2=10.0d6  !m
!s_rad1=0.02d6 !m
!s_rad2=1.0d6 !m

!****for Phot 727.2 tube case with 1050 Gauss
!xc1=0.2d5 !m
!xc2=5.0d5  !m
!s_rad1=0.02d5 !m
!s_rad2=0.1d6 !m
!****


!s temp bump
!xc1=0.2d6 !m
!xc2=5.0d5  !m
!s_rad1=0.02d5 !m
!s_rad2=0.1d6 !m


! single driver
!xc2=2.0d6  !m
!xc1=0.1d6 !m

! single driver shift
xc2=1.0d6  !m
xc1=0.1d6 !m

! single driver
s_rad1=0.1d5 !m
s_rad2=0.2d6 !m


! multiple driver
!s_rad1=0.1d5 !m
!s_rad2=0.1d6 !m


! muliple driver
!xc21=0.66d6  !m
!xc22=1.32d6  !m
!xc23=2.0d6  !m
!xc24=2.66d6  !m
!xc25=3.32d6  !m

!xc1=0.1d6 !m


;print*, '**Imin', ixImin1, ixImax1
;print*, '**Imax', ixImin2, ixImax2

;print*, '**Omin', ixOmin1, ixOmax1
;print*, '**Omax', ixOmin2, ixOmax2

 
do ix_1=ixOmin1,ixOmax1
 do ix_2=ixOmin2,ixOmax2

! single driver
     r1=(x(ix_1,ix_2,1)-xc1)**2.d0
     r2=(x(ix_1,ix_2,2)-xc2)**2.d0

! multiple driver
!     r1=(x(ix_1,ix_2,1)-xc1)**2.d0
!     r21=(x(ix_1,ix_2,2)-xc21)**2.d0
!     r22=(x(ix_1,ix_2,2)-xc22)**2.d0
!     r23=(x(ix_1,ix_2,2)-xc23)**2.d0          
!     r24=(x(ix_1,ix_2,2)-xc24)**2.d0
!     r25=(x(ix_1,ix_2,2)-xc25)**2.d0          
   
     tdep=sin(qt*2.d0*pi/s_period)


!     vvv=50.d0*tdep*exp(-r1/s_rad1**2.d0-r2/s_rad2**2.d0)     

!****for Phot 727.2 tube case with 1050 Gauss
!     w(ix_1,ix_2,e_)=w(ix_1,ix_2,e_)+(w(ix_1,ix_2,rho_)+w(ix_1,ix_2,rhob_))*(vvv**2.d0)*qdt/2.d0
!     w(ix_1,ix_2,m2_)=w(ix_1,ix_2,m2_)+(w(ix_1,ix_2,rho_)+w(ix_1,ix_2,rhob_))*vvv*qdt
!****

! single driver
     vvv=100.d0*tdep*exp(-r1/s_rad1**2.d0-r2/s_rad2**2.d0)

! multiple driver
!     vvv=exp(-r1/s_rad1**2.d0-r21/s_rad2**2.d0)
!     vvv=vvv+exp(-r1/s_rad1**2.d0-r22/s_rad2**2.d0)
!     vvv=vvv+exp(-r1/s_rad1**2.d0-r23/s_rad2**2.d0)
!     vvv=vvv+exp(-r1/s_rad1**2.d0-r24/s_rad2**2.d0)
!     vvv=vvv+exp(-r1/s_rad1**2.d0-r25/s_rad2**2.d0)
!     vvv=100.d0*tdep*vvv

    
     w(ix_1,ix_2,e_)=w(ix_1,ix_2,e_)+(w(ix_1,ix_2,rho_)+w(ix_1,ix_2,rhob_))*(vvv**2.d0)*qdt/2.d0
     w(ix_1,ix_2,m2_)=w(ix_1,ix_2,m2_)+(w(ix_1,ix_2,rho_)+w(ix_1,ix_2,rhob_))*vvv*qdt

    
 enddo
enddo



{^IFMPI if (ipe.eq.0)} write(*,*) '***time=',qt


end



!=============================================================================
subroutine specialbound(qt,ix^L,iw,iB,w)
include 'vacdef.f'

integer:: ix_1,ix_2

integer:: iw^LIM,idim^LIM
double precision:: qt,w(ixG^T,1:nw)
integer:: ix,ix^D,ixe,ixf,ix^L,ixpair^L,idim,iw,iB
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
subroutine getdt_special(w,ix^L)

! If the Coriolis force is made very strong it may require time step limiting,
! but this is not implemented here.

include 'vacdef.f'
double precision:: w(ixG^T,nw)
integer:: ix^L
!-----------------------------------------------------------------------------

!call getdt_diff(w,ix^L)

if(abs(eqpar(nu_))>smalldouble)&
   call getdt_visc(w,ix^L)


call getdt_grav(w,ix^L)

return
end


subroutine specialeta(w,ix^L,idirmin)
 
include 'vacdef.f'
 
double precision:: w(ixG^T,nw)
integer:: ix^L,idirmin
!-----------------------------------------------------------------------------
 
stop 'specialeta is not defined'
end

