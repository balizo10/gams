/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "madara/knowledge_engine/Knowledge_Base.h"

// Platform-specific includes.
#include "platforms/comm/comm.h"
#include "platforms/platform.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

// Includes for HW simulation through VRep.
#include "platforms/v_rep/v-rep_sim_madara_variables.h"
#include "platforms/v_rep/comm_sim.h"

#include "utilities/Position.h"
#include "utilities/string_utils.h"

#include <string>
#include <cmath>
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macros, constants and enums.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the internal expressions.
enum VRepMadaraExpressionId 
{
    // Updated the command id.
    VE_UPDATE_COMMAND_ID, 
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Map of Madara expressions used when simulating the hardware.
static std::map<VRepMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

// The knowledge base used to simulate the hardware.
static Madara::Knowledge_Engine::Knowledge_Base*m_sim_knowledge;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge);
static void setupInternalHardwareKnowledgeBase(int id);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Overrides: init_platform().
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool platform_init()
{
    // The actual initialization is postponed until the setup_knowledge_base function is called,
    // since only then the id is received.
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Overrides: setup_knowledge_base().
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id, bool enableLogging)
{
    // Setup the internal, totally separate knowledge base to be used as a proxy for the hardware.
    setupInternalHardwareKnowledgeBase(id);

    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = comm_setup_knowledge_base(id, enableLogging);
    return knowledge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Overrides: cleanup_platform().
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool platform_cleanup()
{
    m_sim_knowledge->print_knowledge();

	// Cleanup the internal Madara platform.
	m_sim_knowledge->close_transport();
    m_sim_knowledge->clear();
    delete m_sim_knowledge;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up a knowledge base that will be used as a proxy to send comands to simulated hardware, and to receive
// sensed data from simulated sensors.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void setupInternalHardwareKnowledgeBase(int id)
{
    // Create the knowledge base.
    m_sim_knowledge = sim_comm_setup_knowledge_base(id, false);

    // Define Madara functions.
    compileExpressions(m_sim_knowledge);

    // Set the ID inside Madara.
    m_sim_knowledge->set (".id", (Madara::Knowledge_Record::Integer) id);

	// Indicate that we have not sent or received replied to commands yet. The first id sent will be 1.
    // NOTE: this is currently not being used for anything other than debugging. It could be used to fix a bug where
    // commands some times do not get sent for some reason to the Madara base in VRep, by checking if no "acks" have
    // beeen recieved from VRep.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MS_SIM_CMD_SENT_ID), 
                (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MS_SIM_CMD_RCVD_ID), 
                (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    m_expressions[VE_UPDATE_COMMAND_ID] = knowledge->compile(
        "("
			// Send the command id after increasing it. We first increase it so the first id sent is 1.
			"++" MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MS_SIM_CMD_SENT_ID";"
        ")"
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Movement Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool platform_init_control_functions()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_takeoff()
{
    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_TAKEOFF_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_land()
{
    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_LAND_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_up()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_down()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_left()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_right()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_forward()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_backward()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_to_location(double lat, double lon, double alt)
{
    // Set the arguments for this command. Note that we are intentionally ignoring altitude, as the 
    // simulation is doing that as of now.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_CMD_ARG("0")), lat);
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_CMD_ARG("1")), lon);

    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_MOVE_TO_GPS_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_move_to_altitude(double alt)
{
    // Set the arguments for this command. Note that we are intentionally ignoring altitude, as the 
    // simulation is doing that as of now.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_CMD_ARG("0")), alt);

    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_MOVE_TO_ALTITUDE_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_stop_movement()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets the thermal buffer to zero in Madara.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initialize_thermal_variables()
{
    std::stringstream emptyThermalBuffer;
	int x, y;
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
            if(! (x==0 && y==0))
            {
                emptyThermalBuffer << ',';
            }
			
            emptyThermalBuffer << '0';
		}
	}

    std::stringstream thermalBufferName;
    thermalBufferName << MS_SIM_DEVICES_PREFIX << "{" MV_MY_ID "}" << MV_SIM_THERMAL_BUFFER;
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(thermalBufferName.str()), emptyThermalBuffer.str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool platform_init_sensor_functions()
{
    initialize_thermal_variables();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The percentage of battery remaining.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
double platform_get_battery_remaining()
{
    // Because we are not running on batteries...
    return 100;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Loads information from the Madara thermal variables into the buffer given as a parameter.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_read_thermal(double buffer[8][8])
{
    int numRows = 8;
    int numColumns = 8;

    // Initialize the buffer with zeros.
    memset(buffer, 0, numRows*numColumns);

    // Get the thermal string from the knowledge base.
    std::stringstream thermalBufferName;
    thermalBufferName << MS_SIM_DEVICES_PREFIX << "{" MV_MY_ID "}" << MV_SIM_THERMAL_BUFFER;
    std::string thermalValues = m_sim_knowledge->get(m_sim_knowledge->expand_statement(thermalBufferName.str())).to_string();

	// Parse the thermal values.
	std::vector<std::string> thermalValueList = stringSplit(thermalValues, ',');
		
    // Get the thermals from the string, if any.
    if(thermalValueList.size() >= (unsigned) numRows*numColumns)
    {
        for(int row=0; row<numRows; row++)
        {
            // Loop over this row.
            for(int col=0; col<numColumns; col++)
            {
                // Get the current value from the parsed list.
                std::string currValue = std::string(thermalValueList[row*numColumns + col]);

                // Store the current value as a double in the buffer.
                buffer[row][col] = atof(currValue.c_str());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the GPS coordinates from the simulator and gives them back to the 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void platform_read_gps(struct madara_gps * ret)
{
    // Get the latitude and longitude that the simulator set for this drone, in variables with the sim prefix.
    double latitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_LATITUDE)).to_double();
    double longitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_LONGITUDE)).to_double();
	double altitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_ALTITUDE)).to_double();

    //std::cout << "Lat " << latitude << ", Long " << longitude << " from: " << std::string(MS_SIM_PREFIX MV_DEVICE_LAT("{"MV_MY_ID"}")) << std::endl;
    //m_sim_knowledge->print_knowledge();

    // Set the values in the return structure.
	ret->latitude = latitude;
	ret->longitude = longitude;
	ret->altitude = altitude;
	ret->num_sats = 10;             // Just because it should be really exact with the simulator.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the altitude from the ultrasound sensor.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
double platform_read_ultrasound()
{
    double currHeight = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_ALTITUDE)).to_double();
    return currHeight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the accuracy of the GPS for this platform, in meters.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
double platform_get_gps_accuracy()
{
    // Note that this has to match the value used in VRep to determine how close to a location the drone is.
    double gpsAccuracy = 0.3;
    return gpsAccuracy;
}
