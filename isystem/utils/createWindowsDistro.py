import sys
import os
import glob
import subprocess
import time

os.chdir('../..')
print(os.getcwd())

if(len(glob.glob('docker-src*')) == 0):
    #docker is not in qemu folder yet
    print("Docker installing")


else:
    #docker is already there, just need to start it
    print("Starting docker and copying files")

cmd = ['gnome-terminal --execute sudo make docker-test-mingw@fedora V=1 DEBUG=1 J=4 /bin/bash']
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash', stdout=subprocess.PIPE, universal_newlines=True)

poll = p.poll()
if poll == None:
    print('living')


#How to write to terminal with docker ???

print('dead')

#geting id from docker

#a = str(os.popen("sudo docker ps -q").readlines())
#a = a[2:]
#a = a[:-4]
#print(a)


#cmd = ['sudo docker exec -i -d ' + a + ' /bin/bash']
#subprocess.check_call(cmd, shell=True, executable='/bin/bash')





