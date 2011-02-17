AC_INIT(src/MeepBot.h)
AM_INIT_AUTOMAKE(MeepBot,0.5)
AC_CONFIG_HEADER(src/config.h)

AC_PROG_CC
AC_PROG_CXX

AC_MSG_CHECKING([build flags])
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [build with debugging flags])],
	[debug="$enableval"], [debug=no])
AC_MSG_RESULT([$debug])

if test "$debug" = "yes"; then
    AC_DEFINE( [DEBUG],[1],[Debugging build] )
    AM_CXXFLAGS="$AM_CXXFLAGS -ggdb"
else
    AM_CXXFLAGS="$AM_CXXFLAGS -O3 -fno-exceptions -fomit-frame-pointer"
fi

# TODO: add toggle for dynamic/static sqlite3 linking.
# Currently, we just build it statically for versioning's sake.
AC_CONFIG_SUBDIRS(sqlite3)

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(src/Makefile)
AC_OUTPUT
