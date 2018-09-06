--- source3/smbd/avahi_register.c.orig	2015-07-21 09:47:49 UTC
+++ source3/smbd/avahi_register.c
@@ -24,6 +24,8 @@
 #include <avahi-client/client.h>
 #include <avahi-client/publish.h>
 #include <avahi-common/error.h>
+#include <avahi-common/malloc.h>
+#include <avahi-common/strlst.h>
 
 struct avahi_state_struct {
 	struct AvahiPoll *poll;
@@ -32,6 +34,39 @@ struct avahi_state_struct {
 	uint16_t port;
 };
 
+static void *avahi_allocator_ctx = NULL;
+
+static void * avahi_allocator_malloc(size_t size)
+{
+	return talloc_size(avahi_allocator_ctx, size);
+}
+
+static void avahi_allocator_free(void *p)
+{
+	TALLOC_FREE(p);
+}
+
+static void * avahi_allocator_realloc(void *p, size_t size)
+{
+	return talloc_realloc_size(avahi_allocator_ctx, p, size);
+}
+
+static void * avahi_allocator_calloc(size_t count, size_t size)
+{
+	void *p = talloc_array_size(avahi_allocator_ctx, size, count);
+	if (p) {
+		memset(p, 0, size * count);
+	}
+	return p;
+}
+
+static const struct AvahiAllocator avahi_talloc_allocator = {
+	&avahi_allocator_malloc,
+	&avahi_allocator_free,
+	&avahi_allocator_realloc,
+	&avahi_allocator_calloc
+};
+
 static void avahi_entry_group_callback(AvahiEntryGroup *g,
 				       AvahiEntryGroupState status,
 				       void *userdata)
@@ -75,8 +110,14 @@ static void avahi_client_callback(AvahiC
 	int error;
 
 	switch (status) {
-	case AVAHI_CLIENT_S_RUNNING:
-		DEBUG(10, ("avahi_client_callback: AVAHI_CLIENT_S_RUNNING\n"));
+	case AVAHI_CLIENT_S_RUNNING: {
+		int snum;
+		int num_services = lp_numservices();
+		int dk = 0;
+		AvahiStringList *adisk = NULL;
+		AvahiStringList *adisk2 = NULL;
+
+                DEBUG(10, ("avahi_client_callback: AVAHI_CLIENT_S_RUNNING\n"));
 
 		state->entry_group = avahi_entry_group_new(
 			c, avahi_entry_group_callback, state);
@@ -97,6 +138,53 @@ static void avahi_client_callback(AvahiC
 			state->entry_group = NULL;
 			break;
 		}
+
+		for (snum = 0; snum < num_services; snum++) {
+			if (lp_snum_ok(snum) &&
+			    lp_parm_bool(snum, "fruit", "time machine", false))
+			{
+				adisk2 = avahi_string_list_add_printf(
+					    adisk, "dk%d=adVN=%s,adVF=0x82",
+					    dk++, lp_const_servicename(snum));
+				if (adisk2 == NULL) {
+					DBG_DEBUG("avahi_string_list_add_printf"
+						  "failed: returned NULL\n");
+					avahi_string_list_free(adisk);
+					avahi_entry_group_free(state->entry_group);
+					state->entry_group = NULL;
+					break;
+				}
+				adisk = adisk2;
+				adisk2 = NULL;
+			}
+		}
+		if (dk > 0) {
+			adisk2 = avahi_string_list_add(adisk, "sys=adVF=0x100");
+			if (adisk2 == NULL) {
+				DBG_DEBUG("avahi_string_list_add failed: "
+					  "returned NULL\n");
+				avahi_string_list_free(adisk);
+				avahi_entry_group_free(state->entry_group);
+				state->entry_group = NULL;
+				break;
+			}
+			adisk = adisk2;
+			adisk2 = NULL;
+ 			error = avahi_entry_group_add_service_strlst(
+				    state->entry_group, AVAHI_IF_UNSPEC,
+				    AVAHI_PROTO_UNSPEC, 0, lp_netbios_name(),
+				    "_adisk._tcp", NULL, NULL, 0, adisk);
+			avahi_string_list_free(adisk);
+			adisk = NULL;
+			if (error != AVAHI_OK) {
+				DBG_DEBUG("avahi_entry_group_add_service_strlst "
+					  "failed: %s\n", avahi_strerror(error));
+				avahi_entry_group_free(state->entry_group);
+				state->entry_group = NULL;
+				break;
+			}
+		}
+
 		if (avahi_entry_group_commit(state->entry_group) < 0) {
 			error = avahi_client_errno(c);
 			DEBUG(10, ("avahi_entry_group_commit failed: "
@@ -106,6 +194,7 @@ static void avahi_client_callback(AvahiC
 			break;
 		}
 		break;
+	}
 	case AVAHI_CLIENT_FAILURE:
 		error = avahi_client_errno(c);
 
@@ -146,6 +235,18 @@ void *avahi_start_register(TALLOC_CTX *m
 	struct avahi_state_struct *state;
 	int error;
 
+	avahi_allocator_ctx = talloc_new(mem_ctx);
+	if (avahi_allocator_ctx == NULL) {
+		return NULL;
+	}
+	avahi_set_allocator(&avahi_talloc_allocator);
+
+	avahi_allocator_ctx = talloc_new(mem_ctx);
+	if (avahi_allocator_ctx == NULL) {
+		return NULL;
+	}
+	avahi_set_allocator(&avahi_talloc_allocator);
+
 	state = talloc(mem_ctx, struct avahi_state_struct);
 	if (state == NULL) {
 		return state;
