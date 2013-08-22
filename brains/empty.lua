function empty (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

	force = {}

	--force[1]=2
	--force[2]=2
	--force[3]=2

	messages = {}


	return force, {0,0,0}, strength, state, messages
end
