plugindir = $(PLUGINDIR)/control
plugin_LTLIBRARIES = libcontrol.la
#noinst_LTLIBRARIES = libcontrol.la

libcontrol_la_SOURCES =                           \
	rb-control-plugin.vala                          \
  RBControlPluginServer.vala
libcontrol_la_LDFLAGS = $(PLUGIN_LIBTOOL_FLAGS)

# this causes automake to barf confusingly
# libsample_vala_la_LIBTOOLFLAGS = --tag=disable-static

libcontrol_la_CFLAGS = $(WNOERROR_CFLAGS)

INCLUDES = 						                              \
        -DGNOMELOCALEDIR=\""$(datadir)/locale"\"    \
	-DG_LOG_DOMAIN=\"Rhythmbox\"		 	                \
	-I$(top_srcdir) 				                          \
	-I$(top_srcdir)/lib                        	      \
	-I$(top_srcdir)/metadata                       	  \
	-I$(top_srcdir)/player                       	    \
	-I$(top_srcdir)/rhythmdb                       	  \
	-I$(top_srcdir)/widgets                    	      \
	-I$(top_srcdir)/sources                    	      \
	-I$(top_srcdir)/iradio                    	      \
	-I$(top_srcdir)/podcast                    	      \
	-I$(top_srcdir)/plugins				                    \
	-I$(top_srcdir)/shell				                      \
	-DPIXMAP_DIR=\""$(datadir)/pixmaps"\"		          \
	-DSHARE_DIR=\"$(pkgdatadir)\"                     \
	-DDATADIR=\""$(datadir)"\"			                  \
	$(RHYTHMBOX_CFLAGS)				                        \
	-D_XOPEN_SOURCE -D_BSD_SOURCE


plugin_in_files = control.rb-plugin.in


VALAFLAGS = --vapidir=$(top_srcdir)/bindings/vala --pkg=rhythmdb --pkg=rb --pkg=avahi-gobject

%.c %.h: %.vala
	$(VALAC) -C $(VALAFLAGS) $^
	
%.rb-plugin: %.rb-plugin.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

BUILT_SOURCES =                                     \
  $(plugin_in_files:.rb-plugin.in=.rb-plugin)       \
  $(NULL)

plugin_DATA =                                       \
  $(BUILT_SOURCES)                                  \
  $(NULL)

EXTRA_DIST = $(plugin_in_files)

CLEANFILES = $(plugin_DATA) $(libcontrol_la_SOURCES:.vala=.h) $(libcontrol_la_SOURCES:.vala=.c)
DISTCLEANFILES = $(CLEANFILES)
