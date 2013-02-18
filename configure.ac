dnl Process this file with autoconf to produce a configure script
dnl
dnl Author: Niklas Karlsson <niklas.karlsson.phd@gmail.com>
dnl

AC_INIT([echod], [0.1.0])
AC_CONFIG_SRCDIR([src/main.c])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
dnl Do not forget to change library interface versions if used!

dnl Checks for a C compiler
AC_PROG_CC

dnl Disable building static libraries
AM_DISABLE_STATIC

dnl Setup install program
AM_PROG_INSTALL

dnl Setup libtool
AM_PROG_LIBTOOL

dnl Check for gmake and use it
AC_CHECK_PROG(GMAKE, gmake, gmake, make)
AC_SUBST(GMAKE)

dnl Turn on -Wall if compiling with gcc
#AC_COMPILE_WARNINGS

dnl Check for std C header files
dnl AC_CHECK_HEADERS([stdint.h])

AM_SANITY_CHECK

dnl
dnl Files to be generated 
dnl
AC_OUTPUT(Makefile \
		  src/Makefile
)
