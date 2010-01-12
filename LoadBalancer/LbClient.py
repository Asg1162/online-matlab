import sys, traceback, Ice
import Matcloud
status = 0
ic = None
try:
     ic = Ice.initialize(sys.argv)
     base = ic.stringToProxy("Backend:tcp -h os56.csc.ncsu.edu -p 10000")
     backend = Matcloud.BackendPrx.checkedCast(base)
     if not backend:
         raise RuntimeError("Invalid proxy")
     backend.delNode("os40.csc.ncsu.edu")
     backend.delNode("os40.csc.ncsu.edu")

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
