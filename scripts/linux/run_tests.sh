#!/bin/bash

TEST_SUITE=$1
HOME_DIR=$2

if [ $TEST_SUITE != "failure" ]; then
  cd $HOME_DIR/scripts/linux/ && ./run_failure_tests.sh
fi

if [ $TEST_SUITE != "success" ]; then
  cd $HOME_DIR/scripts/linux/ && ./run_success_tests.sh
fi
