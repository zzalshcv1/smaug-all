


real g  = 9.81;
real u0 = 0;                               
real v0 = 0;
real b0  = 0;                               
real h0 = 5030; 

real cmax[NDIM];
real courantmax;

int ngi=2;
int ngj=2;
int ngk=2;



//Domain definition
// Define the x domain


//#ifdef USE_SAC
int ni;
ni=124; //BW tests
//ni=252;//2d model
ni=ni+2*ngi;
//ni=512;
//real xmax = 6.2831853;  

real xmax=1591959.8;
real xmin=24120.603;
real dx = (xmax-xmin)/(ni);
//#endif



// Define the y domain



int nj=124;  //BW test
//nj=252;//2d model
nj=nj+2*ngj;
//nj=512;
//real ymax = 6.2831853; 
real ymax=2000000.0;
real ymin=0.0;
//real dx = xmax/(ni-4);
real dy = (ymax-ymin)/(nj);  
//nj=41;


               

#ifdef USE_SAC_3D

int nk;
nk=124;    //BW tests

nk=nk+2*ngk;
real zmax=2000000.0;
real zmin=0.0;
//real dx = xmax/(ni-4);
real dz = (zmax-zmin)/(nk);
#endif     
real *x=(real *)calloc(ni,sizeof(real));
for(i=0;i<ni;i++)
		x[i]=i*dx;

real *y=(real *)calloc(nj,sizeof(real));
for(i=0;i<nj;i++)
		y[i]=i*dy;



int step=0;
//real tmax = 200;
real tmax = 0.2;
int steeringenabled=1;
int finishsteering=0;
char configfile[300];
//char *cfgfile="zero1.ini";
char *cfgfile="3D_tube_128_128_128_asc.ini";
//char *cfgfile="zero1_BW_bin.ini";
char *cfgout="3D_tube_128_128_128";


struct params *d_p;
struct params *p=(struct params *)malloc(sizeof(struct params));

struct state *d_state;
struct state *state=(struct state *)malloc(sizeof(struct state));

#ifdef USE_SAC
dt=2.0;  //bach test

#endif

#ifdef USE_SAC_3D
//dt=2.0;  //BACH3D
dt=0.13;  //BACH3D
#endif


/*//dt=0.15;

//#ifdef USE_SAC
//dt=0.00065;  //OZT test
//dt=6.5/10000000.0; //BW test
//dt=0.00000065;  //BW tests
dt=0.000000493;  //BW tests
//dt=0.005;
//dt=0.000139;
//dt=3.0/10000000.0; //BW test
//#endif*/


int nt=(int)((tmax)/dt);
//nt=3000;
//nt=5000;
//nt=200000;
nt=50000;
//nt=100;
real *t=(real *)calloc(nt,sizeof(real));
printf("runsim 1%d \n",nt);
//t = [0:dt:tdomain];
for(i=0;i<nt;i++)
		t[i]=i*dt;

//real courant = wavespeed*dt/dx;

p->n[0]=ni;
p->n[1]=nj;
p->ng[0]=ngi;
p->ng[1]=ngj;

p->npgp[0]=1;
p->npgp[1]=1;

#ifdef USE_SAC_3D
p->n[2]=nk;
p->ng[2]=ngk;
p->npgp[2]=1;
#endif

p->dt=dt;
p->dx[0]=dx;
p->dx[1]=dy;

#ifdef USE_SAC_3D
p->dx[2]=dz;
#endif
//p->g=g;



/*constants used for adiabatic hydrodynamics*/
/*
p->gamma=2.0;
p->adiab=0.5;
*/

p->gamma=1.66666667;






p->mu=1.0;
p->eta=0.0;
p->g[0]=-274.0;
p->g[1]=0.0;
p->g[2]=0.0;
#ifdef USE_SAC_3D

#endif
//p->cmax=1.0;
p->cmax=0.02;

p->rkon=0.0;
p->sodifon=0.0;
p->moddton=0.0;
p->divbon=0.0;
p->divbfix=0.0;
p->hyperdifmom=1.0;
p->readini=1.0;
p->cfgsavefrequency=25;


p->xmax[0]=xmax;
p->xmax[1]=ymax;
p->xmin[0]=xmin;
p->xmin[1]=ymin;
#ifdef USE_SAC_3D
p->xmax[2]=zmax;
p->xmin[2]=zmin;
#endif

p->nt=nt;
p->tmax=tmax;

p->steeringenabled=steeringenabled;
p->finishsteering=finishsteering;

p->maxviscoef=0;
//p->chyp=0.0;       
//p->chyp=0.00000;
p->chyp3=0.00000;


for(i=0;i<NVAR;i++)
  p->chyp[i]=0.0;

p->chyp[rho]=0.02;
p->chyp[energy]=0.02;
p->chyp[b1]=0.02;
p->chyp[b2]=0.02;
p->chyp[mom1]=0.4;
p->chyp[mom2]=0.4;

p->chyp[rho]=0.02;
p->chyp[mom1]=0;
p->chyp[mom2]=0;





p->chyp[rho]=0.02;
p->chyp[energy]=0.02;
p->chyp[b1]=0.02;
p->chyp[b2]=0.02;
p->chyp[mom1]=0.4;
p->chyp[mom2]=0.4;
#ifdef USE_SAC_3D
p->chyp[mom3]=0.4;
p->chyp[b3]=0.02;
#endif



iome elist;
meta meta;


//set boundary types
for(int ii=0; ii<NVAR; ii++)
for(int idir=0; idir<NDIM; idir++)
{
   (p->boundtype[ii][idir])=4;  //period=0 mpi=1 mpiperiod=2  cont=3 contcd4=4 fixed=5 symm=6 asymm=7
}



elist.server=(char *)calloc(500,sizeof(char));


meta.directory=(char *)calloc(500,sizeof(char));
meta.author=(char *)calloc(500,sizeof(char));
meta.sdate=(char *)calloc(500,sizeof(char));
meta.platform=(char *)calloc(500,sizeof(char));
meta.desc=(char *)calloc(500,sizeof(char));
meta.name=(char *)calloc(500,sizeof(char));
meta.ini_file=(char *)calloc(500,sizeof(char));
meta.log_file=(char *)calloc(500,sizeof(char));
meta.out_file=(char *)calloc(500,sizeof(char));

strcpy(meta.directory,"out");
strcpy(meta.author,"MikeG");
strcpy(meta.sdate,"Nov 2009");
strcpy(meta.platform,"swat");
strcpy(meta.desc,"A simple test of SAAS");
strcpy(meta.name,"test1");
strcpy(meta.ini_file,"test1.ini");
strcpy(meta.log_file,"test1.log");
strcpy(meta.out_file,"test1.out");
//meta.directory="out";
//meta.author="MikeG";
//meta.sdate="Nov 2009";
//meta.platform="felix";
//meta.desc="A simple test of SAAS";
//meta.name="tsteer1";

	strcpy(elist.server,"localhost1");
	elist.port=80801;
	elist.id=0;


