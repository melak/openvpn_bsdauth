# openvpn_bsdauth

A small helper application that makes it possible to authenticate
[OpenVPN](http://openvpn.net/) against the [BSD Authentication System](http://en.wikipedia.org/wiki/RADIUS).

BSD Auth is an authentication system originally of
[BSD/OS](http://en.wikipedia.org/wiki/BSD/OS) origins, similar
[PAM](http://en.wikipedia.org/wiki/Pluggable_Authentication_Modules)
mechanism, these days only to be found in [OpenBSD](http://www.openbsd.org/).

Essentially, it makes it possible for OpenVPN to authenticate users
against the OpenBSD system password database.

It is of no use to users of other systems.

## &#x1f435; Documentation

The online version of the manual page is [available](http://www.wormhole.hu/~ice/openvpn_bsdauth/openvpn_bsdauth.8.html).

The origin home page is [here](http://www.wormhole.hu/~ice/openvpn_bsdauth/).

## Downloads

Since OpenBSD 4.3, openvpn_bsdauth is available as a [package](http://www.openbsd.org/faq/faq15.html).
You are advised **not** to download and compile this source distribution,
but [install](http://www.openbsd.org/cgi-bin/man.cgi?query=pkg_add) the
appropriate package for your system. This source is not to make your life easy, but to make
[port maintainer](http://www.hazardous.org/~fkr/)'s life easy.

