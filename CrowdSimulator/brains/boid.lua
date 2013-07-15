function boid (agentID, position, strength, velocity, state, attributes, inbox, neighbours)

	flockForce = flockSteering(position, velocity, attributes, neighbours);

	messages = {}

	return flockForce, strength, state, messages 

end

function flockSteering (position, velocity, attributes, neighbours)

	-- IMPLEMENTATION OF THE CRAIG REINOLD'S FLOCKING BEHAVIOUS
	sumPositions = {0,0,0}
	separationVector = {0,0,0}
	velocities = {0,0,0}

	counter = 0
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

		separation[1] = separationVector[1] / counter
		separation[2] = separationVector[2] / counter
		separation[3] = separationVector[3] / counter
	end

	-- the separation force is inversely proportional to the distance
	separationModulus = math.sqrt(separation[1]^2 + separation[2]^2 + separation[3]^2)
	
	if (separationModulus ~= 0)
	then
		separation[1] = separation[1] / separationModulus
		separation[2] = separation[2] / separationModulus
		separation[3] = separation[3] / separationModulus
	end

	-- Aligmnet rule with 2 cross products
	vxvn = {}
	vxvn[1] = velocity.y*velocities[3] - velocity.z*velocities[2]
	vxvn[2] = - velocity.x*velocities[3] + velocity.z*velocities[1]
	vxvn[3] = velocity.x*velocities[2] - velocity.y*velocities[1]

	vxvnxv = {}
	vxvnxv[1] = vxvn[2]*velocity.z - vxvn[3]*velocity.y
	vxvnxv[2] = - vxvn[1]*velocity.z + vxvn[3]*velocity.x
	vxvnxv[3] = vxvn[1]*velocity.y - vxvn[2]*velocity.x

	force = {}

	wc = 0.15
	ws = 0.3
	wa = 0.005
	force[1] = cohesion[1]*wc + separation[1]*ws + vxvnxv[1]*wa
	force[2] = cohesion[2]*wc + separation[2]*ws + vxvnxv[2]*wa
	force[3] = cohesion[3]*wc + separation[3]*ws + vxvnxv[3]*wa

	--print("AGENT "..agentID.."("..counter..")")
	--print("cohesion: "..cohesion[1]..", "..cohesion[2]..", "..cohesion[3])
	--print("separation: "..separation[1]..", "..separation[2].." "..separation[3])
	--print("alignment: "..vxvnxv[1]..", "..vxvnxv[2].." "..vxvnxv[3])
	--print("force: "..force[1]..", "..force[2].." "..force[3])
	--print("velocity: "..velocity.x..velocity.y..velocity.z)
	--print("\n")

	messages = {}

	--print("sending ",force,strength,messages,"\n")

	return force
end
