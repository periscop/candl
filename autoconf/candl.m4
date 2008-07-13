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
dnl Offer --with-clan.
  AC_ARG_WITH(clan,
	      AC_HELP_STRING([--with-clan=DIR],
              	             [DIR Location of CLAN package]),
              [with_clan=$withval;
	       CPPFLAGS="${CPPFLAGS} -I$withval/include";
	       LDFLAGS="${LDFLAGS} -L$withval/lib"
	      ],
              [with_clan=check])
dnl Check for clan existence.
  AS_IF([test "x$with_clan" != xno],
	[AC_CHECK_LIB([clan], [clan_scop_read],
	 [LIBS="-lclan $LIBS";
	 DEFINE_HAS_CLAN_LIB="# define CANDL_SUPPORTS_CLAN"
	 ],
         [DEFINE_HAS_CLAN_LIB=""
  	  if test "x$with_clan" != xcheck; then
           AC_MSG_FAILURE([Test for CLAN failed. Use --with-clan to specify libclan path.])
          fi
         ])
	])
])


