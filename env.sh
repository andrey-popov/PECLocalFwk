#!/bin/bash


function set_environment
{
    case $1 in
        "-r")
        source /afs/cern.ch/sw/lcg/external/gcc/4.7.2/x86_64-slc5/setup.sh ""
        root_version="5.34.05"
        source /afs/cern.ch/sw/lcg/app/releases/ROOT/$root_version/x86_64-slc5-gcc46-opt/root/bin/thisroot.sh
        echo "GCC 4.7 and ROOT $root_version are configured";;
        
        "-b")
        boost_lib="/afs/cern.ch/sw/lcg/external/Boost/1.50.0_python2.7/x86_64-slc5-gcc46-opt/lib/"
        if [ -n $LD_LIBRARY_PATH ] ; then
            export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$boost_lib
        else
            export LD_LIBRARY_PATH=$boost_lib
        fi
        echo "Boost is configured";;
    esac
}

if [[ $# -eq 0 ]] ||  [[ $# -gt 0 && $1 = "-h" ]] ; then
    echo "Usage:"
    echo " . env.sh [options]"
    echo "Options:"
    echo " -r ROOT environment"
    echo " -b Boost environment"
    echo " -a All the above"
    echo " -h This usage message"
else
    export PEC_FWK_INSTALL=`pwd`
    if [ $1 = "-a" ] ; then
        set_environment -r
        set_environment -b
    else
        for arg in "$@" ; do
            set_environment $arg
        done
    fi
fi
