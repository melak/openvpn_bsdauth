
PREFIX?=	/usr/local
BINDIR=		${PREFIX}/libexec
MANDIR=		${PREFIX}/man/cat

BINMODE=	2550
BINOWN=		_openvpn
BINGRP=		auth

PROG=		openvpn_bsdauth
SRCS=		${PROG}.c credentials.c
MAN=		${PROG}.8

CFLAGS+=	-Wall -Werror -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare -Wbounded -Wformat=2 -Wextra -W

beforeinstall:
	@perl -pi -e 's#!!PREFIX!!#${PREFIX}#g' openvpn_bsdauth.cat8

.include <bsd.prog.mk>
