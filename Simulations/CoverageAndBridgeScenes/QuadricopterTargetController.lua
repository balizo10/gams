--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require("Utils")
    
-- The speed defines how far the target moves, and therefore how fast the drone will follow.
TARGET_SPEED = 0.02        

-- This margin (in meters) indicates how close to a person we use to declare that we found it.
PERSON_FOUND_ERROR_MARGIN = 0.2

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation starts.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doInitialSetup()   
	-- Get my name
	g_mySuffix = simGetNameSuffix(nil)
    g_myDroneName = getDroneInfoFromSuffix(g_mySuffix)
	g_myDroneId = g_mySuffix + 1     -- Drone ids start from 0, while suffixes start from -1.

    -- Load the positions of people on the grid, so we will know when we find one.
    loadPeoplePositions()
    
    -- Control continuos movement.
    g_myTargetSetup = false
    g_myTargetx = 0
    g_myTargety = 0
    
    -- Setup the plugin to communicate to the network. Only do this once, for the first drone.
    if(g_myDroneId == 0) then
        simExtMadaraQuadrotorControlSetup(g_myDroneId)   
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation ends.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doCleanup()
    -- Stop the network plugin.
    if(g_myDroneId == 0) then    
        simExtMadaraQuadrotorControlCleanup()
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Load the people's locations, so we are able to check when we find one.
--/////////////////////////////////////////////////////////////////////////////////////////////
function loadPeoplePositions()
	g_numPeople = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfPeople')
	g_personCoords = {}
    
	local counter = 1
	for i=1, g_numPeople, 1 do
		if(i==1) then
			personHandle = simGetObjectHandle('Bill#')
		else
			personHandle = simGetObjectHandle('Bill#' .. (i-2))
		end

        local billposition = simGetObjectPosition(personHandle, -1)
		g_personCoords[counter] = billposition[1]
		g_personCoords[counter+1] = billposition[2]
		--simAddStatusbarMessage('Person ' .. counter .. ' : ' .. g_personCoords[counter] .. ', ' .. counter+1 .. ' : '..g_personCoords[counter+1])
		counter = counter + 2
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()   
    -- Sensors simulation.
    simulateSensors()

    -- Movement simulation.
    simulateMovementCommands()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Gets information about position and people found to send back to the drone.
--/////////////////////////////////////////////////////////////////////////////////////////////
function simulateSensors()
    -- "GPS": Update our current position to the real drone.
    updateDronePosition()
    
    -- "Thermal": Check if we have found a person to stop on top of it (only if we are patrolling).
    lookForPersonBelow()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Updates the position of the drone to the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function updateDronePosition()
	local droneName, dronePosition = getDroneInfoFromId(g_myDroneId)
	if(dronePosition ~= nil) then
		simExtMadaraQuadrotorControlUpdateStatus(g_myDroneId, dronePosition[1], dronePosition[2], dronePosition[3])
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check if we have found a person to stop on top of it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function lookForPersonBelow()
    -- Get my drone position.
    local droneName, dronePos = getDroneInfoFromSuffix(g_mySuffix)

    -- Check if we found a person, to stop.
    local margin = PERSON_FOUND_ERROR_MARGIN
    local counter = 1
    for i=1, g_numPeople, 1 do
        if( (dronePos[1] >= g_personCoords[counter] - margin) and (dronePos[1] <= g_personCoords[counter] + margin) ) then
            if((dronePos[2] >= g_personCoords[counter + 1] - margin) and (dronePos[2] <= g_personCoords[counter + 1] + margin)) then
                -- Notifiy our shared memory that a person was found, and that I was the one to find it.
                local sourceSuffix, sourceName = simGetNameSuffix(nil)
                simSetScriptSimulationParameter(sim_handle_main_script, 'personFound', 'true')
                simSetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound', sourceSuffix)
                simSetScriptSimulationParameter(sim_handle_main_script, 'personFoundId', i)
                simAddStatusbarMessage('Drone with suffix ' .. sourceSuffix .. ' found person ' .. i .. '!')
            end
        end
        counter = counter + 2
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if there is movement to be done, and do it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function simulateMovementCommands()
    local myNewX = nil
    local myNewY = nil
    local myNewAlt = nil
    local command = ''
    
    -- We check if there is a new command.
    command, myNewX, myNewY, myNewAlt = simExtMadaraQuadrotorControlGetNewCmd(g_myDroneId)
    if(not (command == nil)) then
        --simAddStatusbarMessage('Command: '..command)    
        local isGoToCmd = simExtMadaraQuadrotorControlIsGoToCmd(command) 
        if(isGoToCmd == true) then
            -- If we have to move to a new location, move our target there so the drone will follow it. Altitude is ignored.
            g_myTargetSetup = true
            --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') In Lua, target position found: ' .. myNewX .. ',' .. myNewY)
            g_myTargetx = myNewX
            g_myTargety = myNewY
        end
    end
    
    -- Move if required.
    if(g_myTargetSetup) then
        moveTargetToPosition(g_myTargetx, g_myTargety)
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function moveTargetToPosition(newPositionX, newPositionY)
    -- Get the current position of the target.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)
    
    local speed = TARGET_SPEED
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Curr target position' .. droneTargetPosition[1] .. ',' .. droneTargetPosition[2]..':'..speed)
    
    -- Check if the target is already at the required X position. If not, define that the
    -- new X position is our current plus the speed we move at in the correct direction.
    if(droneTargetPosition[1] > newPositionX) then        
        droneTargetPosition[1] = droneTargetPosition[1] - speed
    else
        droneTargetPosition[1] = droneTargetPosition[1] + speed
    end

    -- Check if the target is already at the required Y position. If not, define that the
    -- new Y position is our current plus the speed we move at in the correct direction.    
    if(droneTargetPosition[2] > newPositionY) then
        droneTargetPosition[2] = droneTargetPosition[2] - speed
    else
        droneTargetPosition[2] = droneTargetPosition[2] + speed
    end
       
    -- Move the target to a new position, so the drone will follow it there.
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Moving target to position' .. droneTargetPosition[1] .. ',' .. droneTargetPosition[2])
    simSetObjectPosition(droneTargetHandle, -1, droneTargetPosition)
end