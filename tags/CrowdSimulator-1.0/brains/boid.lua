function boid (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

	-- BOID BEHAVIOUR

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
	wc = 0.05
	ws = 0.1
	wa = 0.05
	flockForce[1] = cohesion[1]*wc + separation[1]*ws + alignment[1]*wa
	flockForce[2] = cohesion[2]*wc + separation[2]*ws + alignment[2]*wa
	flockForce[3] = cohesion[3]*wc + separation[3]*ws + alignment[3]*wa
	
	
	--SYNTHESIS OF ALL THE FORCES
	force = {}
	force[1] = flockForce[1]
	force[2] = flockForce[2]
	force[3] = flockForce[3]

	heading = {}
	--forceMagnitude = math.sqrt(force[1]^2,force[2]^2,force[3]^2)
	--heading[1] = force[1] / forceMagnitude
	--heading[2] = force[2] / forceMagnitude
	--heading[3] = force[3] / forceMagnitude

	messages = {}

	--print("FORCE",force[1],force[2],force[3])

	return force, heading, strength, state, messages 

end
