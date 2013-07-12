function printer (strength, position, velocity, state,  messages, neighbours)
	
	print("Information arrived to the brain PRINTER")
	print("strength= ",strength)
	print("position= ",position)
	print(position.m_x, position.m_y, position.m_z)
	print("velocity= ",velocity)
	print(velocity.m_x, velocity.m_y, velocity.m_z)
	print("state= ",state)
	print("messages ",messages)
	for key,message in pairs(messages)
	do
		print(key,message)
	end
	print(messages.flock)
	print("neighbours= ",neighbours)
	for index,neighbour in pairs(neighbours)
	do
		print("mass= ",neighbour.m_mass)
		print("strength= ",neighbour.m_strength)
		print("position= ",neighbour.position)
		print(neighbour.m_position.m_x,neighbour.m_position.m_y, neighbour.m_position.m_z)
		print("state= ",neighbour.state)
		print("messages= ",neighbour.m_messages)
		for key,message in pairs(neighbour.m_messages)
		do
			print(key,message)
		end
	end
	return
end
