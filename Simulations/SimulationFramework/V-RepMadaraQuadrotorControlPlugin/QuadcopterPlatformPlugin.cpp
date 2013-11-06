/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* QuadcopterPlatformPlugin.cpp - Specifies a QuadcopterPlatform plugin for VRep.
*********************************************************************/

#include "QuadcopterPlatformPlugin.h"
#include "v_repLib.h"
#include "PluginUtils.h"
#include "SimUtils.h"
#include "ThermalSensor.h"
#include "utilities/gps_utils.h"
#include "utilities/string_utils.h"
#include <string>
#include <vector>

using namespace SMASHSim;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// We have to create an actual object of this type so that the plugin DLL entry points will have access to it.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VREP::ISimplePlugin* createPlugin()
{
  QuadcopterPlatformPlugin* plugin = new QuadcopterPlatformPlugin();
  return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is initialized.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::initialize(int suffix)
{
  simAddStatusbarMessage("QuadcopterPlatformPlugin::initialize: Initializing Quadcopter Platform.");

  // The object"s suffixes will start at 1. The ids start at 0.
  int droneId = suffix + 1;

  // Setup Madara for communications.
  // Only set this up once (this has to be checked since this void will be called by many drones.
  if(m_madaraController == NULL)
  {
    m_madaraController = new MadaraQuadrotorControl(droneId);
  }
  else
  {
    m_madaraController->incrementNumDrones();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is closing.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::cleanup(int suffix)
{
  simAddStatusbarMessage("QuadcopterPlatformPlugin::cleanup: Cleaning up Quadcopter Platform.");

  if(m_madaraController != NULL)
  {
    m_madaraController->decrementNumDrones();

    // Try to terminate the controller. If unsuccesful, it is beacuse other drones are still referencing it.
    if(m_madaraController->terminate())
    {
      simAddStatusbarMessage("QuadcopterPlatformPlugin: removing controller");
      delete m_madaraController;
      m_madaraController = NULL;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called in each step of the simulation.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::executeStep(int suffix)
{
  int droneId = suffix + 1;
  simulateSensors(droneId);
  simulateMovement(droneId);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Returns a textual ID of this plugin.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string QuadcopterPlatformPlugin::getId()
{
  return "QuadcopterPlatform";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Gets data from the simulated sensors.
///////////////////////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::simulateSensors(int droneId)
{
  // "GPS": Update our current position to the real drone.
  updateDronePosition(droneId);

  // "Thermal": Check if we have found a person to stop on top of it (only if we are patrolling).
  //updateThermals(droneId);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::updateDronePosition(int droneId)
{
  Location droneLocation = SimUtils::getObjectPositionInDegrees(SimUtils::getDroneName(droneId));
  m_madaraController->updateQuadrotorPosition(droneId, droneLocation);
}


///////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::updateThermals(int droneId)
{
  // Set the thermal buffer.
  ThermalSensor tSensor;
  std::string thermalBuffer = tSensor.getThermalBuffer(droneId);
  m_madaraController->setNewThermalScan(droneId, thermalBuffer, ThermalSensor::THERMAL_BUFFER_HEIGHT, ThermalSensor::THERMAL_BUFFER_WIDTH);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Moves the drone in the simulation.
///////////////////////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::simulateMovement(int droneId)
{

}
