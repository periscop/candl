AC_DEFUN([CANDL_ARG_LIBS_DEPENDENCIES],
[
dnl Add $prefix to the library path (convenience).
  if test -e ${prefix}/include; then
    CPPFLAGS="${CPPFLAGS} -I${prefix}/include"
  fi;
  if test -e ${prefix}/lib; then
    LDFLAGS="${LDFLAGS} -L${prefix}/lib"
  fi;
dnl Offer --with-piplib.
  AC_ARG_WITH(piplib,
	      AC_HELP_STRING([--with-piplib=DIR],
              	             [DIR Location of PIPLib package]),
              [with_piplib=$withval;
	       CPPFLAGS="${CPPFLAGS} -I$withval/include";
	       LDFLAGS="${LDFLAGS} -L$withval/lib"
	      ],
              [with_piplib=yes])
dnl Check for piplib existence.
  AS_IF([test "x$with_piplib" != xno],
	[AC_CHECK_LIB([piplib$BITS], [pip_solve],
	 [LIBS="-lpiplib$BITS $LIBS";
	 AC_DEFINE([HAVE_LIBPIPLIB], [1], [Define if you have libpiplib$BITS])
         ],
         [if test "x$with_piplib" != xcheck; then
           AC_MSG_FAILURE([--with-piplib was given, but test for piplib failed])
          fi
         ])
	])
dnl Offer --with-scoplib.
  AC_ARG_WITH(scoplib,
	      AC_HELP_STRING([--with-scoplib=DIR],
              	             [DIR Location of ScopLib package]),
              [with_scoplib=$withval;
	       CPPFLAGS="${CPPFLAGS} -I$withval/include";
	       LDFLAGS="${LDFLAGS} -L$withval/lib"
	      ],
              [with_scoplib=check])
dnl Check for scoplib existence.
  AS_IF([test "x$with_scoplib" != xno],
	[AC_CHECK_LIB([scoplib], [scoplib_scop_read],
	 [LIBS="-lscoplib $LIBS";
	 DEFINE_HAS_SCOPLIB_LIB="# define CANDL_SUPPORTS_SCOPLIB"
	 ],
         [DEFINE_HAS_SCOPLIB_LIB=""
  	  if test "x$with_scoplib" != xcheck; then
           AC_MSG_FAILURE([Test for ScopLib failed. Use --with-scoplib to specify libscoplib path.])
          fi
         ])
	])
dnl Offer --with-gmp-prefix.
  AC_ARG_WITH(gmp-prefix,
	      AC_HELP_STRING([--with-gmp-prefix=DIR],
              	             [DIR Location of GMP package (only headers are needed)]),
              [CPPFLAGS="${CPPFLAGS} -I$withval/include";
               LDFLAGS="${LDFLAGS} -L$withval/lib";
	      ])
dnl Offer --with-isl.
  AC_ARG_WITH(isl,
	      AC_HELP_STRING([--with-isl=DIR],
              	             [DIR Location of Isl package]),
              [with_isl=$withval;
	       CPPFLAGS="${CPPFLAGS} -I$withval/include";
	       LDFLAGS="${LDFLAGS} -L$withval/lib"
	      ],
              [with_isl=check])
dnl Check for isl existence.
  AS_IF([test "x$with_isl" != xno],
	[AC_CHECK_LIB([isl], [isl_version],
	 [LIBS="-lisl $LIBS";
	 DEFINE_HAS_ISL_LIB="# define CANDL_SUPPORTS_ISL"
	 ],
         [DEFINE_HAS_ISL_LIB=""
  	  if test "x$with_isl" != xcheck; then
           AC_MSG_FAILURE([Test for Isl failed. Use --with-isl to specify libisl path.])
          fi
         ])
	])
])


