/*
 * Copyright (c) 2007, 2009-2013 Tamas Tevesz <ice@extreme.hu>
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
#include <errno.h>
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

#define OPENVPN_AUTH_SUCC		0
#define OPENVPN_AUTH_FAIL		1
#define OPENVPN_USERPASS_LEN		128

extern	char	*__progname;
static	int	 is_ingroup( const char * );
static	int	 check_bsdauth( const char *username, const char *password );

int main(int argc, char **argv) {

	char username[ OPENVPN_USERPASS_LEN ], password[ OPENVPN_USERPASS_LEN ];
	int res;

	res = OPENVPN_AUTH_FAIL;

	openlog( __progname, LOG_PID|LOG_NDELAY, LOG_AUTH );

	if( getcreds( argc, argv, username, password, OPENVPN_USERPASS_LEN ) < 0 ) {
#ifdef DEBUG
		syslog( LOG_ERR, "Unable to get supplied credentials" );
#endif
		goto out;
	}

	if( check_bsdauth( username, password ) == 1 ) {
		syslog( LOG_INFO, "Accepted password for %s from %s",
			username, peer_address() ? peer_address() : "UNKNOWN" );
		res = OPENVPN_AUTH_SUCC;
	} else {
		syslog( LOG_INFO, "Bad password for %s from %s,",
			username, peer_address() ? peer_address() : "UNKNOWN" );
	}

out:
	memset( username, 0, OPENVPN_USERPASS_LEN );
	memset( password, 0, OPENVPN_USERPASS_LEN );

	closelog();
	return res;
}

static int check_bsdauth( const char *username, const char *password )
{
	int res;
	char *u, *p;

	res = 0;

	u = strdup( username );
	p = strdup( password );

	if( u == NULL ||
	    p == NULL ) {
		syslog( LOG_ERR, "Error: strdup: %s", strerror( errno ) );
		goto out;
	}

	if( !is_ingroup( username ) ) {
		goto out;
	}

	if( auth_userokay( u, NULL, NULL, p ) != 0 )  {
		res = 1;
	}

#ifdef DEBUG
	syslog( LOG_INFO, "BSD Auth %sed %s", res == 1 ? "accept" : "reject", username );
#endif

out:
	if( u ) {
		memset( u, 0, strlen( u ) );
		free( u );
	}
	if( p ) {
		memset( p, 0, strlen( p ) );
		free( p );
	}

	return res;
}

/* butchered from src/usr.bin/id/id.c */
static int is_ingroup( const char *username ) {

	gid_t groups[ NGROUPS + 1 ], vpngroup_gid;
	struct group *gr;
	struct passwd *pw;
	int ngroups, i;
	int res;

	res = 0;

	gr = getgrnam( VPN_GROUP );
	if( gr == NULL ) {
#ifdef DEBUG
		syslog( LOG_ERR, "Can not find group " VPN_GROUP 
				 " in the system group database" );
#endif
		goto out;
	}

	vpngroup_gid = gr->gr_gid;

	pw = getpwnam( username );
	if( pw == NULL ) {
#ifdef DEBUG
		syslog( LOG_ERR, "Bad user %s", username );
#endif
		goto out;
	}

	ngroups = NGROUPS + 1;
	if( getgrouplist( pw->pw_name, pw->pw_gid, groups, &ngroups ) == -1 ) {
#ifdef DEBUG
		syslog( LOG_ERR, "System group database is corrupt "
				 "(user is in more than %d groups)", NGROUPS );
#endif
		goto out;
	}

	for (i = 0; i < ngroups; i++) {
		if( vpngroup_gid == groups[i] ) {
			res = 1;
			goto out;
		}
	}

out:
	if( res == 0 ) {
#ifdef DEBUG
		syslog( LOG_ERR, "User %s is not a member of " VPN_GROUP , username );
#endif
	}

	return res;
}

