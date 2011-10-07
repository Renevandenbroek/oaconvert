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

#ifndef LATLON_H 
#define LATLON_H 

#include <iostream>

/**
 * Class offering common functionality to Latitude and Longitude classes.
 */
class LatLon {

  public:

    /**
     * Construct a LatLon object without angle nor direction.
     *
     * TODO: we shouldn't be able to construct these kind of objects.  How
     *       to enforce this?  In Java-terminology: how to make this an
     *       'abstract' class?
     */
    LatLon();

    /**
     * Construct a LatLon object with the specified angle in decimal degrees.
     *
     * @param degrees Angle in degrees.
     */
    LatLon(double degrees);
    LatLon(int degrees, double minutes, char direction);
    LatLon(int degrees, int minutes, int seconds, char direction);

    double getAngle() const;
    char getDirection() const;
    int getDegrees() const;
    int getMinutes() const;
    double getSeconds() const;
    const double toRadians() const;

    void setDirection(char c);

    bool operator ==(const LatLon& other) const;
    friend std::ostream& operator <<(std::ostream& outputStream,
                                     const LatLon& l);

  private:
    double angle;
    char direction;

};

#endif /* LATLON_H */