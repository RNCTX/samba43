### Samba 4.3 backport for FreeBSD

Remember Samba with pam_smbpass.so?  When you didn't have to set up a domain to have simple auth on your home LAN?  I do.
So I backported some things from Samba 4.4+ that aren't in 4.3 since it was deprecated.  To address 4.3 security issues that
were not patched after its deprecation, you must do the following:

* Replace Kerberos with MIT kerberos (nixes the Heimdal security exploit)
* Turn off nt pipe support (nixes the NT pipe exploit)
* Manually install p5-Parse-Pidl44 ([link](https://svnweb.freebsd.org/ports/head/devel/p5-Parse-Pidl44/?pathrev=457765))

To do those:

```
cd /usr/ports/net
(sudo) git clone https://github.com/RNCTX/samba43
(sudo) pkg install krb5
(sudo) mv /usr/bin/krb5-config /usr/bin/krb5-config.old
cd ../devel
(sudo) mkdir p5-Parse-Pidl44 && cd p5-Parse-Pidl44
```

Get the files from the above linked SVN into that folder you just made, and build that port.  You'll have to remove any
existing Samba installations before building and installing it.

Then:

```
cd ../../net/samba43
(sudo) make install
(sudo) pkg lock samba43
```

This will take awhile but when you're done, you have a Samba install with smb_passwd.so, with Time Machine support (install
and enable avahi for this), and with as many security patches/avoidances as you can possibly have as of this writing.

See [this post](https://forums.freebsd.org/threads/howto-samba-with-pam_smbpass-using-your-system-account-with-samba.49581/) for a 
guide on how to set up automatic Samba password maintenance with pam_smbpass.so

You can use your other Samba4 config(s), but disable NT pipe support to get around one of the unpached security exploits as mentioned above:

```
[global]
nt pipe support = no
```

Should you use this exposed to the internet or on a public network?  Nope.  Can you use it on a small firewalled LAN? Up to you. Enjoy

Tested on FreeBSD 11.2-RELEASE and 11.1-RELEASE
