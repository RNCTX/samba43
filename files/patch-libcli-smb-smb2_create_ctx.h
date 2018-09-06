--- libcli/smb/smb2_create_ctx.h.orig	2015-07-21 09:47:49 UTC
+++ libcli/smb/smb2_create_ctx.h
@@ -42,5 +42,6 @@
 /* "AAPL" Volume Capabilities bitmap */
 #define SMB2_CRTCTX_AAPL_SUPPORT_RESOLVE_ID 1
 #define SMB2_CRTCTX_AAPL_CASE_SENSITIVE     2
+#define SMB2_CRTCTX_AAPL_FULL_SYNC          4
 
 #endif
