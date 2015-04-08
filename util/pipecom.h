/* ----------------------------------------------------------------------
 * Where........: CMAP - Polytechnique 
 * File.........: pipecom.h
 * Author.......: Bertrand Lamy (Equipe genetique)
 * Created......: Thu Mar  9 17:21:15 1995
 * Description..: Pipe communication with a process
 * 
 * Ident........: $Id: pipecom.h,v 1.2 1995/05/11 16:16:27 lamy Exp $
 * ----------------------------------------------------------------------
 */

#ifndef PIPECOM_H
#define PIPECOM_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


typedef struct PipeCommunication { 
    FILE	*fWrit;
    FILE	*fRead;
    int		pid;
} PCom;
    


PCom *PipeComOpen( char *prog );
PCom *PipeComOpenArgv( char *prog, char *argv[] );

int PipeComSend( PCom *to, char *line );
int PipeComSendn( PCom *to, char *data, int n );

int PipeComReceive( PCom *from, char *data, int max );

int PipeComClose( PCom *to );

int PipeComWaitFor( PCom *from, char *what );

#ifdef __cplusplus
} /* ferme extern "C" */
#endif

#endif	/* 		PIPECOM_H */
