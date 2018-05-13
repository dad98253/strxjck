/* recognize '...' otherwise see " as start of string: */

     int c='\"', d='\'', e = '\012'; // comment line 3

 /* recognize "..." otherwise see comments here: */

     char s[] = "abc/*not a comment*/efg\"ZZ\'";
#pragma warning( push )
#pragma warning( disable : 4129 )
     char t[] = "ABC//not a comment//EFG\x012\/\/";
#pragma warning( pop )

char *p = ""; //

int dd = '/*'; // comment line 13

/*/*/
/**/
/*Z*/
/***/
/****/
/**A**/
#ifndef bool
    #define bool int
    #define false ((bool)0)
    #define true  ((bool)1)
#endif
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
// usage:
// \dos\find \users\dad\passwin -name *.cpp -print0 -maxdepth 1 | xargs -0 strxjck
//
// or, on WINDOZ (one file at a time):
//
// dir /s /b *.cpp > temp.txt
// FOR /F %%k in (temp.txt) DO strxjck %%k
//
int main(int argc, char** argv)
{
  FILE* f;
  FILE* f2;
  FILE* f3;
  FILE* f4;
  FILE* f5;
  int c[2];
  int linum;
  int i,j,iPreviousState,nSubName;
  int iPrevisousParenState[500];
  int parennest;
  int idefine = 200;
  int debug = 0;
  int colnum;
  int colnumin;
  char *idot;
  char *irev;
  char namex[250];
  char namef[250];
  char subname[250];
  char AA = 'A';
  char AB;
  bool nextLF;
  bool isLF;

  enum {
      INITIAL,
      CCOMMENT1,
      CCOMMENT2,
      CCOMMENT3,
      CPPCOMMENT1,
      CPPCOMMENT2,
	  CPPCOMMENT3,
      STRING0,
      STRING1,
      STRING2,
      STRING3,
      CHAR1,
      CHAR2,
	  PAREN0,
	  PAREN1,
	  PAREN2,
	  PRAGMA
  } state = INITIAL;

  nextLF = false;
  isLF = false;

  f2 = fopen("johns.txt","wt");
  f3 = fopen("johnsstr.h.txt","wt");
  if (debug == 1 ) f5 = fopen("debug.txt","wt");

  for (i=1;i<argc && (f = fopen(argv[i], "rt")) != NULL;i++) 
  {
	fprintf(f2,"\nSTRINGTABLE DISCARDABLE\nBEGIN\n");
	idot = strrchr(argv[i],'.');
	if (idot == NULL) idot = argv[i]+strlen(argv[i]);
	irev = strrchr(argv[i],'\\');
	if (irev == NULL) {
		irev = argv[i];
	} else {
		irev++;
	}
	for(j=0;irev<idot;j++,irev++)namex[j]=toupper(*irev),namef[j]=*irev;
	namex[j]=namef[j]='\000';
	strcat(namef,"_strx.cpp");
//	strcat(namef,".cpp");
	f4 = fopen(namef,"wt");
	AB = AA;
	linum = 1;
	colnumin = 0;
	colnum = 0;
    parennest = 0;
	nextLF = false;
	isLF = false;
	iPrevisousParenState[0] = INITIAL;
	nSubName = 0;

    if ((c[0] = fgetc(f)) == EOF)
      goto line100;
	colnumin++;
	if (c[0] == '\n') isLF = true;

    while ((c[1] = fgetc(f)) != EOF)
    {
		colnumin++;
		if (isLF) {
			colnumin = 0;
			isLF = false;
		}
		if (nextLF) {
			isLF = true;
			nextLF = false;
		}
		if (c[1] == '\n') {
		if (debug == 1 ) fprintf (f5,"\n//%s line = %i, state = %i\n", namef, linum, state);
			linum++;
			AB = AA;
			colnum = 0;
			nextLF = true;
			if (state == PRAGMA) state = iPreviousState;
		}
      switch (state)
      {
      case INITIAL:
		fprintf(f4,"%c", c[0]);
		colnum++;
        if (c[0] == '/' && c[1] == '*')
          state = CCOMMENT1, printf("<C comment>\n");
        else if (c[0] == '/' && c[1] == '/')
		{
          state = CPPCOMMENT1;
		  printf("<C++ comment>\n");
		}
        else if (c[0] == '(')
//		  state = PAREN1, printf("<inside parenthetical>\n");
		{
			if ( strncmp(subname,"MsgBox",strlen("MsgBox")) == 0 ) {
				state = PAREN1;
			} else {
				state = PAREN0;
			}
			printf("<inside parenthetical>\n");
			parennest++;
			iPrevisousParenState[parennest] = state;
//			fprintf(f4,"%c", c[0]);
//			fprintf(f2,"<inside parenthetical line %i> \"",linum);
		}
        else if (c[0] == ')')
//		  state = PAREN1, printf("<inside parenthetical>\n");
		{
			state = INITIAL;
			printf("<pop out of last parenthetical>\n");
			parennest = 0;
//			fprintf(f2,"<inside parenthetical line %i> \"",linum);
		}
        else if (c[0] == '\'')
          state = CHAR1, printf("<Char literal>\n");
		else if (c[0] == '#')
		{
			iPreviousState = state;
			state = PRAGMA;
			printf("<precompiler>\n");
		}

        break;

      case CCOMMENT1:
      case CPPCOMMENT1:
        /* skip * in /* and 2nd / in // */
        state++;
		fprintf(f4,"%c", c[0]);
		colnum++;
        break;

      case CCOMMENT2:
		fprintf(f4,"%c", c[0]);
		colnum++;
        if (c[0] == '*' && c[1] == '/')
          state++, printf("\n</C comment>\n");
        else
          printf("%c", c[0]);
        break;

      case CCOMMENT3:
		colnum++;
		fprintf(f4,"%c", c[0]);
        // skip / in */
			if ( parennest == 0 ) {
				state = INITIAL;
			} else {
				state = PAREN1;
			}
        break;

      case CPPCOMMENT2:
		fprintf(f4,"%c", c[0]);
		colnum++;
		if ( colnumin == 3 && c[0] == '/' && c[1] == ' ' ) {
			state = CPPCOMMENT3;
			fprintf(f2,"///");
		}
        if (c[0] == '\n')
          state = INITIAL, printf("\n</C++ " "comment>\n");
        else
          printf("%c", c[0]);
        break;

      case CPPCOMMENT3:
		fprintf(f4,"%c", c[0]);
		fprintf(f2,"%c", c[0]);
		colnum++;
        if (c[0] == '\n') {
          state = INITIAL;
		  printf("\n</C++ " "comment>\n");
        } else {
          printf("%c", c[0]);
		}
        break;

      case STRING0:
		 if (c[0] == '"')
//          state = INITIAL, printf("\n</String literal>\n");
		{   
			state = PAREN0;
			printf("\n</String literal>\n");
//			fprintf(f2,"\"\n");
		}
        fprintf(f4,"%c", c[0]);
		colnum++;
        break;

      case STRING1:
		if (c[0] == '"')
//          state = INITIAL, printf("\n</String literal>\n");
		{   
			state = STRING3;
			printf("\n</String literal>\n");
//			fprintf(f2,"\"\n");
		}
        else if (c[0] == '\\')
//          state = STRING2, printf("%c", c[0]);
		{
			state = STRING2;
			printf("%c", c[0]);
			fprintf(f2,"\\");
		}
        else
//          printf("%c", c[0]);
		{
			printf("%c", c[0]);
			fprintf(f2,"%c", c[0]);
		}
        break;

      case STRING2:
        // skip escaped character
//        state = STRING1, printf("%c", c[0]);
		  {
			  state = STRING1, printf("%c", c[0]);
			  fprintf(f2,"%c", c[0]);
		  }
        break;

      case STRING3:
		  if ( isspace(c[0]) != 0 ) {
			  fprintf(f4,"%c", c[0]);
			  colnum++;
		  } else if ( c[0] == '"' ) {
			  state = STRING1;
			  printf("%c", c[0]);
		  }	else if (c[0] == ')') {
			  fprintf(f4,"%c", c[0]);
			  colnum++;
			  parennest--;
			  state = iPrevisousParenState[parennest];
			  fprintf(f2,"\"\n");
		  } else {
			  state = iPrevisousParenState[parennest];
			  printf("\n</String literal>\n");
			  fprintf(f2,"\"\n");
			  fprintf(f4,"%c", c[0]);
			  colnum++;
		  }
		break;

      case CHAR1:
		fprintf(f4,"%c", c[0]);
		colnum++;
        if (c[0] == '\'')
		{
		  state = iPrevisousParenState[parennest];
          printf("\n</Char literal>\n");
		}
        else if (c[0] == '\\')
          state = CHAR2, printf("%c", c[0]);
        else
          printf("%c", c[0]);
        break;

      case CHAR2:
		fprintf(f4,"%c", c[0]);
		colnum++;
        // skip escaped character
        state = CHAR1, printf("%c", c[0]);
        break;

	  case PAREN0:
        if (c[0] == '(')
		  {
			if ( strncmp(subname,"MsgBox",strlen("MsgBox")) == 0 ) {
				state = PAREN1;
			} else {
				state = PAREN0;
			}
			printf("<inside parenthetical>\n");
			parennest++;
			iPrevisousParenState[parennest] = state;
			fprintf(f4,"%c", c[0]);
			colnum++;
//			fprintf(f2,"<inside parenthetical line %i> \"",linum);
		}
        else if (c[0] == ')')
		{
			fprintf(f4,"%c", c[0]);
			colnum++;
			parennest--;
			state = iPrevisousParenState[parennest];
//			fprintf(f2,"\\");
		}
        else if (c[0] == '\'')
		{
			fprintf(f4,"%c", c[0]);
			colnum++;
			state = CHAR1, printf("<Char literal>\n");
		}
        else if (c[0] == '"')
		{
			state = STRING0;
			printf("<String literal>\n");
			fprintf(f4,"%c", c[0]);
			colnum++;
		} else {
			fprintf(f4,"%c", c[0]);
			colnum++;
			printf("%c", c[0]);
//			fprintf(f2,"%c", c[0]);
		}
        break;


      case PAREN1:
        if (c[0] == '(')
		{
			if ( strncmp(subname,"MsgBox",strlen("MsgBox")) == 0 ) {
				state = PAREN1;
			} else {
				state = PAREN0;
			}
			fprintf(f4,"%c", c[0]);
			colnum++;
			parennest++;
			iPrevisousParenState[parennest] = state;
//			fprintf(f2,"\"\n");
		}
        else if (c[0] == ')')
		{
			fprintf(f4,"%c", c[0]);
			colnum++;
			parennest--;
			state = iPrevisousParenState[parennest];
//			fprintf(f2,"\\");
		}
        else if (c[0] == '"')
		{
			state = STRING1;
			printf("<String literal>\n");
			fprintf(f2,"\t%s%i%c\t\t\"",namex,linum,AB);
//			fprintf(f4,"strxBW(%s%i%c)",namex,linum,AB);
			fprintf(f4,"%s%i%c",namex,linum,AB);
			colnum++;
			fprintf(f3,"#define %s%i%c\t\t%i\n",namex,linum,AB,idefine);
			AB++;
			idefine++;
		}
        else if (c[0] == '\'')
		{
			fprintf(f4,"%c", c[0]);
			colnum++;
			state = CHAR1, printf("<Char literal>\n");
		}
        else
//          printf("%c", c[0]);
		{
			fprintf(f4,"%c", c[0]);
			colnum++;
			printf("%c", c[0]);
//			fprintf(f2,"%c", c[0]);
		}
        break;

      case PAREN2:
        // skip escaped character
//        state = PAREN1, printf("%c", c[0]);
		  {
			  state = PAREN1;
			  printf("%c", c[0]);
			  fprintf(f2,"%c", c[0]);
		  }
        break;

	  case PRAGMA:
		  {
			  fprintf(f4,"%c", c[0]);
			  colnum++;
			  printf("%c", c[0]);
		  }


      }   // end of case

      c[0] = c[1];
	  if ( !isspace(c[0]) && ( c[0] != ')' ) ) {
		  if ( (nSubName == 0 && __iscsymf(c[0])) || (nSubName != 0 && __iscsym(c[0])) ) {
			  subname[nSubName] = c[0];
			  nSubName++;
			  assert(nSubName<250);
		  } else {
			  subname[nSubName] = '\000';
			  nSubName = 0;
		  }
	  }

    }  // end of while
line100:
    fclose(f);
	state = INITIAL;
	fprintf(f2,"END\n\n");
	fprintf(f3,"\n");
	fprintf(f4,"%c", c[0]);
	colnum++;
	fclose(f4);
  }  // end of for

  fclose(f2);
  fclose(f3);
  return 0;
}