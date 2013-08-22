function verticalLeader (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	--VERTICAL LEADER
	stateAction = {}

	--VERTICALFLOCKFORCE AUXILIAR FUNCTION
	function verticalFlockForce(position,attributes,neighbours,wc,ws,wa)

		-- FLOCKING BEHAVIOUR
		positionSum = {0,0,0}
		separation = {0,0,0}
		averageHeading = {0,0,0}
		neighbourSpeed = 0

		distanceVector = {}
		distance = 0

		counter = 0
		enemyCounter = 0
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

	--STATE VERTICALGOINGUP
	function verticalGoingUp(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		if (position.z < -40)
		then
			return {0,0,0},{0,0,0},strength,"verticalGoingDown",{}
		end

		
		flockForce = verticalFlockForce(position,attributes,neighbours,0.1,0.1,0)

		leadershipForce = {}
		leadershipForce[1] = 0
		leadershipForce[2] = 0
		leadershipForce[3] = -1


		--SYNTHESIS OF ALL THE FORCES
		force = {}
		leaderW = 0.3
		flockW = 0.1
		force[1] = leadershipForce[1]*leaderW + flockForce[1]*flockW
		force[2] = leadershipForce[2]*leaderW + flockForce[2]*flockW
		force[3] = leadershipForce[3]*leaderW + flockForce[3]*flockW

		messages = {}
	
		return force, {0,0,0}, strength, state, messages
	end
	
	stateAction.verticalGoingUp = verticalGoingUp

	--STATE PARALLELGOINGLEFT
	function verticalGoingDown(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		if (position.z > 40)
		then
			return {0,0,0},{0,0,0},strength,"verticalGoingUp",{}
		end

		
		flockForce = verticalFlockForce(position,attributes,neighbours,0.1,0.1,0)

		leadershipForce = {}
		leadershipForce[1] = 0
		leadershipForce[2] = 0
		leadershipForce[3] = 1


		--SYNTHESIS OF ALL THE FORCES
		force = {}
		leaderW = 0.3
		flockW = 0.1
		force[1] = leadershipForce[1]*leaderW + flockForce[1]*flockW
		force[2] = leadershipForce[2]*leaderW + flockForce[2]*flockW
		force[3] = leadershipForce[3]*leaderW + flockForce[3]*flockW

		messages = {}
	
		return force, {0,0,0}, strength, state, messages
	end
	
	stateAction.verticalGoingDown = verticalGoingDown
	
	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("VerticalLeader behaviour: WARNING: unknown state "..state)
		return stateAction.verticalGoingUp(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end

end
