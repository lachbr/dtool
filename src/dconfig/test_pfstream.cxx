// Filename: test_pfstream.cxx
// Created by:  cary (31Aug98)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#include <pfstream.h>
#include <string>

void ReadIt(istream& ifs) {
   std::string line;

   while (!ifs.eof()) {
      std::getline(ifs, line);
      if (line.length() != 0)
         cout << line << endl;
   }
}

main()
{
   IPipeStream ipfs("ls -l");

   ReadIt(ipfs);
}
