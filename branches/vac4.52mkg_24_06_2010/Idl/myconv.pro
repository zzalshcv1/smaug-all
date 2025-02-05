;^CFG COPYRIGHT VAC_UM
;===========================================================================
;    Read the npict-th picture from an ascii or binary ini or out file 
;
;    Usage: 
;
; .r getpict
;
;    "getpict" will prompt you for "filename(s)" and "npict"
;    unless they are already set. Previous settings can be erased by 
;
; .r defaults
;
;    or modified explicitly, e.g.:
;
; filename='data/example.ini'
; npict=1
;
;    The "x" and "w" arrays and the header info will be read from the file. 
;
;    If a file is read with generalized coordinates, "gencoord=1" is set,
;    and the original data is transformed according to the "transform"
;    string variable into "xreg" and "wreg".
;
;    The same npict-th snapshot can be read from 2 or 3 files by e.g. setting
;
; filename='data/file1.ini data/file2.out'
;
;    In this case the data is read into x0,w0 and x1,w1 for the two files,
;    and possibly transformeed into wreg0,wreg1.
;
;    To plot a variable, type e.g.:
;
; surface,w(*,*,2)
;
;    or 
;
; .r plotfunc
;
;===========================================================================
filename='../data/shearalfven2d_2.out'
nfile=0
npictinfile=1
str2arr,filename,filenames,nfile
gettype,filenames,filetypes,npictinfiles
openfile,10,filename,filetypes(0)

getpict,10,filetypes(0),npict,x,w,headline,phys,it,time,$
          gencoord,ndim,neqpar,nw,nx,eqpar,variables,rBody,error


close,10
for npict=1,36 do begin

nfile=0
if filename eq '' and logfilename ne '' then begin
   filename=logfilename
   while strpos(filename,'.log') ge 0 $
      do strput,filename,'.out',strpos(filename,'.log')
   askstr,'filename(s)   ',filename,1
endif else $
   askstr,'filename(s)   ',filename,doask
str2arr,filename,filenames,nfile
if nfile gt 3 then begin
   print,'Error in GetPict: cannot handle more than 3 files.'
   retall
endif
gettype,filenames,filetypes,npictinfiles
print,'filetype(s)   =','',filetypes
print,'npictinfile(s)=',npictinfiles
if max(npictinfiles) eq 1 then npict=1
asknum,'npict',npict,doask
print


str2arr,physics,physicss,nfile
physics=''
for ifile=0,nfile-1 do begin

   phys=physicss(ifile)

   ; Read data from file

   print
   if nfile gt 1 then print,'filename  =',filenames(ifile)

   openfile,10,filenames(ifile),filetypes(ifile)

   getpict,10,filetypes(ifile),npict,x,w,headline,phys,it,time,$
          gencoord,ndim,neqpar,nw,nx,eqpar,variables,rBody,error

   print,         'headline  =',strtrim(headline,2)
   print,FORMAT='("ndim      =",i2,", neqpar=",i2,", nw=",i2)',ndim,neqpar,nw
   if gencoord eq 1 then print,'Generalized coordinates'
   print,         'it        =',it,', time=',time
   print,FORMAT='("nx        = ",3(i8))',nx
   print,         'eqpar     =',eqpar
   print,         'variables =',variables
   askstr,'physics (eg. mhd12)',phys,doask
   physicss(ifile)=phys
   physics=physics + phys + ' '

   if nfile gt 1 then begin
     case ifile of
     0: begin
          w0=w
          x0=x
        end
     1: begin
          w1=w
          x1=x
        end
     2: begin
          w2=w
          x2=x
        end
     endcase
     print,'Read x',ifile,' and w',ifile,FORMAT='(a,i1,a,i1)'
   endif else print,'Read x and w'

   readtransform,ndim,nx,gencoord,transform,nxreg,xreglimits,wregpad,$
		physics,nvector,vectors,grid,doask

   if (gencoord and (transform eq 'polar' or transform eq 'regular')) or $
      (not gencoord and transform eq 'unpolar') then begin
      if nfile eq 1 then $
           print,'...transform to xreg and wreg' $
      else print,'...transform to xreg and wreg',ifile,FORMAT='(a,i1)'
      case transform of
         'regular':regulargrid,x_old,nxreg_old,xreglimits_old,$
                   x,xreg,nxreg,xreglimits,w,wreg,nw,wregpad,triangles,symmtri
         'polar'  :begin
                     polargrid,nvector,vectors,x,w,xreg,wreg
                     variables(0:1)=['r','phi']
                   end
	 'unpolar':begin
                     unpolargrid,nvector,vectors,x,w,xreg,wreg
                     variables(0:1)=['x','y']
                   end
         else     :print,'Unknown value for transform:',transform
      endcase

      if nfile gt 1 then case ifile of
         0: wreg0=wreg
         1: wreg1=wreg
         2: wreg2=wreg
      endcase
   endif
endfor
close,10

vtkfile='e';
vacscalar2vtk,npict,w(*,*,*),x(*,*),3,1,vtkfile

vtkfile='rho';
vacscalar2vtk,npict,w(*,*,*),x(*,*),0,1,vtkfile

vtkfile='mom';
vac2vtk,npict,w(*,*,*),x(*,*),1,2,vtkfile

vtkfile='b';
vac2vtk,npict,w(*,*,*),x(*,*),4,2,vtkfile

endfor

end
