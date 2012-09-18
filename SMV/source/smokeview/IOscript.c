// $Date$ 
// $Revision$
// $Author$

// svn revision character string
char IOscript_revision[]="$Revision$";

#include "options.h"
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#ifdef pp_OSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "string_util.h"
#include "update.h"
#include "smokeviewvars.h"
#include "IOvolsmoke.h"

void update_menu(void);

//
// script commands
//

// ---------- rendering images -----------

// default image file names are CHID_seq
//   where CHID is the base file name and seq is the frame number.
//   Smokeview will automatically add an .jpg or .png extension
//   depending on what kind of files are rendered.

// RENDERDIR
//  directory name (char) (where rendered files will go)

// RENDERCLIP
// flag ) left right bottom top indentations in pixels, clip if flag==1

// RENDERONCE
// file name base (char) (or blank to use smokeview default)

// RENDERDOUBLEONCE
// file name base (char) (or blank to use smokeview default)

// RENDERALL 
//  skip (int)
// file name base (char) (or blank to use smokeview default)

// VOLSMOKERENDERALL 
//  skip (int)
// file name base (char) (or blank to use smokeview default)

// ---------- loading, unloading files -----------
//
//  Use LOADFILE to load a particular file.  Smokeview will figure
//  out what kind of file it is (3d smoke, slice etc.)  and call the
//  appropriate routine.
//
//  Use other LOAD commands to load files of the specified type for 
//  all meshes.

// LOADINIFILE 
//  file (char)

// LOADFILE 
//  file (char)

// LOADVFILE
//  file (char)

// LOADBOUNDARY
//   type (char)

// LOAD3DSMOKE
//  type (char)

// LOADVOLSMOKE
//  mesh number (-1 for all meshes) (int)  

// LOADVOLSMOKEFRAME
//  mesh index, frame (int)  

// LOADPARTICLES

// PARTCLASSCOLOR
//   color (char)

// PARTCLASSTYPE
//   type (char)

// LOADISO
//  type (char)

// LOADSLICE
//  type (char)
//  1/2/3 (int)  val (float)

// LOADVSLICE
//  type (char)
//  1/2/3 (int)  val (float)

// LOADPLOT3D
//  mesh number (int) time (float)

// PLOT3DPROPS
//  plot3d type (int) showvector (0/1) (int) vector length index (int) plot3d display type (int)

// SHOWPLOT3DDATA
//  mesh number (int) orientation (int)  value (0/1) (int) position (float)

// UNLOADALL

// ---------- controlling scene -----------
//
// tours and viewpoints are referenced using names defined
// previously in a Smokeview session.  These names are
// stored in the .ini file.

// LOADTOUR
//  type (char)

// UNLOADTOUR

// SETTIMEVAL
//  time (float)

// SETVIEWPOINT
//  viewpoint (char)

// EXIT

/* ------------------ insert_scriptfile ------------------------ */

void get_newscriptfilename(char *newscriptfilename){
  char buffer[1024];
  int i;
  int nexti;
  scriptfiledata *scriptfile;

  for(i=0;i<1000;i++){
    if(i==0){
      strcpy(buffer,fdsprefix);
      strcat(buffer,".ssf");
    }
    else{
      sprintf(buffer,"%s_%03i.ssf",fdsprefix,i);
    }
    nexti=0;
    for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
      if(strcmp(scriptfile->file,buffer)==0){
        nexti=1;
        break;
      }
    }
    if(nexti==0){
      strcpy(newscriptfilename,buffer);
      return;
    }
  }
  strcpy(newscriptfilename,"");
}

/* ------------------ get_scriptfilename ------------------------ */

char *get_scriptfilename(int id){
  scriptfiledata *scriptfile;

  for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
    if(scriptfile->id==id)return scriptfile->file;
    if(scriptfile->file==NULL)continue;
  }
  return NULL;
}

/* ------------------ get_scriptfilename ------------------------ */

char *get_inifilename(int id){
  inifiledata *inifile;

  for(inifile=first_inifile.next;inifile->next!=NULL;inifile=inifile->next){
    if(inifile->id==id)return inifile->file;
    if(inifile->file==NULL)continue;
  }
  return NULL;
}

/* ------------------ insert_inifile ------------------------ */

inifiledata *insert_inifile(char *file){
  inifiledata *thisptr,*prevptr,*nextptr;
  int len;
  inifiledata *inifile;
  int idmax=-1;

  for(inifile=first_inifile.next;inifile->next!=NULL;inifile=inifile->next){
    if(inifile->id>idmax)idmax=inifile->id;
    if(inifile->file==NULL)continue;
    if(strcmp(file,inifile->file)==0)return NULL;
  }

  NewMemory((void **)&thisptr,sizeof(inifiledata));
  nextptr = &last_inifile;
  prevptr = nextptr->prev;
  nextptr->prev=thisptr;
  prevptr->next=thisptr;

  thisptr->next=nextptr;
  thisptr->prev=prevptr;
  thisptr->file=NULL;
  thisptr->id=idmax+1;

  if(file!=NULL){
    len = strlen(file);
    if(len>0){
      NewMemory((void **)&thisptr->file,len+1);
      strcpy(thisptr->file,file);
    }
  }
  return thisptr;
}

/* ------------------ insert_scriptfile ------------------------ */

scriptfiledata *insert_scriptfile(char *file){
  scriptfiledata *thisptr,*prevptr,*nextptr;
  int len;
  scriptfiledata *scriptfile;
  int idmax=-1;

  for(scriptfile=first_scriptfile.next;scriptfile->next!=NULL;scriptfile=scriptfile->next){
    if(scriptfile->id>idmax)idmax=scriptfile->id;
    if(scriptfile->file==NULL)continue;
    if(strcmp(file,scriptfile->file)==0)return NULL;
  }

  NewMemory((void **)&thisptr,sizeof(scriptfiledata));
  nextptr = &last_scriptfile;
  prevptr = nextptr->prev;
  nextptr->prev=thisptr;
  prevptr->next=thisptr;

  thisptr->next=nextptr;
  thisptr->prev=prevptr;
  thisptr->file=NULL;
  thisptr->recording=0;
  thisptr->id=idmax+1;

  if(file!=NULL){
    len = strlen(file);
    if(len>0){
      NewMemory((void **)&thisptr->file,len+1);
      strcpy(thisptr->file,file);
    }
  }
  return thisptr;
}

/* ------------------ cleanbuffer ------------------------ */

void cleanbuffer(char *buffer, char *buffer2){
  char *buff_ptr;

  remove_comment(buffer2);
  buff_ptr = trim_front(buffer2);
  trim(buff_ptr);
  strcpy(buffer,buff_ptr);
}

/* ------------------ start_script ------------------------ */

void start_script(void){
  if(scriptinfo==NULL){
    fprintf(stderr,"*** Warning: Smokeview script does not exist\n");
    return;
  }
  glui_script_disable();
  current_script_command=scriptinfo-1;
}

/* ------------------ script_set_buffer ------------------------ */

char *script_set_buffer(char *buffer2){
  char *cval;
  char buffer[1024];
  int len;

  cval=NULL;
  cleanbuffer(buffer,buffer2);
  len = strlen(buffer);
  if(len>0){
    NewMemory((void **)&cval,len+1);
    strcpy(cval,buffer);
  }
  return cval;
}

/* ------------------ free_script ------------------------ */

void free_script(void){
  scriptdata *scripti;
  int i;

  if(nscriptinfo>0){
    for(i=0;i<nscriptinfo;i++){
      scripti = scriptinfo + i;

      FREEMEMORY(scripti->cval);
      FREEMEMORY(scripti->cval2);
    }
    FREEMEMORY(scriptinfo);
    nscriptinfo=0;
  }

}

/* ------------------ init_scripti ------------------------ */

void init_scripti(scriptdata *scripti, int command,char *label){
  char *label2;

  trim(label);
  label2 = trim_front(label);
  strcpy(scripti->command_label,label2);
  scripti->command=command;
  scripti->cval=NULL;
  scripti->cval2=NULL;
  scripti->fval=0.0;
  scripti->ival=0;
  scripti->ival2=0;
  scripti->ival3=0;
  scripti->ival4=0;
  scripti->ival5=0;
}

/* ------------------ compile_script ------------------------ */

int compile_script(char *scriptfile){
  FILE *stream;
  char buffer[1024], buffer2[1024];
  scriptdata *scripti;
  int return_val;

  return_val=1;
  if(scriptfile==NULL){
    fprintf(stderr,"*** Error: scriptfile name is NULL\n");
    return return_val;
  }
  stream=fopen(scriptfile,"r");
  if(stream==NULL){
    fprintf(stderr,"*** Error: scriptfile, %s, could not be opened for input\n",scriptfile);
    return return_val;
  }

  return_val=0;
  
  /* 
   ************************************************************************
   ************************ start of pass 1 ********************************* 
   ************************************************************************
 */

  free_script();

  while(!feof(stream)){
    if(fgets(buffer2,255,stream)==NULL)break;
    cleanbuffer(buffer,buffer2);

    if(strncmp(buffer," ",1)==0)continue;

    if(match_upper(buffer,"UNLOADALL") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"EXIT") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"KEYBOARD") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERDIR") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SCENECLIP") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"XSCENECLIP") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"YSCENECLIP") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"ZSCENECLIP") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERCLIP") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERONCE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERDOUBLEONCE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERALL") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"VOLSMOKERENDERALL") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADFILE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADINIFILE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVFILE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADBOUNDARY") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"PARTCLASSCOLOR") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"PARTCLASSTYPE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SHOWPLOT3DDATA") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"PLOT3DPROPS") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADTOUR") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"UNLOADTOUR") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOAD3DSMOKE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVOLSMOKE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVOLSMOKEFRAME") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADSLICE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVSLICE") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADISO") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADPARTICLES") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADPLOT3D") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SETTIMEVAL") == 1){
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SETVIEWPOINT") == 1){
      nscriptinfo++;
      continue;
    }
  }

  if(nscriptinfo==0){
    fprintf(stderr,"*** Warning: scriptfile has no usable commands\n");
    return 1;
  }

  NewMemory((void **)&scriptinfo,nscriptinfo*sizeof(scriptdata));

  /* 
   ************************************************************************
   ************************ start of pass 2 ********************************* 
   ************************************************************************
 */

  nscriptinfo=0;
  rewind(stream);
  while(!feof(stream)){
    if(fgets(buffer2,255,stream)==NULL)break;
    cleanbuffer(buffer,buffer2);
    if(strlen(buffer)==0)continue;

    if(match_upper(buffer,"UNLOADALL") == 1){//x0
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_UNLOADALL,buffer);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERDIR") == 1){
      int len;
      int i;

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_RENDERDIR,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      cleanbuffer(buffer,buffer2);
      len = strlen(buffer);
      if(len>0){
#ifdef WIN32
        for(i=0;i<len;i++){
          if(buffer[i]=='/')buffer[i]='\\';
        }
        if(buffer[len-1]!='\\')strcat(buffer,dirseparator);        
#else
        for(i=0;i<len;i++){
          if(buffer[i]=='\\')buffer[i]='/';
        }
        if(buffer[len-1]!='/')strcat(buffer,dirseparator);        
#endif
        scripti->cval=script_set_buffer(buffer);
      }

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"KEYBOARD") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_KEYBOARD,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SCENECLIP") == 1){

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_SCENECLIP,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      sscanf(buffer2,"%i",&scripti->ival);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"XSCENECLIP") == 1){

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_XSCENECLIP,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      sscanf(buffer2,"%i %f %i %f",&scripti->ival,&scripti->fval,&scripti->ival2,&scripti->fval2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"XSCENECLIP") == 1){

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_XSCENECLIP,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      sscanf(buffer2,"%i %f %i %f",&scripti->ival,&scripti->fval,&scripti->ival2,&scripti->fval2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"YSCENECLIP") == 1){

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_YSCENECLIP,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      sscanf(buffer2,"%i %f %i %f",&scripti->ival,&scripti->fval,&scripti->ival2,&scripti->fval2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"ZSCENECLIP") == 1){

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_ZSCENECLIP,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      sscanf(buffer2,"%i %f %i %f",&scripti->ival,&scripti->fval,&scripti->ival2,&scripti->fval2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERCLIP") == 1){

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_RENDERCLIP,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      sscanf(buffer2,"%i %i %i %i %i",&scripti->ival,&scripti->ival2,&scripti->ival3,&scripti->ival4, &scripti->ival5);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERONCE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_RENDERONCE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval2=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERDOUBLEONCE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_RENDERDOUBLEONCE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval2=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"RENDERALL") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_RENDERALL,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer,"%i",&scripti->ival);
      if(scripti->ival<1)scripti->ival=1;
      if(scripti->ival>20)scripti->ival=20;

      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval2=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"VOLSMOKERENDERALL") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_VOLSMOKERENDERALL,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer,"%i",&scripti->ival);
      if(scripti->ival<1)scripti->ival=1;
      if(scripti->ival>20)scripti->ival=20;
      scripti->exit=0;
      scripti->first=1;
      scripti->remove_frame=-1;

      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval2=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADINIFILE") == 1){
      int len;

      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADINIFILE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      len=strlen(buffer);
      NewMemory((void **)&scripti->cval,len+1);
      strcpy(scripti->cval,buffer);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADFILE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADFILE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVFILE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADVFILE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"EXIT") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_EXIT,buffer);
      scripti->cval=NULL;

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADBOUNDARY") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADBOUNDARY,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"PARTCLASSCOLOR") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_PARTCLASSCOLOR,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"PARTCLASSTYPE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_PARTCLASSTYPE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"PLOT3DPROPS") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_PLOT3DPROPS,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer2,"%i %i %i %i",&scripti->ival,&scripti->ival2,&scripti->ival3,&scripti->ival4);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SHOWPLOT3DDATA") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_SHOWPLOT3DDATA,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer2,"%i %i %i %i %f",&scripti->ival,&scripti->ival2,&scripti->ival3,&scripti->ival4,&scripti->fval);
      if(scripti->ival2==4){
        sscanf(buffer2,"%i %i %i %i %i",&scripti->ival,&scripti->ival2,&scripti->ival3,&scripti->ival4,&scripti->ival5);
      }

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADTOUR") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADTOUR,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"UNLOADTOUR") == 1){
      scripti = scriptinfo + nscriptinfo;
      scripti->cval=NULL;
      init_scripti(scripti,SCRIPT_UNLOADTOUR,buffer);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOAD3DSMOKE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOAD3DSMOKE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVOLSMOKE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADVOLSMOKE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer,"%i",&scripti->ival);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVOLSMOKEFRAME") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADVOLSMOKEFRAME,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer,"%i %i",&scripti->ival,&scripti->ival2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADSLICE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADSLICE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval2=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer,"%i %f",&scripti->ival,&scripti->fval);
      if(scripti->ival<1)scripti->ival=1;
      if(scripti->ival>3)scripti->ival=3;

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADVSLICE") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADVSLICE,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      if(fgets(buffer2,255,stream)==NULL)break;
      cleanbuffer(buffer,buffer2);
      scripti->cval2=script_set_buffer(buffer2);
      sscanf(buffer,"%i %f",&scripti->ival,&scripti->fval);
      if(scripti->ival<1)scripti->ival=1;
      if(scripti->ival>3)scripti->ival=3;

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADISO") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADISO,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADPARTICLES") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADPARTICLES,buffer);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"LOADPLOT3D") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_LOADPLOT3D,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer," %i %f",&scripti->ival,&scripti->fval);

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SETTIMEVAL") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_SETTIMEVAL,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);
      cleanbuffer(buffer,buffer2);
      sscanf(buffer,"%f",&scripti->fval);
      if(scripti->fval<0.0)scripti->fval=0.0;

      nscriptinfo++;
      continue;
    }
    if(match_upper(buffer,"SETVIEWPOINT") == 1){
      scripti = scriptinfo + nscriptinfo;
      init_scripti(scripti,SCRIPT_SETVIEWPOINT,buffer);
      if(fgets(buffer2,255,stream)==NULL)break;
      scripti->cval=script_set_buffer(buffer2);

      nscriptinfo++;
      continue;
    }
  }
  fclose(stream);
  return return_val;
}

/* ------------------ run_renderall ------------------------ */

void script_renderall(scriptdata *scripti){
  int skip_local;

  skip_local=scripti->ival;
  if(skip_local<1)skip_local=1;
  printf("script: Rendering every %i frames\n\n",skip_local);
  RenderMenu(skip_local);
}

/* ------------------ run_volsmokerenderall ------------------------ */

void script_volsmokerenderall(scriptdata *scripti){
  int skip_local;

  script_loadvolsmokeframe2();
  skip_local=scripti->ival;
  if(skip_local<1)skip_local=1;
  printf("script: Rendering every %i frames\n\n",skip_local);
  RenderMenu(skip_local);
}

/* ------------------ script_loadparticles ------------------------ */

void script_loadparticles(scriptdata *scripti){
  int i;
  int errorcode;

  printf("script: loading particles files\n\n");

  npartframes_max=get_min_partframes();
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo + i;
    if(parti->evac==1)continue;
    if(parti->version==1){
      readpart(parti->file,i,UNLOAD,&errorcode);
    }
  }
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo + i;
    if(parti->evac==1)continue;
    if(parti->version==1){
      readpart(parti->file,i,LOAD,&errorcode);
    }
  }
  force_redisplay=1;
  update_framenumber(0);
  updatemenu=1;
}

/* ------------------ script_loadiso ------------------------ */

void script_loadiso(scriptdata *scripti){
  int i;
  int errorcode;

  printf("script: loading isosurface files of type: %s\n\n",scripti->cval);

  for(i=0;i<nisoinfo;i++){
    isodata *isoi;
    int len;
    int imatch;

    isoi = isoinfo + i;
    len = strlen(scripti->cval);
    imatch = match_upper(isoi->surface_label.longlabel,scripti->cval);
    if(imatch==1||imatch==2){
      readiso(isoi->file,i,LOAD,&errorcode);
    }
  }
  force_redisplay=1;
  updatemenu=1;

}

/* ------------------ script_loadvolsmoke ------------------------ */

void script_loadvolsmoke(scriptdata *scripti){
  int imesh;

  imesh = scripti->ival;
  if(imesh==-1){
    read_vol_mesh=-1;
    read_volsmoke_allframes_allmeshes2(NULL);
  }
  else if(imesh>=0&&imesh<nmeshes){
    mesh *meshi;
    volrenderdata *vr;

    meshi = meshinfo + imesh;
    vr = &meshi->volrenderinfo;
    read_volsmoke_allframes(vr);
  }
}

/* ------------------ script_loadvolsmokeframe ------------------------ */

void script_loadvolsmokeframe(scriptdata *scripti){
  int framenum,index;
  volrenderdata *vr;
  mesh *meshi;
  int first=1;
  int i;

  index = scripti->ival;
  framenum = scripti->ival2;
  if(index>nmeshes-1)index=nmeshes-1;
  for(i=0;i<nmeshes;i++){
    if(index==i||index<0){
      meshi = meshinfo + i;
      vr = &meshi->volrenderinfo;
      read_volsmoke_frame(vr,framenum,&first);
      if(vr->times_defined==0){
        vr->times_defined=1;
        get_volsmoke_all_times(vr);
      }
      vr->loaded=1;
      vr->display=1;
    }
  }
  plotstate=getplotstate(DYNAMIC_PLOTS);
  stept=1;
  updatetimes();
  force_redisplay=1;
  update_framenumber(framenum);
  i = framenum;
  itimes=i;
  script_itime=i;
  stept=1;
  force_redisplay=1;
  update_framenumber(0);
  UpdateTimeLabels();
  keyboard('r',FROM_SMOKEVIEW);
}

/* ------------------ script_loadvolsmokeframe2 ------------------------ */

void script_loadvolsmokeframe2(void){
  scriptdata scripti;

  scripti.ival=-1;
  scripti.ival2=itimes;
  script_loadvolsmokeframe(&scripti);
}

/* ------------------ script_load3dsmoke ------------------------ */

void script_load3dsmoke(scriptdata *scripti){
  int i;
  int errorcode;

  printf("script: loading smoke3d files of type: %s\n\n",scripti->cval);

  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(match_upper(smoke3di->label.longlabel,scripti->cval)==1){
      readsmoke3d(i,LOAD,&errorcode);
    }
  }
  force_redisplay=1;
  updatemenu=1;

}

/* ------------------ script_loadslice ------------------------ */

void script_loadslice(scriptdata *scripti){
  int i;

  printf("script: loading slice files of type: %s\n\n",scripti->cval);

  for(i=0;i<nmultislices;i++){
    multislicedata *mslicei;
    slicedata *slicei;
    int j;
    float delta_orig;

    mslicei = multisliceinfo + i;
    if(mslicei->nslices<=0)continue;
    slicei = sliceinfo + mslicei->islices[0];
    if(match_upper(slicei->label.longlabel,scripti->cval)==0)continue;
    if(slicei->idir!=scripti->ival)continue;
    delta_orig = slicei->position_orig - scripti->fval;
    if(delta_orig<0.0)delta_orig = -delta_orig;
    if(delta_orig>slicei->delta_orig)continue;

    for(j=0;j<mslicei->nslices;j++){
      LoadSliceMenu(mslicei->islices[j]);
    } 
    break;
  }
}

/* ------------------ script_loadvslice ------------------------ */

void script_loadvslice(scriptdata *scripti){
  int i;
  float delta_orig;

  printf("script: loading vector slice files of type: %s\n\n",scripti->cval);

  for(i=0;i<nmultivslices;i++){
    multivslicedata *mvslicei;
    int j;
    slicedata *slicei;

    mvslicei = multivsliceinfo + i;
    if(mvslicei->nvslices<=0)continue;
    slicei = sliceinfo + mvslicei->ivslices[0];
    if(match_upper(slicei->label.longlabel,scripti->cval)==0)continue;
    if(slicei->idir!=scripti->ival)continue;
    delta_orig = slicei->position_orig - scripti->fval;
    if(delta_orig<0.0)delta_orig = -delta_orig;
    if(delta_orig>slicei->delta_orig)continue;

    for(j=0;j<mvslicei->nvslices;j++){
      LoadVSliceMenu(mvslicei->ivslices[j]);
    } 
    break;
  }
}

/* ------------------ script_loadtour ------------------------ */

void script_loadtour(scriptdata *scripti){
  int i;

  printf("script: loading tour %s\n\n",scripti->cval);
  
  for(i=0;i<ntours;i++){
    tourdata *touri;

    touri = tourinfo + i;
    if(strcmp(touri->label,scripti->cval)==0){
      TourMenu(i);
      viewtourfrompath=0;
      TourMenu(-5);
      break;
    }
  }

  force_redisplay=1;
  updatemenu=1;
}

/* ------------------ script_loadboundary ------------------------ */

void script_loadboundary(scriptdata *scripti){
  int i;
  int errorcode;

  printf("Script: loading boundary files of type: %s\n\n",scripti->cval);

  for(i=0;i<npatchinfo;i++){
    patchdata *patchi;

    patchi = patchinfo + i;
    if(strcmp(patchi->label.longlabel,scripti->cval)==0){
      LOCK_COMPRESS
      readpatch(i,LOAD,&errorcode);
      UNLOCK_COMPRESS
    }
  }
  force_redisplay=1;
  updatemenu=1;
  update_framenumber(0);

}

/* ------------------ script_partclasscolor ------------------------ */

void script_partclasscolor(scriptdata *scripti){
  int i;

  for(i=0;i<npart5prop;i++){
    part5prop *propi;

    propi = part5propinfo + i;
    if(propi->particle_property==0)continue;
    if(strcmp(propi->label->longlabel,scripti->cval)==0){
      ParticlePropShowMenu(i);
    }
  }
}


/* ------------------ script_plot3dprops ------------------------ */

void script_plot3dprops(scriptdata *scripti){
  int i, p_index;

  p_index = scripti->ival;
  if(p_index<1)p_index=1;
  if(p_index>5)p_index=5;

  visVector = scripti->ival2;
  if(visVector!=1)visVector=0;

  iveclengths = scripti->ival3;

  plotn = p_index;
  if(plotn<1){
    plotn=numplot3dvars;
  }
  if(plotn>numplot3dvars){
    plotn=1;
  }
  updateallplotslices();
  if(visiso==1)updatesurface();
  updateplot3dlistindex();

  vecfactor = get_vecfactor(&iveclengths);
  update_vector_widgets();

  printf("script: iveclengths=%i\n",iveclengths);

  contour_type=CLAMP(scripti->ival4,0,2);
  update_plot3d_display();

  if(visVector==1&&ReadPlot3dFile==1){
    mesh *gbsave,*gbi;

    gbsave=current_mesh;
    for(i=0;i<nmeshes;i++){
      gbi = meshinfo + i;
      if(gbi->plot3dfilenum==-1)continue;
      update_current_mesh(gbi);
      updateplotslice(1);
      updateplotslice(2);
      updateplotslice(3);
    }
    update_current_mesh(gbsave);
  }
}

/* ------------------ script_showplot3ddata ------------------------ */

void script_showplot3ddata(scriptdata *scripti){
  mesh *meshi;
  int imesh, dir, showhide;
  float val;
  int isolevel;

  imesh = scripti->ival-1;
  if(imesh<0||imesh>nmeshes-1)return;

  meshi = meshinfo + imesh;
  update_current_mesh(meshi);

  dir = scripti->ival2;
  if(dir<1)dir=1;
  if(dir>4)dir=4;

  plotn=scripti->ival3;

  showhide = scripti->ival4;
  val = scripti->fval;

  switch (dir){
    case 1:
      visx_all=showhide;
      iplotx_all=get_index(val,1,plotx_all,nplotx_all);
      next_xindex(1,0);
      next_xindex(-1,0);
      break;
    case 2:
      visy_all=showhide;
      iploty_all=get_index(val,2,ploty_all,nploty_all);
      next_yindex(1,0);
      next_yindex(-1,0);
      break;
    case 3:
      visz_all=showhide;
      iplotz_all=get_index(val,3,plotz_all,nplotz_all);
      next_zindex(1,0);
      next_zindex(-1,0);
      break;
    case 4:
      isolevel=scripti->ival5;
      plotiso[plotn-1]=isolevel;
      updateshowstep(showhide,ISO);
      updatesurface();
      updatemenu=1;  
      break;
    default:
      ASSERT(0);
      break;
  }
  updateplotslice(dir);

}

/* ------------------ script_partclasstype ------------------------ */

void script_partclasstype(scriptdata *scripti){
  int i;

  for(i=0;i<npart5prop;i++){
    part5prop *propi;
    int j;

    propi = part5propinfo + i;
    if(propi->display==0)continue;
    for(j=0;j<npartclassinfo;j++){
      part5class *partclassj;

      if(propi->class_present[j]==0)continue;
      partclassj = partclassinfo + j;
      if(partclassj->kind==HUMANS)continue;
      if(strcmp(partclassj->name,scripti->cval)==0){
        ParticlePropShowMenu(-10-j);
      }
    }
  }
}

/* ------------------ script_loadinifile ------------------------ */

void script_loadinifile(scriptdata *scripti){
  printf("script: loading ini file %s\n\n",scripti->cval);
  scriptinifilename2=scripti->cval;
  windowresized=0;
  readini(2);
  scriptinifilename2=NULL;

}

/* ------------------ script_loadfile ------------------------ */

void script_loadfile(scriptdata *scripti){
  int i;
  int errorcode;

  printf("script: loading file %s\n\n",scripti->cval);
  for(i=0;i<nsliceinfo;i++){
    slicedata *sd;

    sd = sliceinfo + i;
    if(strcmp(sd->file,scripti->cval)==0){
      if(i<nsliceinfo-nfedinfo){
        readslice(sd->file,i,LOAD,&errorcode);
      }
      else{
        readfed(i,LOAD,FED_SLICE,&errorcode);
      }
      return;
    }

  }
  for(i=0;i<npatchinfo;i++){
    patchdata *patchi;

    patchi = patchinfo + i;
    if(strcmp(patchi->file,scripti->cval)==0){
      readpatch(i,LOAD,&errorcode);
      return;
    }
  }
  npartframes_max=get_min_partframes();
  for(i=0;i<npartinfo;i++){
    partdata *parti;

    parti = partinfo + i;
    if(strcmp(parti->file,scripti->cval)==0){
      readpart(parti->file,i,LOAD,&errorcode);
      return;
    }
  }
  for(i=0;i<nisoinfo;i++){
    isodata *isoi;

    isoi = isoinfo + i;
    if(strcmp(isoi->file,scripti->cval)==0){
      readiso(isoi->file,i,LOAD,&errorcode);
      return;
    }
  }
  for(i=0;i<nsmoke3dinfo;i++){
    smoke3ddata *smoke3di;

    smoke3di = smoke3dinfo + i;
    if(strcmp(smoke3di->file,scripti->cval)==0){
      readsmoke3d(i,LOAD,&errorcode);
      return;
    }
  }
  for(i=0;i<nzoneinfo;i++){
    zonedata *zonei;

    zonei = zoneinfo + i;
    if(strcmp(zonei->file,scripti->cval)==0){
      readzone(i,LOAD,&errorcode);
      return;
    }
  }
  for(i=0;i<nplot3dinfo;i++){
    plot3ddata *plot3di;

    plot3di = plot3dinfo + i;
    if(strcmp(plot3di->file,scripti->cval)==0){
      ReadPlot3dFile=1;
      readplot3d(plot3di->file,i,LOAD,&errorcode);
      update_menu();
      return;
    }
  }

  printf("script: file %s was not loaded\n",scripti->cval);

}


/* ------------------ script_loadplot3d ------------------------ */

void script_loadplot3d(scriptdata *scripti){
  int i;
  float time_local;
  int blocknum;

  time_local = scripti->fval;
  blocknum = scripti->ival-1;

  for(i=0;i<nplot3dinfo;i++){
    plot3ddata *plot3di;

    plot3di = plot3dinfo + i;
    if(plot3di->blocknumber==blocknum&&ABS(plot3di->time-time_local)<0.5){
      LoadPlot3dMenu(i);
    }
  }
  updatecolors(-1);
  set_labels_controls();

  //update_menu();
}

/* ------------------ script_loadvfile ------------------------ */

void script_loadvfile(scriptdata *scripti){
  int i;

  printf("script: loading vector slice file %s\n\n",scripti->cval);
  for(i=0;i<nvsliceinfo;i++){
    slicedata *val;
    vslicedata *vslicei;

    vslicei = vsliceinfo + i;
    val = sliceinfo + vslicei->ival;
    if(val==NULL)continue;
    if(strcmp(val->reg_file,scripti->cval)==0){
      LoadVSliceMenu(i);
      return;
    }
  }
  printf("script: vector slice file %s was not loaded\n",scripti->cval);

}

/* ------------------ script_settimeval ------------------------ */

void script_settimeval(scriptdata *scripti){
  float timeval;
  int i;

  timeval = scripti->fval;
  printf("script: setting time to %f\n\n",timeval);
  if(global_times!=NULL&&nglobal_times>0){
    if(timeval<global_times[0])timeval=global_times[0];
    if(timeval>global_times[nglobal_times-1]-0.0001)timeval=global_times[nglobal_times-1]-0.0001;
    for(i=0;i<nglobal_times-1;i++){
      if(global_times[i]<=timeval&&timeval<global_times[i+1]){
        itimes=i;
        script_itime=i;
        stept=0;
        force_redisplay=1;
        update_framenumber(0);
        UpdateTimeLabels();
        break;
      }
    }
  }
}

/* ------------------ settimeval ------------------------ */

void settimeval(float timeval){
  int i;

  if(global_times!=NULL&&nglobal_times>0){
    if(timeval<global_times[0])timeval=global_times[0];
    if(timeval>global_times[nglobal_times-1]-0.0001)timeval=global_times[nglobal_times-1]-0.0001;
    for(i=0;i<nglobal_times;i++){
      float tlow, thigh;

      if(i==0){
        tlow = global_times[i];
        thigh = (global_times[i]+global_times[i+1])/2.0;
      }
      else if(i==nglobal_times-1){
        tlow = (global_times[i-1]+global_times[i])/2.0;
        thigh = global_times[i];
      }
      else{
        tlow=(global_times[i-1]+global_times[i])/2.0;
        thigh=(global_times[i]+global_times[i+1])/2.0;
      }
      if(tlow<=timeval&&timeval<thigh){
        itimes=i;
        stept=1;
        force_redisplay=1;
        update_framenumber(0);
        UpdateTimeLabels();
        keyboard('t',FROM_SMOKEVIEW);
        break;
      }
    }
  }
}

/* ------------------ script_setviewpoint ------------------------ */

void script_setviewpoint(scriptdata *scripti){
  char *viewpoint;
  camera *ca;

  viewpoint = scripti->cval;
  printf("script: set viewpoint to %s\n\n",viewpoint);
  for(ca=camera_list_first.next;ca->next!=NULL;ca=ca->next){
    if(strcmp(scripti->cval,ca->name)==0){
      ResetMenu(ca->view_id);
      break;
    }
  }
}

/* ------------------ run_script ------------------------ */

int run_script(void){

// This procedure should return 1 if the smokeview frame should not be advanced.
// (to ensure images are rendered at the right time step)

  scriptdata *scripti;
  int returnval;

  returnval=0;

  if(current_script_command>scriptinfo+nscriptinfo-1){
    current_script_command=NULL;
    return returnval;
  }
  scripti = current_script_command;
  printf("\n");
  printf("script: %s\n",scripti->command_label);
  if(scripti->cval!=NULL){
    printf("script:  %s\n",scripti->cval);
  }
  if(scripti->cval2!=NULL){
    printf("script:  %s\n",scripti->cval2);
  }
  printf("\n");
  switch (scripti->command){
    case SCRIPT_UNLOADALL:
      LoadUnloadMenu(UNLOADALL);
      break;
    case SCRIPT_RENDERDIR:
      if(scripti->cval!=NULL&&strlen(scripti->cval)>0){
        script_dir_path=scripti->cval;
        if(can_write_to_dir(script_dir_path)==0){
          fprintf(stderr,"*** Error: Cannot write to the RENDERDIR directory: %s\n",script_dir_path);
        }
        printf("script: setting render path to %s\n",script_dir_path);
      }
      else{
        script_dir_path=NULL;
      }
      break;
    case SCRIPT_KEYBOARD:
      {
        char *key;

        script_keystate=0;
        key = scripti->cval + strlen(scripti->cval) - 1;
        if(strncmp(scripti->cval,"ALT",3)==0)script_keystate=GLUT_ACTIVE_ALT;

        keyboard(*key,FROM_SCRIPT);
        returnval=1;
      }
      break;
    case SCRIPT_SCENECLIP:
      xyz_clipplane=scripti->ival;
      updatefacelists=1;
      break;
    case SCRIPT_XSCENECLIP:
      clip_x=scripti->ival;
      clip_x_val = scripti->fval;
      
      clip_X=scripti->ival2;
      clip_X_val = scripti->fval2;
      updatefacelists=1;
      break;
    case SCRIPT_YSCENECLIP:
      clip_y=scripti->ival;
      clip_y_val = scripti->fval;
      
      clip_Y=scripti->ival2;
      clip_Y_val = scripti->fval2;
      updatefacelists=1;
      break;
    case SCRIPT_ZSCENECLIP:
      clip_z=scripti->ival;
      clip_z_val = scripti->fval;
      
      clip_Z=scripti->ival2;
      clip_Z_val = scripti->fval2;
      updatefacelists=1;
      break;
    case SCRIPT_RENDERCLIP:
      clip_rendered_scene=scripti->ival;
      render_clip_left=scripti->ival2;
      render_clip_right=scripti->ival3;
      render_clip_bottom=scripti->ival4;
      render_clip_top=scripti->ival5;
      break;
    case SCRIPT_RENDERONCE:
      keyboard('r',FROM_SMOKEVIEW);
      returnval=1;
      break;
    case SCRIPT_RENDERDOUBLEONCE:
      keyboard('R',FROM_SMOKEVIEW);
      returnval=1;
      break;
    case SCRIPT_RENDERALL:
      script_renderall(scripti);
      break;
    case SCRIPT_VOLSMOKERENDERALL:
      script_volsmokerenderall(scripti);
      break;
    case SCRIPT_LOADFILE:
      script_loadfile(scripti);
      break;
    case SCRIPT_LOADINIFILE:
      script_loadinifile(scripti);
      break;
    case SCRIPT_LOADVFILE:
      script_loadvfile(scripti);
      break;
    case SCRIPT_LOADBOUNDARY:
      script_loadboundary(scripti);
      break;
    case SCRIPT_PARTCLASSCOLOR:
      script_partclasscolor(scripti);
      break;
    case SCRIPT_SHOWPLOT3DDATA:
      script_showplot3ddata(scripti);
      break;
    case SCRIPT_PLOT3DPROPS:
      script_plot3dprops(scripti);
      break;
    case SCRIPT_PARTCLASSTYPE:
      script_partclasstype(scripti);
      break;
    case SCRIPT_LOADTOUR:
      script_loadtour(scripti);
      break;
    case SCRIPT_UNLOADTOUR:
      TourMenu(-2);
      break;
    case SCRIPT_EXIT:
#ifndef _DEBUG
      exit(0);
#endif
      break;
    case SCRIPT_LOADISO:
      script_loadiso(scripti);
      break;
    case SCRIPT_LOAD3DSMOKE:
      script_load3dsmoke(scripti);
      break;
    case SCRIPT_LOADVOLSMOKE:
      script_loadvolsmoke(scripti);
      break;
    case SCRIPT_LOADVOLSMOKEFRAME:
      script_loadvolsmokeframe(scripti);
      returnval=1;
      break;
    case SCRIPT_LOADPARTICLES:
      script_loadparticles(scripti);
      break;
    case SCRIPT_LOADSLICE:
      script_loadslice(scripti);
      break;
    case SCRIPT_LOADVSLICE:
      script_loadvslice(scripti);
      break;
    case SCRIPT_LOADPLOT3D:
      script_loadplot3d(scripti);
      break;
    case SCRIPT_SETTIMEVAL:
      returnval=1;
      script_settimeval(scripti);
      break;
    case SCRIPT_SETVIEWPOINT:
      script_setviewpoint(scripti);
      break;
    default:
      ASSERT(0);
      break;
  }
  glutPostRedisplay();
  return returnval;
}
