#!/usr/bin/python

import sys, threading, Ice, Matcloud, traceback

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
        print "add node %s" % newnode
        self._hosts[newnode] = 0
    
    def delNode(self, obsnode, current=None):
        # TODO can't remove brutally need to check if it's idle
        del self._hosts.remove[obsnode]

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
    
# BackendI servant class ...
class FrontendI(Matcloud.Frontend):
    # Constructor and operations here...
	_adapter = None
	def __init__(self, host):
		self._hosts = host
		#self._client = {}
		self._mutex = threading.Lock()
		self.unmap = {}

		myID = Ice.Identity();
		myID.name = "Frontend"
		# Add the identity to the object adapter
		#
		self._adapter.add(self, myID)
		# Create a proxy for the new node and
		# add it as a child to the parent
		#
		thisNode = Matcloud.FrontendPrx.uncheckedCast(self._adapter.createProxy(myID))
                
	def FrontendAgent(self, cmd, current=None):
		print "received command %s" % cmd
		if len(self._hosts) <= 0:
			return "No available server"

		if cmd[0: cmd.find(":")] == "LOGIN":
			return self.FrontendLogin(cmd)
		elif cmd[0: cmd.find(":")] == "LOGOUT":
			return self.FrontendLogout(cmd)
		else:
			return self.FrontendCalc(cmd)
	
	def FrontendCalc(self, cmd, current=None):
		rtn = ""
		try:
			username = cmd.split(':')[1]
			try:
				server = self.unmap[username]
			except KeyError:
				return "Please login"

			ic = Ice.initialize(sys.argv)
			bebase = ic.stringToProxy("Parser:tcp -h "+server[0: server.find(":")]+".csc.ncsu.edu -p 10001")
			parser = Matcloud.ParserPrx.checkedCast(bebase)
			if not parser:
				raise RuntimeError("Invalid proxy")
			rtn = parser.command(cmd)
			print "received return string %s " % rtn
		except:
			traceback.print_exc()
			status = 1
			if ic:
				try:
					ic.destroy()
				except:
					traceback.print_exc()
			return "Internal Error"

		return rtn

	def FrontendLogin(self, cmd, current=None):
		username = ""
		minserver = ""
		rtn = ""
		try:
			username = cmd[cmd.find(":")+1 : len(cmd)]
			load = 0
			minload = -1
			self._mutex.acquire()
			for k, v in self._hosts.iteritems():
				load = v
				if minload == -1 or load < minload:
					minload = load
					minserver = k

			self.unmap[username] = minserver
			self._hosts[minserver] = minload + 1;
			self._mutex.release()

			print self.unmap[username]

			ic = Ice.initialize(sys.argv)
			bebase = ic.stringToProxy("Parser:tcp -h "+minserver[0: minserver.find(":")]+".csc.ncsu.edu -p 10001")
			parser = Matcloud.ParserPrx.checkedCast(bebase)
			if not parser:
				raise RuntimeError("Invalid proxy")
			rtn = parser.command(cmd)
			print "Login succeeded"
		except:
			traceback.print_exc()
			status = 1
			if ic:
				try:
					ic.destroy()
				except:
					traceback.print_exc()
			return "Internal Error"

		return rtn

	def FrontendLogout(self, cmd, current=None):
		server = ""
		rtn = ""
		try:
			username = cmd[cmd.find(":")+1 : len(cmd)]
			try:
				server = self.unmap[username]
				del self.unmap[username]
				self._mutex.acquire()
				if self._hosts[server] > 0:
					self._hosts[server] -= 1
				else:
					self._hosts[server] = 0
				self._mutex.release()
			except KeyError:
				print "KeyError found when LOGOUT"

			ic = Ice.initialize(sys.argv)
			bebase = ic.stringToProxy("Parser:tcp -h "+server[0: server.find(":")]+".csc.ncsu.edu -p 10001")
			parser = Matcloud.ParserPrx.checkedCast(bebase)
			if not parser:
				raise RuntimeError("Invalid proxy")
			rtn = parser.command(cmd)
			print "received return string %s " % rtn
		except:
			traceback.print_exc()
			status = 1
			if ic:
				try:
					ic.destroy()
				except:
					traceback.print_exc()
			return "Internal Error"

		return rtn


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
        FrontendI._adapter = adapter
        self._hosts = {}
        # create the backend
        be = BackendI(self._hosts)
        qu = QueryI(self._hosts)
        fe = FrontendI(self._hosts)
        
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
