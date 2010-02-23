using GLib;
using RB;
using RhythmDB;

class RBControlPluginServer {

  private const uint16 SERVER_PORT = 9000;

  /* Listens for incoming control sessions */
  public void listen() throws Error {
    SocketAddress serv_sock;
	  
	  SocketService server = new SocketService();
	  InetAddress inetAddr = new InetAddress.any(SocketFamily.IPV6);
	  SocketAddress sockAddr = new InetSocketAddress(inetAddr, SERVER_PORT);
	  server.add_address(sockAddr, SocketType.STREAM, SocketProtocol.TCP, server, out serv_sock);
	  server.incoming.connect(handler);
	  server.start();
  }
  
  private bool handler(SocketConnection connection, Object source_object) {
    return true;
  }
}
