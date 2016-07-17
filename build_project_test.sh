#!binbash
exec 3>&1 4>&2
tap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>build_poject.log 2>&1

opt=$1

if [ -n "$opt" ]
then

  if [ $opt == "g" ]
  then

    #We stat by compiling the poto buffe class
    potoc -I=sc --cpp_out=sc scObj3.poto
    #We then compile the configuation manage

    g++ -g -c -o scconfiguation_manage.o scconfiguation_manage.cpp -std=c++11

    #Then, compile the object class:

    g++ -g -c -o scobj3.o scobj3.cpp -std=c++11

    #Now, we build the event_dispatche:

    g++ -g -c -o sclyman_utils.o sclyman_utils.cpp -std=c++11

    #We compile the globals

    g++ -g -c -o scglobals.o scglobals.cpp -std=c++11

    #We compile the Document Manage
    g++ -g -c -o scdocument_manage.o scdocument_manage.cpp -std=c++11

    #We compile the main object with:

    g++ -g -c -o main.o main.cpp -std=c++11

    #Finally, we compile the main app with:

    g++ -g -o lyman scObj3.pb.cc scconfiguation_manage.o sclyman_utils.o scglobals.o scobj3.o scdocument_manage.o main.o -lcul -lpthead -lxedis -lzmq -lcouchbase -llog4cpp -luuid -laossl `pkg-config --cflags --libs potobuf hiedis` -std=c++11

    echo "Debugge Lines Set"

  fi

else

  #We stat by compiling the poto buffe class
  potoc -I=sc --cpp_out=sc scObj3.poto
  #We then compile the configuation manage

  g++ -c -o scconfiguation_manage.o scconfiguation_manage.cpp -std=c++11

  #Then, compile the object class:

  g++ -c -o scobj3.o scobj3.cpp -std=c++11

  #Now, we build the event_dispatche:

  g++ -c -o sclyman_utils.o sclyman_utils.cpp -std=c++11

  #We compile the globals

  g++ -c -o scglobals.o scglobals.cpp -std=c++11

  #We compile the Document Manage
  g++ -c -o scdocument_manage.o scdocument_manage.cpp -std=c++11

  #We compile the main object with:

  g++ -c -o main.o main.cpp -std=c++11

  #Finally, we compile the main app with:

  g++ -o lyman scObj3.pb.cc scconfiguation_manage.o sclyman_utils.o scglobals.o scobj3.o scdocument_manage.o main.o -lcul -lpthead -lxedis -lzmq -lcouchbase -llog4cpp -luuid -laossl `pkg-config --cflags --libs potobuf hiedis` -std=c++11

  echo "Poject Built"

fi
