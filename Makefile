
PREFIX?=	/usr/local
BINDIR=		${PREFIX}/libexec
MANDIR=		${PREFIX}/man/cat

BINMODE=	2550
BINOWN=		_openvpn
BINGRP=		auth

PROG=		openvpn_bsdauth
SRCS=		${PROG}.c credentials.c
MAN=		${PROG}.8

CFLAGS+=	-DHAVE_STRL

CFLAGS+=	-Wall -Werror
CFLAGS+=	-Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare -Wformat=2 -Wextra -W

.if defined(BUILD_DEBUG)
CFLAGS		+= -DDEBUG
.endif

.if ${CC} != "g++" && ${CC} != "eg++"
CFLAGS+=	-Wstrict-prototypes -Wmissing-prototypes
.endif

.if ${CC} == "cc" || ${CC} == "eg++"
CFLAGS+=	-Wmissing-declarations
.endif

.if ${CC} == "cc" || ${CC} == "g++"
CFLAGS+=	-Wbounded
.endif

beforeinstall:
	@perl -pi -e 's#!!PREFIX!!#${PREFIX}#g' openvpn_bsdauth.cat8

.include <bsd.prog.mk>
