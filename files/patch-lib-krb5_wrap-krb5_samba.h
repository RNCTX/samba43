--- lib/krb5_wrap/krb5_samba.h.orig	2015-07-21 09:47:48 UTC
+++ lib/krb5_wrap/krb5_samba.h
@@ -74,6 +74,17 @@
 #define CKSUMTYPE_HMAC_MD5 CKSUMTYPE_HMAC_MD5_ARCFOUR
 #endif
 
+/*
+ * CKSUMTYPE_HMAC_SHA1_96_AES_* in Heimdal
+ * CKSUMTYPE_HMAC_SHA1_96_AES* in MIT
+ */
+#if defined(CKSUMTYPE_HMAC_SHA1_96_AES128) && !defined(CKSUMTYPE_HMAC_SHA1_96_AES_128)
+#define CKSUMTYPE_HMAC_SHA1_96_AES_128 CKSUMTYPE_HMAC_SHA1_96_AES128
+#endif
+#if defined(CKSUMTYPE_HMAC_SHA1_96_AES256) && !defined(CKSUMTYPE_HMAC_SHA1_96_AES_256)
+#define CKSUMTYPE_HMAC_SHA1_96_AES_256 CKSUMTYPE_HMAC_SHA1_96_AES256
+#endif
+
 typedef struct {
 #if defined(HAVE_MAGIC_IN_KRB5_ADDRESS) && defined(HAVE_ADDRTYPE_IN_KRB5_ADDRESS) /* MIT */
 	krb5_address **addrs;
