#include "info.h"
#include <iostream>
#include "CommandLineParams.h"

using namespace std;


void info::inform()
{

    QString help_clp = CommandLineParser::instance().getValDef("-help", "2");

    if (help_clp == "1") {
        cout<<"     key = value"<<"\n";
        cout<<"     -help=1     information"<<"\n";
        cout<<"     -t          technological delay (10 seconds)"<<"\n"; //Linux
        //cout<<"     -t          technological delay (10000 milliseconds)"<<"\n"; //Windows
        cout<<"     -pc         copy path on controller (/home/firmware)"<<"\n";
        cout<<"     -mode       program mode key "<<"\n";
        cout<<"     -mode=1     Creating firmware in command mode "<<"\n";
        cout<<"     example:    ./FWCreator -mode=1 -l=root -p=root -c=K000 -o=/home/bin/ -v=ps100 -i=/home/firmware/ "<<"\n";  //Linux
        //cout<<"     example:    .\FWCreator.exe  -mode=1 -c=K000 -o=C:\Users\bin -i=C:\Users\firmware -p=root -l=root -v=PS100  "<<"\n";  //Windows
        cout<<"     -m          magic code </p>"<<"\n";
        cout<<"     -m=old      magic code = 0x245A245A "<<"\n";
        cout<<"     -m=default  (any value except old) 0x245A245B"<<"\n";
        cout<<"     -v          firmware version (name railway carriage) (PS) "<<"\n";
        cout<<"     -l          login (root)"<<"\n";
        cout<<"     -p          password (root) "<<"\n";
        cout<<"     -ip         controller ip address  (192.168.0.120)"<<"\n";
        cout<<"     -i          firmware source path (/home/firmware/)"<<"\n"; //Linux
        cout<<"     -o          path to save firmware (/home/bin/)"<<"\n"; //Linux
        //cout<<"     -i          firmware source path (C:\Users\firmware)"<<"\n"; //Windows
        //cout<<"     -o          path to save firmware (C:\Users\bin)"<<"\n"; //Windows
        cout<<"     -c          controller name  (K000) "<<"\n";
        cout<<"     -calibr     exception file calibr.cfg from firmware  (0), 1 - remove from firmware, 2 - remove from disk "<<"\n";
        cout<<"     -geometry   exception file geometry.cfg from firmware  (0), 1 - remove from firmware, 2 - remove from disk "<<"\n";
        cout<<"     -s          suffix"<<"\n";
        cout<<"     -mode=2     firmware unpacking mode"<<"\n";
        cout<<"     example:    ./FWCreator -mode=2 -i=/home/bin/install_20_04_01_ps100.bin -o=/home/firmware/"<<"\n"; //Linux
        cout<<"     -i          path to firmware and name of firmware (/home/bin/*.bin)"<<"\n"; //Linux
        cout<<"     -o          path to unpack the firmware (/home/firmware/)"<<"\n"; //Linux
        //cout<<"     example:    .\FWCreator.exe  -mode=2 -c=K000 -i=C:\Users\bin -o=C:\Users\firmware0 "<<"\n"; //Windows
        //cout<<"     -i          path to firmware and name of firmware (C:\Users\bin\*.bin)"<<"\n"; //Windows
        //cout<<"     -o          path to unpack the firmware (C:\Users\firmware0)"<<"\n"; //Windows
        cout<<"     -mode=3     graphic mode "<<"\n";
        cout<<"     example:    ./FWCreator -mode=3 -l=root -p=root -c=K000 -v=ps100"<<"\n";  //Linux
        //cout<<"     example:    .\FWCreator -mode=3 -l=root -p=root -c=K000 -v=ps100"<<"\n";  //Windows
        cout<<"     -i/-o       not used"<<"\n";
        system("pause");
        exit(0);
    }
};
