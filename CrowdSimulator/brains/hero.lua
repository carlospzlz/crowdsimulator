function hero(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	

	-- TARGET
	neighbourCounter = 0
	local target = nil
	local targetIndex = nil
	distanceVector = {0,0,0}
	distance = 99
	tmpDistance = 0 
	for key,neighbour in pairs(neighbours)
	do
		neighbourCounter = neighbourCounter + 1

		distanceVector[1] = neighbour.position.x - position.x
		distanceVector[2] = neighbour.position.y - position.y
		distanceVector[3] = neighbour.position.z - position.z

		tmpDistance = math.sqrt(distanceVector[1]^2 +
					distanceVector[2]^2 +
					distanceVector[3]^2)
		if (tmpDistance<distance)
		then
			target = neighbour
			targetIndex = key
			distance = tmpDistance
		end
	end	

	--MESSAGES (TTACKS)
	messages = {}
	enemiesForSuperHit = 40
	attackDistance = 4
	if (neighbourCounter > enemiesForSuperHit and strength > 0.9*maxStrength)
	then
		for key,neighbour in pairs(neighbours)
		do
			print("SUPERATTACK")
			messages[key] = {}
			messages[key][0] = "superattack"
		end
	else if (distance < attackDistance)
		then
		print("ATTACK")
		messages[targetIndex] = {}
		messages[targetIndex][0] = "attack"
		end
	end

	--FORCES

	--FACING FORCE
	targetForce = {0,0,0}
	heading = {0,0,0}
	distanceVector = {0,0,0}
	distance = 0
	if (target~=nil)
	then
		distanceVector[1] = target.position.x - position.x
		distanceVector[2] = target.position.y - position.y
		distanceVector[3] = target.position.z - position.z

		--normalize
		distance = math.sqrt(distanceVector[1]^2 + distanceVector[2]^2 + distanceVector[3]^2)
		heading[1] = distanceVector[1] / distance
		heading[2] = distanceVector[2] / distance
		heading[3] = distanceVector[3] / distance

		--weighted by the distance
		targetForce[1] = heading[1] / distance
		targetForce[2] = heading[2] / distance
		targetForce[3] = heading[3] / distance
	end

	--CENTRE FORCE
	centreForce = {0,0,0}

	centreForce[1] = -position.x
	centreForce[2] = -position.y
	centreForce[3] = -position.z

	--SYNTHESIS OF ALL THE FORCES
	force = {}
	tw = 0.5
	cw = 0.5
	force[1] = targetForce[1]*tw + centreForce[1]*cw
	force[2] = targetForce[2]*tw + centreForce[2]*cw
	force[3] = targetForce[3]*tw + centreForce[3]*cw

	return force, heading, strength, state, messages

end
