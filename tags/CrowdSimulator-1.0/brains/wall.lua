function wall(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	zTarget = -50

	--SEEKING BEHAVIOUR

	desiredDirection = {}
	if (math.abs(position.x) > 0.5)
	then
		desiredDirection[1] = -position.x
	else
		desiredDirection[1] = 0
	end
	desiredDirection[2] = 0
	desiredDirection[3] = zTarget - position.z

	--normalize
	magnitude = math.sqrt(desiredDirection[1]*desiredDirection[1] +
			      desiredDirection[2]*desiredDirection[2] +
			      desiredDirection[3]*desiredDirection[3])
	
	desiredDirection[1] = desiredDirection[1] / magnitude
	desiredDirection[2] = desiredDirection[2] / magnitude
	desiredDirection[3] = desiredDirection[3] / magnitude

	currentDirection = {}

	speed = math.sqrt(velocity.x*velocity.x +
			  velocity.y*velocity.y +
			  velocity.z*velocity.z)

	if (speed > 0)
	then
		currentDirection[1] = velocity.x / speed
		currentDirection[2] = velocity.y / speed
		currentDirection[3] = velocity.z / speed
	else
		currentDirection[1] = 0
		currentDirection[2] = 0
		currentDirection[3] = -1
	end


	--FORCE DIRECTION
	force = {}

	sw = 2
	force[1] = desiredDirection[1]*sw - currentDirection[1]
	force[2] = desiredDirection[2]*sw - currentDirection[2]
	force[3] = desiredDirection[3]*sw - currentDirection[3]

	--FORCE MAGNITUDE
	tw = 0.5
	force[1] = force[1]*strength*tw
	force[2] = force[2]*strength*tw
	force[3] = force[3]*strength*tw

	return force, {0,0,0}, strength, state, {}
end
