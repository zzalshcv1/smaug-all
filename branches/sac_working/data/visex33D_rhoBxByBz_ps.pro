tarr=dblarr(1)
maxa=fltarr(1)
mina=fltarr(1)
cuta=fltarr(2000,50)

DEVICE, PSEUDO=8, DECOMPOSED=0, RETAIN=2
WINDOW, /FREE, /PIXMAP, COLORS=256 & WDELETE, !D.WINDOW
PRINT, 'Date:      ', systime(0)
PRINT, 'n_colors   ', STRCOMPRESS(!D.N_COLORS,/REM)
PRINT, 'table_size ', STRCOMPRESS(!D.TABLE_SIZE,/REM)


ii=1

if (ii eq 1) then begin
;loadct,4
;mixct
endif else begin
loadct,0
tek_color
endelse




mass=dblarr(1)
egas=dblarr(1)
tm=dblarr(1)
dtt=dblarr(1)

ia=1.0

headline='                                                                               '
it=long(1)
ndim=long(1)
neqpar=long(1)
nw=long(1)
varname='                                                                               '
time=double(1)
dum=long(1)
dumd=long(1)

; Open an MPEG sequence: 
;mpegID = MPEG_OPEN([700,1200],FILENAME='myMovie.mpg') 

window, 0,xsize=550,ysize=450,XPOS = 950, YPOS = 300 
window, 1,xsize=550,ysize=450,XPOS = 500, YPOS = 80



nn=0
np=0
kkk=4

nn_i=0

close,1
close,2


;openr,1,'/data/ap1vf/3D_509_36_36_300s.out',/f77_unf
;openr,1,'/data/ap1vf/3D_tube_196_100_100.ini',/f77_unf

;openr,1,'/data/ap1vf/3D_396_60_60t.out',/f77_unf

;openr,1,'/data/ap1vf/background_3Dtube.ini',/f77_unf

openr,1,'/data/ap1vf/3D_tube_196_100_100_120s_full.out',/f77_unf
;openr,1,'/data/ap1vf/3D_tube.ini',/f77_unf

;***********************  files path *************************

dr='/data/ap1vf/png/3D/tube/P120_R100_A200_B1000_Dxy2_Dz1.6_Nxy100_Nz196/ps/slice'

traj=dr+'/VrVpVz/'

while not(eof(1)) do begin
readu,1,headline
readu,1,it,time,ndim,neqpar,nw
gencoord=(ndim lt 0)
tarr=[tarr,time]
ndim=abs(ndim)
nx=lonarr(ndim)
readu,1,nx
print,'tuta', neqpar
eqpar=dblarr(neqpar)
readu,1,eqpar
readu,1,varname


print, 'tuta1'
xout=dblarr(3)
yout=dblarr(3)


n1=nx(0)
n2=nx(1)
n3=nx(2)
x=dblarr(n1,n2,n3,ndim)

wi=dblarr(n1,n2,n3)

w=dblarr(n1,n2,n3,nw)

readu,1,x
for iw=0,nw-1 do begin
 print, iw
 readu,1,wi
  w(*,*,*,iw)=wi
endfor

xx=dblarr(n2)
yy=dblarr(n3)
zz=dblarr(n1)


xx(*)=x(1,*,1,1)
yy(*)=x(1,1,*,2)
zz(*)=x(*,1,1,0)

Vt=dblarr(n1,n2,n3)
B=dblarr(n1,n2,n3)
B_bg=dblarr(n1,n2,n3)

p=dblarr(n1,n2,n3,1)


mu=4.0*!PI/1.0e7

print,'******************* time = ', time


label_rho='!4q!X'+' ('+'!19kg/m!X!U3'+'!N)'
label_p='p'+' ('+'!19H/m!X!U2'+'!N)'
label_Bx='Bx'
label_By='By'
label_Bz='Bz'

scale=1.d6

R=8.3e+003
mu=1.257E-6
mu_gas=0.6
gamma=1.66667

xstart=0
xend=99
ystart=0
yend=99

pp=50 ;x
pm=50 ;x
kk=5  ;y

wset,0
!p.multi = [0,2,2,0,1]


zstart=0
zend=195

wt=dblarr(zend-zstart+1,xend-xstart+1,iw)
wtm=dblarr(zend-zstart+1,xend-xstart+1,iw)


wt=reform(w(zstart:zend,xstart:xend,pp,*))
wtm=reform(w(zstart:zend,xstart:xend,pm,*))

wy=dblarr(n1,n3,iw)
wy=reform(w(zstart:zend,pp,*,*))

wt(*,*,3)=reform(w(zstart:zend,xstart:xend,pp+2,3))

wt(*,*,12)=reform(w(zstart:zend,pp,ystart:yend,12))

saeb=dblarr(zend-zstart+1,xend-xstart+1)
sabz_t=dblarr(zend-zstart+1,xend-xstart+1)
sabx_t=dblarr(zend-zstart+1,xend-xstart+1)
saby_t=dblarr(zend-zstart+1,xend-xstart+1)

saeb(*,*)=wt(*,*,8)
sabz_t(*,*)=wt(*,*,10)
sabx_t(*,*)=wt(*,*,11)
saby_t(*,*)=wt(*,*,12)

vt=dblarr(n1,n2,n3)
vvt=dblarr(n2,n3)
vt(*,*,*)=sqrt(w(*,*,*,1)^2.d0+w(*,*,*,2)^2.d0+w(*,*,*,3)^2.d0)/(w(*,*,*,0)+w(*,*,*,9))


;****************** Pressure background begin ********************
TP=saeb
TP=TP-(sabx_t^2.0+saby_t^2.0+sabz_t^2.0)/2.0
TP=(gamma-1.d0)*TP
;****************** Pressure background end ********************

indexs=strtrim(np,2)

a = strlen(indexs)                                                  
case a of                                                           
 1:indexss='0000'+indexs                                             
 2:indexss='000'+indexs                                              
 3:indexss='00'+indexs                                               
 4:indexss='0'+indexs                                               
endcase   

if (ii eq 1) then begin

cs=0.8



; **********************************************************

scale=1.0d6

; ****************** ps, eps begin ****************************************
   xs=19.d0
   k=0.75d0
   
   SET_PLOT,'ps'  

   device, filename=traj+indexss+'.eps', $
   BITS=8, /color, xsize=xs, ysize=k*xs, /encap
   
!p.thick = 2
!x.thick = 2
!y.thick = 2
!z.thick = 2

px1 = !x.window * !d.x_vsize     ;Position of frame in device units
py1 = !y.window * !d.y_vsize
sx1 = px1(1)-px1(0)                ;Size of frame in device units
sy1 = py1(1)-py1(0)
print, px1, py1, sx1, sy1


tvframe,rotate(wt(*,*,9),1)*1.d4,/bar,/sample, $
        xtitle='x [Mm]', ytitle='z [Mm]',charsize=cs, CT='dPdT', $
	xrange=[xx[xstart]/scale, xx[xend]/scale], $
	yrange=[zz[zstart]/scale, zz[zend]/scale], $
	BTITLE='!7q!N!3*10!U -4!N [kg!N!3/m!U 3!N!3]'

yd=dblarr(2)
xd=dblarr(2)
L_thick=3.0


xd[0]=min(xx)/scale
xd[1]=max(xx)/scale
;tek_color

;TR_U=2.05
TR_D=1.88
Phot=0.26

;yd[0]=TR_U
;yd[1]=TR_U
;oplot, xd, yd, color=255, linestyle=2, thick=L_thick
yd[0]=TR_D
yd[1]=TR_D
oplot, xd, yd, color=255, linestyle=2, thick=L_thick
yd[0]=Phot
yd[1]=Phot
oplot, xd, yd, color=0, linestyle=2, thick=L_thick


xyouts, 5200,8500, 'Torsional driver',color=0, charsize=0.8, /device

xyouts, 2000, 8500, 'Photosphere', color=0,/device
xyouts, 2000,13000, 'Chromosphere', color=255, /device
;xyouts, px1(1)/8+100,1.062*py1(1)/2, 'Transition region', color=255, /device
;xyouts, px1(1)/8+100,1.9*py1(1)/2, 'Corona', color=255, /device


;******************* begin source ****************************
for j=0,60,2 do begin
tvellipse,5*j,j,4650,8400, thick=2, color=150
endfor
tvellipse,5*j,j,4650,8400,thick=2, color=0
;******************* end source ******************************




tvframe,rotate(wt(*,*,12),1)*sqrt(mu)*1.0e4,/bar,/sample, $
        xtitle='x [Mm]', ytitle='z [Mm]', charsize=cs, $
	xrange=[xx[xstart]/scale, xx[xend]/scale], $
	yrange=[zz[zstart]/scale, zz[zend]/scale], $
	BTITLE='  B!D Y!N!3!N [Gauss]'
	

tvframe,rotate(wt(*,*,11),1)*sqrt(mu)*1.0e4,/bar,/sample, $
        xtitle='x [Mm]', ytitle='z [Mm]', charsize=cs, $
	xrange=[xx[xstart]/scale, xx[xend]/scale], $
	yrange=[zz[zstart]/scale, zz[zend]/scale], $
	BTITLE='  B!D X!N!3!N [Gauss]'	

tvframe,rotate(wt(*,*,10),1)*sqrt(mu)*1.0e4,/bar,/sample, $
        xtitle='x[Mm]', ytitle='z [Mm]', charsize=cs, $
	xrange=[xx[xstart]/scale, xx[xend]/scale], $
	yrange=[zz[zstart]/scale, zz[zend]/scale], $
	BTITLE='  B!D Z!N!3!N [Gauss]'	

	

;ss='time ='+strTrim(string(FORMAT='(6F10.2)', time),2)
; xyouts,20,5, ss, /device, color=200	



device, /close
set_plot, 'x'

stop
np=np+1


wset,1
!p.multi = [0,3,2,0,1]

;for hh=0,n1-1 do begin

hh=12

vvt(*,*)=vt(hh,*,*)
cs=2

hxmin=20
hymin=20

hxmax=80
hymax=80

wv=reform(w(hh,*,*,*))


hight=strTrim(string(FORMAT='(6F10.2)',x(hh,1,1,0)/1.0d6),1)+' [Mm]'




r=dblarr(n2,n3)
vr=dblarr(n2,n3)
vphi=dblarr(n2,n3)

br=dblarr(n2,n3)
bphi=dblarr(n2,n3)


for i=0,n2-1 do begin
 for j=0,n3-1 do begin
 r[i,j]=sqrt((xx[i]-xx[n2/2])^2.d0+(yy[j]-yy[n3/2])^2.d0)

 vr[i,j]= wv(i,j,2)/(wv(i,j,0)+wv(i,j,9))*(xx[i]-xx[n2/2])/r[i,j]+$
          wv(i,j,3)/(wv(i,j,0)+wv(i,j,9))*(yy[j]-yy[n3/2])/r[i,j]
  
 vphi[i,j]=-wv(i,j,2)/(wv(i,j,0)+wv(i,j,9))*(yy[j]-yy[n3/2])/r[i,j]+$
          wv(i,j,3)/(wv(i,j,0)+wv(i,j,9))*(xx[i]-xx[n2/2])/r[i,j]

 br[i,j]= wv(i,j,6)*(xx[i]-xx[n2/2])/r[i,j]+$
          wv(i,j,7)*(yy[j]-yy[n3/2])/r[i,j]
  
 bphi[i,j]=-wv(i,j,6)*(yy[j]-yy[n3/2])/r[i,j]+$
          wv(i,j,7)*(xx[i]-xx[n2/2])/r[i,j]
 br[n2/2,n3/2]=0.d0	  
 bphi[n2/2,n3/2]=0.d0
 endfor
 
endfor


tvframe,br(hxmin:hxmax,hymin:hymax),/bar,/sample, title='br [Gauss], h='+hight, $
        xtitle='x [Mm]', ytitle='y [Mm]', charsize=cs, $
	xrange=[xx(hxmin)/scale, xx(hxmax)/scale], yrange=[yy(hymin)/scale, yy(hymax)/scale]	
	

tvframe,vr(hxmin:hxmax,hymin:hymax),/bar,/sample, title='Vr, zslice, h='+hight, $
        xtitle='x [Mm]', ytitle='y [Mm]', charsize=cs, $
	xrange=[xx(hxmin)/scale, xx(hxmax)/scale], yrange=[yy(hymin)/scale, yy(hymax)/scale]	
	

tvframe,bphi(hxmin:hxmax,hymin:hymax),/bar,/sample, title='bphi [Gauss], h='+hight, $
        xtitle='x [Mm]', ytitle='y [Mm]', charsize=cs, $
	xrange=[xx(hxmin)/scale, xx(hxmax)/scale], yrange=[yy(hymin)/scale, yy(hymax)/scale]	
	
tvframe,vphi(hxmin:hxmax,hymin:hymax),/bar,/sample, title='Vphi, zslice, h='+hight, $
        xtitle='x [Mm]', ytitle='y [Mm]', charsize=cs, $
	xrange=[xx(hxmin)/scale, xx(hxmax)/scale], yrange=[yy(hymin)/scale, yy(hymax)/scale]	
	

tvframe,wv(hxmin:hxmax,hymin:hymax,5)*sqrt(mu)*1.0e4,/bar,/sample, $
        title='bz [Gauss], h='+hight, $
        xtitle='x [Mm]', ytitle='y [Mm]', charsize=cs, $
	xrange=[xx(hxmin)/scale, xx(hxmax)/scale], yrange=[yy(hymin)/scale, yy(hymax)/scale]	


tvframe,wv(hxmin:hxmax,hymin:hymax,1)/(wv(hxmin:hxmax,hymin:hymax,0)+wv(hxmin:hxmax,hymin:hymax,9)),$
        /bar,/sample, title='Vz [m/s], h='+hight, $
        xtitle='x [Mm]', ytitle='y [Mm]', charsize=cs, $
	xrange=[xx(hxmin)/scale, xx(hxmax)/scale], yrange=[yy(hymin)/scale, yy(hymax)/scale]	

ss='time ='+strTrim(string(FORMAT='(6F10.2)', time),2)
 xyouts,20,5, ss, /device, color=200	


endif else begin

endelse


indexs=strtrim(nn,2)

a = strlen(indexs)                                                  
case a of                                                           
 1:indexss='0000'+indexs                                             
 2:indexss='000'+indexs                                              
 3:indexss='00'+indexs                                               
 4:indexss='0'+indexs                                               
endcase   

;image_p = TVRD_24()
;write_png,'/data/ap1vf/png/3D/tube/test_120s/slice/'+indexss+'.png',image_p, red,green, blue


nn=nn+1


endwhile



end
