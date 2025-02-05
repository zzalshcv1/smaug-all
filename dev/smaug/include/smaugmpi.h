#include "mpi.h"
#include "iotypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


MPI::Intracomm comm;
MPI::Request request;
double gwall_time;
real *gmpisendbuffer;
real *gmpirecvbuffer;


real **gmpisrcbufferl;
real **gmpisrcbufferr;
real **gmpitgtbufferl;
real **gmpitgtbufferr;


int gnmpirequest,gnmpibuffer,gnmpibuffermod;
int gnmpibuffer0,gnmpibuffer1,gnmpibuffer2;
int gnmpibuffermod0,gnmpibuffermod1,gnmpibuffermod2;

MPI::Request *gmpirequest;

/*a test with the mac*/



int sacencodempivisc0 (struct params *p,int ix, int iy, int iz, int bound,int dim) {
  #ifdef USE_SAC_3D
    return (  bound* ((   ((p->n[1])+2)*((p->n[2])+2)   ))+       (    (     iy+iz*((p->n[1])+2)    )    )      );
  #else
    return (   bound*(    ((p->n[1])+2)  )  +   iy     );
  #endif
}



int sacencodempivisc1 (struct params *p,int ix, int iy, int iz, int bound,int dim) {
  #ifdef USE_SAC_3D
    return (bound* ((   ((p->n[0])+2)*((p->n[2])+2))      )+   ((ix+iz*((p->n[0])+2)))  );
  #else
    return (   bound*(    ((p->n[0])+2)  )  +   ix     );
  #endif

  return 0;
}


int sacencodempivisc2 (struct params *p,int ix, int iy, int iz, int bound,int dim) {
  #ifdef USE_SAC_3D
    return (bound* ((   ((p->n[0])+2)*((p->n[1])+2))      )+   (  (ix+iy*((p->n[0])+2))    ));
  #endif
  return 0;
}

int sacencodempiw0 (struct params *p,int ix, int iy, int iz, int field,int bound) {
  #ifdef USE_SAC_3D
    return (4*field*(         ((p->n[1])*(p->n[2]))   )+bound*(            +  ((p->n[1])*(p->n[2]))      )+   (  (iy+iz*(p->n[1]))    ));
  #else
    return (4*field*(p->n[1]) +bound*((p->n[1]))  +   (iy));
  #endif
  return 0;
}

int sacencodempiw1 (struct params *p,int ix, int iy, int iz, int field,int bound) {
  #ifdef USE_SAC_3D
    return (4*field*(         ((p->n[0])*(p->n[2]))   )+
bound*(            +  ((p->n[0])*(p->n[2]))      )+   (  (ix+iz*(p->n[0]))    ));
  #else
    return (4*field*(p->n[0]) +bound*((p->n[0]))  +   (ix));
  #endif
return 0;
}


int sacencodempiw2 (struct params *p,int ix, int iy, int iz, int field,int bound) {
  #ifdef USE_SAC_3D
    return (4*field*(         ((p->n[0])*(p->n[1]))   )+
bound*(            +  ((p->n[0])*(p->n[1]))      )+   (  (ix+iy*(p->n[0]))    ));
  #endif
   return 0;
}

int encode3p2_sacmpi (struct params *dp,int ix, int iy, int iz, int field) {


  #ifdef USE_SAC_3D
    return ( (iz*(((dp)->n[0])+2)*(((dp)->n[1])+2)  + iy * (((dp)->n[0])+2) + ix)+(field*(((dp)->n[0])+2)*(((dp)->n[1])+2)*(((dp)->n[2])+2)));
  #else
    return ( (iy * (((dp)->n[0])+2) + ix)+(field*(((dp)->n[0])+2)*(((dp)->n[1])+2)));
  #endif
  return 0;
}

/*void mpiinit(params *p);
void mpifinalize(params *p);
void mpisetnpediped(params *p, char *string);
void ipe2iped(params *p);
void iped2ipe(params *p);*/

//!=============================================================================
//subroutine mpiinit
//
//! Initialize MPI variables
//

//!----------------------------------------------------------------------------
//call MPI_INIT(ierrmpi)
//call MPI_COMM_RANK (MPI_COMM_WORLD, ipe, ierrmpi)
//call MPI_COMM_SIZE (MPI_COMM_WORLD, npe, ierrmpi)

//! unset values for directional processor numbers
//npe1=-1;npe2=-1;
//! default value for test processor
//ipetest=0

void mgpuinit(params *p)
{
  int nmpibuffer;
  int numbuffers=4;
  int i;
     
  //MPI::Intracomm comm;   
  //MPI_Init(&argc, &argv);

  gwall_time = MPI_Wtime();
  comm=MPI::COMM_WORLD;
  p->npe=comm.Get_size();
  p->ipe=comm.Get_rank();

  #ifdef USE_SAC_3D

  gnmpibuffer0=NDERV*(p->n[2])*(p->n[1])*(p->ng[0]);
  gnmpibuffer1=NDERV*(p->n[0])*(p->n[2])*(p->ng[1]);
  gnmpibuffer2=NDERV*(p->n[0])*(p->n[1])*(p->ng[2]);

  gnmpibuffermod0=NVAR*(p->n[2])*(p->n[1])*(p->ng[0]);
  gnmpibuffermod1=NVAR*(p->n[0])*(p->n[2])*(p->ng[1]);
  gnmpibuffermod2=NVAR*(p->n[0])*(p->n[1])*(p->ng[2]);

  if((p->n[0])>=(p->n[1]) &&(p->n[0])>=(p->n[2]))
    {
      if((p->n[1])>(p->n[2])) nmpibuffer=NDERV*(p->n[0])*(p->n[1])*(p->ng[0]);
      else nmpibuffer=NDERV*(p->n[0])*(p->n[2])*(p->ng[0]);
    }

  else if((p->n[1])>=(p->n[0]) && (p->n[1])>=(p->n[2]))
    {
     if((p->n[0])>(p->n[2])) nmpibuffer=NDERV*(p->n[1])*(p->n[0])*(p->ng[1]);
     else nmpibuffer=NDERV*(p->n[1])*(p->n[2])*(p->ng[1]);
    }

  else if((p->n[2])>=(p->n[0]) && (p->n[2])>=(p->n[1]))
    {
      if((p->n[0])>(p->n[1])) nmpibuffer=NDERV*(p->n[2])*(p->n[0])*(p->ng[2]);
      else nmpibuffer=NDERV*(p->n[2])*(p->n[1])*(p->ng[2]);
    }

  #else

  gnmpibuffer0=NDERV*(p->n[1])*(p->ng[0]);
  gnmpibuffer1=NDERV*(p->n[0])*(p->ng[1]);
  gnmpibuffermod0=NVAR*(p->n[1])*(p->ng[0]);
  gnmpibuffermod1=NVAR*(p->n[0])*(p->ng[1]);

  if((p->n[0])>(p->n[1])) nmpibuffer=NDERV*(p->n[0])*(p->ng[0]);
  else nmpibuffer=NDERV*(p->n[1])*(p->ng[1]);

  #endif
   
  gnmpirequest=0;
  gnmpibuffer=nmpibuffer;
  gnmpibuffermod=nmpibuffer*NVAR/NDERV;
  gmpirequest=(MPI::Request *)calloc(numbuffers,sizeof(MPI::Request));
  gmpisendbuffer=(real *)calloc(nmpibuffer,sizeof(real));
  gmpirecvbuffer=(real *)calloc(nmpibuffer*numbuffers,sizeof(real));	
     
  for(i=0;i<NDIM;i++)
    {
      gmpisrcbufferl=(real **)calloc(NDIM,sizeof(real *));
      gmpisrcbufferr=(real **)calloc(NDIM,sizeof(real *));
      gmpitgtbufferl=(real **)calloc(NDIM,sizeof(real *));
      gmpitgtbufferr=(real **)calloc(NDIM,sizeof(real *));
    }

  for(i=0;i<NDIM;i++)
    {
      switch(i)
	{
          case 0:
            #ifdef USE_SAC_3D
	    gmpisrcbufferl[i]=(real *)calloc( ((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real));
	    gmpisrcbufferr[i]=(real *)calloc( ((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real ));

            #else
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
         
            #endif
	    break;

          case 1:
            #ifdef USE_SAC_3D
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));

            #else
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));

            #endif       
            break;

            #ifdef USE_SAC_3D         
          case 2:
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
            break;
            #endif                             
        }    
 
  }    
     	
  comm.Barrier();

// Function mgpuinit() ends here.
}



void mgpuinit_stage1(params *p)
{
  int nmpibuffer;
  int numbuffers=4;
  int i;
     
  //MPI::Intracomm comm;   
  //MPI_Init(&argc, &argv);

  gwall_time = MPI_Wtime();
  comm=MPI::COMM_WORLD;
  p->npe=comm.Get_size();
  p->ipe=comm.Get_rank();

  #ifdef USE_SAC_3D

  gnmpibuffer0=NDERV*(p->n[2])*(p->n[1])*(p->ng[0]);
  gnmpibuffer1=NDERV*(p->n[0])*(p->n[2])*(p->ng[1]);
  gnmpibuffer2=NDERV*(p->n[0])*(p->n[1])*(p->ng[2]);

  gnmpibuffermod0=NVAR*(p->n[2])*(p->n[1])*(p->ng[0]);
  gnmpibuffermod1=NVAR*(p->n[0])*(p->n[2])*(p->ng[1]);
  gnmpibuffermod2=NVAR*(p->n[0])*(p->n[1])*(p->ng[2]);

  if((p->n[0])>=(p->n[1]) &&(p->n[0])>=(p->n[2]))
    {
      if((p->n[1])>(p->n[2])) nmpibuffer=NDERV*(p->n[0])*(p->n[1])*(p->ng[0]);
      else nmpibuffer=NDERV*(p->n[0])*(p->n[2])*(p->ng[0]);
    }

  else if((p->n[1])>=(p->n[0]) && (p->n[1])>=(p->n[2]))
    {
     if((p->n[0])>(p->n[2])) nmpibuffer=NDERV*(p->n[1])*(p->n[0])*(p->ng[1]);
     else nmpibuffer=NDERV*(p->n[1])*(p->n[2])*(p->ng[1]);
    }

  else if((p->n[2])>=(p->n[0]) && (p->n[2])>=(p->n[1]))
    {
      if((p->n[0])>(p->n[1])) nmpibuffer=NDERV*(p->n[2])*(p->n[0])*(p->ng[2]);
      else nmpibuffer=NDERV*(p->n[2])*(p->n[1])*(p->ng[2]);
    }

  #else

  gnmpibuffer0=NDERV*(p->n[1])*(p->ng[0]);
  gnmpibuffer1=NDERV*(p->n[0])*(p->ng[1]);
  gnmpibuffermod0=NVAR*(p->n[1])*(p->ng[0]);
  gnmpibuffermod1=NVAR*(p->n[0])*(p->ng[1]);

  if((p->n[0])>(p->n[1])) nmpibuffer=NDERV*(p->n[0])*(p->ng[0]);
  else nmpibuffer=NDERV*(p->n[1])*(p->ng[1]);

  #endif
   
  gnmpirequest=0;
  gnmpibuffer=nmpibuffer;
  gnmpibuffermod=nmpibuffer*NVAR/NDERV;
  gmpirequest=(MPI::Request *)calloc(numbuffers,sizeof(MPI::Request));
  gmpisendbuffer=(real *)calloc(nmpibuffer,sizeof(real));
  gmpirecvbuffer=(real *)calloc(nmpibuffer*numbuffers,sizeof(real));	
  
  comm.Barrier();

// Function mgpuinit() ends here.
}

void mgpuinit_stage2(params *p)
{
  int i;

  for(i=0;i<NDIM;i++)
    {
      gmpisrcbufferl=(real **)calloc(NDIM,sizeof(real *));
      gmpisrcbufferr=(real **)calloc(NDIM,sizeof(real *));
      gmpitgtbufferl=(real **)calloc(NDIM,sizeof(real *));
      gmpitgtbufferr=(real **)calloc(NDIM,sizeof(real *));
    }

  for(i=0;i<NDIM;i++)
    {
      switch(i)
	{
          case 0:
            #ifdef USE_SAC_3D
	    gmpisrcbufferl[i]=(real *)calloc( ((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real));
	    gmpisrcbufferr[i]=(real *)calloc( ((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[1])+2)*((p->n[2])+2)*(p->ng[0]),sizeof(real ));

            #else
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[1])+2)*(p->ng[0]),sizeof(real ));
         
            #endif
	    break;

          case 1:
            #ifdef USE_SAC_3D
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[2])+2)*(p->ng[1]),sizeof(real ));

            #else
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[0])+2)*(p->ng[1]),sizeof(real ));

            #endif       
            break;

            #ifdef USE_SAC_3D         
          case 2:
	    gmpisrcbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
	    gmpisrcbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
	    gmpitgtbufferl[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
	    gmpitgtbufferr[i]=(real *)calloc(((p->n[0])+2)*((p->n[1])+2)*(p->ng[2]),sizeof(real ));
            break;
            #endif                             
        }    
 
  }    
     	
  comm.Barrier();

// Function mgpuinit() ends here.
}



//!==============================================================================
//subroutine mpifinalize

//call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
//call MPI_FINALIZE(ierrmpi)

void mgpufinalize(params *p)
{
     gwall_time = MPI_Wtime() - gwall_time;
     if ((p->ipe) == 0) printf("\n Wall clock time = %f secs\n", gwall_time);
     //free(gmpisendbuffer);
     //free(gmpirecvbuffer);
     //free(gmpirequest);
     MPI_Finalize();
}





//!==============================================================================
//subroutine mpisetnpeDipeD(name)

//! Set directional processor numbers and indexes based on a filename.
//! The filename contains _np followed by np^D written with 2 digit integers.
//! For example _np0203 means np1=2, np2=3 for 2D.

//! Extract and check the directional processor numbers and indexes
//! and concat the PE number to the input and output filenames

void mgpusetnpediped(params *p, char *string)
{    
  //we don't need to call this because the values p->pnpe[0],p->pnpe[1],p->pnpe[2]
  //have been set in the params file (they should be the same as the values in the filename)
  // for SAC these vaules are read from the filename
}


//!==============================================================================
//subroutine ipe2ipeD(qipe,qipe1,qipe2)
//
//! Convert serial processor index to directional processor indexes

//integer:: qipe1,qipe2, qipe
//!-----------------------------------------------------------------------------
//qipe1 = qipe - npe1*(qipe/npe1)
//qipe2 = qipe/npe1 - npe2*(qipe/(npe1*npe2)) 

void ipe2iped(params *p)
{
int ib;
#ifdef USE_SAC_3D
//qipe1 = qipe - npe1*(qipe/npe1)
//qipe2 = qipe/npe1 - npe2*(qipe/(npe1*npe2)) 
//qipe3 = qipe/(npe1*npe2)
//(p->pipe[0])=(p->ipe)-(p->pnpe[0])*((p->ipe)/(p->pnpe[0]));
//(p->pipe[1])=((p->ipe)/(p->pnpe[0]))-(p->pnpe[1])*((p->ipe)/((p->pnpe[0])*(p->pnpe[1])));
//(p->pipe[2])=(p->ipe)/((p->pnpe[0])*(p->pnpe[1]));   

(p->pipe[2])=(p->ipe)/((p->pnpe[0])*(p->pnpe[1]));
(p->pipe[1])=((p->ipe)-((p->pipe[2])*(p->pnpe[0])*(p->pnpe[1])))/(p->pnpe[1]);
(p->pipe[0])=(p->ipe)-((p->pipe[2])*(p->pnpe[0])*(p->pnpe[1]))-((p->pipe[1])*(p->pnpe[1]));


//set upper boundary flags
//mpiupperB(1)=ipe1<npe1-1
//mpilowerB(1)=ipe1>0 

//mpiupperB(2)=ipe2<npe2-1
//mpilowerB(2)=ipe2>0 

(p->mpiupperb[0])=(p->pipe[0])<((p->pnpe[0])-1);
(p->mpiupperb[1])=(p->pipe[1])<((p->pnpe[1])-1);
(p->mpiupperb[2])=(p->pipe[2])<((p->pnpe[2])-1);

(p->mpilowerb[0])=(p->pipe[0])>0;
(p->mpilowerb[1])=(p->pipe[1])>0;
(p->mpilowerb[2])=(p->pipe[2])>0;
#else
//qipe1 = qipe - npe1*(qipe/npe1)
//qipe2 = qipe/npe1 - npe2*(qipe/(npe1*npe2)) 
//(p->pipe[0])=(p->ipe)-(p->pnpe[0])*(p->ipe)/(p->pnpe[0]);
//(p->pipe[1])=((p->ipe)/(p->pnpe[0]))-(p->pnpe[1])*(p->ipe)/((p->pnpe[0])*(p->pnpe[1]));
(p->pipe[1])=((p->ipe)/(p->pnpe[0]));
(p->pipe[0])=(p->ipe)-(p->pnpe[0])*(p->pipe[1]);

(p->mpiupperb[0])=(p->pipe[0])<((p->pnpe[0])-1);
(p->mpiupperb[1])=(p->pipe[1])<((p->pnpe[1])-1);

(p->mpilowerb[0])=(p->pipe[0])>0;
(p->mpilowerb[1])=(p->pipe[1])>0;


  if(p->ipe==0)
    for(int i=0; i<2;i++)
      printf("mpibc %d %d %d %d\n",i,p->pipe[i],p->mpiupperb[i],p->mpilowerb[i]);

#endif



    //ensure boundary set correctly 
    for(int ii=0; ii<NVAR; ii++)
    for(int idir=0; idir<NDIM; idir++)
    for(int ibound=0; ibound<2; ibound++)
    {
      // if( ((p->boundtype[ii][idir][ibound])==0) && ((p->pnpe[idir])>0) )
      //                               p->boundtype[ii][idir][ibound]=2;

      // if(  ((p->pipe[idir])==((p->pnpe[idir])-1)) && (ibound==1))
      //                               p->boundtype[ii][idir][ibound]=-1;
				     
				     
      // if(  ((p->pipe[idir])==0) && (ibound==0))
      //                               p->boundtype[ii][idir][ibound]=-1;



// deadloc with just these two
				     
       if( ((p->boundtype[ii][idir][ibound])==0) && ((p->pipe[idir])==((p->pnpe[idir])-1)) /*&& (ibound==1)*/  && ((p->pnpe[idir])>1))
                                     p->boundtype[ii][idir][ibound]=2;
       else if( ((p->boundtype[ii][idir][ibound])==0) && ((p->pipe[idir])==0) /*&& (ibound==0)*/ && ((p->pnpe[idir])>1))
                                     p->boundtype[ii][idir][ibound]=2;
       else if(    (((p->mpiupperb[idir])==1) && (p->pipe[idir])<((p->pnpe[idir])-1))   || (((p->mpiupperb[idir])!=1)  && ((p->pipe[idir])>1) )     && ((p->pnpe[idir])>1)  )
                                  p->boundtype[ii][idir][ibound]=1;				     				     
       else if(   (((p->mpilowerb[idir])==1) && (p->pipe[idir])>0)  ||    (  ((p->mpilowerb[idir])!=1)  && ((p->pipe[idir])<((p->pnpe[idir])-1))    )   && ((p->pnpe[idir])>1)   )
                                  p->boundtype[ii][idir][ibound]=1;				     





    }



  


/*boundary setting terms should eventually look like this*/
/*    
//ensure boundary set correctly 
    for(int ii=0; ii<NVAR; ii++)
    for(int idir=0; idir<NDIM; idir++)
    for(int ibound=0; ibound<2; ibound++)
    {

       if( ((p->boundtype[ii][idir][ibound])==0)   && (p->pipe[idir])==((p->pnpe[idir])-1)    && ((p->pnpe[idir])>0) && ibound==1     )
                                     p->boundtype[ii][idir][ibound]=2;
	else
                                     p->boundtype[ii][idir][ibound]=1;


       if( ((p->boundtype[ii][idir][ibound])==0)   && (p->pipe[idir])==0    && ((p->pnpe[idir])>0) && ibound==0     )
                                     p->boundtype[ii][idir][ibound]=2;
	else
                                     p->boundtype[ii][idir][ibound]=1;



	ib=p->boundtype[ii][idir][ibound];
       if( ((p->boundtype[ii][idir][ibound])>1)   && (p->pipe[idir])==((p->pnpe[idir])-1)    && ((p->pnpe[idir])>0) && ibound==1     )
                                     p->boundtype[ii][idir][ibound]=ib;
	else
                                     p->boundtype[ii][idir][ibound]=1;

	ib=p->boundtype[ii][idir][ibound];
       if( ((p->boundtype[ii][idir][ibound])>1)   && (p->pipe[idir])==0    && ((p->pnpe[idir])>0) && ibound==0     )
                                     p->boundtype[ii][idir][ibound]=ib;
	else
                                     p->boundtype[ii][idir][ibound]=1;

    }*/





}

//!==============================================================================
//subroutine ipeD2ipe(qipe1,qipe2,qipe)
//
//! Convert directional processor indexes to serial processor index

//include 'vacdef.f'

//integer:: qipe1,qipe2, qipe
//!-----------------------------------------------------------------------------
//qipe = qipe1  + npe1*qipe2

void iped2ipe(int *tpipe,int *tpnp, int *oipe)
{
  #ifdef USE_SAC_3D
  //qipe = qipe1  + npe1*qipe2  + npe1*npe2*qipe3
  (*oipe)=tpipe[0]+(tpnp[0])*(tpipe[1])+(tpnp[0])*(tpnp[1])*(tpipe[2]);
  #else
  (*oipe)=tpipe[0]+(tpnp[0])*(tpipe[1]);
  #endif

}

//!==============================================================================
//subroutine mpineighbors(idir,hpe,jpe)

//! Find the hpe and jpe processors on the left and right side of this processor 
//! in direction idir. The processor cube is taken to be periodic in every
//! direction.

//!-----------------------------------------------------------------------------
//hpe1=ipe1-kr(1,idir);hpe2=ipe2-kr(2,idir);
//jpe1=ipe1+kr(1,idir);jpe2=ipe2+kr(2,idir);

//if(hpe1<0)hpe1=npe1-1
//if(jpe1>=npe1)jpe1=0

//if(hpe2<0)hpe2=npe2-1
//if(jpe2>=npe2)jpe2=0

//call ipeD2ipe(hpe1,hpe2,hpe)
//call ipeD2ipe(jpe1,jpe2,jpe)

// Find the hpe and jpe processors on the left and right side of this processor 
// in direction idir. The processor cube is taken to be periodic in every
// direction.
void mgpuneighbours(int dir, params *p)
{
  int i;
  for(i=0; i<NDIM;i++)
    {         
      (p->phpe[i])=(p->pipe[i])-(dir==i);
      (p->pjpe[i])=(p->pipe[i])+(dir==i); 

      #ifdef USE_SAC_3D
       	(p->pkpe[i])=(p->pipe[i])+(dir==i);
      #endif             
    }

  for(i=0; i<NDIM;i++)
    {
      if((p->phpe[i])<0) (p->phpe[i])=(p->pnpe[i])-1; 
      if((p->pjpe[i])<0) (p->pjpe[i])=(p->pnpe[i])-1;

      if((p->phpe[i])>=(p->pnpe[i])) (p->phpe[i])=0; 
      if((p->pjpe[i])>=(p->pnpe[i])) (p->pjpe[i])=0; 

      #ifdef USE_SAC_3D
	if((p->pkpe[i])<0) (p->pkpe[i])=(p->pnpe[i])-1; 
        if((p->pkpe[i])>=(p->pnpe[i])) (p->pkpe[i])=0; 
      #endif                    
    }
   
  iped2ipe(p->phpe,p->pnpe,&(p->hpe));
  iped2ipe(p->pjpe,p->pnpe,&(p->jpe));

  #ifdef USE_SAC_3D
    iped2ipe(p->pkpe,p->pnpe,&(p->kpe));
  #endif       
}

//!==============================================================================
//subroutine mpisend(nvar,var,ixmin1,ixmin2,ixmax1,ixmax2,qipe,iside)
//
//! Send var(ix^L,1:nvar) to processor qipe.
//! jside is 0 for min and 1 for max side of the grid for the sending PE
void mpisend(int nvar,real *var, int *ixmin, int *ixmax  ,int qipe,int iside, int dim, params *p)
{
    int n=0;
   int ivar,i1,i2,i3,bound;
   i3=0;
;//#ifndef USE_GPUDIRECT
	switch(dim)
	{
		case 0:
		   for(ivar=0; ivar<nvar;ivar++)
		     for(i1=0;i1<=1;i1++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif

		      for(i2=0;i2<p->n[1];i2++)
		      {
			
                        bound=i1+2*(iside>0);
			 gmpisendbuffer[n]=var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)];



			//if((p->ipe==0) && ivar==rho && p->it != -1     /*&& iside==1 && (100*(p->ipe)+10*dim+iside)==101*/ )
			//{

                        //   bound=i1+2*(iside>0);
                        //    printf(" %d %d %d %lg  \n",bound,i2,i1,gmpisendbuffer[n]);

			//}
			//if((p->ipe==2) && ivar==pos2      /*&& iside==1 && (100*(p->ipe)+10*dim+iside)==101*/ )
                        //    printf(" %lg  \n",gmpisendbuffer[n]);


                         n++;






		      }
		   


		break;
		case 1:
//printf("before nsend ipe %d is %d nvar %d iside %d bound %d\n",n,p->ipe,nvar, iside,bound);

		   for(ivar=0; ivar<nvar;ivar++)
                     for(i2=0;i2<=1;i2++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif

		     for(i1=0;i1<p->n[0];i1++)
		      
		      {
			
                        bound=i2+2*(iside>0);

                     // if(n<77824)
			 gmpisendbuffer[n]=var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)];

			//if((p->ipe==0  ) && (ivar==delx1 || ivar==delx2) /* && p->it != -1 && ((p)->it)==2*/)
			//{
			 //for(int i=0;i<nvar;i++)
			 //  printf("mpiseend %d %d %d %d %lg \n",ivar, bound,i2,i1,gmpisendbuffer[n]);
                         //printf(" %d %d %d %lg ",bound,i2,i1,var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]);
                         // ;//printf(" %d %d %d %lg  %lg\n",i1,i2,iside,var[sacencodempiw1 (p,i1, i2, i3, pos1,bound)],var[sacencodempiw1 (p,i1, i2, i3, pos2,bound)]);
			 //printf("\n");
			//}
                        n++;

		      }

 		/*for(i2=0;i2<=1;i2++)
                      for(i1=0;i1<p->n[0];i1++)
			if((p->ipe==2  )   && iside==1 && (100*(p->ipe)+10*dim+iside)==101 )
			{
			 //for(int i=0;i<nvar;i++)
                           bound=i2+2*(iside>0);
			   printf(" %d %d %d %lg  %lg\n",i1,i2,iside,var[sacencodempiw1 (p,i1, i2, i3, pos1,bound)],var[sacencodempiw1 (p,i1, i2, i3, pos2,bound)]);
			// printf("\n");
			}*/

			//printf("nsend ipe %d is %d nvar %d iside %d bound %d\n",n,p->ipe,nvar, iside,bound);

		break;
		case 2:

		#ifdef USE_SAC_3D
		   for(ivar=0; ivar<nvar;ivar++)
                     for(i3=0;i3<=1;i3++)
                     for(i2=0;i2<p->n[1];i2++)
		     for(i1=0;i1<p->n[0];i1++)
		      		
						
		      {
			n++;
                        bound=i3+2*(iside>0);
			 gmpisendbuffer[n]=var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)];
		      }
		#endif

		      
		break;
	}






//if(p->ipe==1  && dim==0)
   //   printf("ipe %d send tag %d nb %d  to %d %d %d\n",p->ipe,100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0),n,qipe,iside,dim);
   
   comm.Rsend(gmpisendbuffer, n, MPI_DOUBLE_PRECISION, qipe, 100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0));
;//#else


/*	switch(dim)
	{
		case 0:
			#ifdef USE_SAC_3D
			   n = 2*nvar* (p->n[1])*(p->n[2]);
			#else
			   n = 2*nvar* (p->n[1]);
			#endif

		break;
		case 1:
			#ifdef USE_SAC_3D
			   n = 2*nvar* (p->n[0])*(p->n[2]);
			#else
			   n = 2*nvar* (p->n[0]);
			#endif

		break;
		case 2:
			#ifdef USE_SAC_3D
			   n = 2*nvar* (p->n[1])*(p->n[0]);
			#endif

		break;
	}




   comm.Rsend(var, n, MPI_DOUBLE_PRECISION, qipe, 100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0));*/

;//#endif

}





void mpisendmod(int nvar,real *var, int *ixmin, int *ixmax  ,int qipe,int iside, int dim, params *p)
{
    int n=0;
   int ivar,i1,i2,i3,bound;
   i3=0;
#ifndef USE_GPUDIRECT
	switch(dim)
	{
		case 0:
		   for(ivar=0; ivar<nvar;ivar++)
		     for(i1=0;i1<=1;i1++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif

		      for(i2=0;i2<p->n[1];i2++)
		      {
			
                        bound=i1+2*(iside>0);
			 gmpisendbuffer[n]=var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)];



			//if((p->ipe==0) && ivar==rho /*&& p->it != -1  */   && iside==1/* && (100*(p->ipe)+10*dim+iside)==101*/ )
			//{

                        //   bound=i1+2*(iside>0);
                        //    printf(" %d %d %d %d %d %lg  \n",bound,i2,i1,n,sacencodempiw0 (p,i1, i2, i3, ivar,bound),gmpisendbuffer[n]);

                        // printf("3 %d %d %d %lg  %lg\n",i1,i2,n,var[sacencodempiw0 (p,i1, i2, i3, rho,bound)],gmpisendbuffer[n]);


			//}
			//if(/*(p->ipe==2) &&*/ ivar==rhob      /*&& iside==1 && (100*(p->ipe)+10*dim+iside)==101*/ )
                      //      printf(" %lg  \n",gmpisendbuffer[n],i1,iside,bound);


                         n++;






		      }
		   


		break;
		case 1:
		   for(ivar=0; ivar<nvar;ivar++)
                     for(i2=0;i2<=1;i2++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif

		     for(i1=0;i1<p->n[0];i1++)
		      
		      {
			
                        bound=i2+2*(iside>0);
			 gmpisendbuffer[n]=var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)];
                        
			if((p->ipe==3  ) && ivar==rho  /*&& p->it != -1/* && ((p)->it)==2*/)
			{
			 //for(int i=0;i<nvar;i++)
			 //  printf("mpiseend %d %d %d %lg %lg\n",bound,i2,i1,gmpisendbuffer[n],var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]);
                         //printf(" %d %d %d %lg ",bound,i2,i1,var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]);
                         //printf("send %d %d %d %lg  %lg\n",i1,i2,iside,var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)],var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]);
			 //printf("\n");
			}

			//if(/*(p->ipe==2) &&*/ ivar==rhob  && (i1>0 && i1<15)    /*&& iside==1 && (100*(p->ipe)+10*dim+iside)==101*/ )
                        //    printf("sendmod %d %lg %d %d %d  \n",p->ipe,gmpisendbuffer[n],i1,iside,bound);

                        n++;

		      }

 		/*for(i2=0;i2<=1;i2++)
                      for(i1=0;i1<p->n[0];i1++)
			if((p->ipe==2  )   && iside==1 && (100*(p->ipe)+10*dim+iside)==101 )
			{
			 //for(int i=0;i<nvar;i++)
                           bound=i2+2*(iside>0);
			   printf(" %d %d %d %lg  %lg\n",i1,i2,iside,var[sacencodempiw1 (p,i1, i2, i3, pos1,bound)],var[sacencodempiw1 (p,i1, i2, i3, pos2,bound)]);
			// printf("\n");
			}*/

		break;
		case 2:

		#ifdef USE_SAC_3D
		   for(ivar=0; ivar<nvar;ivar++)
                     for(i3=0;i3<=1;i3++)
                     for(i2=0;p->n[1];i2++)
		     for(i1=0;i1<p->n[0];i1++)
		      		
						
		      {
			n++;
                        bound=i3+2*(iside>0);
			 gmpisendbuffer[n]=var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)];
		      }


		#endif

		      
		break;
	}






/*if(p->ipe==0  && dim==0 && iside==0)
{
  for(i1=0; i1<5120;i1++)
      printf("ipe %d send tag %d nb %d  to %d %d %d %lg %lg\n",p->ipe,100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0),n,qipe,iside,i1,gmpisendbuffer[i1],var[i1]);
  printf("end\n\n");
}*/
   
   comm.Rsend(gmpisendbuffer, n, MPI_DOUBLE_PRECISION, qipe, 100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0));

//if((p->ipe==1  )   )
//			printf("%d %d %d\n",p->ipe,iside,n);
#else


//if(p->ipe==3  && dim==0 && iside==0)
//{
//  for(i1=0; i1<120;i1++)
//      printf("ipe %d send tag %d nb %d  to %d %d %d %lg
//      \n",p->ipe,100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0),n,qipe,iside,i1,vari1[0]);
//  printf("end\n\n");
//}



	switch(dim)
	{
		case 0:
			#ifdef USE_SAC_3D
			   n = 2*nvar* (p->n[1])*(p->n[2]);
			#else
			   n = 2*nvar* (p->n[1]);
			#endif

		break;
		case 1:
			#ifdef USE_SAC_3D
			   n = 2*nvar* (p->n[0])*(p->n[2]);
			#else
			   n = 2*nvar* (p->n[0]);
			#endif

		break;
		case 2:
			#ifdef USE_SAC_3D
			   n = 2*nvar* (p->n[1])*(p->n[0]);
			#endif

		break;
	}





  //n=200*nvar;
  comm.Rsend(var, n, MPI_DOUBLE_PRECISION, qipe, 100*((p->ipe)+1)+10*(dim+1)+(iside==0?1:0));
#endif



}



//!==============================================================================
//subroutine mpirecvbuffer(nvar,ixmin1,ixmin2,ixmax1,ixmax2,qipe,iside)
//
//! receive buffer for a ghost cell region of size ix^L sent from processor qipe
//! and sent from side iside of the grid
//
//include 'vacdef.f'
//
//integer:: nvar, ixmin1,ixmin2,ixmax1,ixmax2, qipe, iside, n
//
//integer :: nmpirequest, mpirequests(2)
//integer :: mpistatus(MPI_STATUS_SIZE,2)
//common /mpirecv/ nmpirequest,mpirequests,mpistatus
//!----------------------------------------------------------------------------
void mpirecvbuffer(int nvar, real *var, int *ixmin, int *ixmax  ,int qipe,int iside,int dim, params *p)
{
int nrecv;
/*#ifdef USE_SAC_3D
   nrecv = nvar* (ixmax[0]-ixmin[0]+1)*(ixmax[1]-ixmin[1]+1)*(ixmax[2]-ixmin[2]+1);
#else
   nrecv = nvar* (ixmax[0]-ixmin[0]+1)*(ixmax[1]-ixmin[1]+1);
#endif*/

	switch(dim)
	{
		case 0:
			#ifdef USE_SAC_3D
			   nrecv = 2*nvar* (p->n[1])*(p->n[2]);
			#else
			   nrecv = 2*nvar* (p->n[1]);
			#endif
			;//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);

		break;
		case 1:
			#ifdef USE_SAC_3D
			   nrecv = 2*nvar* (p->n[0])*(p->n[2]);
			#else
			   nrecv = 2*nvar* (p->n[0]);
			#endif
                     //nrecv=10;
			;//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer1),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);

		break;
		case 2:
			#ifdef USE_SAC_3D
			   nrecv = 2*nvar* (p->n[1])*(p->n[0]);
			#endif

			;//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer2),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
	}
//printf("nrecv gmpirecv %d %d %d\n",nrecv,2*iside*gnmpibuffer1,2*gnmpibuffer);

gnmpirequest++;
//  if((p->ipe)==0  && dim==0)
//      printf("ipe %d recv tag %d nb %d  to %d  %d %d\n",p->ipe, 100*(qipe+1)+10*(dim+1)+iside/*(iside==0?1:0)*/ ,nrecv,qipe,iside,dim);

//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,10*(p->ipe)+iside);
//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,MPI_ANY_TAG);

//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);



;//#ifdef USE_GPUDIRECT
//gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);


	/*switch(dim)
	{
		case 0:
			gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffer0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside);
		break;
		case 1:
			gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffer1),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside);
		break;
		case 2:
			gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffer2),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside);
		break;
	}*/






;//#else


	switch(dim)
	{
		case 0:
			gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);

		break;
		case 1:
			gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer1),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 2:
			gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer2),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
	}





//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);

;//#endif




}





void mpirecvbuffermod(int nvar,real *var,int *ixmin, int *ixmax  ,int qipe,int iside,int dim, params *p)
{
int nrecv;
/*#ifdef USE_SAC_3D
   nrecv = nvar* (ixmax[0]-ixmin[0]+1)*(ixmax[1]-ixmin[1]+1)*(ixmax[2]-ixmin[2]+1);
#else
   nrecv = nvar* (ixmax[0]-ixmin[0]+1)*(ixmax[1]-ixmin[1]+1);
#endif*/

	switch(dim)
	{
		case 0:
			#ifdef USE_SAC_3D
			   nrecv = 2*nvar* (p->n[1])*(p->n[2]);
			#else
			   nrecv = 2*nvar* (p->n[1]);
			#endif
			//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 1:
			#ifdef USE_SAC_3D
			   nrecv = 2*nvar* (p->n[0])*(p->n[2]);
			#else
			   nrecv = 2*nvar* (p->n[0]);
			#endif
			//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod1),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 2:
			#ifdef USE_SAC_3D
			   nrecv = 2*nvar* (p->n[1])*(p->n[0]);
			#endif
			//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod2),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
	}


gnmpirequest++;
//  if((p->ipe)==0  && dim==0)
//      printf("ipe %d recv tag %d nb %d  to %d  %d %d\n",p->ipe, 100*(qipe+1)+10*(dim+1)+iside/*(iside==0?1:0)*/ ,nrecv,qipe,iside,dim);

//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,10*(p->ipe)+iside);
//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(iside*gnmpibuffer),nrecv,MPI_DOUBLE_PRECISION,qipe,MPI_ANY_TAG);


//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(iside*gnmpibuffermod),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);

#ifdef USE_GPUDIRECT
	//gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffermod0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
	//gmpirequest[gnmpirequest]=comm.Irecv(var,nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
	
	switch(dim)
	{
		case 0:
			gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffermod0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 1:
			gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffermod1),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 2:
			gmpirequest[gnmpirequest]=comm.Irecv(var+(2*iside*gnmpibuffermod2),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
	}

#else

	//gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
	switch(dim)
	{
		case 0:
			gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod0),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 1:
			gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod1),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
		case 2:
			gmpirequest[gnmpirequest]=comm.Irecv(gmpirecvbuffer+(2*iside*gnmpibuffermod2),nrecv,MPI_DOUBLE_PRECISION,qipe,100*(qipe+1)+10*(dim+1)+iside/**(iside==0?1:0)*/);
		break;
	}

#endif




}




//!==============================================================================
//subroutine mpibuffer2var(iside,nvar,var,ixmin1,ixmin2,ixmax1,ixmax2)
//
//! Copy mpibuffer(:,iside) into var(ix^L,1:nvar)
//include 'vacdef.f'
//
//integer :: nvar
//double precision:: var(ixGlo1:ixGhi1,ixGlo2:ixGhi2,nvar)
//integer:: ixmin1,ixmin2,ixmax1,ixmax2,iside,n,ix1,ix2,ivar
//!-----------------------------------------------------------------------------
void mpibuffer2var(int iside,int nvar,real *var, int *ixmin, int *ixmax, int dim, params *p)
{
   int n=0;
   int ivar,i1,i2,i3,bound;
   i3=0;
//printf("mpibuffertovar %d %d %d %d \n",p->ipe,dim,iside,nvar);
	switch(dim)
	{
		case 0:
		   for(ivar=0; ivar<nvar;ivar++)
		     for(i1=0;i1<=1;i1++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif
		      for(i2=0;i2<p->n[1];i2++)

		      {
			
                        //bound=i1+iside+2*(iside>0);
                        bound=i1+2*(iside==0?1:0);
			// var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n/*+iside*gnmpibuffer*/];

//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
 			var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer0];
			//var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffer];
                        //n++;
                       //iside=1;
                       //if(/*iside==0  && p->it != -1 &&*/ p->ipe==0  && ivar==rho /*&& (100+10*dim+(iside==0?1:0))==101*/)
                       //   printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer]);
                       n++;
                         
			//printf("\n");

		      }
                      n=0;
                   /* if(p->ipe==2)
			// for(i1=0;i1<=1;i1++)
			 //    for(i2=0;i2<p->n[1];i2++)
                            for(i1=0; i1<4*gnmpibuffer; i1++)
                             {
                               
                                 printf("%d %d %d %lg\n",i1 ,i2 , iside,gmpirecvbuffer[i1]);
                                 n++;
                             }*/
		break;
		case 1:
		   for(ivar=0; ivar<nvar;ivar++)
                    for(i2=0;i2<=1;i2++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif
		     for(i1=0;i1<p->n[0];i1++)
		      

		      {
			
                        bound=i2+2*(iside==0?1:0);
//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
			 var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer1];
			// var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffer];



			//if(/*iside==0  && p->it != -1 &&*/ p->ipe==0  && (ivar==delx1  || ivar==delx2) /*&& (100+10*dim+(iside==0?1:0))==101*/)
                        //  printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer1]);


                      // if(/*iside==0  && p->it != -1 &&*/ p->ipe==0  && ivar==rho /*&& (100+10*dim+(iside==0?1:0))==101*/)
                      //    printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]);

                     //   if(/*iside==0  &&*/ p->ipe==0  && (ivar==pos1 || ivar==pos2) && (i1>1000 && i1<1010) /*&& (100+10*dim+(iside==0?1:0))==101*/)
                      //    printf("v %d %d %d %d %d %lg\n",n,ivar,i1 ,i2 , iside,gmpirecvbuffer[n+iside*gnmpibuffer]);

                        n++;

		      }

		break;
		case 2:

		#ifdef USE_SAC_3D
		   for(ivar=0; ivar<nvar;ivar++)
			for(i3=0;i3<=1;i3++)			
		      for(i2=0;p->n[1];i2++)		
		     for(i1=0;i1<p->n[0];i1++)
		      {
			
                        bound=i3+2*(iside==0?1:0);

//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
			 var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer2];
                      //var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffer];

                          n++;
		      }


		#endif

		      
		break;
	}
}



void mpibuffer2varmod(int iside,int nvar,real *var, int *ixmin, int *ixmax, int dim, params *p)
{
   int n=0;
   int ivar,i1,i2,i3,bound;
   i3=0;
                 //  ivar=0;
                //  for(n=0;n<gnmpibuffermod;n++)
                 //     if(dim==0  &&/* p->it != -1 &&*/ p->ipe==0  /*&& ivar==rho && (100+10*dim+(iside==0?1:0))==101*/){
                  //         printf("mpib2var %d %d %lg\n" , iside,n,gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod]);
			// printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]);
                      //   printf("\n");
                   //    }
    

         n=0;

	switch(dim)
	{
		case 0:
		   for(ivar=0; ivar<nvar;ivar++)
		     for(i1=0;i1<=1;i1++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif
		      for(i2=0;i2<p->n[1];i2++)

		      {
			
                        //bound=i1+iside+2*(iside>0);
                        bound=i1+2*(iside==0?1:0);
			// var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n/*+iside*gnmpibuffer*/];
 			//var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffer/nvar];
                       // var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer];

//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
                        var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod0];
 			//var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffermod];

                        //n++;
                       //iside=1;
                      // if(/*iside==0  && p->it != -1 &&*/ p->ipe==0  && ivar==rho /*&& (100+10*dim+(iside==0?1:0))==101*/){
                      //    printf("mpib2vart %d %d %d %d %lg\n",i1 ,i2 , iside,n,gmpirecvbuffer[n+2*iside*gnmpibuffer]);
			// printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,var[sacencodempiw0 (p,i1, i2, i3, ivar,bound)]);
                      //   printf("\n");
                     //  }



                       n++;
                         
			

		      }
                     // n=0;
                   /* if(p->ipe==0)
			// for(i1=0;i1<=1;i1++)
			 //    for(i2=0;i2<p->n[1];i2++)
                            for(i1=0; i1<4*gnmpibuffer; i1++)
                             {

                               
                                 printf("%d %d %d %lg\n",i1 ,i2 , iside,gmpirecvbuffer[i1]);
                                 n++;
                             }*/
		break;
		case 1:
		   for(ivar=0; ivar<nvar;ivar++)
                    for(i2=0;i2<=1;i2++)
		#ifdef USE_SAC_3D
			for(i3=0;i3<p->n[2];i3++)
		#endif
		     for(i1=0;i1<p->n[0];i1++)
		      

		      {
			
                        bound=i2+2*(iside==0?1:0);


//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue

			if((p->ipe==3) && ivar==rho /* && (i1>0 && i1<15)*/    /*&& iside==1 && (100*(p->ipe)+10*dim+iside)==101*/ )
                            //printf("recvmod %d %lg %d %d %d %d  \n",p->ipe,gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod1],n,i1,iside,bound);


			var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod1];
			//var[sacencodempiw1 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffermod];


			//if(/*iside==0  && p->it != -1 && p->ipe==0  &&*/ ivar==rho /*&& (100+10*dim+(iside==0?1:0))==101*/  &&   (gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod1]==0))
		//gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod1]=0.221049;
                          



                       //if(/*iside==0  && p->it != -1 &&*/ p->ipe==0  && ivar==rho /*&& (100+10*dim+(iside==0?1:0))==101*/){
                       //   printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod1]);
                       //   printf("\n");
                       // }


			//if(/*iside==0  && p->it != -1 &&*/ p->ipe==0  && ivar==rho /*&& (100+10*dim+(iside==0?1:0))==101*/)
                        //  printf("mpib2var %d %d %d %lg\n",i1 ,i2 , iside,gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod1]);



                       // if(/*iside==0  &&*/ p->ipe==2  && ivar==pos1 || ivar==pos2 /*&& (100+10*dim+(iside==0?1:0))==101*/)
                       //   printf("v %d %d %d %d %lg\n",ivar,i1 ,i2 , iside,gmpirecvbuffer[n+iside*gnmpibuffer]);




                        n++;

		      }

		break;
		case 2:

		#ifdef USE_SAC_3D
		   for(ivar=0; ivar<nvar;ivar++)
			for(i3=0;i3<=1;i3++)			
		      for(i2=0;p->n[1];i2++)		
		     for(i1=0;i1<p->n[0];i1++)
		      {
			
                        bound=i3+2*(iside==0?1:0);
			// var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffer];

//with ni <> nj code fails at this point
// the second commented line "fixes" this memory issue
 			var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+2*(iside==0?1:0)*gnmpibuffermod2];
			//var[sacencodempiw2 (p,i1, i2, i3, ivar,bound)]=gmpirecvbuffer[n+(iside==0?1:0)*gnmpibuffermod];

                          n++;
		      }


		#endif

		      
		break;
	}





}




void gpusync()
{
//printf("mpisync\n");
comm.Barrier();
}









//!==============================================================================
//subroutine mpibound(nvar,var)
//
//! Fill in ghost cells of var(ixG,nvar) from other processors
//
//include 'vacdef.f'
//
//integer :: nvar
//double precision :: var(ixGlo1:ixGhi1,ixGlo2:ixGhi2,nvar)
//
//! processor indexes for left and right neighbors
//integer :: hpe,jpe
//! index limits for the left and right side mesh and ghost cells 
//integer :: ixLMmin1,ixLMmin2,ixLMmax1,ixLMmax2, ixRMmin1,ixRMmin2,ixRMmax1,&
//   ixRMmax2, ixLGmin1,ixLGmin2,ixLGmax1,ixLGmax2, ixRGmin1,ixRGmin2,ixRGmax1,&
//   ixRGmax2
//logical :: periodic


void mpibound(int nvar,  real *var1, real *var2, real *var3,  real *rvar1, real *rvar2, real *rvar3, params *p, int idir)
{
   int i;

   int ixlgmin[NDIM],ixlgmax[NDIM];
   int ixrgmin[NDIM],ixrgmax[NDIM];

   int ixlmmin[NDIM],ixlmmax[NDIM];
   int ixrmmin[NDIM],ixrmmax[NDIM];


if((p->pnpe[0])>1   && idir==0)
{
   gnmpirequest=0;
   for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   //periodic=typeB(1,2*1)=='mpiperiod'
   //! Left and right side ghost cell regions (target)
   for(i=0;i<NDIM;i++)
   {
	ixlgmin[i]=0;
	ixlgmax[i]=(p->n[i])-1;
	ixrgmin[i]=0;
	ixrgmax[i]=(p->n[i])-1;
   }
      ixlgmax[0]=1;
   ixrgmin[0]=(p->n[0])-2;
   //! Left and right side mesh cell regions (source)
   for(i=0;i<NDIM;i++)
   {
	ixlmmin[i]=0;
	ixlmmax[i]=(p->n[i])-1;
	ixrmmin[i]=0;
	ixrmmax[i]=(p->n[i])-1;
   }
   ixlmmin[0]=2;   
   ixlmmax[0]=1;
   ixrmmax[0]=(p->n[0])-3;
   ixrmmin[0]=(p->n[0])-4; 
     //! Obtain left and right neighbor processors for this direction
   //call mpineighbors(1,hpe,jpe)
   
   mgpuneighbours(0, p);
   //already computed initially use phpe pjpe arrays



//#ifndef USE_GPUDIRECT
   //! receive right (2) boundary from left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpirecvbuffer(nvar,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,hpe,2)*/

   //if(p->ipe==0)
   //  printf("ipe %d  recv right (from left) %d recv left (from right neigh) %d\n",p->ipe,p->hpe,p->jpe);
   if(((p->mpilowerb[0])==1) ||  /*((p->boundtype[0][0][0])==0)||*/  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
	mpirecvbuffer(nvar,rvar1,ixrmmin,ixrmmax,p->hpe,0,0,p);

   //! receive left (1) boundary from right neighbor jpe
   //if(mpiupperB(1) .or. periodic)call mpirecvbuffer(nvar,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,jpe,1)
   if(((p->mpiupperb[0])==1) || /* ((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffer(nvar,rvar1,ixlmmin,ixlmmax,p->jpe,1,0,p);
//#endif

  
   //! Wait for all receives to be posted
   //call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
//printf("ipe %d barrier before send\n",p->ipe);
   comm.Barrier();
   
   //! Ready send left (1) boundary to left neighbor hpe

   //if(mpilowerB(1) .or. periodic)call mpisend(nvar,var,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,hpe,1)
   if(((p->mpilowerb[0])==1) ||  /*((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
            mpisend(nvar,var1,ixlmmin,ixlmmax,p->hpe,0,0,p);

   //! Ready send right (2) boundary to right neighbor
   //if(mpiupperB(1) .or. periodic)call mpisend(nvar,var,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,jpe,2)
   if(((p->mpiupperb[0])==1) || /* ((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpisend(nvar,var1,ixrmmin,ixrmmax,p->jpe,1,0,p);

   //! Wait for messages to arrive
   //call MPI_WAITALL(nmpirequest,mpirequests,mpistatus,ierrmpi)
  
   request.Waitall(gnmpirequest,gmpirequest);
;//#ifndef USE_GPUDIRECT
   //! Copy buffer received from right (2) physical cells into left ghost cells
   //if(mpilowerB(1) .or. periodic)call mpibuffer2var(2,nvar,var,ixLGmin1,&
   //   ixLGmin2,ixLGmax1,ixLGmax2)
   if(((p->mpilowerb[0])==1) ||  /*((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2var(1,nvar,var1,ixlgmin,ixlgmax,0,p);
   //! Copy buffer received from left (1) physical cells into right ghost cells
   //if(mpiupperB(1) .or. periodic)call mpibuffer2var(1,nvar,var,ixRGmin1,&
   //   ixRGmin2,ixRGmax1,ixRGmax2)
   if(((p->mpiupperb[0])==1) || /* ((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2var(0,nvar,var1,ixrgmin,ixrgmax,0,p);    
;//#endif  
 

}



   comm.Barrier();


//printf("to here1 %d\n");
if((p->pnpe[1])>1   && idir==1)
{
  gnmpirequest=0;  
  for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   //periodic=typeB(1,2*1)=='mpiperiod'
   //! Left and right side ghost cell regions (target)
   for(i=0;i<NDIM;i++)
   {
	ixlgmin[i]=0;
	ixlgmax[i]=(p->n[i])-1;
	ixrgmin[i]=0;
	ixrgmax[i]=(p->n[i])-1;
   }
   ixlgmax[1]=1;
   ixrgmin[1]=(p->n[1])-2;
   //! Left and right side mesh cell regions (source)
   for(i=0;i<NDIM;i++)
   {
	ixlmmin[i]=0;
	ixlmmax[i]=(p->n[i])-1;
	ixrmmin[i]=0;
	ixrmmax[i]=(p->n[i])-1;
   }
   ixlmmin[1]=2;   
   ixlmmax[1]=1;
   ixrmmax[1]=(p->n[1])-3;
   ixrmmin[1]=(p->n[1])-4; 
     //! Obtain left and right neighbor processors for this direction
   //call mpineighbors(1,hpe,jpe)
   mgpuneighbours(1, p);

   //already computed initially use phpe pjpe arrays
//printf("to here2 %d\n");

   //! receive right (2) boundary from left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpirecvbuffer(nvar,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,hpe,2)
   if(((p->mpilowerb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffer(nvar,rvar2,ixrmmin,ixrmmax,p->hpe,0,1,p);
   //! receive left (1) boundary from right neighbor jpe
   //if(mpiupperB(1) .or. periodic)call mpirecvbuffer(nvar,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,jpe,1)
   if(((p->mpiupperb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffer(nvar,rvar2,ixlmmin,ixlmmax,p->jpe,1,1,p);
   //! Wait for all receives to be posted
   //call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
   comm.Barrier();
//printf("to here3 %d\n");

   //! Ready send left (1) boundary to left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpisend(nvar,var,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,hpe,1)
   if(((p->mpilowerb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpisend(nvar,var2,ixlmmin,ixlmmax,p->hpe,0,1,p);
   //! Ready send right (2) boundary to right neighbor
   //if(mpiupperB(1) .or. periodic)call mpisend(nvar,var,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,jpe,2)
   if(((p->mpiupperb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpisend(nvar,var2,ixrmmin,ixrmmax,p->jpe,1,1,p);
   //! Wait for messages to arrive
   //call MPI_WAITALL(nmpirequest,mpirequests,mpistatus,ierrmpi)
   request.Waitall(gnmpirequest,gmpirequest);
;//#ifndef USE_GPUDIRECT
   //! Copy buffer received from right (2) physical cells into left ghost cells
   //if(mpilowerB(1) .or. periodic)call mpibuffer2var(2,nvar,var,ixLGmin1,&
   //   ixLGmin2,ixLGmax1,ixLGmax2)
   if(((p->mpilowerb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2var(1,nvar,var2,ixlgmin,ixlgmax,1,p);
   //! Copy buffer received from left (1) physical cells into right ghost cells
   //if(mpiupperB(1) .or. periodic)call mpibuffer2var(1,nvar,var,ixRGmin1,&
   //   ixRGmin2,ixRGmax1,ixRGmax2)
   if(((p->mpiupperb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2))
             mpibuffer2var(0,nvar,var2,ixrgmin,ixrgmax,1,p);
	    ;// #endif
}
//printf("before barrier %d\n",p->ipe);
 comm.Barrier();

//printf("after barrier %d\n",p->ipe);



#ifdef USE_SAC_3D
 comm.Barrier();
if((p->pnpe[2])>1)
{
  gnmpirequest=0;  
  for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   //periodic=typeB(1,2*1)=='mpiperiod'
   //! Left and right side ghost cell regions (target)
   for(i=0;i<NDIM;i++)
   {
	ixlgmin[i]=0;
	ixlgmax[i]=(p->n[i])-1;
	ixrgmin[i]=0;
	ixrgmax[i]=(p->n[i])-1;
   }
   ixlgmax[2]=1;
   ixrgmin[2]=(p->n[2])-2;
   //! Left and right side mesh cell regions (source)
   for(i=0;i<NDIM;i++)
   {
	ixlmmin[i]=0;
	ixlmmax[i]=(p->n[i])-1;
	ixrmmin[i]=0;
	ixrmmax[i]=(p->n[i])-1;
   }
   ixlmmin[2]=2;   
   ixlmmax[2]=1;
   ixrmmax[2]=(p->n[2])-3;
   ixrmmin[2]=(p->n[2])-4; 
     //! Obtain left and right neighbor processors for this direction
   //call mpineighbors(1,hpe,jpe)
   mgpuneighbours(2, p);

   //already computed initially use phpe pjpe arrays

   //! receive right (2) boundary from left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpirecvbuffer(nvar,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,hpe,2)
   if(((p->mpilowerb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffer(nvar,rvar3,ixrmmin,ixrmmax,p->hpe,0,2,p);
   //! receive left (1) boundary from right neighbor jpe
   //if(mpiupperB(1) .or. periodic)call mpirecvbuffer(nvar,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,jpe,1)
   if(((p->mpiupperb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffer(nvar,rvar3,ixlmmin,ixlmmax,p->jpe,1,2,p);
   //! Wait for all receives to be posted
   //call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
   comm.Barrier();
   //! Ready send left (1) boundary to left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpisend(nvar,var,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,hpe,1)
   if(((p->mpilowerb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpisend(nvar,var3,ixlmmin,ixlmmax,p->hpe,0,2,p);
   //! Ready send right (2) boundary to right neighbor
   //if(mpiupperB(1) .or. periodic)call mpisend(nvar,var,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,jpe,2)
   if(((p->mpiupperb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpisend(nvar,var3,ixrmmin,ixrmmax,p->jpe,1,2,p);
   //! Wait for messages to arrive
   //call MPI_WAITALL(nmpirequest,mpirequests,mpistatus,ierrmpi)
   request.Waitall(gnmpirequest,gmpirequest);
#ifndef USE_GPUDIRECT
   //! Copy buffer received from right (2) physical cells into left ghost cells
   //if(mpilowerB(1) .or. periodic)call mpibuffer2var(2,nvar,var,ixLGmin1,&
   //   ixLGmin2,ixLGmax1,ixLGmax2)
   if(((p->mpilowerb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2var(1,nvar,var3,ixlgmin,ixlgmax,2,p);
   //! Copy buffer received from left (1) physical cells into right ghost cells
   //if(mpiupperB(1) .or. periodic)call mpibuffer2var(1,nvar,var,ixRGmin1,&
   //   ixRGmin2,ixRGmax1,ixRGmax2)
   if(((p->mpiupperb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2var(0,nvar,var3,ixrgmin,ixrgmax,2,p);
#endif	     
}


#endif


}






void mpiboundmod(int nvar,  real *var1, real *var2, real *var3,  real *rvar1, real *rvar2, real *rvar3, params *p, int idir)
{
   int i;

   int ixlgmin[NDIM],ixlgmax[NDIM];
   int ixrgmin[NDIM],ixrgmax[NDIM];

   int ixlmmin[NDIM],ixlmmax[NDIM];
   int ixrmmin[NDIM],ixrmmax[NDIM];


if((p->pnpe[0])>1   && idir==0)
{
   gnmpirequest=0;
   for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   //periodic=typeB(1,2*1)=='mpiperiod'
   //! Left and right side ghost cell regions (target)
   for(i=0;i<NDIM;i++)
   {
	ixlgmin[i]=0;
	ixlgmax[i]=(p->n[i])-1;
	ixrgmin[i]=0;
	ixrgmax[i]=(p->n[i])-1;
   }
      ixlgmax[0]=1;
   ixrgmin[0]=(p->n[0])-2;
   //! Left and right side mesh cell regions (source)
   for(i=0;i<NDIM;i++)
   {
	ixlmmin[i]=0;
	ixlmmax[i]=(p->n[i])-1;
	ixrmmin[i]=0;
	ixrmmax[i]=(p->n[i])-1;
   }
   ixlmmin[0]=2;   
   ixlmmax[0]=1;
   ixrmmax[0]=(p->n[0])-3;
   ixrmmin[0]=(p->n[0])-4; 
     //! Obtain left and right neighbor processors for this direction
   //call mpineighbors(1,hpe,jpe)
   
   mgpuneighbours(0, p);
   //already computed initially use phpe pjpe arrays

   //! receive right (2) boundary from left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpirecvbuffer(nvar,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,hpe,2)*/

   //if(p->ipe==0)
   //  printf("ipe %d  recv right (from left) %d recv left (from right neigh) %d\n",p->ipe,p->hpe,p->jpe);
   if(((p->mpilowerb[0])==1) ||  /*((p->boundtype[0][0][0])==0)||*/  ((p->boundtype[0][0][0])==2)  /*|| ((p->boundtype[0][0][0])==1)*/ )
	mpirecvbuffermod(nvar,rvar1,ixrmmin,ixrmmax,p->hpe,0,0,p);

   //! receive left (1) boundary from right neighbor jpe
   //if(mpiupperB(1) .or. periodic)call mpirecvbuffer(nvar,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,jpe,1)
   if(((p->mpiupperb[0])==1) || /* ((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffermod(nvar,rvar1,ixlmmin,ixlmmax,p->jpe,1,0,p);


  
   //! Wait for all receives to be posted
   //call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
//printf("ipe %d barrier before send\n",p->ipe);
   comm.Barrier();
   
   //! Ready send left (1) boundary to left neighbor hpe

   //if(mpilowerB(1) .or. periodic)call mpisend(nvar,var,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,hpe,1)
   if(((p->mpilowerb[0])==1) ||  /*((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
            mpisendmod(nvar,var1,ixlmmin,ixlmmax,p->hpe,0,0,p);

   //! Ready send right (2) boundary to right neighbor
   //if(mpiupperB(1) .or. periodic)call mpisend(nvar,var,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,jpe,2)
   if(((p->mpiupperb[0])==1) || /* ((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/ )
             mpisendmod(nvar,var1,ixrmmin,ixrmmax,p->jpe,1,0,p);

   //! Wait for messages to arrive
   //call MPI_WAITALL(nmpirequest,mpirequests,mpistatus,ierrmpi)
  
   request.Waitall(gnmpirequest,gmpirequest);

   //! Copy buffer received from right (2) physical cells into left ghost cells
   //if(mpilowerB(1) .or. periodic)call mpibuffer2var(2,nvar,var,ixLGmin1,&
   //   ixLGmin2,ixLGmax1,ixLGmax2)
 #ifndef USE_GPUDIRECT  
//comm.Barrier();
if(((p->mpilowerb[0])==1) ||  /*((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
   {
     //if((p->ipe)==0)
      //  printf("lowerb");
             mpibuffer2varmod(1,nvar,var1,ixlgmin,ixlgmax,0,p);
   }
   //! Copy buffer received from left (1) physical cells into right ghost cells
   //if(mpiupperB(1) .or. periodic)call mpibuffer2var(1,nvar,var,ixRGmin1,&
   //   ixRGmin2,ixRGmax1,ixRGmax2)
//comm.Barrier();
   if(((p->mpiupperb[0])==1) || /* ((p->boundtype[0][0][0])==0)|| */ ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
   {
     //if((p->ipe)==0)
     //   printf("upperb");
             mpibuffer2varmod(0,nvar,var1,ixrgmin,ixrgmax,0,p);  
   }  
  #endif
 

}



   comm.Barrier();


;//printf("mpimod to here1 %d\n", p->ipe);
if((p->pnpe[1])>1   && idir==1)
{
  gnmpirequest=0;  
  for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   //periodic=typeB(1,2*1)=='mpiperiod'
   //! Left and right side ghost cell regions (target)
   for(i=0;i<NDIM;i++)
   {
	ixlgmin[i]=0;
	ixlgmax[i]=(p->n[i])-1;
	ixrgmin[i]=0;
	ixrgmax[i]=(p->n[i])-1;
   }
   ixlgmax[1]=1;
   ixrgmin[1]=(p->n[1])-2;
   //! Left and right side mesh cell regions (source)
   for(i=0;i<NDIM;i++)
   {
	ixlmmin[i]=0;
	ixlmmax[i]=(p->n[i])-1;
	ixrmmin[i]=0;
	ixrmmax[i]=(p->n[i])-1;
   }
   ixlmmin[1]=2;   
   ixlmmax[1]=1;
   ixrmmax[1]=(p->n[1])-3;
   ixrmmin[1]=(p->n[1])-4; 
     //! Obtain left and right neighbor processors for this direction
   //call mpineighbors(1,hpe,jpe)
   mgpuneighbours(1, p);

   //already computed initially use phpe pjpe arrays

   //! receive right (2) boundary from left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpirecvbuffer(nvar,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,hpe,2)
   if(((p->mpilowerb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
   {
          //printf("recv mpilowerb==1 ipe=%d %d %d %d\n",p->ipe,p->hpe,p->boundtype[0][0][0],p->mpilowerb[1]);
             mpirecvbuffermod(nvar,rvar2,ixrmmin,ixrmmax,p->hpe,0,1,p);
   }
   //! receive left (1) boundary from right neighbor jpe
   //if(mpiupperB(1) .or. periodic)call mpirecvbuffer(nvar,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,jpe,1)
   if(((p->mpiupperb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
   {
          //printf("recv mpupperrb==1 ipe=%d %d %d %d\n",p->ipe,p->jpe,p->boundtype[0][0][0],p->mpiupperb[1]);

             mpirecvbuffermod(nvar,rvar2,ixlmmin,ixlmmax,p->jpe,1,1,p);

   }
   //! Wait for all receives to be posted
   //call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
   comm.Barrier();
   //! Ready send left (1) boundary to left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpisend(nvar,var,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,hpe,1)
   if(((p->mpilowerb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
{
;//printf("send mpilowerb==1 ipe=%d %d %d %d\n",p->ipe,p->hpe,p->boundtype[0][0][0],p->mpilowerb[1]);

             mpisendmod(nvar,var2,ixlmmin,ixlmmax,p->hpe,0,1,p);
}
   //! Ready send right (2) boundary to right neighbor
   //if(mpiupperB(1) .or. periodic)call mpisend(nvar,var,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,jpe,2)
   if(((p->mpiupperb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
{
;//printf("send mpiupperb==1 ipe=%d %d %d %d\n",p->ipe,p->jpe,p->boundtype[0][0][0],p->mpiupperb[1]);

             mpisendmod(nvar,var2,ixrmmin,ixrmmax,p->jpe,1,1,p);
}
   //! Wait for messages to arrive
   //call MPI_WAITALL(nmpirequest,mpirequests,mpistatus,ierrmpi)
   request.Waitall(gnmpirequest,gmpirequest);


#ifndef USE_GPUDIRECT
   //! Copy buffer received from right (2) physical cells into left ghost cells
   //if(mpilowerB(1) .or. periodic)call mpibuffer2var(2,nvar,var,ixLGmin1,&
   //   ixLGmin2,ixLGmax1,ixLGmax2)
   if(((p->mpilowerb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2varmod(1,nvar,var2,ixlgmin,ixlgmax,1,p);
   //! Copy buffer received from left (1) physical cells into right ghost cells
   //if(mpiupperB(1) .or. periodic)call mpibuffer2var(1,nvar,var,ixRGmin1,&
   //   ixRGmin2,ixRGmax1,ixRGmax2)
   if(((p->mpiupperb[1])==1) ||  /*((p->boundtype[0][1][0])==0)||*/  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2varmod(0,nvar,var2,ixrgmin,ixrgmax,1,p);

#endif

}

 comm.Barrier();




#ifdef USE_SAC_3D
 comm.Barrier();
if((p->pnpe[2])>1)
{
  gnmpirequest=0;  
  for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   //periodic=typeB(1,2*1)=='mpiperiod'
   //! Left and right side ghost cell regions (target)
   for(i=0;i<NDIM;i++)
   {
	ixlgmin[i]=0;

	ixlgmax[i]=(p->n[i])-1;
	ixrgmin[i]=0;
	ixrgmax[i]=(p->n[i])-1;
   }
   ixlgmax[2]=1;
   ixrgmin[2]=(p->n[2])-2;
   //! Left and right side mesh cell regions (source)
   for(i=0;i<NDIM;i++)
   {
	ixlmmin[i]=0;
	ixlmmax[i]=(p->n[i])-1;
	ixrmmin[i]=0;
	ixrmmax[i]=(p->n[i])-1;
   }
   ixlmmin[2]=2;   
   ixlmmax[2]=1;
   ixrmmax[2]=(p->n[2])-3;
   ixrmmin[2]=(p->n[2])-4; 
     //! Obtain left and right neighbor processors for this direction
   //call mpineighbors(1,hpe,jpe)
   mgpuneighbours(2, p);

   //already computed initially use phpe pjpe arrays

   //! receive right (2) boundary from left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpirecvbuffer(nvar,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,hpe,2)
   if(((p->mpilowerb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffermod(nvar,rvar3,ixrmmin,ixrmmax,p->hpe,0,2,p);
   //! receive left (1) boundary from right neighbor jpe
   //if(mpiupperB(1) .or. periodic)call mpirecvbuffer(nvar,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,jpe,1)
   if(((p->mpiupperb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpirecvbuffermod(nvar,rvar3,ixlmmin,ixlmmax,p->jpe,1,2,p);
   //! Wait for all receives to be posted
   //call MPI_BARRIER(MPI_COMM_WORLD,ierrmpi)
   comm.Barrier();
   //! Ready send left (1) boundary to left neighbor hpe
   //if(mpilowerB(1) .or. periodic)call mpisend(nvar,var,ixLMmin1,ixLMmin2,&
   //   ixLMmax1,ixLMmax2,hpe,1)
   if(((p->mpilowerb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpisendmod(nvar,var3,ixlmmin,ixlmmax,p->hpe,0,2,p);
   //! Ready send right (2) boundary to right neighbor
   //if(mpiupperB(1) .or. periodic)call mpisend(nvar,var,ixRMmin1,ixRMmin2,&
   //   ixRMmax1,ixRMmax2,jpe,2)
   if(((p->mpiupperb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2) /*|| ((p->boundtype[0][0][0])==1)*/)
             mpisendmod(nvar,var3,ixrmmin,ixrmmax,p->jpe,1,2,p);
   //! Wait for messages to arrive
   //call MPI_WAITALL(nmpirequest,mpirequests,mpistatus,ierrmpi)
   request.Waitall(gnmpirequest,gmpirequest);
#ifndef USE_GPUDIRECT
   //! Copy buffer received from right (2) physical cells into left ghost cells
   //if(mpilowerB(1) .or. periodic)call mpibuffer2var(2,nvar,var,ixLGmin1,&
   //   ixLGmin2,ixLGmax1,ixLGmax2)
   if(((p->mpilowerb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2varmod(1,nvar,var3,ixlgmin,ixlgmax,2,p);
   //! Copy buffer received from left (1) physical cells into right ghost cells
   //if(mpiupperB(1) .or. periodic)call mpibuffer2var(1,nvar,var,ixRGmin1,&
   //   ixRGmin2,ixRGmax1,ixRGmax2)
   if(((p->mpiupperb[2])==1) ||  ((p->boundtype[0][2][0])==0)||  ((p->boundtype[0][0][0])==2)/*|| ((p->boundtype[0][0][0])==1)*/)
             mpibuffer2varmod(0,nvar,var3,ixrgmin,ixrgmax,2,p);
#endif
}


#endif


}




//!=============================================================================
//subroutine mpireduce(a,mpifunc)
//
//! reduce input for one PE 0 using mpifunc
//
//include 'mpif.h'
//
//double precision :: a, alocal
//integer          :: mpifunc, ierrmpi
//!----------------------------------------------------------------------------
//alocal = a
//call MPI_REDUCE(alocal,a,1,MPI_DOUBLE_PRECISION,mpifunc,0,MPI_COMM_WORLD,&
//   ierrmpi)
void mpireduce(real *a, MPI::Op mpifunc)
{
  real alocal;

   alocal=*a;
   comm.Reduce(&alocal, a, 1, MPI_DOUBLE_PRECISION, mpifunc, 0);

}



//!==============================================================================
//subroutine mpiallreduce(a,mpifunc)
//
//! reduce input onto all PE-s using mpifunc
//
//include 'mpif.h'
//
//double precision :: a, alocal
//integer          :: mpifunc, ierrmpi
//!-----------------------------------------------------------------------------
//alocal = a
//call MPI_ALLREDUCE(alocal,a,1,MPI_DOUBLE_PRECISION,mpifunc,MPI_COMM_WORLD,&
//   ierrmpi)
void mpiallreduce(real *a, MPI::Op mpifunc)
{
   real alocal;

   alocal=*a;
   comm.Allreduce(&alocal, a, 1, MPI_DOUBLE_PRECISION, mpifunc);

}

//update viscosity term
void mpivisc( int idim,params *p, real *var1, real *var2, real *var3)
{
   comm.Barrier();

   int i,n;
   int i1,i2,i3;
   int bound;

   i3=0;

   #ifdef USE_SAC_3D

   // Define the size
   n=(p->n[0])*(p->n[1])*(p->n[2]);

   switch(idim)
     {
     case 0:
       n/=(p->n[0]);
       break;
     case 1:
       n/=(p->n[1]);
       break;
     case 2:
       n/=(p->n[2]);
       break;               
     }
   #else

   n=(p->n[0])*(p->n[1]);

   //int iolowerb=p->mpilowerb[idim];
   //int ioupperb=p->mpiupperb[idim];

   //p->mpilowerb[idim]=1;
   //p->mpiupperb[idim]=1;

   switch(idim)
     {
     case 0:
       n/=(p->n[0]);
       break;
     case 1:
       n/=(p->n[1]);
       break;           
     }   
   #endif
   
   n*=2; //multiply up for all four boundaries
   
   switch(idim)
     {
     case 0:

       if((p->pnpe[0])>1)
	 {

	   mgpuneighbours(0, p);

	   gnmpirequest=0;  

	   for(i=0; i<2; i++)
	     {
	       gmpirequest[i]=MPI_REQUEST_NULL;
	     }

	   if((p->mpiupperb[idim])==1) gnmpirequest++;


	   // -------------------------------------- send/recv method start here
	   // npe/n is the size
	   // ipe stands for the rank
	   // hpe and jpe are the processor indexes for left and right neighbors
	   
	   if((p->mpiupperb[idim])==1 )
	     { 
	       gmpirequest[gnmpirequest]=comm.Irecv(gmpitgtbufferr[0],n,MPI_DOUBLE_PRECISION
	       			    ,p->jpe,100*(p->jpe)+10*(idim+1));
	     }

	   if((p->mpilowerb[idim])==1) gnmpirequest++;

	   if((p->mpilowerb[idim])==1)
	     {
	       gmpirequest[gnmpirequest]=comm.Irecv(gmpitgtbufferl[0],n,MPI_DOUBLE_PRECISION
	       					    ,p->hpe,100*(p->hpe)+10*(idim+1)+1);
	     }
	   
	   comm.Barrier();

	   if((p->mpiupperb[idim])==1) 
	     {
	       comm.Rsend(gmpisrcbufferr[0], n, MPI_DOUBLE_PRECISION, p->jpe, 100*(p->ipe)+10*(idim+1)+1);
	     }
	 
	   if((p->mpilowerb[idim])==1)
	     {
	       comm.Rsend(gmpisrcbufferl[0], n, MPI_DOUBLE_PRECISION, p->hpe, 100*(p->ipe)+10*(idim+1));
	     }

	   comm.Barrier();
	   request.Waitall(gnmpirequest,gmpirequest);

	   //copy data from buffer to the viscosity data in temp2
	   //organise buffers so that pointers are swapped instead

           #ifdef USE_SAC_3D

	   //tmp_nuI(ixFhi1+1,ixFlo2:ixFhi2,ixFlo3:ixFhi3)=tgtbufferR1(1,ixFlo2:ixFhi2,&
	   //ixFlo3:ixFhi3) !right, upper R
	   //tmp_nuI(ixFlo1-1,ixFlo2:ixFhi2,ixFlo3:ixFhi3)=tgtbufferL1(1,ixFlo2:ixFhi2,&
	   //ixFlo3:ixFhi3) !left, lower  L
         
	   for(i2=1;i2<((p->n[1])+2);i2++ )
	     {
	       for(i3=1;i3<((p->n[2])+2);i3++ )
	          {

	             //i1=(p->n[0])+1;
	             //bound=i1;
	             //var1[encode3p2_sacmpi (p,i1, i2, i3, tmpnui)]=gmpitgtbufferr[0][i2+i3*((p->n[1])+2)];
	             //var1[encode3p2_sacmpi (p,0, i2, i3, tmpnui)]=gmpitgtbufferl[0][i2+i3*((p->n[1])+2)];

	             i1=0;

	             for(bound=0; bound<2; bound++)
	 	        {
	                   var1[sacencodempivisc0(p,i1,i2,i3,bound+2,idim)]=
			     gmpitgtbufferr[0][i2+i3*((p->n[1])+2)+bound*((p->n[1])+2)*((p->n[2])+2)];
	                }

	             for(bound=0; bound<2; bound++)
		        {
	                   var1[sacencodempivisc0(p,i1,i2,i3,bound,idim)]=
			     gmpitgtbufferl[0][i2+i3*((p->n[1])+2)+bound*((p->n[1])+2)*((p->n[2])+2)];
	                }
	          }
	     }


         #else

	 //tmp_nuI(ixFhi1+1,ixFlo2:ixFhi2)=tgtbufferR1(1,ixFlo2:ixFhi2) !right, upper R
	 //tmp_nuI(ixFlo1-1,ixFlo2:ixFhi2)=tgtbufferL1(1,ixFlo2:ixFhi2) !left, lower  L




	 for(i2=1;i2<((p->n[1])+2);i2++ )
         {

	   //i1=(p->n[0])+1;
	   //var1[encode3p2_sacmpi (p,i1, i2, i3, tmpnui)]=gmpitgtbufferr[0][i2];
           //var1[encode3p2_sacmpi (p,0, i2, i3, tmpnui)]=gmpitgtbufferl[0][i2];

	   i1=0;

	   for(bound=0; bound<2; bound++)
	     {
	       //printf("T: %d \n",i2+bound*((p->n[1])+2));
	       // printf("K: %d \n",gmpitgtbufferr[0]);
	       //printf("R: %f \n",gmpitgtbufferr[0][i2+bound*((p->n[1])+2)]);

	       var1[sacencodempivisc0(p,i1,i2,i3,bound+2,idim)]=
		 gmpitgtbufferr[0][i2+bound*((p->n[1])+2)];
	     }

          for(bound=0; bound<2; bound++)
	     {
	       //printf("T: %d \n",i2+bound*((p->n[1])+2));	       
	       //printf("L: %f \n",gmpitgtbufferl[0][i2+bound*((p->n[1])+2)]);

	       var1[sacencodempivisc0(p,i1,i2,i3,bound,idim)]=
		 gmpitgtbufferl[0][i2+bound*((p->n[1])+2)];
	     }

          }
         
         #endif
	 }
        

	 break;
     
	 case 1:

if((p->pnpe[1])>1  )
{
     mgpuneighbours(1, p);

     gnmpirequest=0;  
  for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
     

      // printf("visc %d %d %d %d %d\n",p->ipe,p->mpiupperb[idim],p->mpilowerb[idim],p->jpe,p->hpe);


        if((p->mpiupperb[idim])==1  ) gnmpirequest++;
        if((p->mpiupperb[idim])==1 )
gmpirequest[gnmpirequest]=comm.Irecv(gmpitgtbufferr[1],n,MPI_DOUBLE_PRECISION,p->jpe,100*(p->jpe)+10*(idim+1));
        if((p->mpilowerb[idim])==1  ) gnmpirequest++;
        if((p->mpilowerb[idim])==1  )
         gmpirequest[gnmpirequest]=comm.Irecv(gmpitgtbufferl[1],n,MPI_DOUBLE_PRECISION,p->hpe,100*(p->hpe)+10*(idim+1)+1);
        comm.Barrier();
          
        if((p->mpiupperb[idim])==1  ) 
comm.Rsend(gmpisrcbufferr[1], n, MPI_DOUBLE_PRECISION, p->jpe,100*(p->ipe)+10*(idim+1)+1 );
          
        if((p->mpilowerb[idim])==1  )
 comm.Rsend(gmpisrcbufferl[1], n, MPI_DOUBLE_PRECISION, p->hpe,  100*(p->ipe)+10*(idim+1));

     comm.Barrier();
 
        request.Waitall(gnmpirequest,gmpirequest);














      #ifdef USE_SAC_3D
  //tmp_nuI(ixFlo1:ixFhi1,ixFhi2+1,ixFlo3:ixFhi3)=tgtbufferR2(ixFlo1:ixFhi1,1,&
  //    ixFlo3:ixFhi3) !right, upper R
  // tmp_nuI(ixFlo1:ixFhi1,ixFlo2-1,ixFlo3:ixFhi3)=tgtbufferL2(ixFlo1:ixFhi1,1,&
   //   ixFlo3:ixFhi3) !left, lower  L
         for(i1=1;i1<((p->n[0])+2);i1++ )
                  for(i3=1;i3<((p->n[2])+2);i3++ )
         {
          //i2=(p->n[1])+1;
         
          //var2[encode3p2_sacmpi (p,i1, i2, i3, tmpnui)]=gmpitgtbufferr[1][i1+i3*((p->n[0])+2)];
          //var2[encode3p2_sacmpi (p,i1, 0, i3, tmpnui)]=gmpitgtbufferl[1][i1+i3*((p->n[0])+2)];

          i2=0;
          for(bound=0; bound<2; bound++)
            var2[sacencodempivisc1(p,i1,i2,i3,bound+2,idim)]=gmpitgtbufferr[0][i1+i3*((p->n[0])+2)+bound*((p->n[0])+2)*((p->n[3])+2)];
          for(bound=0; bound<2; bound++)
             var2[sacencodempivisc1(p,i1,i2,i3,bound,idim)]=gmpitgtbufferl[0][i1+i3*((p->n[0])+2)+bound*((p->n[0])+2)*((p->n[3])+2)];




          }


     #else
       // tmp_nuI(ixFlo1:ixFhi1,ixFhi2+1)=tgtbufferR2(ixFlo1:ixFhi1,1) !right, upper R
   //tmp_nuI(ixFlo1:ixFhi1,ixFlo2-1)=tgtbufferL2(ixFlo1:ixFhi1,1) !left, lower  L
                  for(i1=1;i1<((p->n[0])+2);i1++ )
         {
          //i2=(p->n[1])+1;
         
          //var2[encode3p2_sacmpi (p,i1, i2, i3, tmpnui)]=gmpitgtbufferr[1][i1];
          //var2[encode3p2_sacmpi (p,i1, 0, i3, tmpnui)]=gmpitgtbufferl[1][i1];


          i2=0;
          for(bound=0; bound<2; bound++)
            var2[sacencodempivisc1(p,i1,i2,i3,bound+2,idim)]=gmpitgtbufferr[0][i1+bound*((p->n[0])+2)];
          for(bound=0; bound<2; bound++)
             var2[sacencodempivisc1(p,i1,i2,i3,bound,idim)]=gmpitgtbufferl[0][i1+bound*((p->n[0])+2)];


          //if(i1>90 && i1 <110)
          //         printf("r0 r1 l0 l1 %d %g %g %g %g\n",p->ipe,gmpitgtbufferr[0][i1+0*((p->n[0])+2)],gmpitgtbufferr[0][i1+1*((p->n[0])+2)],gmpitgtbufferl[0][i1+0*((p->n[0])+2)],gmpitgtbufferl[0][i1+1*((p->n[0])+2)]);
          //printf("\n");

          }

      #endif

}


     break;
     #ifdef USE_SAC_3D
        case 2:

if((p->pnpe[2])>1  )
{
      mgpuneighbours(2, p);

     gnmpirequest=0;  
  for(i=0; i<2; i++)
     gmpirequest[i]=MPI_REQUEST_NULL;
   


        if((p->mpiupperb[idim])==1  ) gnmpirequest++;
        if((p->mpiupperb[idim])==1 )
gmpirequest[gnmpirequest]=comm.Irecv(gmpitgtbufferr[2],n,MPI_DOUBLE_PRECISION,p->jpe,100*(p->jpe)+10*(idim+1));
        if((p->mpilowerb[idim])==1  ) gnmpirequest++;
        if((p->mpilowerb[idim])==1  )
         gmpirequest[gnmpirequest]=comm.Irecv(gmpitgtbufferl[2],n,MPI_DOUBLE_PRECISION,p->hpe,100*(p->hpe)+10*(idim+1)+1);
        comm.Barrier();
          
        if((p->mpiupperb[idim])==1  ) 
comm.Rsend(gmpisrcbufferr[2], n, MPI_DOUBLE_PRECISION, p->jpe,100*(p->ipe)+10*(idim+1)+1 );
          
        if((p->mpilowerb[idim])==1  )
 comm.Rsend(gmpisrcbufferl[2], n, MPI_DOUBLE_PRECISION, p->hpe,  100*(p->ipe)+10*(idim+1));

     comm.Barrier();
 
        request.Waitall(gnmpirequest,gmpirequest);



   //  tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFhi3+1)=tgtbufferR3(ixFlo1:ixFhi1,&
   //   ixFlo2:ixFhi2,1) !right, upper R

   //tmp_nuI(ixFlo1:ixFhi1,ixFlo2:ixFhi2,ixFlo3-1)=tgtbufferL3(ixFlo1:ixFhi1,&
   //   ixFlo2:ixFhi2,1) !left, lower  L
        for(i1=1;i1<((p->n[0])+2);i1++ )
                  for(i2=1;i2<((p->n[1])+2);i2++ )
         {
         // i3=(p->n[2])+1;
         
        //  var3[encode3p2_sacmpi (p,i1, i2, i3, tmpnui)]=gmpitgtbufferr[2][i1+i2*((p->n[0])+2)];
         // var3[encode3p2_sacmpi (p,i1, i2, 0, tmpnui)]=gmpitgtbufferl[2][i1+i2*((p->n[0])+2)];


          i3=0;
          for(bound=0; bound<2; bound++)
            var3[sacencodempivisc2(p,i1,i2,i3,bound+2,idim)]=gmpitgtbufferr[0][i1+i2*((p->n[0])+2)+bound*((p->n[0])+2)*((p->n[1])+2)];
          for(bound=0; bound<2; bound++)
             var3[sacencodempivisc2(p,i1,i2,i3,bound,idim)]=gmpitgtbufferl[0][i1+i2*((p->n[0])+2)+bound*((p->n[0])+2)*((p->n[1])+2)];




          }
}
     
     break;
     #endif
   
  }
   comm.Barrier();


//p->mpilowerb[idim]=iolowerb;
//p->mpiupperb[idim]=ioupperb;
}






