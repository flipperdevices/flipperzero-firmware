#!/bin/bash
#
# testing script: compileAllExamples
#
# This script will compile all the local examples, optionally installing wolfSSL in the ESP-IDF components directory.
#
# Example usage:
#   cd wolfssl && docker run --rm -v $PWD:/project -w /project espressif/idf:latest IDE/Espressif/ESP-IDF/compileAllExamples.sh
#
# Parameter option to also run the ./setup.sh to install the wolfSSL component in ESP-IDF and test for multiple installs:
#
#   --run-setup
#
# Note that once installed, the wolfSSL component will need to be manually removed to successfully run this script.
#
if  [[ "$IDF_PATH" == "" ]]; then
    echo "Error: $IDF_PATH not found; run Espressif export.sh"
    exit 1
fi
SCRIPT_DIR=$(builtin cd ${BASH_SOURCE%/*}; pwd)
RUN_SETUP=$1
THIS_ERR=0

echo "Found IDF_PATH = $IDF_PATH"

# Regular tests of wolfSSL in local component directories of each project:
#
# Note these tests should FAIL if wolfSSL is already installed in ESP-IDF
#
for file in "benchmark" "client" "server" "test"; do
    pushd ${SCRIPT_DIR}/examples/wolfssl_${file}/ && idf.py fullclean build;
    THIS_ERR=$?
    popd
    if [ $THIS_ERR -ne 0 ]; then
        echo "Failed in ${file}"
        exit 1
    fi
done


# Check for option to also install wolfSSL.
#
# When doing so, we'll run a check that multiple installs should cause build failure.
if  [[ "$RUN_SETUP" == "--run-setup" ]]; then
    echo "Running wolfSSL setup.sh"

    # install wolfSSL into EDP-IDF shared components directory.
    ./setup.sh --verbose

    THIS_ERR=$?
    if [ $? -ne 0 ]; then
        echo "Failed running setup.sh"
        exit 1
    fi

    # Check ESP-IDF install:
    #
    # The wolfssl_test_idf should NOT have a local components/wolfssl when testing!
    # This test is to confirm the ESP-IDF component build properly after setup.
    #
    echo ""
    echo "Testing a build of wolfSSL in ESP-IDF components directory"
    echo ""
    for file in "test_idf"; do
        pushd ${SCRIPT_DIR}/examples/wolfssl_${file}/ && idf.py fullclean build;
        THIS_ERR=$?
        popd
        if [ $? -ne 0 ]; then
            echo "Failed in ${file}"
            exit 1
        fi
    done

    # Check multiple installs: the wolfSSL component in ESP-IDF and local directory:
    #
    # The wolfssl_test project already has a local wolfSSL component directory.
    #
    # Once wolfssl has been installed to ESP-IDF components, the local
    # component build SHOULD fail:
    echo ""
    echo "Testing a build of wolfSSL in both local and ESP-IDF components directory"
    echo ""
    for file in "test"; do
        pushd ${SCRIPT_DIR}/examples/wolfssl_${file}/ && idf.py fullclean build;
        THIS_ERR=$?
        popd
        if [ $THIS_ERR -ne 0 ]; then
            echo ""
            echo "Success: Confirmed build fails when wolfSSL found in ESP-IDF and local project."
            echo ""
        else
            echo "Error: build should have failed when wolfSSL found in ESP-IDF and local project."
            exit 1
        fi
    done
else
    echo "Skipping ESP-IDF install tests. For these tests, use parameter: --run-setup"
fi


# Show a reminder that wolfSSL was installed as a shared component.
if  [[ "$RUN_SETUP" == "--run-setup" ]]; then
    echo ""
    echo "wolfSSL was installed as an ESP-IDF component. This will be in conflict with any project that has a local component."
    echo ""
    echo "Delete the installed component before re-running this test."
    echo ""
fi

# Done
echo "Completed compileAllExamples in $SCRIPT_DIR"
