import sys
import os
import glob
import subprocess
import time
import shutil
import filterDlls as filter_dll

if os.path.exists("windows32"):
    shutil.rmtree("windows32")
os.makedirs("windows32")

os.chdir('../..')
print(os.getcwd())

cmd = ['sudo gnome-terminal --execute sudo make docker-test-mingw@fedora V=1 DEBUG=1 J=4 /bin/bash']
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')

#geting id from docker

isNotStarted = True
id = ""

while isNotStarted:
    time.sleep(1)
    id = str(os.popen("sudo docker ps --filter ancestor=qemu:fedora -q").readlines())
    if len(id) > 4:
        isNotStarted = False
        id = id[2:]
        id = id[:-4]
        print(id)

cmd = ["sudo docker exec -i " + id + " sh -c 'cd /tmp/qemu-test/src/ ;pwd"
                            " cd QEMU_SRC ; ./configure --cross-prefix=i686-w64-mingw32- --target-list=rh850-softmmu ;make'"]

p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

cmd = ["sudo docker exec -i " + id + " sh -c 'mkdir ~/dlls;  cd /usr/i686-w64-mingw32/sys-root/mingw/ ; cp **/*.dll ~/dlls'"]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

cmd = ["sudo docker cp " + id+":tmp/qemu-test/src/rh850-softmmu/qemu-system-rh850.exe isystem/utils/windows32"]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

cmd = ["sudo docker cp " + id+":/root/dlls isystem/utils/windows32"]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

os.chdir('isystem/utils')

shutil.make_archive("windows32", 'zip', "windows32")

os.chdir('../../')
print(os.getcwd())

cmd = ["sudo docker stop " + id]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

#remove docker
cmd = ['sudo rm -rf docker-src*']
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')


print("------------FINISHED------------")
