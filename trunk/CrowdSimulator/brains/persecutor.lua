function persecutor(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	stateAction = {}
	flyingHeight = 4

	function persecutorPersecute(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		-- CHECKING IF FLYING
		if (position.y > flyingHeight)
		then
			return {0,0,0}, {0,0,0}, strength, "persecutorFlying", {}
		end

		-- TARGET
		local target = nil
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.target=="yes")
			then
				target=neighbour
			end
		end	

		if (target==nil)
		then
			return {0,0,0}, {0,0,0}, strength, "persecutorGoCentre", {}	
		end

		desiredDirection = {0,0,0}
		desiredDirection[1] = target.position.x - position.x
		desiredDirection[2] = target.position.y - position.y
		desiredDirection[3] = target.position.z - position.z

		--normalize desiredDirection
		magnitude = math.sqrt(desiredDirection[1]^2 + desiredDirection[2]^2 + desiredDirection[3]^2)
		desiredDirection[1] = desiredDirection[1] / magnitude
		desiredDirection[2] = desiredDirection[2] / magnitude
		desiredDirection[3] = desiredDirection[3] / magnitude

		--currentDirection
		currentDirection = {0,0,0}
		speed = math.sqrt(velocity.x^2 + velocity.y^2 + velocity.z^2)
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

		seekDirection = {0,0,0}
		seekDirection[1] = desiredDirection[1] -- currentDirection[1] 
		seekDirection[2] = desiredDirection[2] -- currentDirection[2]
		seekDirection[3] = desiredDirection[3] -- currentDirection[3]

		--MOVEMENT FORCE
		tw = 0.05
		force[1] = seekDirection[1]*strength*tw
		force[2] = seekDirection[2]*strength*tw
		force[3] = seekDirection[3]*strength*tw

		maxSpeed = 2
		if (speed > maxSpeed)
		then
			force = {0,0,0}
		end
		
		--IMPACT FORCE
		attackDirection = {0,0,0}
		wa = 0
		for key,message in pairs(inbox)
		do
			if (message.label=="superattack" or message.label=="attack")
			then
				attackDirection[1] = position.x - message.position.x
				attackDirection[2] = position.y - message.position.y
				attackDirection[3] = position.z - message.position.z
			end

			if (message.label=="superattack")
			then
				wa = 3
			end

			if (message.label=="attack")
			then
				wa = 0.5
			end
		end

		force[1] = force[1] + attackDirection[1]*wa
		force[2] = force[2] + attackDirection[2]*wa
		force[3] = force[3] + attackDirection[3]*wa

		return force, desiredDirection, strength, state, {}
	end

	stateAction.persecutorPersecute = persecutorPersecute


	function persecutorFlying(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		-- CHECK IF ON GROUND
		if (position.y < flyingHeight)
		then
			return {0,0,0}, {0,0,0}, strength, "persecutorPersecute", {}
		end

		return {0,0,0}, {0,0,0}, strenght, state, {}
	end

	stateAction.persecutorFlying = persecutorFlying


	function persecutorGoCentre(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		-- CHECKING IF FLYING
		if (position.y > flyingHeight)
		then
			return {0,0,0}, {0,0,0}, strength, "persecutorFlying", {}
		end
		local target = nil
		
		--TARGET
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.target=="yes")
			then
				target=neighbour
			end
		end	

		if (target~=nil)
		then
			return {0,0,0}, {0,0,0}, strength, "persecutorPersecute", {}
		end
		
		
		desiredDirection = {0,0,0}
		desiredDirection[1] = -position.x
		desiredDirection[2] = -position.y
		desiredDirection[3] = -position.z

		--normalize
		magnitude = math.sqrt(desiredDirection[1]^2 + desiredDirection[2]^2 + desiredDirection[3]^2)
		desiredDirection[1] = desiredDirection[1] / magnitude
		desiredDirection[2] = desiredDirection[2] / magnitude
		desiredDirection[3] = desiredDirection[3] / magnitude

		--FORCE
		force = {}
		fw =0.1
		force[1] = desiredDirection[1] * fw
		force[2] = desiredDirection[2] * fw
		force[3] = desiredDirection[3] * fw
		
		--CLAMP VELOCITY
		maxSpeed = 2
		speed = math.sqrt(velocity.x^2 + velocity.y^2 + velocity.z^2)
		if (speed > maxSpeed)
		then
			force = {0,0,0}
		end

		return force, {0,0,0}, strength, state, {}

	end

	stateAction.persecutorGoCentre = persecutorGoCentre

	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Catcher behaviour: WARNING: unknown state "..state)
		return stateAction.persecutorGoCentre(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end


end
