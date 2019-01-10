head	1.1;
access;
symbols;
locks
	malcolm:1.1; strict;
comment	@:: @;


1.1
date	2008.10.07.06.36.13;	author malcolm;	state Exp;
branches;
next	;


desc
@@


1.1
log
@Initial revision
@
text
@zip -X  ..\sedt32bck-try4.zip *.c 
zip -X  ..\sedt32bck-try4.zip *.h
zip -X  ..\sedt32bck-try4.zip *.ide
zip -X  ..\sedt32bck-try4.zip *.asm
zip -X  ..\sedt32bck-try4.zip *.mar
zip -X  ..\sedt32bck-try4.zip *.cpp
zip -X  ..\sedt32bck-try4.zip *.bat
zip -rX ..\sedt32bck-try4.zip documentation\*
zip -rX ..\sedt32bck-try4.zip mapfiles.etc\*
zip -X  ..\sedt32bck-try4.zip redhat\*
zip -X  ..\sedt32bck-try4.zip RCS\*
zip -X  ..\sedt32bck-try4.zip WIN32\* -x *.obj
zip -X  ..\sedt32bck-try4.zip WIN32\RCS\* -x *.obj
zip -X  ..\sedt32bck-try4.zip DOS\*   -x *.obj
zip -X  ..\sedt32bck-try4.zip DOS\RCS\*   -x *.obj
@
