#!/usr/bin/env python

##-----------------------------------------------------------------------
##
## HLA Tests Suite
##
## Copyright (c) 2006-2008 Eric NOULARD, Jean-Yves ROUSSELOT 
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
##-----------------------------------------------------------------------

import logging
import os
import time
import threading
import getopt, sys
import dtest  

def usage():
    print "Usage:\n %s [--help] [--certi_home=<path>] --rtig=[[<user>@]<host>]:<rtig_path> --federate=[[<user>@]<host>]:<federate_path>" % sys.argv[0]

try:
    opts, args = getopt.getopt(sys.argv[1:], "hr:f:c:", ["help","rtig=", "federate=","certi_home="])
except getopt.GetoptError, err:
    print >> sys.stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
    usage()
    sys.exit(2)

## default values
certi_home_defined=False
rtig_param = dtest.Utils.getUserHostPath("rtig")
federate_param = dtest.Utils.getUserHostPath("test_TRTCCallbacks")
federate_param['fom']="test_TRTCCallbacks.fed"
    
for o, a in opts:
    if o in ("--help"):
            usage()
            sys.exit(2)
    if o in ("-r", "--rtig"):
        rtig_param   = dtest.Utils.getUserHostPath(a)
    if o in ("-f", "--federate"):
        federate_param = dtest.Utils.getUserHostPath(a)
    if o in ("-c", "--certi_home"):
        certi_home = a
        certi_home_defined=True
        
if not certi_home_defined:
    if os.environ.has_key("CERTI_HOME"):
        certi_home=os.environ["CERTI_HOME"]
    else: 
        print "You must define CERTI_HOME environment variable"
        sys.exit(2)

rtig = dtest.DTester("RTIG",
                     session=dtest.SSHSessionHandler(rtig_param['user'],host=rtig_param['host']))

firstFederate = dtest.DTester("test_TRTCCallbacks_First",
                               session=dtest.SSHSessionHandler(federate_param['user'],host=federate_param['host']))

# you may change the default time out value
rtig.timeout = 40
# you add want to save the output of your dtester to a file.
rtig.stdout    = file(rtig.name + ".out",'w+')
rtig.stdin     = file(rtig.name + ".in",'w+')
rtig.stderr    = file(rtig.name + ".err",'w+')

# describe RTIG run steps
rtig.addRunStep("ok",True,"HLA test test_TRTCCallbacks Starts.")
dtest.ReusableSequences.addConditionalRunShellScript(rtig,c_shell_cmd="source "+certi_home+"/share/scripts/myCERTI_env.csh "+rtig_param['host'],
                               bourne_shell_cmd="source "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
rtig.addRunStep("runCommand",command=rtig_param['path'])
rtig.addRunStep("expectFromCommand",pattern="CERTI RTIG up and running",timeout=5)
rtig.addRunStep("barrier","RTIG started")
rtig.addRunStep("barrier","All Federate(s) ended")
rtig.addRunStep("terminateCommand")
rtig.addRunStep("waitCommandTermination")
rtig.addRunStep("ok",True,"HLA test test_TRTCCallbacks Ends.")

#dtest.DTester.logger.setLevel(level=logging.DEBUG)

# describe first federate run steps
firstFederate.timeout = 20
firstFederate.stdout  = file(firstFederate.name + ".out",'w+')
firstFederate.stdin   = file(firstFederate.name + ".in",'w+')
firstFederate.stderr  = file(firstFederate.name + ".err",'w+')
firstFederate.addRunStep("barrier","RTIG started")
dtest.ReusableSequences.addConditionalRunShellScript(firstFederate,c_shell_cmd="source "+certi_home+"/share/scripts/myCERTI_env.csh "+rtig_param['host'],
                               bourne_shell_cmd="source "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
firstFederate.addRunStep("runCommand",command=federate_param['path'])
firstFederate.addRunStep("expectFromCommand",pattern="Joined federation.*")
firstFederate.addRunStep("ok",firstFederate.getFutureLastStepStatus,"First Federate started and has joined federation")
firstFederate.addRunStep("expectFromCommand",pattern="Press ENTER to start execution")
#firstFederate.addRunStep("barrier","First Federate waiting other(s) before going on")
#firstFederate.addRunStep("barrier","Other Federate started")
firstFederate.addRunStep("sendToCommand",string="\n")

firstFederate.addRunStep("barrier","TRTC Sequence starts...")

firstFederate.addRunStep("expectFromCommand",pattern="Time Regulation Enabled, press ENTER to continue")
firstFederate.addRunStep("ok",firstFederate.getFutureLastStepStatus,"TimeRegulationEnabled received - "+firstFederate.name)
firstFederate.addRunStep("sendToCommand",string="\n")

firstFederate.addRunStep("expectFromCommand",pattern="Time Constrained Enabled, press ENTER to continue")
firstFederate.addRunStep("ok",firstFederate.getFutureLastStepStatus,"TimeConstrainedEnabled received - "+firstFederate.name)
firstFederate.addRunStep("sendToCommand",string="\n")

firstFederate.addRunStep("barrier","TRTC Sequence end.")
firstFederate.addRunStep("terminateCommand")
firstFederate.addRunStep("barrier","All Federate(s) ended")

def goTest():
    myDTestMaster = dtest.DTestMaster("HLA test test_TRTCCallbacks Starts","Launch RTIG + 1 federate for testing TimeRegulated and TimeConstrained callbacks,...")
    myDTestMaster.timeout = 40
    myDTestMaster.register(rtig)
    myDTestMaster.register(firstFederate)
    myDTestMaster.startTestSequence()
    myDTestMaster.waitTestSequenceEnd()
    
goTest()

