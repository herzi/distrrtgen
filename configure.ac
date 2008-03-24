AC_INIT(distrrtgen,2007-11-30,http://www.freerainboxtables.com/phpBB3/viewforum.php?f=10)

AC_PROG_CC
AC_PROG_CXX

AC_MSG_CHECKING([if this is a Mac])
if test `uname -s` = "Darwin"; then
	PLATFORM_LDADD="-framework CoreServices"
	AC_MSG_RESULT([yes, adding CoreServices framework])
else
	PLATFORM_CFLAGS="-march=pentium3"
	AC_MSG_RESULT([no])
fi
AC_SUBST(PLATFORM_CFLAGS)
AC_SUBST(PLATFORM_LDADD)

AC_OUTPUT(Makefile)
