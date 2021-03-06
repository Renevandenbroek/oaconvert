/*
  Copyright 2011 Bart Vandewoestyne, Yves Frederix.

  This file is part of oaconvert.

  oaconvert is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  oaconvert is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with oaconvert.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include "Airspace.h"
#include "OutputState.h"

class OutputWriter
{

public:
    OutputWriter();
    OutputWriter(const std::string& filename);
    OutputWriter(std::ostream& stream);

    ~OutputWriter();

    void writeHeader();
    void write(const Airspace& s);
    void writeFooter();

    //! \param[in] state The new state. This output writer does \e not own the
    //!                  given state.
    void changeState(OutputState* state);

private:
    OutputState* _state;

    //! If writing to file, this Writer owns the ofstream object.
    // Note: do NOT change the order of outfile and out in this header file!
    std::ofstream *outfile;

    //! The internal output stream to write to.
    std::ostream out;

    // We only declare, but do not implement the copy constructor
    // and assignment operator.  See [Meyers 2005], Item 11, second last
    // paragraph.
    OutputWriter(const OutputWriter& w);
    OutputWriter operator=(const OutputWriter& w);

};

#endif /* OUTPUTWRITER_H */
