Import ('env')
import threading
from threading import Thread
from base64 import b64decode
import sys
import glob
import serial
from platformio import util
import os

print("jej")
#print(env.Dump())
print ("Current build targets", map(str, BUILD_TARGETS))

returnCodes=[]#list of tuples storing com port and upload result
def run(port):
    env.Replace(UPLOAD_PORT=port)
    for i in range (0,3):#try up to 3 times
        if(i>0):
            env.Replace(UPLOAD_SPEED="115200")#try slowing down baud
        command = env.subst('$UPLOADCMD') +" "+ env.subst('$BUILD_DIR\$PROGNAME') +".bin"
        errorCode=env.Execute(command)
        if (errorCode==0):
            returnCodes.append((port,errorCode))
            return

    returnCodes.append((port,errorCode))

def after_build(source, target, env):
    print("----------------------------------------------------------")
    #print(ARGUMENTS)
    ini = b64decode(ARGUMENTS.get("PROJECT_CONFIG"))
    f = open(ini)
    l = f.readlines()
    simultaneous_upload_ports = ""
    for li in l:
        if li.strip().startswith("simultaneous_upload_ports"):
            print(li)
            simultaneous_upload_ports = li.split("=")[1].strip()
            break
    if (simultaneous_upload_ports!=None):#check if defined
        threads = []
        
        print(simultaneous_upload_ports)
        ports = simultaneous_upload_ports.split(",")
        print(ports)
        #ports = map(str.strip, b64decode(simultaneous_upload_ports).split(','))
        if(ports[0]=="AUTO"):
            ports=serial_ports()
        print (("Uploading to " + str(ports)))
        for port in ports :
            port = port.strip()
            thread = Thread(target=run, args=(port,))
            thread.start()
            threads.append(thread)
        for thread in threads:
            thread.join()#wait for all threads to finish
        encounteredError=False
        sorted(returnCodes, key=lambda code: code[0])
        for code in returnCodes:
            if(code[1]==0):
                print (code[0] + " Uploaded Successfully")
            elif(code[1]==1):
                print (code[0] + " Encountered Exception, Check serial port")
                encounteredError=True
            elif(code[1]==2):
                print (code[0] + " Encountered Fatal Error")
                encounteredError=True
        if(encounteredError):
            Exit(1)
    print("Uspesno uploadano")
    Exit(0)


env.AddPreAction("upload", after_build)#"buildprog" dowesnt seem to work
# env.AddPostAction("upload", after_build)#uploads to port specified by upload_port first then does simultaneous_upload_ports
# AlwaysBuild(env.Alias("uploadAll", "", after_build))#not sure how to make custom target to build first