import sys, threading, Ice, Matcloud

# DirectoryI servant class ...
class BackendI(Matcloud.Backend):
    # Constructor and operations here...
    _adapter = None
#    def __init__(self, name, parent):
    def __init__(self, hosts):
        self._hosts = hosts;
#        self._name = name
#        self._parent = parent
#        self._contents = []
        # Create an identity. The
        # parent has the fixed identity "RootDir"
        #
        myID = Ice.Identity();
        myID.name = "Backend"
        # Add the identity to the object adapter
        #
        self._adapter.add(self, myID)
        # Create a proxy for the new node and
        # add it as a child to the parent
        #
        thisNode = Matcloud.BackendPrx.uncheckedCast(self._adapter.createProxy(myID))
                
    def addNode(self, newnode, current=None):
        print "add user\n"
        self._hosts.append(newnode)
    
    def delNode(self, obsnode, current=None):
        self._hosts.remove(obsnode)


class Server(Ice.Application):
    def run(self, args):
        # Terminate cleanly on receipt of a signal
        #
        self.shutdownOnInterrupt()
        # Create an object adapter (stored in the _adapter
        # static members)
        #
        adapter = self.communicator().\
                     createObjectAdapterWithEndpoints(
                         "LoadBalancer", "tcp -p 10000")
        BackendI._adapter = adapter
        self._hosts = []
        # create the backend
        be = BackendI(self._hosts)
        
        # Create the root directory (with name "/" and no parent)
        #
        #
        adapter.activate()
        # Wait until we are done
        #
        self.communicator().waitForShutdown()
        if self.interrupted():
             print self.appName() + ": terminating"
        return 0

app = Server()
sys.exit(app.main(sys.argv))
