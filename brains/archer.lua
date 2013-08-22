function archer (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	--SHOOTER
	stateAction = {}
	
	maxSpeed = 1
	
	shootMaxDistance = 15
	shootMinDistance = 5
	
	superAttackW = 3
	superAttackY = 3


	--SHOOTERFLOCKFORCE AUXILIAR FUNCTION
	function archerFlockForce(position,attributes,neighbours,wc,ws,wa)

		-- FLOCKING BEHAVIOUR
		positionSum = {0,0,0}
		separation = {0,0,0}
		averageHeading = {0,0,0}
		neighbourSpeed = 0

		distanceVector = {}
		distance = 0

		counter = 0
		targetsCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			if (attributes.flock==neighbour.attributes.flock)
			then
				positionSum[1] = positionSum[1] + neighbour.position.x
				positionSum[2] = positionSum[2] + neighbour.position.y
				positionSum[3] = positionSum[3] + neighbour.position.z
			
				distanceVector[1] = position.x - neighbour.position.x
				distanceVector[2] = position.y - neighbour.position.y
				distanceVector[3] = position.z - neighbour.position.z
				distance = math.sqrt(distanceVector[1]^2 + distanceVector[2]^2 + distanceVector[3]^2)
			
				--separation normalized and weighted, that's the reason of ^2
				if (distance > 0)
				then
					separation[1] = separation[1] + distanceVector[1]/distance^2
					separation[2] = separation[2] + distanceVector[2]/distance^2
					separation[3] = separation[3] + distanceVector[3]/distance^2
				end
				
				neighbourSpeed = math.sqrt(neighbour.velocity.x^2+neighbour.velocity.y^2+neighbour.velocity.z^2)
				if (neighbourSpeed > 0)
				then
					averageHeading[1] = averageHeading[1] + neighbour.velocity.x / neighbourSpeed
					averageHeading[2] = averageHeading[2] + neighbour.velocity.y / neighbourSpeed
					averageHeading[3] = averageHeading[3] + neighbour.velocity.z / neighbourSpeed
				end
				counter = counter+1
			end	
		end

		alignment = {0,0,0}
		cohesion = {0,0,0}
		if (counter>0)
		then
			alignment[1] = averageHeading[1] / counter - velocity.x
			alignment[2] = averageHeading[2] / counter - velocity.y
			alignment[3] = averageHeading[3] / counter - velocity.z

			cohesion[1] = (positionSum[1] / counter - position.x) - velocity.x
			cohesion[2] = (positionSum[2] / counter - position.y) - velocity.y
			cohesion[3] = (positionSum[3] / counter - position.z) - velocity.z
		end

		--Weights for the flocking force
		flockForce = {}
		--wc = 1
		--ws = 1
		--wa = 1
		flockForce[1] = cohesion[1]*wc + separation[1]*ws + alignment[1]*wa
		flockForce[2] = cohesion[2]*wc + separation[2]*ws + alignment[2]*wa
		flockForce[3] = cohesion[3]*wc + separation[3]*ws + alignment[3]*wa

		--normalize
		magnitude = math.sqrt(flockForce[1]^2+flockForce[2]^2+flockForce[3]^2)
		if (magnitude>0)
		then
			flockForce[1] = flockForce[1] / magnitude
			flockForce[2] = flockForce[2] / magnitude
			flockForce[3] = flockForce[3] / magnitude
		end
	
	return flockForce
	end

	--SHOOTSTATE
	function archerShoot(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		--CHECKING IF THERES IS CHANGE OF STATE
		targetsCounter = 0
		for key, neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="warriorDead" and 
			    neighbour.state~="archerDead" and
			    neighbour.state~="trollDead" and
			    neighbour.state~="captainDead")
			then
				targetsCounter = targetsCounter + 1
			end
		end
	
		if (targetsCounter==0)
		then
			return {0,0,0}, {0,0,0}, strength, "archerRun", {}
		end

		--CHECK TROLLSUPERATTACK
		for key,message in pairs(inbox)
		do
			if (message.label=="superattack")
			then
				attackForce = {0,0,0}
				
				attackForce[1] = position.x - message.position.x
				attackForce[3] = position.z - message.position.z

				attackForce[1] = attackForce[1] * superAttackW
				attackForce[2] = superAttackY * superAttackW
				attackForce[3] = attackForce[3] * superAttackW

				return attackForce, {0,0,0}, strength, "archerFlying", {}
			end	
		end

		--SHOOT
		closestTargetPosition = {0,0,0}
		targetIndex = 0
		neighbourCounter = 0
		distance = 99

		for key, neighbour in pairs(neighbours)
		do
			neighbourCounter = neighbourCounter + 1
			
			if (neighbour.attributes.army~=attributes.army and 
			    neighbour.state~="warriorDead" and
			    neighbour.state~="archerDead" and
			    neighbour.state~="trollDead")
			then
				tmpDistance = math.sqrt( (neighbour.position.x-position.x)^2 +
							 (neighbour.position.y-position.y)^2 +
							 (neighbour.position.z-position.z)^2 )
				if (tmpDistance<distance)
				then
					closestTargetPosition[1]= neighbour.position.x
					closestTargetPosition[2]= neighbour.position.y
					closestTargetPosition[3]= neighbour.position.z

					distance = tmpDistance
					targetIndex = neighbourCounter
				end
			end
		end
	
		if (distance<shootMinDistance)
		then
			return {0,0,0}, {0,0,0}, strength, "archerGoBack", {}
		end

		messages = {}
		if (targetIndex~=0)
		then
			messages[targetIndex] = {}
			messages[targetIndex][0] = "arrow"
		end
		
		heading = {0,0,0}
		if (targetsCounter > 0)
		then
			heading[1] = (closestTargetPosition[1] - position.x)
			heading[2] = (closestTargetPosition[2] - position.y)
			heading[3] = (closestTargetPosition[3] - position.z)
			
			magnitude = math.sqrt(heading[1]^2+heading[2]^2+heading[3]^2)
			heading[1] = heading[1] / magnitude
			heading[2] = heading[2] / magnitude
			heading[3] = heading[3] / magnitude
		end

		force = {0,0,0}
		if (distance>shootMaxDistance)
		then
			force[1] = heading[1]
			force[2] = heading[2]
			force[3] = heading[3]
			
			tw = 0.1
			force[1] = force[1]*strength*tw
			force[2] = force[2]*strength*tw
			force[3] = force[3]*strength*tw
		end

		--MAKING THE FORCE NIL FOR MAXSPEED
		speed = math.sqrt(velocity.x^2 + velocity.y^2 + velocity.z^2)
		if (speed > maxSpeed)
		then
			force = {0,0,0}
		end

		return force, heading, strength, state, messages

	end
	
	stateAction.archerShoot = archerShoot

	--DEFEND STATE
	function archerGoBack(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
		-- CHECK FOR THE CHANGE OF STATE
		targetsCounter = 0
		neighboursCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="warriorDead" and
			    neighbour.state~="archerDead" and
			    neighbour.state~="trollDead")
			then
				targetsCounter = targetsCounter + 1
			end
		end
		
		if (targetsCounter == 0)
		then
			return {0,0,0}, {0,0,0}, strength, "archerRun", {}
		end

		--CHECK TROLLSUPERATTACK
		for key,message in pairs(inbox)
		do
			if (message.label=="superattack")
			then
				attackForce = {0,0,0}
				
				attackForce[1] = position.x - message.position.x
				attackForce[3] = position.z - message.position.z

				attackForce[1] = attackForce[1] * superAttackW
				attackForce[2] = superAttackY * superAttackW
				attackForce[3] = attackForce[3] * superAttackW

				return attackForce, {0,0,0}, strength, "archerFlying", {}
			end	
		end

		closestTargetPosition = {0,0,0}
		distance = 99
		tmpDistance = 0

		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and 
			    neighbour.state~="warriorDead" and
			    neighbour.state~="archerDead" and
			    neighbour.state~="trollDead")
			then
				tmpDistance = math.sqrt( (neighbour.position.x-position.x)^2 +
							 (neighbour.position.y-position.y)^2 +
							 (neighbour.position.z-position.z)^2 )
				if (tmpDistance<distance)
				then
					closestTargetPosition[1]= neighbour.position.x
					closestTargetPosition[2]= neighbour.position.y
					closestTargetPosition[3]= neighbour.position.z

					distance = tmpDistance
				end
			end
		end

		if (distance>shootMaxDistance)
		then
			return {0,0,0}, {0,0,0}, strength, "archerShoot", {}
		end


		force = {}
		force[1] = position.x - closestTargetPosition[1]
		force[2] = position.y - closestTargetPosition[2]
		force[3] = position.z - closestTargetPosition[3]

		force[1] = force[1] / distance
		force[2] = force[2] / distance
		force[3] = force[3] / distance

		--GOINGBACK FORCE
		tw = 0.1
		force[1] = force[1] * strength * tw
		force[2] = force[2] * strength * tw
		force[3] = force[3] * strength * tw

		--MAKING THE FORCE NIL FOR MAXSPEED
		speed = math.sqrt(velocity.x^2 + velocity.y^2 + velocity.z^2)
		if (speed > maxSpeed)
		then
			force = {0,0,0}
		end
		
		return force, {0,0,0}, strength, state, {}

	end

	stateAction.archerGoBack = archerGoBack
	
	
	--RUN STATE
	function archerRun(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		-- CHECKING CHANGE OF STATE
		targetsCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="warriorDead" and
			    neighbour.state~="archerDead" and
			    neighbour.state~="trollDead")
			then
				targetsCounter = targetsCounter + 1
			end
		end
		
		--print(targetsCounter)

		if (targetsCounter>0)
		then
			return {0,0,0}, {0,0,0}, strength, "archerShoot", {}
		end
	
		--CHECK TROLLSUPERATTACK
		for key,message in pairs(inbox)
		do
			if (message.label=="superattack")
			then
				attackForce = {0,0,0}
				
				attackForce[1] = position.x - message.position.x
				attackForce[3] = position.z - message.position.z

				attackForce[1] = attackForce[1] * superAttackW
				attackForce[2] = superAttackY * superAttackW
				attackForce[3] = attackForce[3] * superAttackW

				return attackForce, {0,0,0}, strength, "archerFlying", {}
			end	
		end


		--FLOCK BEHAVIOUR
		flockForce = archerFlockForce(position,attributes,neighbours,0.05,0.1,0.05)

		--NO FLOCKING		
		force = {0,0,-1}
		magnitude = math.sqrt(flockForce[1]^2+flockForce[2]^2+flockForce[3]^2)
		if (magnitude~=0)
		then
			force = flockForce
			--THE FORCE DEPENDS ON THE STRENGTH
			tw = 0.01
			force[1] = force[1] * tw * strength
			force[2] = force[2] * tw * strength
			force[3] = force[3] * tw * strength
		end

		--MAKING THE FORCE NIL FOR MAXSPEED
		speed = math.sqrt(velocity.x^2 + velocity.y^2 + velocity.z^2)
		if (speed > maxSpeed)
		then
			force = {0,0,0}
		end
		
		messages = {}

		return force, {0,0,0}, strength, state, messages

	end

	stateAction.archerRun = archerRun


	function archerFlying(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)

		if (position.y > 0.05)
		then
			return {0,0,0}, {0,0,0}, strength, state, {}
		else
			return {0,0,0}, {0,0,0}, strength, "archerRun", {}
		end

	end

	stateAction.archerFlying = archerFlying


	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Shooter behaviour: WARNING: unknown state "..state)
		return stateAction.archerRun(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end
end
