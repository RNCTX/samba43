--- source3/modules/vfs_fruit.c.orig	2016-06-14 07:37:36 UTC
+++ source3/modules/vfs_fruit.c
@@ -133,6 +133,7 @@ struct fruit_config_data {
 	bool copyfile_enabled;
 	bool veto_appledouble;
 	bool posix_rename;
+        bool time_machine;
 
 	/*
 	 * Additional options, all enabled by default,
@@ -1350,6 +1351,9 @@ static int init_fruit_config(vfs_handle_
 	config->use_aapl = lp_parm_bool(
 		-1, FRUIT_PARAM_TYPE_NAME, "aapl", true);
 
+	config->time_machine = lp_parm_bool(
+		SNUM(handle->conn), FRUIT_PARAM_TYPE_NAME, "time machine", false);
+
 	config->unix_info_enabled = lp_parm_bool(
 		-1, FRUIT_PARAM_TYPE_NAME, "nfs_aces", true);
 
@@ -1825,6 +1829,7 @@ static NTSTATUS check_aapl(vfs_handle_st
 	DATA_BLOB blob = data_blob_talloc(req, NULL, 0);
 	uint64_t req_bitmap, client_caps;
 	uint64_t server_caps = SMB2_CRTCTX_AAPL_UNIX_BASED;
+	uint64_t volume_caps;
 	smb_ucs2_t *model;
 	size_t modellen;
 
@@ -1896,7 +1901,8 @@ static NTSTATUS check_aapl(vfs_handle_st
 	if (req_bitmap & SMB2_CRTCTX_AAPL_VOLUME_CAPS) {
 		SBVAL(p, 0,
 		      lp_case_sensitive(SNUM(handle->conn->tcon->compat)) ?
-		      SMB2_CRTCTX_AAPL_CASE_SENSITIVE : 0);
+		      volume_caps |= SMB2_CRTCTX_AAPL_CASE_SENSITIVE : 0);
+		SBVAL(p, 0, volume_caps |= SMB2_CRTCTX_AAPL_FULL_SYNC);
 		ok = data_blob_append(req, &blob, p, 8);
 		if (!ok) {
 			return NT_STATUS_UNSUCCESSFUL;
@@ -2111,6 +2117,19 @@ static int fruit_connect(vfs_handle_stru
 			"0x0d:0xf00d");
 	}
 
+	if (config->time_machine) {
+		DBG_NOTICE("Enabling durable handles for Time Machine "
+			   "support on [%s]\n", service);
+		lp_do_parameter(SNUM(handle->conn), "durable handles", "yes");
+		lp_do_parameter(SNUM(handle->conn), "kernel oplocks", "no");
+		lp_do_parameter(SNUM(handle->conn), "kernel share modes", "no");
+		if (!lp_strict_sync(SNUM(handle->conn))) {
+			DBG_WARNING("Time Machine without strict sync is not "
+				    "recommended!\n");
+		}
+		lp_do_parameter(SNUM(handle->conn), "posix locking", "no");
+	}
+
 	return rc;
 }
 
