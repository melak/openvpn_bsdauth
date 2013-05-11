/*
 * Copyright (c) 2007, 2009-2010 Tamas Tevesz <ice@extreme.hu>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/types.h>

#include <bsd_auth.h>
#include <err.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "credentials.h"

#ifndef	VPN_GROUP
#define	VPN_GROUP			"_openvpnusers"
#endif

#define	OPENVPN_AUTH_OK			0
#define	OPENVPN_AUTH_FAIL		1
#define	OPENVPN_USER_PASS_MAXLEN	128

extern	char	*__progname;
static	int	 is_ingroup(char *);
static	void	 logx(int, int, const char *, ...)
		    __attribute__((__noreturn__, format(printf, 3, 4)));
static	char	 username[OPENVPN_USER_PASS_MAXLEN], password[OPENVPN_USER_PASS_MAXLEN];

int main(int argc, char **argv) {

	openlog(__progname, LOG_PID|LOG_NDELAY, LOG_AUTH);

	if( getcreds( argc, argv, username, password, OPENVPN_USER_PASS_MAXLEN ) < 0 ) {
		logx(OPENVPN_AUTH_FAIL, LOG_ERR,
		    "Unable to get credentials from OpenVPN");
	}

	if (!is_ingroup(username)) {
		logx(OPENVPN_AUTH_FAIL, LOG_ERR,
		    "User %s is not a member of " VPN_GROUP , username);
	}

	if(auth_userokay(username, NULL, NULL, password) != 0)  {
		logx(OPENVPN_AUTH_OK, LOG_INFO,
		    "Accepted password for %s from %s", username,
		    peer_address() ? peer_address() : "UNKNOWN");
	}

	logx(OPENVPN_AUTH_FAIL, LOG_ERR, "Bad password for %s from %s", username,
	    peer_address() ? peer_address() : "UNKNOWN");

	/* NOTREACHED */
	return OPENVPN_AUTH_FAIL;
}

/* butchered from src/usr.bin/id/id.c */
static int is_ingroup(char *user) {

	gid_t groups[NGROUPS+1], vpngroup_gid;
	struct group *gr;
	struct passwd *pw;
	int ngroups, i;

	gr = getgrnam(VPN_GROUP);
	if (gr == NULL)
		logx(OPENVPN_AUTH_FAIL, LOG_ERR,
		    "Can not find group %s in the system group database", VPN_GROUP);

	vpngroup_gid = gr->gr_gid;

	pw = getpwnam(user);
	if (pw == NULL)
		logx(OPENVPN_AUTH_FAIL, LOG_ERR, "Bad user %s", user);

	ngroups = NGROUPS + 1;
	if (getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups) == -1)
		logx(OPENVPN_AUTH_FAIL, LOG_ERR,
		    "System group database is corrupt "
		    "(user is in more than %d groups)", NGROUPS);

	for (i = 0; i < ngroups; i++)
		if (vpngroup_gid == groups[i])
			return 1;

	return 0;
}

static void logx(int exitval, int prio, const char *fmt, ...) {

	va_list va;

	va_start(va, fmt);
	vsyslog(prio, fmt, va);
	closelog();
	va_end(va);

	memset(username, 0, OPENVPN_USER_PASS_MAXLEN);
	memset(password, 0, OPENVPN_USER_PASS_MAXLEN);

	exit(exitval);
}
