function danceLeader (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

	stateAction = {}

	--STRENGTH
	function danceLeaderSearch (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
		partner = {}
		neighbourCounter = 0
		distance = 99
		tmpDistance = 0
		foundPartner = false
		for key,neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			if (neighbour.state == "dancerFree")
			then
				tmpDistance = math.sqrt(neighbour.position.x^2 + 
							neighbour.position.y^2 +
						     	neighbour.position.z^2)
				if (tmpDistance < distance)
				then
					partner[1] = neighbour[1]
					partner[2] = neighbour[2]
					partner[3] = neighbour[3]

					tmpDistance = distance
					foundPartner = true
				end
			end
		end

		if (foundPartner)
		then
			messages = {}
			messages[neighbourCounter] = {}
			messages[neighbourCounter][0] = "dance"
			return {0,0,0}, {0,0,0}, strength, "danceLeaderSwing", messages
		end

		speed = math.sqrt(velocity.x^2+velocity.y^2+velocity.z^2)

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
		force[1] = direction.x*strength*tw
		force[2] = 0
		force[3] = direction.z*strength*tw

		return force, {0,0,0}, strength, state, {}
	end
	
	stateAction.danceLeaderSearch = danceLeaderSearch

	function danceLeaderSwing (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		
		-- PICK PARTNER
		partner = {}
		neighbourCounter = 0
		distance = 99
		tmpDistance = 0
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
		
		if (neighbourCounter==0)
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
			if (neighbour.state=="danceLeaderSwing" || neighbour.state=="danceLeaderWaitForSpin")
			then
				distanceVector[1] = neighbour.position.x - position.x
				distanceVector[2] = neighbour.position.y - position.y
				distanceVector[3] = neighbour.position.z - position.z

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
		dw = 0.02
		aw = 0.05
		sw = 0
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
		partner = {}
		neighbourCounter = 0
		distance = 99
		tmpDistance = 0
		for key,neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			
			if (neighbour.state="dancerSpin")
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
		
		if (neighbourCounter==0)
		then
			return {0,0,0}, {0,0,0}, strength,"danceLeaderSearch",{}
		
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
