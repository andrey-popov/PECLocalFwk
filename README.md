PECLocalFwk
===========

A framework to analyse [PlainEventContent files](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PlainEventContentTuples)
locally. The files are produced by [single-top subpackage](https://github.com/andrey-popov/single-top) for CMSSW. In a
normal workflow this step is performed in Grid. The framework allows to read the files, perform a user-defined event
selection, evaluate systematical variations, and enables user to define plugins to calcualte and store desired quantities.

Documentation for the framework is available in a dedicated [page](https://twiki.cern.ch/twiki/bin/view/CMS/PECLocalFramework)
of [CMS](http://cms.cern.ch) TWiki. Developer notes (including description of releases) are aggregated at
[this page](https://twiki.cern.ch/twiki/bin/view/CMS/PECLocalFrameworkDev).

Old versions of the framework are hosted at CERN [SVN](https://svnweb.cern.ch/trac/singletop/browser/PECLocalFwk/tags)
service. They have not been incorporated into this repository.
