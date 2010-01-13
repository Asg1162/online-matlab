import sys, traceback, Ice
import Matcloud
status = 0
ic = None
try:
     ic = Ice.initialize(sys.argv)
     bebase = ic.stringToProxy("Backend:tcp -h os56.csc.ncsu.edu -p 10000")
     backend = Matcloud.BackendPrx.checkedCast(bebase)
     if not backend:
         raise RuntimeError("Invalid proxy")
     backend.addNode("os40.csc.ncsu.edu")

     qebase = ic.stringToProxy("Query:tcp -h os56.csc.ncsu.edu -p 10000")
     query = Matcloud.QueryPrx.checkedCast(qebase)
     nbNode = query.queryNbNodes()

     print "there are %d node" % nbNode

except:
     traceback.print_exc()
     status = 1
if ic:
    # Clean up
    try:
        ic.destroy()
    except:
        traceback.print_exc()
        status = 1
sys.exit(status)
