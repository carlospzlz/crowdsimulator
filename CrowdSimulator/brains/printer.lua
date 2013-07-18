function printer (agentID, position, strength, maxStrength, velocity, state,  attributes, inbox, neighbours)
	
	print("AGENT "..agentID.." uses brain PRINTER")
	print("position= ",position)
	print(position.x, position.y, position.z)
	print("strength= ",strength)
	print("maxStrength= ",maxStrength)
	print("velocity= ",velocity)
	print(velocity.x, velocity.y, velocity.z)
	print("state= ",state)

	print("attributes: ",attributes,#attributes)
	for key,value in pairs(attributes)
	do
		print(key,value)
	end

	print("inbox: ",inbox,#inbox)
	for key,message in pairs(inbox)
	do
		print(message)
		print(message.agentID)
		print(message.label)
		print(message.position)
		print(message.strength)
	end

	print("neighbours= ",neighbours,#neighbours)
	for index,neighbour in pairs(neighbours)
	do
		print("\tagentID=",neighbour.agentID)
		print("\tmass= ",neighbour.mass)
		print("\tstrength= ",neighbour.strength)
		print("\tposition= ",neighbour.position)
		print("\t"..neighbour.position.x,neighbour.position.y, neighbour.position.z)
		print("\tvelocity= ",neighbour.position)
		print("\t"..neighbour.velocity.x,neighbour.velocity.y, neighbour.velocity.z)
		print("\tstate= ",neighbour.state)
		print("\tattributes= ",neighbour.attributes,#neighbour.attributes)
		for key,value in pairs(neighbour.attributes)
		do
			print("\t",key,value)
		end
	end

	print("\n")

	force = {0,0,0}
	messages = {}
	heading = {}

	return force, heading, strength, state, messages
end
