all :

#cw :
#	cl -MT -EHsc cw.cpp acandidatewindows.cpp -link user32.lib gdi32.lib array.res -machine:x86

array31x86 : adll.cpp arrayts30.cpp arrayts40.cpp arrayts123.cpp aarrayts.cpp acandidatewindows.cpp aclassfactory.cpp aeditsession.cpp aarraylex.cpp akeyboardbaritem.cpp
	rc array.rc
  cl -Ox -LD -MT -EHsc $** -link -def:adll.def -dll user32.lib gdi32.lib Advapi32.lib ole32.lib oleaut32.lib -manifest -manifestfile:adll.manifest array.res -machine:x86
	ren adll.dll aarray.dll

array31x64 : adll.cpp arrayts30.cpp arrayts40.cpp arrayts123.cpp aarrayts.cpp acandidatewindows.cpp aclassfactory.cpp aeditsession.cpp aarraylex.cpp akeyboardbaritem.cpp
  rc array.rc
  cl -Ox -LD -MT -EHsc $** -link -def:adll.def -dll user32.lib gdi32.lib Advapi32.lib ole32.lib oleaut32.lib -manifest -manifestfile:adll.manifest array.res -machine:x64
  ren adll.dll aarray64.dll

clean:
  del *.obj *.exp *.RES *.dll *.LIB *.manifest