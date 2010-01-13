import sys, threading, Ice, Matcloud

# BackendI servant class ...
class BackendI(Matcloud.Backend):
    # Constructor and operations here...
    _adapter = None
    def __init__(self, hosts):
        self._hosts = hosts

        myID = Ice.Identity()
        myID.name = "Backend"
        # Add the identity to the object adapter
        self._adapter.add(self, myID)
        # Create a proxy for the new node and
        # add it as a child to the parent
        #
        thisNode = Matcloud.BackendPrx.uncheckedCast(self._adapter.createProxy(myID))
                
    def addNode(self, newnode, current=None):
        print "add user"
        self._hosts.append(newnode)
    
    def delNode(self, obsnode, current=None):
        self._hosts.remove(obsnode)

# BackendI servant class ...
class QueryI(Matcloud.Query):
    # Constructor and operations here...
    _adapter = None
    def __init__(self, hosts):
        self._hosts = hosts

        myID = Ice.Identity();
        myID.name = "Query"
        # Add the identity to the object adapter
        #
        self._adapter.add(self, myID)
        # Create a proxy for the new node and
        # add it as a child to the parent
        #
        thisNode = Matcloud.BackendPrx.uncheckedCast(self._adapter.createProxy(myID))
                
    def queryNbNodes(self, current=None):
        return len(self._hosts)
    

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
        QueryI._adapter = adapter
        self._hosts = []
        # create the backend
        be = BackendI(self._hosts)
        qu = QueryI(self._hosts)
        
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
