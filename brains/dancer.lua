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

		--CIRCULAR 
		distance = 99
		tmpDistance = 0
		distanceVector = {0,0,0}
		separation = {0,0,0}
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.state=="dancerFree" or
			    neighbour.state=="dancerEngaged" or
			    neighbour.state=="dancerSpin")
			then
				distanceVector[1] = position.x - neighbour.position.x
				distanceVector[2] = position.y - neighbour.position.y
				distanceVector[3] = position.z - neighbour.position.z

				distance = math.sqrt(distanceVector[1]^2 + distanceVector[2]^2 + distanceVector[3]^2)

				separation[1] = separation[1] + distanceVector[1] / distance^2
				separation[2] = separation[2] + distanceVector[2] / distance^2
				separation[3] = separation[3] + distanceVector[3] / distance^2
			end
		end
	
		--NORMALIZE SEPARATION
		magnitude = math.sqrt(separation[1]^2 + separation[2]^2 + separation[3]^2)
		
		if (magnitude > 0)
		then
			separation[1] = separation[1] / magnitude
			separation[2] = separation[2] / magnitude
			separation[3] = separation[3] / magnitude
		end	
		
		--CIRCULAR FORCE
		direction = {}

		speed = math.sqrt(velocity.x^2+velocity.y^2+velocity.z^2)
		if (speed==0)
		then
			direction[1] = math.cos(agentID*math.pi/4)
			direction[2] = 0
			direction[3] = math.sin(agentID*math.pi/4)
		else
			direction[1] = velocity.x/speed
			direction[2] = velocity.y/speed
			direction[3] = velocity.z/speed
		end

		angle = (math.pi/400)

		circularForce = {0,0,0}
		circularForce[1] = direction[1]*math.cos(angle)-direction[3]*math.sin(angle);
		circularForce[3] = direction[1]*math.sin(angle)+direction[3]*math.cos(angle);

		--FORCE
		force = {}
		cw = 0.5
		sw = 0.5
		force[1] = circularForce[1]*cw + separation[1]*sw
		force[2] = circularForce[2]*cw + separation[2]*sw
		force[3] = circularForce[3]*cw + separation[3]*sw

		return force, {0,0,0}, strength, state, {}
	end
	
	stateAction.dancerFree = dancerFree


	function dancerEngaged(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		--PICK PARTNER
		distanceVector = {}
		distance= 99
		tmpDistance = 0
		neighbourCounter = 0
		partner = nil
		separation = {0,0,0}

		for key,neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			
			if (neighbour.state=="danceLeaderSwing" or neighbour.state=="danceLeaderWaitForSpin")
			then
				distanceVector[1] = neighbour.position.x - position.x
				distanceVector[2] = neighbour.position.y - position.y
				distanceVector[3] = neighbour.position.z - position.z

				tmpDistance = math.sqrt(distanceVector[1]^2 +
							distanceVector[2]^2 +
							distanceVector[3]^2)

				if (tmpDistance < distance)
				then
					partner = neighbour
				end
			end

			if (neighbour.state=="dancerFree" or
			    neighbour.state=="dancerEngaged" or
			    neighbour.state=="dancerSpin")
			then
				distanceVector[1] = position.x - neighbour.position.x
				distanceVector[2] = position.y - neighbour.position.y
				distanceVector[3] = position.z - neighbour.position.z

				distance = math.sqrt(distanceVector[1]^2 + distanceVector[2]^2 + distanceVector[3]^2)

				separation[1] = separation[1] + distanceVector[1] / distance^2
				separation[2] = separation[2] + distanceVector[2] / distance^2
				separation[3] = separation[3] + distanceVector[3] / distance^2
			end
		end
	
		--NORMALIZE SEPARATION
		magnitude = math.sqrt(separation[1]^2 + separation[2]^2 + separation[3]^2)
		
		if (magnitude > 0)
		then
			separation[1] = separation[1] / magnitude
			separation[2] = separation[2] / magnitude
			separation[3] = separation[3] / magnitude
		end	

		--HEADING
		heading = {}

		if (partner==nil)
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
				fw = 0
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
		fw = 0.02
		sw = 0.5
		force[1] = attractingForce[1]*aw + followingForce[1]*fw + separation[1]*sw 
		force[2] = attractingForce[2]*aw + followingForce[2]*fw + separation[2]*sw
		force[3] = attractingForce[3]*aw + followingForce[3]*fw + separation[3]*sw

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
				if (neighbour.state=="danceLeaderWaitForSpin")
				then
					tmpDistance = math.sqrt(neighbour.position.x^2 +
								neighbour.position.y^2 +
								neighbour.position.z^2)
					if (tmpDistance<distance)
					then
						partnerIndex = key
						distance = tmpDistance
					end
				end
			end
			
			if (partnerIndex==0)
			then
				return {0,0,0}, {0,0,0}, maxStrength, "dancerFree", {}
			else
				messages = {}
				messages[partnerIndex] = {}
				messages[partnerIndex][0] = "dance"
			
				return {0,0,0}, {0,0,0}, maxStrength, "dancerEngaged", messages
			end
		end

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
