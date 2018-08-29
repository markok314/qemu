

# This script shows how to get flobal symbols (variables, functions, types, ...
# for all downlaod files in a project.
#
# (c) iSystem AG, Jun 2012

from __future__ import print_function

import isystem.connect as ic


def printVariables(partitionIdx, dataCtrl):
    vars = ic.VariableVector()
    dataCtrl.getVariables(partitionIdx, vars)
    for var in vars:
        print('  Var name: ', var.getName())
        print('  Var type: ', var.getType())
        print('  ------------------')


def printLabels(partitionIdx, dataCtrl):
    labels = ic.VariableVector()
    dataCtrl.getLabels(partitionIdx, labels)
    for label in labels:
        print('  Label name: ', label.getName())
        print('  Label type: ', label.getType())
        print('  ------------------')


def printModules(partitionIdx, dataCtrl):
    modules = ic.ModuleVector()
    dataCtrl.getModules(partitionIdx, modules)
    for module in modules:
        print('  Module name: ', module.getName())
        print('  Module path: ', module.getPath())
        print('  ------------------')


def printFunctions(partitionIdx, dataCtrl):
    functions = ic.FunctionVector()
    dataCtrl.getFunctions(partitionIdx, functions)

    for function in functions:
        print('  Function: ', function.getName())
        print('    Scope: ', function.getScope())
        print('    Type: ', function.getReturnType())
        print('    Module idx: ', function.getModuleIndex())
        print('    Parameters:')
        params = function.getParameters()
        for param in params:
            print('      Name: ', param.getName())
            print('      Type: ', param.getType())
            print('      ----')

        print('  ------------------')


def printTypes(partitionIdx, dataCtrl):
    types = ic.TypeVector()
    dataCtrl.getTypes(partitionIdx, types)
    for ctype in types:
        print('  Type name: ', ctype.getName())
        stype = ctype.getType()
        # see winIDEA/WCommon/globdefs.h, struct SType2
        # for type enum definitions. From Python
        # the type enums are accessible as object members: dir(type).
        print('  type: ', stype.m_byType)
        print('  type2: ', stype.m_byType2)
        print('  bitSize: ', stype.m_byBitSize)
        print('  ------------------')


def printTypedefs(partitionIdx, dataCtrl):
    typedefs = ic.TypedefVector()
    dataCtrl.getTypedefs(partitionIdx, typedefs)
    for typedef in typedefs:
        print('  Typedef name: ', typedef.getTypedefName())
        print('  Type name: ', typedef.getTypeName())
        print('  ------------------')


cmgr = ic.ConnectionMgr()
cmgr.connectMRU('')
debugCtrl = ic.CDebugFacade(cmgr)

debugCtrl.download()

dataCtrl = ic.CDataController(cmgr)

paths = ic.StrVector()
dlFileNames = ic.StrVector()
dataCtrl.getPartitions(paths, dlFileNames)

partitionIdx = 0
for dlFile in dlFileNames:
    print('Download file: ', dlFile)
    printVariables(partitionIdx, dataCtrl)
    printLabels(partitionIdx, dataCtrl)
    printModules(partitionIdx, dataCtrl)
    printFunctions(partitionIdx, dataCtrl)
    printTypes(partitionIdx, dataCtrl)
    printTypedefs(partitionIdx, dataCtrl)

    partitionIdx += 1
