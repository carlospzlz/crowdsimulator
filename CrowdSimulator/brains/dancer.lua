function dancer (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

	stateAction = {}

	function dancerFree (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
		--LISTENING FOR PETITIONS
		for key,message in pairs(inbox)
		do
			if (message.label == "dance")
			then
				return {0,0,0}, {0,0,0}, strength, "dancerEngaged",{}
			end
		end

		--MOVING LOST
		speed = math.sqrt(velocity.x*velocity.x+velocity.y*velocity.y+velocity.z*velocity.z)

		direction = {}
		if (speed==0)
		then
			direction.x = math.cos(agentID*math.pi/4)
			direction.y = 0
			direction.z = math.sin(agentID*math.pi/4)
		else
			direction.x = velocity.x/speed
			direction.y = velocity.y/speed
			direction.z = velocity.z/speed
		end

		angle = (math.pi/400)

		direction.x = direction.x*math.cos(angle)-direction.z*math.sin(angle);
		direction.z = direction.x*math.sin(angle)+direction.z*math.cos(angle);

		--FORCE
		force = {}
		tw = 0.5
		force[1] = direction.x*tw
		force[2] = 0
		force[3] = direction.z*tw

		return force, {0,0,0}, strength, "dancerFree", {}
	end
	
	stateAction.dancerFree = dancerFree


	function dancerEngaged(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		distanceVector = {}
		distance= 99
		tmpDistance = 0
		partner = {}
		neighbourCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			
			distanceVector[1] = neighbour.position.x - position.x
			distanceVector[2] = neighbour.position.y - position.y
			distanceVector[3] = neighbour.position.z - position.z

			tmpDistance = math.sqrt(distanceVector[1]^2 +
						distanceVector[2]^2 +
						distanceVector[3]^2)

			if (tmpDistance < distance)
			then
				partner = neighbours[neighbourCounter]
			end
		end

		--HEADING
		heading = {}

		if (neighbourCounter==0)
		then
			return {0,0,0}, {0,0,0}, strength, "dancerFree", {}
		end

		heading[1] = partner.position.x - position.x
		heading[2] = partner.position.y - position.y
		heading[3] = partner.position.z - position.z

		--normalize
		magnitude = math.sqrt(heading[1]^2 + heading[2]^2 + heading[3]^2)

		heading[1] = heading[1] / magnitude
		heading[2] = heading[2] / magnitude
		heading[3] = heading[3] / magnitude

		-- CHECKING FOR SPIN
		for key,message in pairs(inbox)
		do
			if (message.label=="spin")
			then
				force = {0,0,0}
				fw = 2
				force[1] = -heading[1]*fw
				force[2] = -heading[2]*fw
				force[3] = -heading[3]*fw
				return force, heading, strength, "dancerSpin", {}
			end
		end

		-- PASSIVE DANCING FORCE

		-- ATTRACTING
		attractingForce = heading

		-- FOLLOWING FORCE
		-- based on the velocity
		followingForce = {0,0,0}

		partnerSpeed = math.sqrt(partner.velocity.x^2 + partner.velocity.y^2 + partner.velocity.z^2)

		if (partnerSpeed>0)
		then
			followingForce[1] = partner.velocity.x / partnerSpeed
			followingForce[2] = partner.velocity.y / partnerSpeed
			followingForce[3] = partner.velocity.z / partnerSpeed
		end

		--SYNTHESIS OF ALL THE FORCES
		force = {}
		aw = 0.05
		fw = 0.03
		force[1] = attractingForce[1]*aw + followingForce[1]*fw
		force[2] = attractingForce[2]*aw + followingForce[2]*fw
		force[3] = attractingForce[3]*aw + followingForce[3]*fw

		--print(attractingForce[1], attractingForce[2], attractingForce[3])
		--print(followingForce[1], followingForce[2], followingForce[3])

		return force, heading, strength, state, {}
	end

	stateAction.dancerEngaged = dancerEngaged

	function dancerSpin(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)

		--CHECKING STRENGTH TO STOP
		if (strength < 0)
		then
			-- SEND TO PARTNER DANCE MESSAGE
			partnerIndex = 0
			distance = 99
			tmpDistance = 0
			for key,neighbour in pairs(neighbours)
			do
				tmpDistance = math.sqrt(neighbour.position.x^2 +
							neighbour.position.y^2 +
							neighbour.position.z^2)
				if (tmpDistance<distance)
				then
					partnerIndex = key
					distance = tmpDistance
				end
			end

			messages = {}
			messages[partnerIndex] = {}
			messages[partnerIndex][0] = "dance"
			
			return {0,0,0}, {0,0,0}, maxStrength, "dancerEngaged", messages
		end
		
		repulsionForce = {0,0,0}
		repulsionForce[1] = (position.x - partner.position.x) / distance
		repulsionForce[2] = (position.y - partner.position.y) / distance
		repulsionForce[3] = (position.z - partner.position.z) / distance

		-- HEADING 
		angleScale = 10;
		angle = math.acos(strength)*angleScale
		
		heading[1] = math.cos(angle)
		heading[2] = 0
		heading[3] = math.sin(angle)


		-- FORCE
		force = {0,0,0}

		strength = strength - 0.01

		return {0,0,0}, heading, strength, state, {}

	end

	stateAction.dancerSpin = dancerSpin


	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Dancer behaviour: WARNING: unknown state "..state)
		return stateAction.dancerFree(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end

end
