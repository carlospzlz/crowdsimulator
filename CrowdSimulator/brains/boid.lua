function boid (agentID, position, strength, velocity, state, attributes, inbox, neighbours)
	
	--print("AGENT "..agentID.." uses brain simple")
	
	-- Implementation of the Craig Reinold's boid
	
	-- COHESION
	sumPositions = {0,0,0}
	separationVector = {0,0,0}
	velocities = {0,0,0}

	counter = 0

	-- SEPARATION

	for key,neighbour in pairs(neighbours)
	do
		if (attributes.flock==neighbour.attributes.flock)
		then
			sumPositions[1] = sumPositions[1] + neighbour.position.x
			sumPositions[2] = sumPositions[2] + neighbour.position.y
			sumPositions[3] = sumPositions[3] + neighbour.position.z
			
			separationVector[1] = separationVector[1] + (position.x-neighbour.position.x)
			separationVector[2] = separationVector[2] + (position.y-neighbour.position.y)
			separationVector[3] = separationVector[3] + (position.z-neighbour.position.z)

			velocities[1] = velocities[1] + neighbour.velocity.x
			velocities[2] = velocities[2] + neighbour.velocity.y
			velocities[3] = velocities[3] + neighbour.velocity.z

			counter = counter+1
		end
	end

	cohesion = {0,0,0}
	separation = {0,0,0}
	
	if (counter>0)
	then
		cohesion[1] = sumPositions[1]/counter - position.x
		cohesion[2] = sumPositions[2]/counter - position.y
		cohesion[3] = sumPositions[3]/counter - position.z

		separation[1] = (separationVector[1]/counter)
		separation[2] = (separationVector[2]/counter)
		separation[3] = (separationVector[3]/counter)
	end

	if (separation[1] == 0)
	then
		separation[1] == 10
	else
		separation[1]==

	-- ALIGMENT RULE WITH 2 CROSS PRODUCTS
	vxvn = {}
	vxvn[1] = velocity.y*velocities[3] - velocity.z*velocities[2]
	vxvn[2] = - velocity.x*velocities[3] + velocity.z*velocities[1]
	vxvn[3] = velocity.x*velocities[2] - velocity.y*velocities[1]

	vxvnxv = {}
	vxvnxv[1] = vxvn[2]*velocity.z - vxvn[3]*velocity.y
	vxvnxv[2] = - vxvn[1]*velocity.z + vxvn[3]*velocity.x
	vxvnxv[3] = vxvn[1]*velocity.y - vxvn[2]*velocity.x

	force = {}

	force[1] = 7*cohesion[1] + 0.1*separation[1] + 0.1*vxvnxv[1]
	force[2] = 7*cohesion[2] + 0.1*separation[2] + 0.1*vxvnxv[2]
	force[3] = 7*cohesion[2] + 0.1*separation[3] + 0.1*vxvnxv[3]

	print("AGENT "..agentID.."("..counter..")")
	print("cohesion: "..cohesion[1]..", "..cohesion[2]..", "..cohesion[3])
	print("separation: "..separation[1]..", "..separation[2].." "..separation[3])
	print("alignment: "..vxvnxv[1]..", "..vxvnxv[2].." "..vxvnxv[3])
	print("force: "..force[1]..", "..force[2].." "..force[3])
	print("velocity: "..velocity.x..velocity.y..velocity.z)
	print("\n")

	messages = {}

	--print("sending ",force,strength,messages,"\n")

	return force, strength, state, messages
end
