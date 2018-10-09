import sys
import os
import glob
import subprocess
import time
import shutil
import filterDlls as filter_dll

if os.path.exists("windows64"):
    shutil.rmtree("windows64")
os.makedirs("windows64")

os.chdir('../..')
print(os.getcwd())

cmd = ['sudo gnome-terminal --execute sudo make docker-test-mingw@fedora V=1 DEBUG=1 J=4 /bin/bash']
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')

time.sleep(40)

#geting id from docker

id = str(os.popen("sudo docker ps -q").readlines())
id = id[2:]
id = id[:-4]
print(id)

cmd = ["sudo docker exec -i " + id + " sh -c 'cd /tmp/qemu-test/src/ ;pwd"
                            " cd QEMU_SRC ; ./configure --cross-prefix=x86_64-w64-mingw32- --target-list=rh850-softmmu ;make'"]

p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

cmd = ["sudo docker exec -i " + id + " sh -c 'mkdir ~/dlls;  cd /usr/x86_64-w64-mingw32/sys-root/mingw/bin/ ; cp *.dll ~/dlls'"]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

cmd = ["sudo docker cp " + id+":tmp/qemu-test/src/rh850-softmmu/qemu-system-rh850.exe isystem/utils/windows64"]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

cmd = ["sudo docker cp " + id+":/root/dlls isystem/utils/windows64"]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

os.chdir('isystem/utils')

shutil.make_archive("windows64", 'zip', "windows64")

os.chdir('../../')
print(os.getcwd())

cmd = ["sudo docker stop " + id]
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
p.wait()

#remove docker
cmd = ['sudo rm -rf docker-src*']
p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')


print("------------FINISHED------------")
