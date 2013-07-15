function leaderBoid (agentID, position, strength, velocity, state, attributes, inbox, neighbours)
	
	--LEADER BOID

	--FLOCKING BEHAVIOUR
	sumPositions = {0,0,0}
	separationVector = {0,0,0}
	velocities = {0,0,0}

	desiredSeparation = 2
	distanceVector = {}
	distance = 0

	counter = 0
	for key,neighbour in pairs(neighbours)
	do
		if (attributes.flock==neighbour.attributes.flock)
		then
			sumPositions[1] = sumPositions[1] + neighbour.position.x
			sumPositions[2] = sumPositions[2] + neighbour.position.y
			sumPositions[3] = sumPositions[3] + neighbour.position.z
			
			distanceVector[1] = position.x - neighbour.position.x
			distanceVector[2] = position.y - neighbour.position.y
			distanceVector[3] = position.z - neighbour.position.z
			distance = math.sqrt(distanceVector[1]^2 + distanceVector[2]^2 + distanceVector[3]^2)
			
			--separation normalized and weighted, that's the reason of ^2
			if (distance > 0 and distance < desiredSeparation)
			then
				separationVector[1] = separationVector[1] + distanceVector[1]/distance^2
				separationVector[2] = separationVector[2] + distanceVector[2]/distance^2
				separationVector[3] = separationVector[3] + distanceVector[3]/distance^2
			end

			velocities[1] = velocities[1] + neighbour.velocity.x
			velocities[2] = velocities[2] + neighbour.velocity.y
			velocities[3] = velocities[3] + neighbour.velocity.z

			counter = counter+1
		end
	end

	cohesion = {0,0,0}
	separation = {0,0,0}
	alignment = {0,0,0}
	
	if (counter>0)
	then
		cohesion[1] = sumPositions[1]/counter - position.x
		cohesion[2] = sumPositions[2]/counter - position.y
		cohesion[3] = sumPositions[3]/counter - position.z

		separation[1] = separationVector[1] / counter
		separation[2] = separationVector[2] / counter
		separation[3] = separationVector[3] / counter

		alignment[1] = velocities[1] / counter
		alignment[2] = velocities[2] / counter
		alignment[3] = velocities[3] / counter
	end

	-- CALCULATE 3 RULES
	-- All the calculations are based on: DesiredVelocity - velocity
	speed = math.sqrt(velocity.x^2 + velocity.y^2 + velocity.z^2)

	-- Normalize cohesion
	cohesionMagnitude = math.sqrt(cohesion[1]^2 + cohesion[2]^2 + cohesion[3]^2)
	if (cohesionMagnitude ~= 0)
	then
		cohesion[1] = cohesion[1] / cohesionMagnitude
		cohesion[2] = cohesion[2] / cohesionMagnitude
		cohesion[3] = cohesion[3] / cohesionMagnitude
	end

	-- Calculate cohesion
	cohesion[1] = cohesion[1]*speed - velocity.x
	cohesion[2] = cohesion[2]*speed - velocity.y
	cohesion[3] = cohesion[3]*speed - velocity.z

	-- Normalize separation
	separationMagnitude = math.sqrt(separation[1]^2 + separation[2]^2 + separation[3]^2)
	if (separationMagnitude ~= 0)
	then
		separation[1] = separation[1] / separationMagnitude
		separation[2] = separation[2] / separationMagnitude
		separation[3] = separation[3] / separationMagnitude
	end

	-- Calculate separation
	separation[1] = separation[1]*speed - velocity.x
	separation[2] = separation[2]*speed - velocity.y
	separation[3] = separation[3]*speed - velocity.z


	-- Normalize alignment
	alignmentMagnitude = math.sqrt(alignment[1]^2 + alignment[2]^2 + alignment[3]^2)
	if (alignmentMagnitude ~= 0)
	then
		alignment[1] = alignment[1] / alignmentMagnitude
		alignment[2] = alignment[2] / alignmentMagnitude
		alignment[3] = alignment[3] / alignmentMagnitude
	end

	-- Calculate alignment
	alignment[1] = alignment[1]*speed - velocity.x
	alignment[2] = alignment[2]*speed - velocity.y
	alignment[3] = alignment[3]*speed - velocity.z

	--Weights for the flocking force
	flockForce = {}
	wc = 0.1
	ws = 0.1
	wa = 0.1
	flockForce[1] = cohesion[1]*wc + separation[1]*ws + alignment[1]*wa
	flockForce[2] = cohesion[2]*wc + separation[2]*ws + alignment[2]*wa
	flockForce[3] = cohesion[3]*wc + separation[3]*ws + alignment[3]*wa
	
	--LEADERSHIP FORCE
	magnitude = math.sqrt( (velocity.x*velocity.x)+(velocity.y*velocity.y)+(velocity.z*velocity.z) )

	direction = {}

	if (magnitude==0)
	then
		direction.x = 1;
		direction.y = 0;
		direction.z = 0;
	else
		direction.x = velocity.x/magnitude
		direction.y = velocity.y/magnitude
		direction.z = velocity.z/magnitude
	end

	angle = (math.pi/300)

	direction.x = direction.x*math.cos(angle)-direction.z*math.sin(angle);
	direction.z = direction.x*math.sin(angle)+direction.z*math.cos(angle);
	
	leadershipForce = {}
	leadershipForce[1] = direction.x*3
	leadershipForce[2] = 0
	leadershipForce[3] = direction.z*3


	--SYNTHESIS OF ALL THE FORCES
	force = {}
	leaderW = 1
	flockW = 0.2
	force[1] = leadershipForce[1]*leaderW + flockForce[1]*flockW
	force[2] = leadershipForce[2]*leaderW + flockForce[2]*flockW
	force[3] = leadershipForce[3]*leaderW + flockForce[3]*flockW

	--print(force[1].." "..force[2].." "..force[3]);

	--SEND MESSAGES
	messages = {}
	counter = 1
	for key,neighbour in pairs(neighbours)
	do
		messages[counter] = {}
		if (neighbour.attributes.flock and attributes.flock==neighbour.attributes.flock)
		then
			messages[counter][1] = "follow"	
		end
		counter = counter + 1
	end
	
	return force, strength, state, messages
end