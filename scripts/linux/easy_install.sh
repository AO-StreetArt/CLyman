#!/bin/bash
#This script will install the pre-packaged release of CLyman
#Can be run with no command line arguments to simply install library
#-d flag will run install_deps script
#-r flag will uninstall

OPT="-none"

if [ "$#" -gt 0 ]; then
  OPT=$1
fi

if [ "$#" -gt 1 ]; then
  printf "You can execute this script with no input parameters to simply install the library,"
  printf "or with the '-r' flag to remove the library.  You may also supply the '-d' flag to"
  printf "install the dependencies in addition to the CLyman executable.\n"
  exit 64
fi

if [ $OPT = "-r" ]; then
  printf "Attempting to uninstall CLyman\n"
  sudo rm /usr/local/bin/clyman
  sudo rm /usr/local/bin/stop_clyman.py
  sudo rm -r /etc/clyman
  sudo rm -r /var/log/clyman
  sudo deluser clyman
  sudo groupdel clyman
  sudo rm /etc/systemd/system/clyman.service
  exit 0
else
  if [ $OPT = "-d" ]; then
    #Install the dependencies
    printf "This will install multiple libraries to your system, of various licenses including LGPL.  This may pull development versions,"
    printf "and it is therefore inadvisable to use in Production setups."
    printf "Please be sure to review the dependency page found at http://clyman.readthedocs.io/en/latest/pages/deps.html"
    printf "You may find the source code for this application, and the licenses for all dependencies at https://github.com/AO-StreetArt/CLyman"
    printf "Do you wish to continue? [y/n]:"
    read deps_confirm
    if [deps_confirm = 'y']; then
      printf "Installing Dependencies\n"
      sudo ./scripts/build_deps.sh
    fi
  fi

  printf "Installing CLyman\n"

  # Install the executable to the bin folder
  sudo cp clyman /usr/local/bin

  # Copy the stop_clyman script
  sudo cp scripts/stop_clyman.py /usr/local/bin

  # Copy the systemd unit file
  sudo cp clyman.service /etc/systemd/system

  # Create the clyman user and group
  sudo useradd --system --user-group clyman

  # Build the logging directory
  sudo mkdir /var/log/clyman

  # Create the config directory and copy configuration files
  sudo mkdir /etc/clyman
  sudo cp log4cpp.properties /etc/clyman
  sudo cp app.properties /etc/clyman

  # Ensure that the clyman user has permission to access both the configuration and logging directories
  sudo chown -R clyman:clyman /etc/clyman
  sudo chown -R clyman:clyman /var/log/clyman

  exit 0

fi