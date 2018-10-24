#the script should be tested, its possibly that it does not work right...
# could be some problems with wine executing on 32 bit exe programs?

import sys
import os
import glob
import subprocess
import time
import shutil
import argparse
import shutil


def check_dlls(copyRh850, copyArm, copyTricore, copyPowerPc):
    check = ""
    if copyRh850:
        check = "rh850"
    elif copyArm:
        check = "arm"
    elif copyTricore:
        check = "tricore"
    elif copyPowerPc:
        check = "powerpc"

    os.chdir('isystem/utils')
    if os.path.exists("windows32"):
        os.chdir('windows32')

        if os.path.exists("filtered_dlls"):
            shutil.rmtree("filtered_dlls")
        os.makedirs("filtered_dlls")

        for file in glob.glob("*.dll"):
            os.rename(file, file + '_tst')
            print(file, end='')
            try:
                cmd = ['wine qemu-system-' + check + '.exe -M help > /dev/null 2>&1']
                subprocess.check_call(cmd, shell=True, executable='/bin/bash')
                print()
                os.rename(file + '_tst', file)
            except:
                print(" NEEDED")
                os.rename(file + '_tst', file)
                shutil.copy2(file, '/filtered_dlls')

        os.chdir("../")

    if os.path.exists("windows64"):
        os.chdir('windows64')

        if os.path.exists("filtered_dlls"):
            shutil.rmtree("filtered_dlls")
        os.makedirs("filtered_dlls")

        for file in glob.glob("*.dll"):
            os.rename(file, file + '_tst')
            print(file, end='')
            try:
                cmd = ['wine qemu-system-' + check + '.exe -M help > /dev/null 2>&1']
                subprocess.check_call(cmd, shell=True, executable='/bin/bash')
                print()
                os.rename(file + '_tst', file)

            except:
                print(" NEEDED")
                os.rename(file + '_tst', file)
                shutil.copy2(file, '/filtered_dlls')

        os.chdir("../")


def zip_files():
    #os.chdir('isystem/utils')
    if os.path.exists("windows32"):
        shutil.make_archive("windows32", 'zip', "windows32")
    if os.path.exists("windows64"):
        shutil.make_archive("windows64", 'zip', "windows64")


def start_docker():
    # change dir to /qemu
    os.chdir('../..')
    print(os.getcwd())

    cmd = ['sudo gnome-terminal --execute sudo make docker-test-mingw@fedora V=1 DEBUG=1 J=4 /bin/bash']
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')

    isNotStarted = True

    while isNotStarted:
        time.sleep(1)
        id = str(os.popen("sudo docker ps --filter ancestor=qemu:fedora -q").readlines())
        if len(id) > 4:
            isNotStarted = False
            id = id[2:]
            id = id[:-4]
            print(id)

    return id

def stop_docker():
    os.chdir('../../')
    print(os.getcwd())

    cmd = ["sudo docker stop " + id]
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    # remove docker
    cmd = ['sudo rm -rf docker-src*']
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')


def copy_exe(copyRh850, copyArm, copyTricore, copyPowerPc, architecure):

    path = " "
    if copyRh850:
        cmd = ["sudo docker cp " + id + ":tmp/qemu-test/src/rh850-softmmu/qemu-system-rh850.exe"
                                        " isystem/utils/windows" + architecure]
        p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
        p.wait()

    if copyArm:
        cmd = ["sudo docker cp " + id + ":tmp/qemu-test/src/arm-softmmu/qemu-system-arm.exe"
                                        " isystem/utils/windows" + architecure]
        p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
        p.wait()

    if copyTricore:
        cmd = ["sudo docker cp " + id + ":tmp/qemu-test/src/tricore-softmmu/qemu-system-tricore.exe"
                                        " isystem/utils/windows" + architecure]
        p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
        p.wait()

    if copyPowerPc:
        cmd = ["sudo docker cp " + id + ":tmp/qemu-test/src/powerpc-softmmu/qemu-system-powerpc.exe"
                                        " isystem/utils/windows" + architecure]
        p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
        p.wait()


def build_copy_64(id, target, copyRh850, copyArm, copyTricore, copyPowerPc):

    cmd = ["sudo docker exec -i " + id + " sh -c 'cd /tmp/qemu-test/src/ ;pwd"
                                " cd QEMU_SRC ; ./configure --cross-prefix=x86_64-w64-mingw32- "
                                         "--target-list="+target + " ;make'"]

    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    cmd = ["sudo docker exec -i " + id + " sh -c 'mkdir ~/dlls;  cd /usr/x86_64-w64-mingw32/sys-root/mingw/bin/ ;"
                                         " cp *.dll ~/dlls'"]
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    copy_exe(id,copyRh850, copyArm, copyTricore, copyPowerPc, 64)

    cmd = ["sudo docker cp " + id+":/root/dlls isystem/utils/windows64"]
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()


def build_copy32(id, target, copyRh850, copyArm, copyTricore, copyPowerPc):

    cmd = ["sudo docker exec -i " + id + " sh -c 'cd /tmp/qemu-test/src/ ;pwd"
                                         " cd QEMU_SRC ; ./configure --cross-prefix=i686-w64-mingw32-"
                                         " --target-list="+target + " ;make'"]

    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    cmd = [
        "sudo docker exec -i " + id + " sh -c 'mkdir ~/dlls32;  cd /usr/i686-w64-mingw32/sys-root/mingw/ ;"
                                      " cp **/*.dll ~/dlls32'"]
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    copy_exe(id,copyRh850, copyArm, copyTricore, copyPowerPc, 32)

    cmd = ["sudo docker cp " + id + ":/root/dlls32 isystem/utils/windows32"]
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()


parser = argparse.ArgumentParser()
parser.add_argument('-t', '--target', nargs='+', default=['arm', 'powerpc', 'tricore', 'rh850'],
                    choices=['arm', 'powerpc', 'tricore', 'rh850'])

parser.add_argument("-a", "--architecture", nargs='+', default=['32', '64'], choices=['32', '64'])
args = parser.parse_args()

build = ""
build64 = False
build32 = False

for i in args.target:
    build += ("," + i + "-softmmu")
build = build[1:]

for i in args.architecture:
    if i == '64':
        build64 = True
    if i == '32':
        build32 = True

copyArm = False
copyPowerPc = False
copyTricore = False
copyRh850 = False

if "rh850" in build:
    copyRh850 = True

if "arm" in build:
    copyArm = True

if "tricore" in build:
    copyTricore = True

if "powerpc" in build:
     copyPowerPc = True

#start docker
id = start_docker()

if build64 and build32:
    print("---------Building: " + build + " for 32 and 64 bit Windows---------")
    if os.path.exists("windows32"):
        shutil.rmtree("windows32")
    os.makedirs("windows32")
    if os.path.exists("windows64"):
        shutil.rmtree("windows64")
    os.makedirs("windows64")
    build_copy_64(id, build, copyRh850, copyArm, copyTricore, copyPowerPc)
    build_copy_32(id, build, copyRh850, copyArm, copyTricore, copyPowerPc)

elif build32:
    print("---------Building: " + build + " for 32 bit Windows---------")
    if os.path.exists("windows32"):
        shutil.rmtree("windows32")
    os.makedirs("windows32")
    build_copy_32(id, build, copyRh850, copyArm, copyTricore, copyPowerPc)

elif build64:
    print("---------Building: " + build + " for 64 bit Windows---------")
    if os.path.exists("windows64"):
        shutil.rmtree("windows64")
    os.makedirs("windows64")
    build_copy_64(id, build, copyRh850, copyArm, copyTricore, copyPowerPc)

check_dlls(copyRh850, copyArm, copyTricore, copyPowerPc)
zip_files()
stop_docker()

print("------------FINISHED------------")

