[Import ()]
[CCode (cprefix = "RB", lower_case_cprefix = "rb_")]

namespace RB {
	[CCode (cheader_filename = "rb-shell.h")]
	public class Shell {
		[CCode (cname = "rb_shell_get_type")]
		public static GLib.Type get_type ();
	}

	[CCode (cheader_filename = "rb-plugin.h")]
	public abstract class Plugin : GLib.Object {
		[CCode (cname = "rb_plugin_get_type")]
		public static GLib.Type get_type ();

		[CCode (cname = "rb_plugin_activate")]
		public abstract void activate (RB.Shell shell);
		
    [CCode (cname = "rb_plugin_deactivate")]
		public abstract void deactivate (RB.Shell shell);

		[CCode (cname = "rb_plugin_is_configurable")]
		public virtual bool is_configurable ();
		
		[CCode (cname = "rb_plugin_create_configure_dialog")]
		public virtual Gtk.Widget create_configure_dialog ();
		
		[CCode (cname = "rb_plugin_find_file")]
		public virtual weak string find_file (string file);
	}

}
