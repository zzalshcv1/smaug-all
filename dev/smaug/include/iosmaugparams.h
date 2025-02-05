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
//Define the x domain

//int ni=58; 
int ni=124; 
ni=ni+2*ngi;

real xmax=5866670;
real xmin=400000;
real dx = (xmax-xmin)/(ni-2*ngi);

// Define the y domain

int nj=124;  //BW test
nj=nj+2*ngj;

real ymax=3921880;
real ymin=78125;
real dy = (ymax-ymin)/(nj-2*ngj); 

// Define the z domain

#ifdef USE_SAC_3D
int nk=124;    //BW tests
nk=nk+2*ngk;

real zmax=3921880;
real zmin=78125;
real dz = (zmax-zmin)/(nk-2*ngk);
#endif  

real *x=(real *)calloc(ni,sizeof(real));
for(i=0;i<ni;i++) x[i]=i*dx;

real *y=(real *)calloc(nj,sizeof(real));
for(i=0;i<nj;i++) y[i]=i*dy;

int step=0;
real tmax = 0.2;
int steeringenabled=1;
int finishsteering=0;
char configfile[300];

char *cfgfile="configs/magvert/3D_128_spic_bvert500G_asc.ini";
char *cfgout="tmpout/flux1_.out";
char *cfggathout="out/flux1_.out";

struct params *d_p;
struct params *p=(struct params *)malloc(sizeof(struct params));

struct state *d_state;
struct state *state=(struct state *)malloc(sizeof(struct state));

#ifdef USE_SAC
dt=2.0;  //bach test

#endif

#ifdef USE_SAC_3D
dt=0.001;  //BACH3D
#endif

int nt=(int)((tmax)/dt);
nt=5000;

real *t=(real *)calloc(nt,sizeof(real));
printf("runsim %d \n",nt);
for(i=0;i<nt;i++) t[i]=i*dt;

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


p->gamma=1.66666667;

p->mu=1.0;
p->eta=0.0;
p->g[0]=-274.0;
//p->g[0]=0.0;
p->g[1]=0.0;
p->g[2]=0.0;
#ifdef USE_SAC_3D

#endif
//p->cmax=1.0;
p->cmax=0.02;
p->courant=0.2;
p->rkon=0.0;
p->sodifon=0.0;
p->moddton=0.0;
p->divbon=0.0;
p->divbfix=0.0;
p->hyperdifmom=1.0;
p->readini=1.0;
p->cfgsavefrequency=20;

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

#ifdef USE_MPI
//number of procs in each dim mpi only
p->pnpe[0]=1;
p->pnpe[1]=1;
p->pnpe[2]=1;
#endif

iome elist;
meta meta;

//set boundary types
for(int ii=0; ii<NVAR; ii++)
for(int idir=0; idir<NDIM; idir++)
for(int ibound=0; ibound<2; ibound++)
{
   (p->boundtype[ii][idir][ibound])=5;  //period=0 mpi=1 mpiperiod=2  cont=3 contcd4=4 fixed=5 symm=6 asymm=7
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


