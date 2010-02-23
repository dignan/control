using GLib;
using RB;
using RhythmDB;
using Avahi;

class RBControlPlugin: RB.Plugin {

    private const uint16 SERVICE_PORT = 9000;
    private const string SERVICE_TYPE = "_control._tcp";

    private EntryGroup group;
    private Client client;
    private StringBuilder name;

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
		
		stdout.printf ("starting avahi stuff\n");
		
        this.start_avahi();
		
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
	
	private void start_avahi(){
	    if (group == null){
	        group = new EntryGroup();
	    }
	    
	    client = new Client();
	    
	    group.attach(client);
	    group.state_changed.connect(entry_group_callback);
	}
	
	void entry_group_callback(EntryGroupState state) {
        switch (state) {
            case EntryGroupState.ESTABLISHED:
                break;
            case EntryGroupState.COLLISTION: {
                name.assign(Alternative.service_name(name.str));
                create_services(client);
                break;
            }
            case EntryGroupState.FAILURE:
                break;
            case EntryGroupState.UNCOMMITED:
                break;
            case EntryGroupState.REGISTERING:
                break;
        }
    }
    
    private void create_services(Client *c) {

        if (group == null){
            group = new EntryGroup();
        }

// Find a way to deal with this        if (avahi_entry_group_is_empty(group)) {
            try{
                group.add_service (name.str, SERVICE_TYPE, SERVICE_PORT);
            }catch (Avahi.Error e){
                if (e is Avahi.Error.COLLISION){
                    name.assign(Alternative.service_name(name.str));
                    group.reset();
                    create_services(c);
                }
            }
            
            group.commit();
//        }
    }
}


[ModuleInit]
public GLib.Type register_rb_plugin (GLib.TypeModule module)
{
	stdout.printf ("Registering plugin %s\n", "RBControlPlugin");

	return typeof (RBControlPlugin);
}
