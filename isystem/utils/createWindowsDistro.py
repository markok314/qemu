# This script builds quemu and creates distributables. See help for more info.

import sys
import os
import glob
import subprocess
import time
import shutil
import argparse
import shutil

g_isVerbose = False
BIN_DIR = 'build'


def log(msg: str):
    if g_isVerbose:
        print('\033[1;32;40m ' + msg + '\033[0;37;40m')


def check_dlls(arch: str, targets):
    """
    This f. checks, which dlls are really necessary for running quemu
    as simulator for isystem.test.
    """

    os.chdir(f'{BIN_DIR}{arch}')

    if os.path.exists("filtered_dlls"):
        shutil.rmtree("filtered_dlls")
    os.makedirs("filtered_dlls")

    os.chdir(f'dlls{arch}')

    for dllFName in glob.glob("*.dll"):
        os.rename(dllFName, dllFName + '_tst')
        print(dllFName, end='')
        try:

            cmd = ["su -c 'wine qemu-system-" + check + ".exe -M help > /dev/null 2>&1 '"]
            log('check_dlls: ' + ' '.join(cmd))
            subprocess.check_call(cmd, shell=True, executable='/bin/bash')
            print()
            os.rename(dllFName + '_tst', dllFName)
        except:
            print(" NEEDED")
            os.rename(dllFName + '_tst', dllFName)
            shutil.copy2(dllFName, '../filtered_dlls')

    os.chdir("..")


def zip_files():
    if os.path.exists(f'{BIN_DIR}32'):
        shutil.make_archive("qemu-i686", 'zip', f'{BIN_DIR}32')
    if os.path.exists(f'{BIN_DIR}64'):
        shutil.make_archive("qemu-x64", 'zip', f'{BIN_DIR}64')


def start_docker():

    os.chdir('..')
    # Optione: V=1 for verbose build
    cmd = ['sudo make docker-test-mingw@fedora V=1 DEBUG=1 J=4 /bin/bash']
    log('Start docker: ' + ' '.join(cmd))
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')

    while True:
        time.sleep(1)
        id = str(os.popen("sudo docker ps --filter ancestor=qemu:fedora -q").readlines())
        if len(id) > 4:
            id = id[2:]
            id = id[:-4]
            log(f'Started docker with id: {id}')
            break

    os.chdir('isystem')
    return id


def stop_docker():
    os.chdir('..')
    log(os.getcwd())

    cmd = ["sudo docker stop " + id]
    log('Stop docker: ' + ' '.join(cmd))
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')

    p.wait()

    # remove docker
    cmd = ['sudo rm -rf docker-src*']
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')

    os.chdir('isystem')


def _copyFromDockerToSharedDir(id, arch, targets):
    """
    :param arch: architecture, should be '32' or '64'
    :param targets: list of strings for targets, eg. ['arm', 'ppc']
    """

    for target in targets:
        cmd = [f"sudo docker cp {id}:tmp/qemu-test/src/{target}-softmmu/qemu-system-{target}.exe"
                                        f" {BIN_DIR}{arch}"]
        p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
        p.wait()


def _dockerBuild(dockerId, arch: str, targets):
    """
    :param arch: architecture, should be '32' or '64'
    :param targets: list of strings for targets, eg. ['arm', 'ppc']
    """

    xarch = 'x86_64'
    if arch == '32':
        xarch = 'i686'

    cmd = [f"sudo docker exec -i {dockerId} sh -c "
                                f"'cd $QEMU_SRC;echo $QEMU_SRC; pwd; ./configure --cross-prefix={xarch}-w64-mingw32- "
                                         "--target-list=" + ','.join(targets) + "; make'"]

    log('docker build: ' + ' '.join(cmd))
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    cmd = [f"sudo docker exec -i {dockerId} sh -c 'mkdir ~/dlls{arch};  cd /usr/{xarch}-w64-mingw32/sys-root/mingw/bin/ ;"
                                         f" cp *.dll ~/dlls{arch}'"]
    log('docker cp dlls: ' + ' '.join(cmd))
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()

    cmd = [f"sudo docker cp {dockerId}:/root/dlls{arch}/ {BIN_DIR}{arch}"]
    log('docker cp root dlls: ' + ' '.join(cmd))
    p = subprocess.Popen(cmd, shell=True, executable='/bin/bash')
    p.wait()


def _buildWinExecutables(arch, targets):
    """
    :param arch: architecture, should be '32' or '64'
    :param targets: list of strings for targets, eg. ['arm', 'ppc']
    """
    log("---------Building: " + ','.join(targets) + f" for {arch}-bit Windows---------")
    if os.path.exists(f"{BIN_DIR}{arch}"):
        shutil.rmtree(f"{BIN_DIR}{arch}")
    os.makedirs(f"{BIN_DIR}{arch}")

    # start docker
    dockerId = start_docker()

    _dockerBuild(dockerId, arch, targets)
    _copyFromDockerToSharedDir(dockerId, arch, targets)
    check_dlls(arch, targets)
    zip_files()
    stop_docker()


def parseArgs():

    usage = f"""
This script is used for building and deploying of ARM, RH850, POWERPC and 
TRICORE qemu for Windows. Script downloads and launches docker, configures 
it and starts building. Target '.exe' files and dll files are copied to 
{BIN_DIR}32 or {BIN_DIR}64. The script also filters DLLs and copies required 
DLLs to dir `filtered_dlls`.

You should have docker, python3 and wine installed.

Usage:

sudo python3 createWindowsDistro.py

Examples:
   To build RH850 and ARM for Windows x64:

   sudo python3 createWindowsDistro.py -t rh850,arm -a 64
"""
    parser = argparse.ArgumentParser(description=usage)

    parser.add_argument("-v", '--verbose', dest="isVerbose", action='store_true', default=False,
                        help="Writes more info during execution.")

    parser.add_argument('-t', '--targets', nargs='+', default=['arm', 'ppc', 'tricore', 'rh850'],
                        choices=['arm', 'ppc', 'tricore', 'rh850'])

    parser.add_argument("-a", "--arch", nargs='+', default=['64'], choices=['32', '64'])
    return parser.parse_args()


def main():
    global g_isVerbose

    args = parseArgs() 
   
    g_isVerbose = args.isVerbose

    if not os.getcwd().endswith('/isystem'):
        print("\033[1;31;40m ERROR: This script should be run in directory 'qemu/isystem'!")
        sys.exit(-1)

    for arch in args.arch:
        _buildWinExecutables(arch, args.targets)


if __name__ == '__main__':
    main()
    log("------------FINISHED------------")

