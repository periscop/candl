dnl /**-------------------------------------------------------------------**
dnl  **                              CAnDL                                **
dnl  **-------------------------------------------------------------------**
dnl  **                           configure.in                            **
dnl  **-------------------------------------------------------------------**
dnl  **                 First version: september 8th 2003                 **
dnl  **-------------------------------------------------------------------**/
dnl 
dnl /**************************************************************************
dnl *   CAnDL : the Chunky Analyser for Dependences in Loops (experimental)   *
dnl ***************************************************************************
dnl *                                                                         *
dnl * Copyright (C) 2003-2008 Cedric Bastoul                                  *
dnl *                                                                         *
dnl * This is free software; you can redistribute it and/or modify it under   *
dnl * the terms of the GNU General Public License as published by the Free    *
dnl * Software Foundation; either version 2 of the License, or (at your       *
dnl * option) any later version.                                              *
dnl *                                                                         *
dnl * This software is distributed in the hope that it will be useful, but    *
dnl * WITHOUT ANY WARRANTY; without even the implied warranty of              *
dnl * MERCHANTABILITYor FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
dnl * General Public License for more details.                                *
dnl *                                                                         *
dnl * You should have received a copy of the GNU General Public License along *
dnl * with software; if not, write to the Free Software Foundation, Inc.,     *
dnl * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA                  *
dnl *                                                                         *
dnl * CAnDL, the Chunky Dependence Analyser                                   *
dnl * Written by Cedric Bastoul, Cedric.Bastoul@inria.fr                      *
dnl *                                                                         *
dnl ***************************************************************************/

m4_define([version_major], [0])
m4_define([version_minor], [0])
m4_define([version_revision], [1])

AC_PREREQ(2.53)

dnl Fill here the @bug email adress.
AC_INIT([candl], [0.6.2], [cedric.bastoul@inria.fr,pouchet@cse.ohio-state.edu])

AC_CONFIG_SRCDIR([include/candl/candl.h])

dnl Put as most as possible configuration files to an auxialiry
dnl directory.
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_MACRO_DIR([m4])

dnl Initialize automake. Here, a special tar version that enables
dnl (very) long filenames.
#AM_INIT_AUTOMAKE([1.9 tar-ustar no-define foreign dist-bzip2])
AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

dnl default version
BITS="MP"
CPPFLAGS=-DLINEAR_VALUE_IS_MP


dnl /**************************************************************************
dnl  *                              Checking                                  *
dnl  **************************************************************************/


dnl Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROG(CD, cd)
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_CHECK_PROGS(DOXYGEN,doxygen,doxygen)

AX_CC_MAXOPT
AC_SUBST(CFLAGS_WARN)
AX_CFLAGS_WARN_ALL(CFLAGS_WARN)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stddef.h stdlib.h string.h strings.h unistd.h])

dnl Checks for library functions.
AC_CHECK_FUNCS(strtol)


dnl /**************************************************************************
dnl  *                             Option setting                             *
dnl  **************************************************************************/

dnl /**************************************************************************
dnl  *                   Where is the OpenScop Library?                       *
dnl  **************************************************************************/

AX_SUBMODULE(osl,system|build|bundled,system)

AC_SUBST(OSL_CPPFLAGS)
AC_SUBST(OSL_LDFLAGS)
AC_SUBST(OSL_LIBS)
case "$with_osl" in
bundled)
  OSL_CPPFLAGS="-I$srcdir/osl/include -Iosl/include"
  OSL_LIBS="$srcdir/osl/libosl.la"
  ;;
build)
  OSL_CPPFLAGS="-I$osl_srcdir/include -I$with_osl_builddir/include"
  OSL_LIBS="$with_osl_builddir/libosl.la"
  ;;
system)
  if test "x$with_osl_prefix" != "x"; then
    OSL_CPPFLAGS="-I$with_osl_prefix/include"
  fi
  if test "x$with_osl_exec_prefix" != "x"; then
    OSL_LDFLAGS="-L$with_osl_exec_prefix/lib"
  fi
  OSL_LIBS="$with_osl_prefix/lib/libosl.la -losl"
esac
AM_CONDITIONAL(BUNDLED_OSL, test $with_osl = bundled)



dnl GMP

dnl Some default values cause I'm not sure whether autoconf set them, while
dnl documentation says it does...
gmp_package="yes"
gmp_include_package="yes"
gmp_library_package="yes"

NEED_MP="no"

dnl --with-gmp=gmp-path
AC_ARG_WITH(gmp,
        [  --with-gmp=DIR          DIR where the gmp package is installed],
  [ echo "Package gmp : $withval" &&
    gmp_package=$withval &&
          GMP_INC=$gmp_package/include &&
          GMP_LIB=$gmp_package/lib &&
          CPPFLAGS=-DLINEAR_VALUE_IS_MP &&
          NEED_MP="yes"])

AC_ARG_WITH(gmp-include,
        [  --with-gmp-include=DIR  DIR where gmp.h is installed],
  [ echo "Package gmp-include : $withval" &&
    gmp_include_package=$withval &&
          GMP_INC=$gmp_include_package &&
          CPPFLAGS=-DLINEAR_VALUE_IS_MP &&
    NEED_MP="yes"])

AC_ARG_WITH(gmp-library,
        [  --with-gmp-library=DIR  DIR where the gmp library is installed],
  [ echo "Package gmp-library : $withval" &&
    gmp_library_package=$withval &&
          GMP_LIB=$gmp_library_package &&
          CPPFLAGS=-DLINEAR_VALUE_IS_MP &&
    NEED_MP="yes"])

AC_ARG_ENABLE(int-version,
  [  --enable-int-version    'int' (32 bits) version is built],
  [ echo "Package int : $enableval" &&
    BITS="32" &&
          CPPFLAGS=-DLINEAR_VALUE_IS_LONG])

AC_ARG_ENABLE(llint-version,
  [  --enable-llint-version  'long long int' (64 bits) version is built],
  [ echo "Package long long int : $enableval" &&
    BITS="64" &&
          CPPFLAGS=-DLINEAR_VALUE_IS_LONGLONG])

AC_ARG_ENABLE(mp-version,
  [  --enable-mp-version     'MP' (multiple precision) version is built],
  [ echo "Package mp : $enableval" &&
    BITS="MP" &&
          CPPFLAGS=-DLINEAR_VALUE_IS_MP &&
          NEED_MP="yes"])

AC_ARG_ENABLE(piplib-hybrid,
  [  --enable-piplib-hybrid   Link with piplib-hybrid],
  [ echo "Piplib-hybrid support : $enableval" &&
          if test "x$enableval" = "xyes"; then 
            CPPFLAGS=-DCANDL_HAS_PIPLIB_HYBRID
          fi])


dnl /**************************************************************************
dnl  *                   Where is the PipLib Library?                         *
dnl  **************************************************************************/

AX_SUBMODULE(piplib,system|build|bundled,system)

AC_SUBST(PIPLIB_CPPFLAGS)
AC_SUBST(PIPLIB_LDFLAGS)
AC_SUBST(PIPLIB_LIBS)
case "$with_piplib" in
bundled)
  PIPLIB_CPPFLAGS="-I$srcdir/piplib/include -Ipiplib/include"
  PIPLIB_LIBS="$srcdir/piplib/libpiplib$BITS.la"
  ;;
build)
  PIPLIB_CPPFLAGS="-I$piplib_srcdir/include -I$with_piplib_builddir/include"
  PIPLIB_LIBS="$with_piplib_builddir/libpiplib$BITS.la"
  ;;
system)
  if test "x$with_piplib_prefix" != "x"; then
    PIPLIB_CPPFLAGS="-I$with_piplib_prefix/include"
  fi
  if test "x$with_piplib_exec_prefix" != "x"; then
    PIPLIB_LDFLAGS="-L$with_piplib_exec_prefix/lib"
  fi
  PIPLIB_LIBS="$with_piplib_prefix/lib/libpiplib$BITS.la -lpiplib$BITS"
esac
AM_CONDITIONAL(BUNDLED_PIPLIB, test $with_piplib = bundled)



dnl /**************************************************************************
dnl  *                            Where is GMP?                               *
dnl  **************************************************************************/


dnl Checking for gmp
AC_MSG_CHECKING(whether gmp works)
if test "$gmp_package" = "no"; then
  echo "GMP package not defined"
  AC_MSG_RESULT(no)
  TO_BUILD_MP=""
else
  if test "$NEED_MP" = "no"; then
    echo "Mode normal GMP"
    TO_BUILD="$TO_BUILD MP"
    AC_CHECK_HEADER(gmp.h,
                    [AC_SEARCH_LIBS([__gmpz_init], [gmp],
                      [LIBS="$LIBS -lgmp"],
          [echo "Can't find gmp library." &&
           echo "MP version will not be built." &&
           TO_BUILD_MP=""])],
        [echo "Can't find gmp headers." &&
         echo "MP version will not be built." &&
         TO_BUILD_MP=""])
  else
    dnl Default given by --with-X is "yes", --without-X is "no". We also
    dnl initialized manually all gmp_package* variables to "yes" (thus they are
    dnl supposed to be "yes" except if the user set them himself).
    if test "$gmp_package" != "yes" ; then
      echo "(GMP path has been set by user)"
      GMP_DIR=$gmp_package
      dnl Useful for AC_CHECK_X to find what we want.
      CPPFLAGS="-I$GMP_DIR/include $CPPFLAGS"
      LDFLAGS="-L$GMP_DIR/lib $LDFLAGS"
    fi

    if test "$gmp_include_package" != "yes" ; then
      CPPFLAGS="-I$GMP_INC $CPPFLAGS"
    fi

    if test "$gmp_library_package" != "yes" ; then
      LDFLAGS="-L$GMP_LIB $LDFLAGS"
    fi

    AC_CHECK_HEADER(gmp.h,
                    [],
                    [AC_MSG_ERROR(Can't find gmp headers.)])
    AC_SEARCH_LIBS([__gmpz_init], [gmp],
                 [LIBS="$LIBS -lgmp"],
                 [AC_MSG_ERROR(Can't find gmp library.)])

    AC_MSG_RESULT(yes)
  fi
fi

dnl /**************************************************************************
dnl  *                            Substitutions                               *
dnl  **************************************************************************/


dnl Substitutions to do.
AC_SUBST(BITS)
AC_SUBST(DEFINE_HAS_ISL_LIB)
AC_SUBST(ac_aux_dir)
AC_SUBST(abs_top_srcdir)


dnl Configure Makefiles.
AC_CONFIG_FILES([
  Makefile
  doc/Makefile
  doc/Doxyfile
  include/candl/macros.h
  tests/Makefile
  ],
  [test -z "$CONFIG_HEADERS" || echo timestamp > source/stamp-h.in])

#if test $with_piplib = system; then
#  AC_CONFIG_SUBDIRS(piplib)
#fi
if test $with_osl = bundled; then
  AC_CONFIG_SUBDIRS(osl)
fi

dnl forcing candl to use local libcandl.la
dnl if --prefix is not specified
CANDL_LIBS="$srcdir/libcandl.la -lcandl"
AC_SUBST(CANDL_LIBS)



AC_OUTPUT

echo "             /*-----------------------------------------------*"
echo "              *          Candl configuration is OK            *"
echo "              *-----------------------------------------------*/"
echo "It appears that your system is OK to start Candl compilation. You need"
echo "now to type \"make\". Lastly type \"make install\" to install Candl on"
echo "your system (log as root if necessary)."

