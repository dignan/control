/*
 * heavily based on code from Gedit
 *
 * Copyright (C) 2002-2005 - Paolo Maggi 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */

#ifndef __RB_PLUGIN_H__
#define __RB_PLUGIN_H__

#include <glib-object.h>

#include "rb-shell.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define RB_TYPE_PLUGIN              (rb_plugin_get_type())
#define RB_PLUGIN(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), RB_TYPE_PLUGIN, RBPlugin))
#define RB_PLUGIN_CONST(obj)        (G_TYPE_CHECK_INSTANCE_CAST((obj), RB_TYPE_PLUGIN, RBPlugin const))
#define RB_PLUGIN_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), RB_TYPE_PLUGIN, RBPluginClass))
#define RB_IS_PLUGIN(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), RB_TYPE_PLUGIN))
#define RB_IS_PLUGIN_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), RB_TYPE_PLUGIN))
#define RB_PLUGIN_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), RB_TYPE_PLUGIN, RBPluginClass))

/*
 * Main object structure
 */
typedef struct
{
	GObject parent;
} RBPlugin;

/*
 * Class definition
 */
typedef struct
{
	GObjectClass parent_class;

	/* Virtual public methods */
	
	void 		(*activate)		(RBPlugin *plugin,
						 RBShell *shell);
	void 		(*deactivate)		(RBPlugin *plugin,
						 RBShell *shell);

	GtkWidget*	(*create_configure_dialog)
						(RBPlugin *plugin);

	/* Plugins should not override this, it's handled automatically by
	   the RbPluginClass */
	gboolean 	(*is_configurable)
						(RBPlugin *plugin);
} RBPluginClass;

/*
 * Public methods
 */
GType 		 rb_plugin_get_type 		(void) G_GNUC_CONST;

void 		 rb_plugin_activate		(RBPlugin *plugin,
						 RBShell *shell);
void 		 rb_plugin_deactivate		(RBPlugin *plugin,
						 RBShell *shell);
				 
gboolean	 rb_plugin_is_configurable	(RBPlugin *plugin);
GtkWidget	*rb_plugin_create_configure_dialog		
						(RBPlugin *plugin);

/*
 * Utility macro used to register plugins
 *
 * use: RBT_PLUGIN_REGISTER(RBSamplePlugin, rb_sample_plugin)
 */

#define RB_PLUGIN_REGISTER(PluginName, plugin_name)				\
										\
static GType plugin_name##_type = 0;						\
										\
GType										\
plugin_name##_get_type (void)							\
{										\
	return plugin_name##_type;						\
}										\
										\
static void     plugin_name##_init              (PluginName        *self);	\
static void     plugin_name##_class_init        (PluginName##Class *klass);	\
static gpointer plugin_name##_parent_class = NULL;				\
static void     plugin_name##_class_intern_init (gpointer klass)		\
{										\
	plugin_name##_parent_class = g_type_class_peek_parent (klass);		\
	plugin_name##_class_init ((PluginName##Class *) klass);			\
}										\
										\
G_MODULE_EXPORT GType								\
register_rb_plugin (GTypeModule *module)					\
{										\
	static const GTypeInfo our_info =					\
	{									\
		sizeof (PluginName##Class),					\
		NULL, /* base_init */						\
		NULL, /* base_finalize */					\
		(GClassInitFunc) plugin_name##_class_intern_init,		\
		NULL,								\
		NULL, /* class_data */						\
		sizeof (PluginName),						\
		0, /* n_preallocs */						\
		(GInstanceInitFunc) plugin_name##_init				\
	};									\
										\
	rb_debug ("Registering plugin %s", #PluginName);			\
										\
	/* Initialise the i18n stuff */						\
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);			\
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");			\
										\
	plugin_name##_type = g_type_module_register_type (module,		\
					    RB_TYPE_PLUGIN,			\
					    #PluginName,			\
					    &our_info,				\
					    0);					\
	return plugin_name##_type;						\
}


G_END_DECLS

#endif  /* __GEDIT_PLUGIN_H__ */

