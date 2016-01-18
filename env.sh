#!/bin/bash


function set_environment
{
    case $1 in
        "-r")
        
        gcc_version="5.2.0"
        source /afs/cern.ch/sw/lcg/external/gcc/$gcc_version/x86_64-slc6/setup.sh ""
        
        # Export symbol GCC_ROOT. This is needed because otherwise the Boost setup script will set
        # the environment for whatever default version of GCC is specified among its dependencies
        export GCC_ROOT=${LCG_gcc_home}
        
        root_version="6.04.12"
        source /afs/cern.ch/sw/lcg/app/releases/ROOT/$root_version/x86_64-slc6-gcc49-opt/root/bin/thisroot.sh
        
        #echo "GCC $gcc_version and ROOT $root_version have been configured"
        ;;
        
        
        "-b")
        
        # Version 1.57 used here will generate some deprecation warnings from auto_ptr, but this is
        # the most recent one installed at AFS
        boost_version="1.57.0"
        source /afs/cern.ch/cms/slc6_amd64_gcc493/external/boost/$boost_version/etc/profile.d/init.sh
        export BOOST_ROOT
        #echo "Boost has been configured"
        ;;
        
        
        "-p")
        
        export PEC_FWK_INSTALL=`pwd`
        #echo "Framework-specific variables have been set"
        ;;
    esac
}

if [[ $# -gt 0 && $1 = "-h" ]] ; then
    echo "Usage:"
    echo " . env.sh [options]"
    echo "Options:"
    echo " -r Setup GCC and ROOT"
    echo " -p Setup framework-specific variables"
    echo " -a All the above"
    echo " -h This usage message"
else
    
    if [[ $# -eq 0 ]] || [[ $1 = "-a" ]] ; then
        set_environment -r
        set_environment -b
        set_environment -p
    else
        for arg in "$@" ; do
            set_environment $arg
        done
    fi
fi
