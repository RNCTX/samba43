--- buildtools/wafsamba/samba_pidl.py.orig	2015-07-21 09:47:48 UTC
+++ buildtools/wafsamba/samba_pidl.py
@@ -88,7 +88,7 @@ def SAMBA_PIDL(bld, pname, source,
     # prime the list of nodes we are dependent on with the cached pidl sources
     t.allnodes = pidl_src_nodes
 
-    t.env.PIDL = os.path.join(bld.srcnode.abspath(), 'pidl/pidl')
+    t.env.PIDL = os.path.join('%%LOCALBASE%%', 'bin/pidl')
     t.env.OPTIONS = TO_LIST(options)
     t.env.OUTPUTDIR = bld.bldnode.name + '/' + bld.path.find_dir(output_dir).bldpath(t.env)
 
