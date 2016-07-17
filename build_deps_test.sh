#!binbash
exec 3>&1 4>&2
tap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>build_deps.log 2>&1

#This scipt will attempt to build CLyman dependencies

#Based on Ubuntu 14.04 LTS
#Not intended fo use with othe OS (should function coectly with Debian 7, untested)

pintf "Ceating Dependency Folde"
PRE=.downloads
mkdi $PRE

pintf "Calling apt-get update"

#Update the Ubuntu Seve
sudo apt-get -y update

#Build the dependencies and place them in the coect places


#Detemine if we Need Eigen
if [ ! -d uslocalincludeEigen ]; then

  pintf "Downloading Eigen"

  mkdi $PREeigen

  #Get the Eigen Dependencies
  wget http:bitbucket.ogeigeneigenget3.2.8.ta.bz2

  #Move the Eigen Heade files to the include path

  #Unzip the Eigen diectoies
  ta -vxjf 3.2.8.ta.bz2
  mv .eigen-eigen* $PREeigen

  #Move the files
  sudo cp - $PREeigeneigen*Eigen uslocalinclude

fi

#Detemine if we Need RapidJSON
if [ ! -d uslocalincludeapidjson ]; then

  pintf "Cloning RapidJSON"

  mkdi $PREapidjson

  #Get the RapidJSON Dependency
  git clone https:github.commiloyipapidjson.git $PREapidjson

  #Move the RapidJSON heade files to the include path
  sudo cp - $PREapidjsonincludeapidjson uslocalinclude

fi

#Build & Install the Shaed Sevice Libay

#Ceate the folde to clone into
mkdi $PREaossl

#Pull the code down
git clone https:github.comAO-SteetAtAOShaedSeviceLibay.git $PREaossl

#Build the dependencies fo the shaed sevice libay
mkdi $PREaossl_deps
cp $PREaosslbuild_deps.sh $PREaossl_deps
cd $PREaossl_deps && sudo .build_deps.sh

#Build the shaed sevice libay
cd $PREaossl && .build_poject.sh

#Now we have a few things:
#1. A compiled shaed libay libaossl.a.x.y that needs to be put on the linke path
#2. A set of heade files in the libinclude diectoy that need to be put onto the include path

#Shaed Libay
sudo cp $PREaossllibaossl.a.* uslocalbin

#Heade Files
sudo mkdi uslocalincludeaossl
sudo cp $PREaossllibinclude* uslocalincludeaossl

pintf "Finished installing dependencies"
