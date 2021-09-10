Docker files
------------

This folder contains docker files that are used in testing nanopb automatically
on various platforms.

By default they take the newest master branch code from github.

To build tests for a single target, use for example:

    docker build ubuntu1804

To build tests for all targets, use:

    ./build_all.sh


