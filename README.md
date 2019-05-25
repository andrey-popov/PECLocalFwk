# Mēnsūra

Mensura is a framework to perform physics analysis of data recorded by the [CMS](http://cms.cern.ch) experiment.
It operates with abstracted representations of reconstructed and generator-level objects, such leptons, jets, and missing p<sub>T</sub>, concentrating on their physical properties rather than details of reconstruction.
Events in input datasets are processed with a set of plugins defined by user, which allows to apply a modular approach.
Distribution includes standard plugins for a number of common tasks such as event selection, reweighting to account for b&nbsp;tagging scale factors, and others.
The framework supports multithreaded processing.

Out of the box, the framework supports reading files with events produced by the [PEC-tuples](https://github.com/andrey-popov/PEC-tuples) package.
However, it is possible to read files of a custom format if user implements appropriate reader plugins.

The framework uses Boost and [ROOT 6](http://root.cern.ch), and modern C++ compiler and CMake are needed to build it. This can be done as follows:

```sh
. ./env.sh

mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..  # Change build type if needed
make

cd ../test
mkdir -p build && cd build
cmake .. && make
cd ..

```

Doxygen documentation is available at [GitHub pages](http://andrey-popov.github.io/mensura/).

As of 2019, the framework is not being maintained actively.
