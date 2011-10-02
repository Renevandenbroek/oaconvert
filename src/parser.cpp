#include <iostream>
#include <boost/regex.hpp>

#include "airspace.h"
#include "coordinate.h"
#include "parser.h"
#include "polygon.h"
#include "circle.h"
#include "constants.h"

using namespace boost;
using namespace std;
using Constants::pi;

Parser::Parser()
: _writer(cout)
, currentDirection('+')
{}

Parser::Parser(ostream& stream)
: _writer( stream )
, currentDirection('+')
{}

AirSpace& Parser::getCurrentAirSpace()
{
  return currentAirSpace;
}

const Coordinate& Parser::getCurrentCoordinate() const
{
  return currentCoordinate;
}

char Parser::getCurrentDirection() const
{
  return currentDirection;
}

void Parser::setCurrentAirspace(const AirSpace& s)
{
  currentAirSpace = s;
}

void Parser::setCurrentCoordinate(const Coordinate& c)
{
  currentCoordinate = c;
}

void Parser::setCurrentDirection(char d)
{
  currentDirection = d;
}

Coordinate Parser::getCoordinate(const std::string& s) const
{
  smatch matches;
  regex expression;
  Latitude lat;
  Longitude lon;

  // Example: 101:20:32 S 102:32:12 W
  expression = "[^0-9]*(\\d+):(\\d\\d):(\\d\\d)\\s*([NZSnzs])[\\s,]+(\\d+):(\\d\\d):(\\d\\d)\\s*([OWEowe])*.*";
  if ( regex_match(s, matches, expression) )
  {
    string degreesLat, minutesLat, secondsLat;
    string degreesLon, minutesLon, secondsLon;
    string directionLat, directionLon;

    degreesLat.assign(   matches[1].first, matches[1].second );
    minutesLat.assign(   matches[2].first, matches[2].second );
    secondsLat.assign(   matches[3].first, matches[3].second );
    directionLat.assign( matches[4].first, matches[4].second );
    degreesLon.assign(   matches[5].first, matches[5].second );
    minutesLon.assign(   matches[6].first, matches[6].second );
    secondsLon.assign(   matches[7].first, matches[7].second );
    directionLon.assign( matches[8].first, matches[8].second );

    lat = Latitude(atoi(degreesLat.c_str()),
                   atoi(minutesLat.c_str()),
                   atoi(secondsLat.c_str()), directionLat[0]);
    lon = Longitude(atoi(degreesLon.c_str()),
                    atoi(minutesLon.c_str()),
                    atoi(secondsLon.c_str()), directionLon[0]);
    return Coordinate(lat, lon);
  }

  // Example: 101:20.32 N 102:32.12 E
  expression = "[^0-9]*(\\d+):(\\d+.\\d+)\\s*([NZSnzs])[\\s,]+(\\d+):(\\d+.\\d+)\\s*([OWEowe])*.*";
  if ( regex_match(s, matches, expression) )
  {
    string degreesLat, minutesLat;
    string degreesLon, minutesLon;
    string directionLat, directionLon;

    degreesLat.assign(   matches[1].first, matches[1].second );
    minutesLat.assign(   matches[2].first, matches[2].second );
    directionLat.assign( matches[3].first, matches[3].second );
    degreesLon.assign(   matches[4].first, matches[4].second );
    minutesLon.assign(   matches[5].first, matches[5].second );
    directionLon.assign( matches[6].first, matches[6].second );

    Latitude lat(atoi(degreesLat.c_str()),
                 atof(minutesLat.c_str()),
                 directionLat[0]);
    Longitude lon(atoi(degreesLon.c_str()),
                  atof(minutesLon.c_str()),
                  directionLon[0]);
    return Coordinate(lat, lon);
  }

  cout << "\nERROR: invalid coordinate string: " << s << endl;
  exit(1);
}


void Parser::handleLine(const std::string& line)
{

  smatch matches;
  regex expression;

  expression = "\\s*\\*.*";
  if ( regex_match(line, matches, expression) )
  {
    // do nothing.
  }

  // Although not specified in the OpenAir specs at
  //    http://www.winpilot.com/usersguide/userairspace.asp
  // we *do* accept ICAO airspace classes E, F and G as valid input.
  expression = "\\s*AC\\s+([RQPABCDEFGW]|GP|CTR)\\s*";
  if ( regex_match(line, matches, expression) )
  {
    _writer.write(getCurrentAirSpace());
    getCurrentAirSpace().clear();

    string airspace_class;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      airspace_class.assign(matches[i].first, matches[i].second);
    }
    //cout << "DEBUG: " << airspace_class << endl;
    getCurrentAirSpace().setClass(airspace_class);
  }

  expression = "\\s*AN\\s+(.*)";
  if ( regex_match(line, matches, expression) )
  {
    string airspace_name;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      airspace_name.assign(matches[i].first, matches[i].second);
    }
    //cout << "DEBUG: " << airspace_name << endl;
    getCurrentAirSpace().setName(airspace_name);
  }

  expression = "\\s*AH\\s+(.*)";
  if ( regex_match(line, matches, expression) )
  {
    string airspace_ceiling;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      airspace_ceiling.assign(matches[i].first, matches[i].second);
    }
    //cout << "DEBUG: " << airspace_ceiling << endl;
    getCurrentAirSpace().setCeiling(airspace_ceiling);
  }

  expression = "\\s*AL\\s+(.*)";
  if ( regex_match(line, matches, expression) )
  {
    string airspace_floor;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      airspace_floor.assign(matches[i].first, matches[i].second);
    }
    //cout << "DEBUG: " << airspace_floor << endl;
    getCurrentAirSpace().setFloor(airspace_floor);
  }

  expression = "\\s*AT\\s+(.*)"; // This one is optional.
  if ( regex_match(line, matches, expression) )
  {
    string airspace_coordinate;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      airspace_coordinate.assign(matches[i].first, matches[i].second);
    }
    getCurrentAirSpace().addLabelCoordinate(getCoordinate(airspace_coordinate));
  }

  expression = "\\s*V\\s+X\\s*=\\s*(.*)";
  if ( regex_match(line, matches, expression) )
  {
    string coordinate;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      coordinate.assign(matches[i].first, matches[i].second);
    }
    setCurrentCoordinate(getCoordinate(coordinate));
  }

  expression = "\\s*V\\s+D\\s*=\\s*(.*)";
  if ( regex_match(line, matches, expression) )
  {
    string direction_string;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      direction_string.assign(matches[i].first, matches[i].second);
    }
    setCurrentDirection(direction_string[0]);
  }

  expression = "\\s*DP\\s+(.*)";
  if ( regex_match(line, matches, expression) )
  {
    string point_coordinate;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      point_coordinate.assign(matches[i].first, matches[i].second);
    }
    getCurrentAirSpace().getPolygon().add(getCoordinate(point_coordinate));
    //cout << "DEBUG: " << getCurrentAirSpace() << endl;
  }

  expression = "\\s*DA\\s+(\\d+\\.*\\d*)[\\s,]+(\\d+)[\\s,]+(\\d+)";
  if ( regex_match(line, matches, expression) )
  {
    // Set center-coordinate and direction that we have just parsed.
    getCurrentAirSpace().getArc().setCenter(getCurrentCoordinate());
    getCurrentAirSpace().getArc().setDirection(getCurrentDirection());

    // Read the matched values and create our Arc.
    string radiusNM;
    string angleStart;
    string angleEnd;
    radiusNM.assign(   matches[1].first, matches[1].second );
    angleStart.assign( matches[2].first, matches[2].second );
    angleEnd.assign(   matches[3].first, matches[3].second );
    getCurrentAirSpace().getArc().setRadiusNM(atof(radiusNM.c_str()));
    getCurrentAirSpace().getArc().setStartAngle(atof(angleStart.c_str()));
    getCurrentAirSpace().getArc().setEndAngle(atof(angleEnd.c_str()));

    // Add the arc points to this space's Polygon.
    // TODO: don't use *hardcoded* 100 points for the discretization!
    getCurrentAirSpace().getPolygon().add(getCurrentAirSpace().getArc().toPolygon(100));
  }

  // TODO: check this matching pattern
  expression = "\\s*DB\\s+(.*)\\s*,\\s*(.*)";
  if ( regex_match(line, matches, expression) )
  {
    // Set the center-coordinate and direction that we have just parsed.
    getCurrentAirSpace().getArc().setCenter(getCurrentCoordinate());
    getCurrentAirSpace().getArc().setDirection(getCurrentDirection());

    // Fetch the start and end coordinate for this arc.
    string coord1, coord2;
    coord1.assign(matches[1].first, matches[1].second);
    coord2.assign(matches[2].first, matches[2].second);
    Coordinate c1 = getCoordinate(coord1);
    Coordinate c2 = getCoordinate(coord2);
    //cout << c1 << endl;
    //cout << c2 << endl;

    // Compute start and end angle (in standard coordinate frame!)
    // TODO: it looks like these are not computed 100% exactly... check why!
    double dLat1 = c1.getLatitude().getAngle()  - getCurrentCoordinate().getLatitude().getAngle();
    double dLon1 = c1.getLongitude().getAngle() - getCurrentCoordinate().getLongitude().getAngle();
    double dLat2 = c2.getLatitude().getAngle()  - getCurrentCoordinate().getLatitude().getAngle();
    double dLon2 = c2.getLongitude().getAngle() - getCurrentCoordinate().getLongitude().getAngle();
    double startAngle = 180.0*atan2(dLat1, dLon1)/pi;
    double endAngle   = 180.0*atan2(dLat2, dLon2)/pi;

    // Convert start and end angle to airspace coordinate frame.
    startAngle = 90 - startAngle;
    endAngle   = 90 - endAngle;

    // Use maximum of the two radii (for safety reasons).
    //double radius = max( c1.getDistance(getCurrentCoordinate()), c2.getDistance(getCurrentCoordinate()) );
    // Use minimum of the two radii.
    //double radius = min( c1.getDistance(getCurrentCoordinate()), c2.getDistance(getCurrentCoordinate()) );
    // Use average of the two radii.
    double radius = ( c1.getDistance(getCurrentCoordinate()) + c2.getDistance(getCurrentCoordinate()) )*0.5;

    // Set start and end angle, together with the chosen radius.
    getCurrentAirSpace().getArc().setRadiusNM(radius/1852.0);
    getCurrentAirSpace().getArc().setStartAngle(startAngle);
    getCurrentAirSpace().getArc().setEndAngle(endAngle);

    // Add the arc points to this space's Polygon.
    // TODO: don't use *hardcoded* 100 points for the discretization!
    getCurrentAirSpace().getPolygon().add(getCurrentAirSpace().getArc().toPolygon(100));
  }

  expression = "\\s*DC\\s+(.*)";
  if ( regex_match(line, matches, expression) )
  {
    //cout << "DEBUG: found DC record!" << endl;
    // Set circle center from stored state of the parser.
    getCurrentAirSpace().getCircle().setCenter(getCurrentCoordinate());

    // Set circle radius (in Nautical Miles) from what we've just read.
    string radiusNM;
    for (unsigned int i = 1; i < matches.size(); ++i)
    {
      radiusNM.assign(matches[i].first, matches[i].second);
    }
    getCurrentAirSpace().getCircle().setRadiusNM(atof(radiusNM.c_str()));
  }

}
