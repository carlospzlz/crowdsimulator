function leaderBoid (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	--LEADER BOID

	--FLOCKING BEHAVIOUR
	positionSum = {0,0,0}
	separation = {0,0,0}
	averageHeading = {0,0,0}
	neighbourSpeed = 0

	distanceVector = {}
	distance = 0

	counter = 0
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
	wc = 0.1
	ws = 0.1
	-- THE LEADER DOES NOT OBEY THE ALIGNMENT FORCE
	wa = 0
	flockForce[1] = cohesion[1]*wc + separation[1]*ws + alignment[1]*wa
	flockForce[2] = cohesion[2]*wc + separation[2]*ws + alignment[2]*wa
	flockForce[3] = cohesion[3]*wc + separation[3]*ws + alignment[3]*wa
	
	--LEADERSHIP FORCE
	speed = math.sqrt( (velocity.x*velocity.x)+(velocity.y*velocity.y)+(velocity.z*velocity.z) )

	direction = {}

	if (speed==0)
	then
		direction.x = 1;
		direction.y = 0;
		direction.z = 0;
	else
		direction.x = velocity.x/speed
		direction.y = velocity.y/speed
		direction.z = velocity.z/speed
	end

	angle = (math.pi/300)

	direction.x = direction.x*math.cos(angle)-direction.z*math.sin(angle);
	direction.z = direction.x*math.sin(angle)+direction.z*math.cos(angle);

	leadershipForce = {}
	leadershipForce[1] = direction.x
	leadershipForce[2] = 0
	leadershipForce[3] = direction.z


	--SYNTHESIS OF ALL THE FORCES
	force = {}
	leaderW = 0.3
	flockW = 1
	force[1] = leadershipForce[1]*leaderW + flockForce[1]*flockW
	force[2] = leadershipForce[2]*leaderW + flockForce[2]*flockW
	force[3] = leadershipForce[3]*leaderW + flockForce[3]*flockW

	heading = {}
	--forceMagnitude = math.sqrt(force[1]^2, force[2]^2, force[3]^2)
	--heading[1] = force[1] / forceMagnitude
	--heading[2] = force[2] / forceMagnitude
	--heading[3] = force[3] / forceMagnitude

	--print(force[1].." "..force[2].." "..force[3]);

	messages = {}
	
	return force, heading, strength, state, messages
end
