/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * InsideOutAreaCoverage.cpp - Declares the structures and methods for the
 * algorithm to perform an inside out area search
 ******************************************************************************/

#include "InsideOutAreaCoverage.h"

#include "utilities/Position.h"
#include "utilities/CommonMadaraVariables.h"
#include "utilities/gps_utils.h"

#include <iostream>
using std::cerr;
using std::endl;

using namespace SMASH::Utilities;
using namespace SMASH::AreaCoverage;

// Constructor
InsideOutAreaCoverage::InsideOutAreaCoverage(double delta, direction_t heading,
    bool clockwise) : AreaCoverage(), m_iteration(2), m_clockwise(clockwise),
    m_heading(heading), m_nsMultiplier(1.0), m_ewMultiplier(1.0)
{
   m_delta = delta * DEGREES_PER_METER;
}

// Destructor
InsideOutAreaCoverage::~InsideOutAreaCoverage() {}

// Initialize the area for the drone
Region* InsideOutAreaCoverage::initialize(const Region& grid, int deviceIdx, 
    int numDrones)
{
    printf("Initializing inside out area coverage algorithm.\n");
    // trick to get initial heading correct
    switch(m_heading)
    {
        case NORTH:
            m_heading = m_clockwise ? WEST : EAST;
            break;
        case EAST:
            m_heading = m_clockwise ? NORTH : SOUTH;
            break;
        case SOUTH:
            m_heading = m_clockwise ? EAST : WEST;
            break;
        case WEST:
            m_heading = m_clockwise ? SOUTH : NORTH;
            break;
        default:
            // If for some weird reason we end up here, we just pick a particlar heading.
            cerr << "Invalid direction, picking a default one." << endl;
            m_heading = m_clockwise ? WEST : EAST;
    }

  // find search region
  if(numDrones == 1)
    m_cellToSearch = new Region(grid);
  else
    m_cellToSearch = calculateCellToSearch(deviceIdx, grid, numDrones);

    // set multipliers
    double deltaLong = m_cellToSearch->southEast.longitude - m_cellToSearch->northWest.longitude;
    double deltaLat = m_cellToSearch->northWest.latitude - m_cellToSearch->southEast.latitude;
    if(deltaLong > deltaLat)
        m_ewMultiplier = deltaLong / deltaLat;
    else
        m_nsMultiplier = deltaLat / deltaLong;

  return m_cellToSearch;
}

// Calculates the next location to move to, assuming we have reached our
// current target.
Position InsideOutAreaCoverage::getNextTargetLocation()
{
    if(!m_started) // start in center of the region
    {
        m_targetLocation.latitude = (m_cellToSearch->southEast.latitude +
                                     m_cellToSearch->northWest.latitude) / 2;
        m_targetLocation.longitude = (m_cellToSearch->southEast.longitude +
                                      m_cellToSearch->northWest.longitude) / 2;
        m_started = true;
    }
    else
    {
        // update direction
        switch(m_heading)
        {
            case NORTH:
                m_heading = m_clockwise ? EAST : WEST;
                break;
            case EAST:
                m_heading = m_clockwise ? SOUTH : NORTH;
                break;
            case SOUTH:
                m_heading = m_clockwise ? WEST : EAST;
                break;
            case WEST:
                m_heading = m_clockwise ? NORTH : SOUTH;
                break;
            default:
                // If for some weird reason we end up here, we just pick any heading.
                cerr << "Invalid direction" << endl;
                m_heading = m_clockwise ? WEST : EAST;
        }

        // find new target
        switch(m_heading)
        {
            case NORTH:
                m_targetLocation.latitude += (m_iteration / 2) * m_delta * m_nsMultiplier;
                break;
            case EAST:
                m_targetLocation.longitude += (m_iteration / 2) * m_delta * m_ewMultiplier;
                break;
            case SOUTH:
                m_targetLocation.latitude -= (m_iteration / 2) * m_delta * m_nsMultiplier;
                break;
            case WEST:
                m_targetLocation.longitude -= (m_iteration / 2) * m_delta * m_ewMultiplier;
                break;
            default:
                // If for some weird reason we end up here, we just pick any heading.
                cerr << "Invalid direction" << endl;
                m_targetLocation.latitude += (m_iteration / 2) * m_delta * m_nsMultiplier;
        }

        // go to next iteration
        ++m_iteration;
    }

    return m_targetLocation;
}


// Query if algorithm has reached final target
// @return    false, default to algorithm never finishes
bool InsideOutAreaCoverage::isTargetingFinalWaypoint()
{
    return !m_cellToSearch->contains(m_targetLocation);
}

// Determines the next area coverage that should be used
// TODO: Fix this to do OutsideInCoverage
AreaCoverage* InsideOutAreaCoverage::getNextCoverage()
{
        return new InsideOutAreaCoverage(m_delta, m_heading, m_clockwise);
}