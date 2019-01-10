/*Copyright (c) Anker Berg-Sonne, 1985,1986,1987*/
#include "proto.h"

#if defined(UNIX)
char * tilde (char *old)
{
   
#if !defined(CONVEX) & !defined(LINUX) & !defined(HPUX)
    extern struct passwd *getpwuid (), *getpwnam ();
#endif
 
    register char *o, *p, *t;
    register struct passwd *pw;
    static char person[40] = {0};
    static char tail[256] = {0};
    int i = 0;
    
    while (old[i] == ' ') ++i;  /* strip leading spaces */

    if (old[i] != '~') 
	return (old);
 
    for (p = person, o = &old[i+1]; *o && *o != '/'; *p++ = *o++);
    *p = '\0';
    for (t = tail ; *o && *o != '\0' && *o != ' '; *t++ = *o++);
    *t = '\0';
 
        if (person[0] == '\0')		
	    pw = getpwuid (getuid ());
	else 
	    pw = getpwnam (person);

  	if (pw == NULL)
	    return NULL;
 
	strcpy (old, pw -> pw_dir);
        strcat (old,tail) ;
 
    return (old);
}
#endif

struct File *F_open(char *N,char *A,struct File *F)
{
#if defined(VMS)
 struct RAB *rab;
 struct FAB *fab;
 struct NAM *nam;
 fab=(struct FAB *) malloc(sizeof(struct FAB));
 *fab = cc$rms_fab;
 nam=(struct NAM *) malloc(sizeof(struct NAM));
 *nam = cc$rms_nam;
 fab->fab$l_nam=nam;
 fab->fab$l_fna=N;
 fab->fab$b_fns=strlen(N);
 fab->fab$b_org=FAB$C_SEQ;
 fab->fab$b_rat=FAB$M_CR;
 fab->fab$b_rfm=FAB$C_VAR;
 fab->fab$l_fop=FAB$M_OFP;
 fab->fab$l_dna=NULL;
 fab->fab$b_dns=0;
 nam->nam$l_esa=(char *) malloc(NAM$C_MAXRSS);
 nam->nam$b_ess=NAM$C_MAXRSS;
 if (strcmp(A,"r")==0)
 {fab->fab$b_fac=FAB$M_GET;
  if(((Rms_Sts = sys$open(fab)) & RMS$_NORMAL) !=RMS$_NORMAL)
  {free(fab);
   free(nam->nam$l_esa);
   free(nam);
   return NULL;
  }
  F->Access=0;
 } else
 if (strcmp(A,"w")==0)
 {fab->fab$b_fac=FAB$M_PUT;
  if(((Rms_Sts = sys$create(fab)) & RMS$_NORMAL)!=RMS$_NORMAL)
  {free(fab);
   free(nam->nam$l_esa);
   free(nam);
   return NULL;
  }
  F->Access=1;
 } else
  return NULL;
 F->Parsed_Name=nam->nam$l_esa;
 F->Parsed_Name_Len=nam->nam$b_esl;
 fab->fab$l_nam=NULL;
 free(nam);
 rab=(struct RAB *) malloc(sizeof(struct RAB));
 *rab = cc$rms_rab;
 rab->rab$l_fab = fab;
 rab->rab$b_rac=RAB$C_SEQ;
 if(((Rms_Sts = sys$connect(rab)) & RMS$_NORMAL) !=RMS$_NORMAL)
 {free(rab);
  sys$close(fab);
  free(fab);
  return NULL;
 }
 F->Handle = rab;
 F->Point=F->Length=0;
 return F;
#else
#if defined(OS2)
 int Dummy;
#endif
 if (N==NULL)
  return NULL;
 if (strcmp(A,"r")==0)
#if defined(DOS) | defined(WIN32)
 {if ((F->Handle=open(N,O_RDONLY|O_BINARY))==-1)
#endif
#if defined(NT)
 {if ((F->Handle=_open(N,O_RDONLY|O_BINARY))==-1)
#endif
#if defined(UNIX) 
 {if ((F->Handle=open(tilde(N),O_RDONLY))==-1)
#endif
#if defined(OS2)
 {if (DosOpen((char far *)N,(PHFILE)&F->Handle,(unsigned far *)&Dummy,
   0L,0,0x01,0x20,0L))
#endif
#if defined(VMS) 
 {if ((F->Handle=open(N,0))==-1)
#endif
#if defined(ATARI)
 {if ((F->Handle=Fopen(N,0))<0)
#endif
#if defined(MPE)
 {if ((F->Handle=fopen(N,"rTmR1024V"))==NULL)
#endif
   return NULL;
  F->Access=0;
 } else
 if (strcmp(A,"w")==0)
#if defined(DOS) | defined(WIN32)
 {unlink(N);
  if ((F->Handle=open(N,O_CREAT|O_WRONLY|O_BINARY,S_IREAD|S_IWRITE))==-1)
#endif
#if defined(NT)
 {unlink(N);
  if ((F->Handle=_open(N,O_CREAT|O_WRONLY|O_BINARY,S_IREAD|S_IWRITE))==-1)
#endif
#if defined(UNIX) 
 {if ((F->Handle=open(tilde(N),O_CREAT|O_WRONLY|O_TRUNC,
   00400+00200+00040+00020+00004+00002))==-1)
#endif
#if defined(OS2)
 {DosDelete((char far *)N,0L);
  if (DosOpen((char far *)N,(PHFILE)&F->Handle,(unsigned far *)&Dummy,
   0L,0,0x12,0x11,0L))
#endif
#if defined(VMS) 
 {if ((F->Handle=creat(N,1))==-1)
#endif
#if defined(ATARI)
 {if ((F->Handle=Fcreate(N,0))<0)
#endif
#if defined(MPE)
 {if ((F->Handle=fopen(N,"wR1024V"))==NULL)
#endif
   return NULL;
  F->Access=1;
 } else
  return NULL;
 F->Point=F->Length=0;
 return F;
#endif
}

int Fast F_close(struct File *F)
{
#if defined(VMS)
 struct FAB *fab;
 struct RAB *rab;
 rab=F->Handle;
 fab=rab->rab$l_fab;
 sys$disconnect(rab);
 sys$close(fab);
 free(fab);
 free(rab);
 free(F->Parsed_Name);
 return 0;
#else
#if defined(OS2)
 int Written;
#endif
 if (F->Access&&F->Length!=0)
#if defined(ATARI)
 {if (Fwrite(F->Handle,(long)F->Length,F->Record)!=F->Length)
#else
#if defined(MPE)
 {if (fwrite(F->Record,1,(int)F->Length,F->Handle)!=F->Length)
#else
#if defined(OS2)
 {if (DosWrite((unsigned)F->Handle,(char far *)F->Record,(unsigned)F->Length,
   (unsigned far *)&Written)||Written!=F->Length)
#else
#if defined(NT)
 {if (_write(F->Handle,F->Record,F->Length)==-1)
#else
 {if (write(F->Handle,F->Record,F->Length)==-1)
#endif
#endif
#endif
#endif
#if defined(ATARI)
  {Fclose(F->Handle);
#else
#if defined(MPE)
  {fclose(F->Handle);
#else
#if defined(OS2)
  {DosClose((unsigned)F->Handle);
#else
#if defined(NT)
  {_close(F->Handle);
#else
  {close(F->Handle);
#endif
#endif
#endif
#endif
   return EOF;
  }
 }
#if defined(ATARI)
 if (Fclose(F->Handle)!=0)
#else
#if defined(MPE)
 if (fclose(F->Handle)!=0)
#else
#if defined(OS2)
 if (DosClose((unsigned)F->Handle))
#else
#if defined(NT)
 if (_close(F->Handle)!=0)
#else
 if (close(F->Handle)!=0)
#endif
#endif
#endif
#endif
  return EOF;
 return 0;
#endif
}

char *F_gets(char *S,int N,struct File *F)
{register char *P;
 register int I, J;
 P=S;
 while (N)
 {
#if defined(VMS)
  if (F->Length==0||F->Point==F->Length)
  {F->Point=0;
   if ((F->Length=F_read(F->Record,Recsize,1,F))<=0)
   {F->Length=0;
    if (P==S)
     return NULL;
    *P=0;
    return S;
   }
  }
#else
  if (F->Length==0||F->Point==F->Length)
  {F->Point=0;
#if defined(ATARI)
   if ((F->Length=Fread(F->Handle,(long)Recsize,F->Record))<=0)
#else
#if defined(MPE)
   if ((F->Length=fread(F->Record,1,(int)Recsize,F->Handle))==EOF)
#else
#if defined(OS2)
   if (DosRead((unsigned)F->Handle,(char far *)F->Record,(unsigned)Recsize,
    (unsigned *)&F->Length)||F->Length==0)
#else
#if defined(NT)
   if ((F->Length=_read(F->Handle,F->Record,Recsize))<=0)
#else
   if ((F->Length=read(F->Handle,F->Record,Recsize))<=0)
#endif
#endif
#endif
#endif
   {F->Length=0;
    if (P==S)
     return NULL;
    *P=0;
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
    while (P!=S&&*(--P)==('Z'-64))
     *P=0;
    if (P==S)
     return NULL;
#endif
    return S;
   }
  }
#endif
  if (N<(I=F->Length-F->Point))
   I=N;
#if defined(DOS) | defined(NT) | defined(OS2)
  J=C_Scan((char far *)F->Record+F->Point,I,'\n');
#else
  J=C_Scan(F->Record+F->Point,I,'\n');
#endif
#if defined(DOS) | defined(NT) | defined(OS2) | defined(WIN32)
  C_Move(F->Record+F->Point,P,I);
#else
  C_Move(F->Record+F->Point,P,I,0);
#endif
  P+=J;
  if (I==J)
   F->Point+=J; else
  {F->Point+=(++J);
   *P=0;
   if (P!=S&&*(--P)=='\r')
    *P=0;
   return S;
  }
  N-=J;
 }
 *(P++)=0;
 return S;
}

int Fast F_read(char *S,int L,int N,struct File *F)
{
#if defined(VMS)
 struct RAB *rab;
 int I;
 rab=F->Handle;
 rab->rab$l_ubf=S;
 rab->rab$w_usz=L*N-2;
 if(((Rms_Sts = sys$get(rab)) & RMS$_NORMAL) !=RMS$_NORMAL)
  return 0;
 I=rab->rab$w_rsz;
 if (S[I-2]!='\r'||S[I-1]!='\n')
 {S[I]='\r';
  S[I+1]='\n';
  return I+2;
 } else
  return I;
#else
 N*=L;
#if defined(ATARI)
 if ((L=Fread(F->Handle,(long)N,S))<=0)
#else
#if defined(MPE)
 if ((L=fread(S,1,(int)N,F->Handle))==EOF)
#else
#if defined(OS2)
 if (DosRead((unsigned)F->Handle,(char far *)S,(unsigned)N,(unsigned *)&L)||
  L==0)
#else
#if defined(NT)
 if ((L=_read(F->Handle,S,N))<=0)
#else
 if ((L=read(F->Handle,S,N))<=0)
#endif
#endif
#endif
#endif
   return 0;
 return L;
#endif
}

int Fast F_getc(struct File *F)
{
#if defined(VMS)
 register int C;
 if (F->Length==0||F->Point==F->Length)
 {F->Point=0;
  if ((F->Length=F_read(F->Record,Recsize,1,F))<=0)
  {F->Length=0;
   return EOF;
  }
 }
 C=(F->Record[F->Point++])&0xFF;
 return C;
#else
 register int C;
 if (F->Length==0||F->Point==F->Length)
 {F->Point=0;
#if defined(ATARI)
  if ((F->Length=Fread(F->Handle,(long)Recsize,F->Record))<=0)
#else
#if defined(MPE)
  if ((F->Length=fread(F->Record,1,(int)Recsize,F->Handle))==EOF)
#else
#if defined(OS2)
  if (DosRead((unsigned)F->Handle,(char far *)F->Record,(unsigned)Recsize,
   (unsigned *)&F->Length)||F->Length==0)
#else
#if defined(NT)
  if ((F->Length=_read(F->Handle,F->Record,Recsize))<=0)
#else
  if ((F->Length=read(F->Handle,F->Record,Recsize))<=0)
#endif
#endif
#endif
#endif
  {F->Length=0;
   return EOF;
  }
 }
 C=(F->Record[F->Point++])&0xFF;
 return C;
#endif
}

int Fast F_write(char *S,int L,int N,struct File *F)
{
#if defined(VMS)
 struct RAB *rab;
 int I;
 rab=F->Handle;
 rab->rab$l_rbf=S;
 I=L*N;
 if (S[I-1]=='\n'&&S[I-2]=='\r')
  I-=2;
 rab->rab$w_rsz=I;
 if(((Rms_Sts = sys$put(rab)) & RMS$_NORMAL) !=RMS$_NORMAL)
  return EOF;
 return rab->rab$w_rsz;
#else
#if defined(OS2)
 int Written;
#endif
 N*=L;
#if defined(ATARI)
 if (Fwrite(F->Handle,(long)N,S)<N)
#else
#if defined(MPE)
 if (fwrite(S,1,(int)N,F->Handle)<N)
#else
#if defined(OS2)
 if (DosWrite((unsigned)F->Handle,(char far *)S,(unsigned)N,
  (unsigned *)&Written)||Written!=N)
#else
#if defined(NT)
 if (_write(F->Handle,S,N)<N)
#else
 if (write(F->Handle,S,N)<N)
#endif
#endif
#endif
#endif
   return EOF;
 return N;
#endif
}

int Fast F_putc(char C,struct File *F)
{
#if defined(VMS)
 if (F->Length==Recsize)
 {if (F_write(F->Record,Recsize,1,F)<Recsize)
   return EOF;
  F->Length=0;
 }
 F->Record[F->Length++]=C;
 if (C=='\n'&&F->Record[F->Length-2]=='\r')
 {F->Length-=2;
  if (F_write(F->Record,F->Length,1,F)<F->Length)
   return EOF;
  F->Length=0;
 }
#else
#if defined(OS2)
 int Written;
#endif
 if (F->Length==Recsize)
#if defined(ATARI)
 {if (Fwrite(F->Handle,(long)Recsize,F->Record)!=Recsize)
#else
#if defined(MPE)
 {if (fwrite(F->Record,1,(int)Recsize,F->Handle)!=Recsize)
#else
#if defined(OS2)
 {if (DosWrite((unsigned)F->Handle,(char far *)F->Record,(unsigned)Recsize,
   (unsigned *)&Written)||Written!=Recsize)
#else
#if defined(NT)
 {if (_write(F->Handle,F->Record,Recsize)<Recsize)
#else
 {if (write(F->Handle,F->Record,Recsize)<Recsize)
#endif
#endif
#endif
#endif
   return EOF;
  F->Length=0;
 }
 F->Record[F->Length++]=C;
#endif
 return 0;
}
