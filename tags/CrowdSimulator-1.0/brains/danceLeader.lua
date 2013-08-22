function danceLeader (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

	stateAction = {}

	--STRENGTH
	function danceLeaderSearch (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
		distance = 99
		tmpDistance = 0
		partner = nil
		distanceVector = {0,0,0}
		separation = {0,0,0}
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.state == "dancerFree")
			then
				tmpDistance = math.sqrt(neighbour.position.x^2 + 
							neighbour.position.y^2 +
						     	neighbour.position.z^2)
				if (tmpDistance < distance)
				then
					partner = neighbour
					partnerIndex = key
					tmpDistance = distance
				end
			end
			
			if (neighbour.state=="danceLeaderSearch" or
			    neighbour.state=="danceLeaderSwing" or
			    neighbour.state=="danceLeaderWaitForSpin")
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

		if (partner)
		then
			messages = {}
			messages[partnerIndex] = {}
			messages[partnerIndex][0] = "dance"
			return {0,0,0}, {0,0,0}, strength, "danceLeaderSwing", messages
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
	
	stateAction.danceLeaderSearch = danceLeaderSearch

	function danceLeaderSwing (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		
		-- PICK PARTNER
		neighbourCounter = 0
		distance = 99
		tmpDistance = 0
		partner = nil
		for key,neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			
			if (neighbour.state=="dancerEngaged")
			then
				tmpDistance = math.sqrt(neighbour.position.x^2 +
					                neighbour.position.y^2 +
						        neighbour.position.z^2)
				if (tmpDistance < distance)
				then
					partner = neighbour
					tmpDistance = distance
				end
			end
			
		end
		

		if (partner==nil)
		then
			return {0,0,0}, {0,0,0}, strength,"danceLeaderSearch",{}
		end
		
		-- HEADING
		heading = {}

		heading[1] = partner.position.x - position.x
		heading[2] = partner.position.y - position.y
		heading[3] = partner.position.z - position.z

		magnitude = math.sqrt(heading[1]^2 + heading[2]^2 + heading[3]^2)
		
		heading[1] = heading[1] / magnitude
		heading[2] = heading[2] / magnitude
		heading[3] = heading[3] / magnitude
		
		-- CHECK FORCE TO SPIN
		if (strength < 0)
		then
			--THIS MIGHT PROVOKE SIGSEGV
			messages = {}
			messages[neighbourCounter] = {}
			messages[neighbourCounter][0] = "spin"
			return {0,0,0}, heading, maxStrength, "danceLeaderWaitForSpin", messages
		end


		-- DANCING FORCE
		dancingForce = {}

		-- normal to the heading vector to create the effect of swing
		normalHeading = {}
		normalHeading[1] = -heading[3]
		normalHeading[2] = heading[2]
		normalHeading[3] = heading[1]

		-- ALTERNATING SWING DIRECTION
		swingsNumber = 15
		swingIndex = math.floor(strength/maxStrength * swingsNumber)

		if(swingIndex%2 == 0)
		then
			--swing to right
			dancingForce[1] = normalHeading[1]
			dancingForce[2] = normalHeading[2]
			dancingForce[3] = normalHeading[3]
		else
			--swing to left
			dancingForce[1] = -normalHeading[1]
			dancingForce[2] = -normalHeading[2]
			dancingForce[3] = -normalHeading[3]
		end

		-- ATTRACTING FORCE
		attractingForce = heading


		-- SEPARATION FORCE
		separation = {0,0,0}
		distanceVector = {}
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.state=="danceLeaderSwing" or neighbour.state=="danceLeaderWaitForSpin")
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

		--normalize
		magnitude = math.sqrt(separation[1]^2 + separation[2]^2 + separation[3]^2)
		
		if (magnitude > 0)
		then
			separation[1] = separation[1] / magnitude
			separation[2] = separation[2] / magnitude
			separation[3] = separation[3] / magnitude
		end	


		-- SYNTHESIS OF ALL THE FORCES
		force = {}
		dw = 0.03
		aw = 0.05
		sw = 0.5
		force[1] = dancingForce[1]*dw + attractingForce[1]*aw + separation[1]*sw
		force[2] = dancingForce[2]*dw + attractingForce[2]*aw + separation[2]*sw
		force[3] = dancingForce[3]*dw + attractingForce[3]*aw + separation[3]*sw

		-- STRENGTH
		decrement = 0.001
		strength = strength - decrement

		return force, heading, strength, "danceLeaderSwing", {}
	end

	stateAction.danceLeaderSwing = danceLeaderSwing


	function danceLeaderWaitForSpin(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		-- PICK PARTNER
		neighbourCounter = 0
		distance = 99
		tmpDistance = 0
		partner = nil
		for key,neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			
			if (neighbour.state=="dancerSpin" or neighbour.state=="dancerEngaged")
			then
				tmpDistance = math.sqrt(neighbour.position.x^2 +
					                neighbour.position.y^2 +
						        neighbour.position.z^2)
				if (tmpDistance < distance)
				then
					partner = neighbour
					tmpDistance = distance
				end
			end
			
		end
		
		if (partner==nil)
		then
			return {0,0,0}, {0,0,0}, strength,"danceLeaderSearch",{}
		end
		
		-- HEADING
		heading = {0,0,0}

		heading[1] = partner.position.x - position.x
		heading[2] = partner.position.y - position.y
		heading[3] = partner.position.z - position.z

		magnitude = math.sqrt(heading[1]^2 + heading[2]^2 + heading[3]^2)
		
		heading[1] = heading[1] / magnitude
		heading[2] = heading[2] / magnitude
		heading[3] = heading[3] / magnitude

		--CHEK MESSAGES TO CHANGE TO SWING
		for key,message in pairs(inbox)
		do
			if (message.label=="dance")
			then
				return {0,0,0}, heading, strength, "danceLeaderSwing", {}
			end
		end

		return {0,0,0}, heading, strength, state, {}
		
	end

	stateAction.danceLeaderWaitForSpin = danceLeaderWaitForSpin


	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("DancerLeader behaviour: WARNING: unknown state "..state)
		return stateAction.danceLeaderSearch(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end

end
