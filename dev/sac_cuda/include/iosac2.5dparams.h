



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
//adiab hydro
#ifdef ADIABHYDRO
int ni = 106;

ni=ni+2*ngi;
real xmax = 1.0;  
real dx = 0.55*xmax/(ni-4);
#endif

#ifdef USE_SAC
//vac ozt
int ni;
ni=96;    //OZT tests
//ni=796; //BW tests
ni=ni+2*ngi;
//ni=512;
//real xmax = 6.2831853;  
real xmax=1.0;
//real dx = xmax/(ni-4);
real dx = xmax/(ni);

#endif
#ifdef USE_SAC_3D
//vac ozt
int ni;
ni=28;    //BACH3D tests
ni=ni+2*ngi;

//ni=512;
//real xmax = 6.2831853;  
real xmax=14.19e18;
real xmin=-14.19e18;
//real dx = xmax/(ni-4);
real dx = (xmax-xmin)/(ni);

#endif


// Define the y domain
//adiab hydro
#ifdef ADIABHYDRO
int nj = 106;
nj=196;
nj=nj+2*ngj;
real ymax = 1.0;  
real dy = 0.55*ymax/(nj-4);
#endif

#ifdef USE_SAC
//vac ozt
int nj = 96;  //OZT tests
//int nj=2;  //BW test
nj=nj+2*ngj; 
//nj=512;
//real ymax = 6.2831853; 
real ymax = 1.0;   
//real dy = ymax/(nj-4);
real dy = ymax/(nj);
   
//nj=41;
#endif

#ifdef USE_SAC_3D
//vac bach3d
int nj;
nj=28;    //BACH3D tests


//ni=512;
//real xmax = 6.2831853;  
real ymax=14.19e18;
real ymin=-14.19e18;
//real dx = xmax/(ni-4);
real dy = (ymax-ymin)/(nj);
nj=nj+2*ngj;
#endif                   

#ifdef USE_SAC_3D
//vac bach3d
int nk;
nk=28;    //BACH3D tests


//ni=512;
//real xmax = 6.2831853;  
real zmax=14.19e18;
real zmin=-14.19e18;
//real dx = xmax/(ni-4);
real dz = (zmax-zmin)/(nk);
nk=nk+2*ngk;
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
char *cfgfile="zero1_BW.ini";
//char *cfgfile="zero1_BW_bin.ini";
char *cfgout="zeroOT";

char **hlines; //header lines for vac config files 
hlines=(char **)calloc(5, sizeof(char*));
// Define time-domain
real dt;


real *d_w;
real *d_wnew;

real *d_wmod,  *d_dwn1,  *d_dwn2,  *d_dwn3,  *d_dwn4,  *d_wd;

real *w,*wnew,*wd;
real *d_wtemp,*d_wtemp1,*d_wtemp2;
struct params *d_p;
struct params *p=(struct params *)malloc(sizeof(struct params));

struct state *d_state;
struct state *state=(struct state *)malloc(sizeof(struct state));


#ifdef ADIABHYDRO
dt=0.0002985;  //ADIABHYDRO
#endif
//dt=0.15;

#ifdef USE_SAC
dt=0.0002;  //OZT test
//dt=6.5/10000000.0; //BW test
//dt=0.00000065;  //BW tests
//dt=0.000000493;  //BW tests
//dt=0.005;
//dt=0.000139;
//dt=3.0/10000000.0; //BW test
#endif

#ifdef USE_SAC_3D
dt=3.5e24;;  //BACH3D
#endif
int nt=(int)((tmax)/dt);
//nt=3000;
//nt=5000;
//nt=200000;
//nt=150000;
nt=200;
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
#ifdef ADIABHYDRO
p->npgp[0]=1;
p->npgp[1]=1;
#else
p->npgp[0]=1;
p->npgp[1]=1;
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
#ifdef ADIABHYDRO
p->gamma=2.0;
p->adiab=1.0;
#else

//ozt test
p->gamma=5.0/3.0;  //OZ test
//p->gamma=2.0;  //BW test
//p->gamma=5.0/3.0;  //BACH3D
//alfven test
//p->gamma=1.4;

#endif




p->mu=1.0;
p->eta=0.0;
p->g[0]=0.0;
p->g[1]=0.0;
p->g[2]=0.0;
#ifdef USE_SAC_3D

#endif
//p->cmax=1.0;
p->cmax=0.02;

p->rkon=0.0;
p->sodifon=1.0;
p->moddton=0.0;
p->divbon=0.0;
p->divbfix=0.0;
p->hyperdifmom=1.0;
p->readini=0.0;
p->cfgsavefrequency=1;


p->xmax[0]=xmax;

p->xmax[1]=ymax;
p->nt=nt;
p->tmax=tmax;

p->steeringenabled=steeringenabled;
p->finishsteering=finishsteering;

p->maxviscoef=0;
//p->chyp=0.0;       
//p->chyp=0.00000;
p->chyp3=0.00000;
p->mnthreads=1;

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




iome elist;
meta meta;






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


