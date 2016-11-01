---
layout: main-page
title: "Mēnsūra"
---

Mensura is a framework to perform physics analysis of data recorded by the [CMS](http://cms.cern.ch) experiment.
It operates with abstracted representations of reconstructed and generator-level objects, such leptons, jets, and missing Et, concentrating on their physical properties rather than details of reconstruction.
Events in input datasets are processed with a set of plugins defined by user, which allows to apply a modular approach.
Distribution includes standard plugins for a number of common tasks such as event selection, reweighting to account for b-tagging scale factors, and others.
The framework supports multithreaded processing.

Out of the box, the framework supports reading files with events produced by the [PEC-tuples](https://github.com/andrey-popov/PEC-tuples) package.
However, it is possible to read files of a custom format if user implements appropriate reader plugins.


Link to a [test page]({{ site.baseurl }}/test-page).
