AC_INIT(src/MeepBot.h)
AM_INIT_AUTOMAKE(MeepBot,0.5)
AC_CONFIG_HEADER(src/config.h)

AC_PROG_CC
AC_PROG_CXX

test -z "$CFLAGS" && CFLAGS="-O3 -Wall -pedantic"
test -z "$CXXFLAGS" && CXXFLAGS="-O3 -Wall -pedantic"

# TODO: add toggle for internal/external sqlite3 build
AC_CONFIG_SUBDIRS(sqlite3)

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(src/Makefile)
AC_OUTPUT
