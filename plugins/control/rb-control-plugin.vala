using GLib;
using RB;
using RhythmDB;

class RBControlPlugin: RB.Plugin {
	public override void activate (RB.Shell shell) {
		if (!Thread.supported()){
		  stderr.printf ("Threads are not support\n");
		}
		
		unowned Thread listenThread; 
		
		try {
		  listenThread = Thread.create(listen, true);
		}catch (ThreadError e){
		  stderr.printf ("Error: %s\n", e.message);
		}
		
		stdout.printf ("put avahi stuff here\n");
	}

	public override void deactivate (RB.Shell shell) {
		stdout.printf ("Goodbye world\n");
	}
	
	private static void *listen(){
	  stdout.printf ("threading!\n");
	  
	  RBControlPluginServer server = new RBControlPluginServer();
	  
	  try{
	    server.listen();
	  }catch (GLib.Error e){
	    stderr.printf("Error: %s\n", e.message);
	  } 
	    
	  return null;
	}
}


[ModuleInit]
public GLib.Type register_rb_plugin (GLib.TypeModule module)
{
	stdout.printf ("Registering plugin %s\n", "RBControlPlugin");

	return typeof (RBControlPlugin);
}
