#!/usr/bin/env python
import sys
from omniORB import CORBA
import CosNaming
import vq

orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)

obj = orb.resolve_initial_references("NameService")
rootCtx = obj._narrow(CosNaming.NamingContext)

if rootCtx is None:
	print "Failed to narrow the root naming context"
	sys.exit(1)

name = [ CosNaming.NameComponent("VQ.ivq", "") ]

try:
	obj = rootCtx.resolve(name)

except CosNaming.NamingContext.NotFound, ex:
	print "Name not found"
	sys.exit(1)

vqo = obj._narrow(vq.ivq)

if vqo is None:
	print "Object reference is not an vq::ivq"
	sys.exit(1)

dom = "123123.asd.com.pl"
err = vqo.dom_val(dom)

if err.ec is vq.ivq.err_no:
	print "It's a valid domain name"
else:
	print "It's an invalid domain name"
