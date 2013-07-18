function captain (agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
	--WARRIOR
	stateAction = {}

	--WARRIORFLOCKFORCE AUXILIAR FUNCTION
	function captainFlockForce(position,attributes,neighbours,wc,ws,wa)

		-- FLOCKING BEHAVIOUR
		positionSum = {0,0,0}
		separation = {0,0,0}
		averageHeading = {0,0,0}
		neighbourSpeed = 0

		distanceVector = {}
		distance = 0

		counter = 0
		enemyCounter = 0
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
		--wc = 1
		--ws = 1
		--wa = 1
		flockForce[1] = cohesion[1]*wc + separation[1]*ws + alignment[1]*wa
		flockForce[2] = cohesion[2]*wc + separation[2]*ws + alignment[2]*wa
		flockForce[3] = cohesion[3]*wc + separation[3]*ws + alignment[3]*wa

		--normalize
		magnitude = math.sqrt(flockForce[1]^2+flockForce[2]^2+flockForce[3]^2)
		if (magnitude>0)
		then
			flockForce[1] = flockForce[1] / magnitude
			flockForce[2] = flockForce[2] / magnitude
			flockForce[3] = flockForce[3] / magnitude
		end
	
	return flockForce
	end

	--CAPTAINATTACKFORCE AUXILIAR FORCE
	function captainAttackForce(position, inbox)
	
		--PROCESS INCOMING ATTACKS
		attackForce = {0,0,0}
		attackDirection = {}
		damage = 0
		distance = 0
		attackDistance = 3
		for key,message in pairs(inbox)
		do
			if (message.label=="attack")
			then

				attackDirection[1] = position.x - message.position.x
				attackDirection[2] = position.y - message.position.y
				attackDirection[3] = position.z - message.position.z

				--normalizing
				distance = math.sqrt(attackDirection[1]^2 + attackDirection[2]^2 + attackDirection[3]^2)
				attackDirection[1] = attackDirection[1] / distance
				attackDirection[2] = attackDirection[2] / distance
				attackDirection[3] = attackDirection[3] / distance
				
				--DAMAGE IN ATTACK
				attackForce[1] = attackForce[1] + attackDirection[1]*message.strength
				attackForce[2] = attackForce[2] + attackDirection[2]*message.strength
				attackForce[3] = attackForce[3] + attackDirection[3]*message.strength

				--key point
				damage = damage + message.strength
			end
		end
		
		--normalize
		--magnitude = math.sqrt(attackForce[1]^2 + attackForce[2]^2 + attackForce[3]^2)
		--if (magnitude>0)
		--then
		--	attackForce[1] = attackForce[1] / magnitude
		--	attackForce[2] = attackForce[2] / magnitude
		--	attackForce[3] = attackForce[3] / magnitude
		--end	

		return attackForce, damage

	end
	
	--HOLD STATE
	function captainHold(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
		return {0,0,0}, {0,0,0}, strength, "captainRun", {}
	end

	stateAction.captainHold = captainHold

	--ATTACK STATE
	function captainAttack(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		--CHECKING IF THERES IS CHANGE OF STATE
		if (strength<0)
		then
			return {0,0,0}, {0,0,0}, 0, "captainDead",{}
		end

		enemiesCounter = 0
		for key, neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="captainDead" and neighbour.state~="warriorDead")
			then
				enemiesCounter = enemiesCounter + 1
			end
		end
		
		if (enemiesCounter>0)
		then
			if (strength < 0.1*maxStrength)
			then
				return {0,0,0}, {0,0,0}, strength, "captainDefend", {}
			end
		else	
			return {0,0,0}, {0,0,0}, strength, "captainHold", {}
		end	
		
		--AttackForce
		attackForce, damage = captainAttackForce(position,inbox)

		--MOVEMENT AND ATTACKS
		closestEnemyPosition = {0,0,0}
		messages = {}
		distance = 99
		attackDistance = 1

		enemiesCounter = 1
		neighboursCounter = 1
		for key, neighbour in pairs(neighbours)
		do
			if (attributes.army~=neighbour.attributes.army)
			then
				tmpDistance = math.sqrt( (neighbour.position.x-position.x)^2 +
							 (neighbour.position.y-position.y)^2 +
							 (neighbour.position.z-position.z)^2 )
				if (tmpDistance<distance)
				then
					closestEnemyPosition[1]= neighbour.position.x
					closestEnemyPosition[2]= neighbour.position.y
					closestEnemyPosition[3]= neighbour.position.z

					distance = tmpDistance

					if (distance < attackDistance)
					then
						messages[neighboursCounter] = {}
						messages[neighboursCounter][0] = "attack"
					end
				end
				enemiesCounter = counter + 1
			end
			neighboursCounter = neighboursCounter+1	
		end
		
		movementForce = {0,0,0}
		magnitude = 0
		heading = {0,0,0}
		if (enemiesCounter > 0)
		then
			movementForce[1] = (closestEnemyPosition[1] - position.x)
			movementForce[2] = (closestEnemyPosition[2] - position.y)
			movementForce[3] = (closestEnemyPosition[3] - position.z)

			magnitude = math.sqrt(movementForce[1]^2+movementForce[2]^2+movementForce[3]^2)
			heading[1] = movementForce[1] / magnitude
			heading[2] = movementForce[2] / magnitude
			heading[3] = movementForce[3] / magnitude
		end

		--INTEGRATE FORCES
		force = {}
		aw = 0.05
		mw = 0.05
		--print(enemiesAttackForce[1],enemiesAttackForce[2],enemiesAttackForce[3])
		force[1] = attackForce[1]*aw + movementForce[1]*strength*mw
		force[2] = attackForce[2]*aw + movementForce[2]*strength*mw
		force[3] = attackForce[3]*aw + movementForce[3]*strength*mw

		--CALCULATING STRENGTH
		--Damage and cost of attack
		wd = 0.01
		strength = strength -0.001 - damage*wd


		return force, heading, strength, state, messages

	end
	
	stateAction.captainAttack = captainAttack

	--DEFEND STATE
	function captainDefend(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)
	
		-- CHECK FOR THE CHANGE OF STATE
		if (strength < 0)
		then
			return {0,0,0}, {0,0,0}, 0, "captainDead", {}
		end	

		enemiesCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army and
			    neighbour.state~="captainDead" and neighbour.state~="warriorDead")
			then
				enemiesCounter = enemiesCounter + 1
			end
		end	

		if (enemiesCounter > 0)
		then
			if (strength > 0.9*maxStrength)
			then
				return {0,0,0}, {0,0,0}, strength,"captainAttack", {}
			end
		else	
			return {0,0,0}, {0,0,0}, strength, "captainHold", {}
		end

		--INCOMING ATTACK FORCE
		attackForce, damage = captainAttackForce(position,inbox)

		--HEADING
		enemyPositionsSum = {0,0,0}
		enemyCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			if (neighbour.attributes.army~=attributes.army)
			then
				enemyPositionsSum[1] = enemyPositionsSum[1] + neighbour.position.x
				enemyPositionsSum[2] = enemyPositionsSum[2] + neighbour.position.y
				enemyPositionsSum[3] = enemyPositionsSum[3] + neighbour.position.z

				enemyCounter = enemyCounter + 1	
			end
		end
		
		heading = {0,0,0}
		if (enemyCounter > 0)
		then
			hw = 1
			heading[1] = enemyPositionsSum[1] / enemyCounter - position.x
			heading[2] = enemyPositionsSum[2] / enemyCounter - position.y
			heading[3] = enemyPositionsSum[3] / enemyCounter - position.z
		end

		--INTEGRATING FORCE
		force = {}
		aw = 0.05
		resistance = 0.05
		force[1] = attackForce[1]*aw - velocity.x*resistance*strength
		force[2] = attackForce[2]*aw - velocity.y*resistance*strength
		force[3] = attackForce[3]*aw - velocity.z*resistance*strength

		--CALCULATING STRENGTH
		dw = 0.0001
		strength = strength + 0.005 - damage*dw

		return force, heading, strength, state, messages

	end

	stateAction.captainDefend = captainDefend

	--DEAD STATE
	function captainDead(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		return {0,0,0},{0,0,0},0,"captainDead",{}

	end

	stateAction.captainDead = captainDead

	--LOST
	function captainLost(velocity)
		angle = (math.pi/300)

		lostForce = {0,0,0}
		lostForce[1] = velocity.x*math.cos(angle)-velocity.z*math.sin(angle);
		lostForce[3] = velocity.x*math.sin(angle)+velocity.z*math.cos(angle);

		magnitude = math.sqrt(lostForce[1]^2 + lostForce[2]^2 + lostForce[3]^2)
		if (magnitude>0)
		then		
			lostForce[1] = lostForce[1] / magnitude
			lostForce[2] = lostForce[2] / magnitude
			lostForce[3] = lostForce[3] / magnitude
		end

		return lostForce
	end


	--RUN STATE
	function captainRun(agentID, position, strength, maxStrength, velocity, state, attributes, inbox, neighbours)

		-- CHECKING CHANGE OF STATE
		enemiesCounter = 0
		for key,neighbour in pairs(neighbours)
		do
			if (attributes.army ~= neighbour.attributes.army and
			    neighbour.state~="captainDead" and neighbour.state~="warriorDead")
			then
				enemiesCounter = enemiesCounter + 1
			end
		end

		if (enemiesCounter>0)
		then
			return {0,0,0}, {0,0,0}, strength, "captainAttack", {}
		end
		
		--FLOCK BEHAVIOUR
		flockForce = captainFlockForce(position,attributes,neighbours,1,1,0)		
	
		--LEADERSHIP FORCE
		leadershipForce = {0,0,0}
	
		if (attributes.army == "army1")
		then
			leadershipForce[3] = -1
		elseif (attributes.army == "army2")
		then
			leadershipForce[3] = 1
		end

		--NO FLOCKING -> NOTHING TO LEAD		
		force = {}
		magnitude = math.sqrt(flockForce[1]^2+flockForce[2]^2+flockForce[3]^2)
		if (magnitude==0)
		then
			force = captainLost(velocity)
		else
			lw = 1
			fw = 1
			force[1] = (leadershipForce[1]*lw + flockForce[1]*fw)
			force[2] = (leadershipForce[2]*lw + flockForce[2]*fw)
			force[3] = (leadershipForce[3]*lw + flockForce[3]*fw)
		end
		
		--THE FORCE DEPENDS ON THE STRENGTH
		tw = 0.1
		force[1] = force[1] * tw * strength
		force[2] = force[2] * tw * strength
		force[3] = force[3] * tw * strength

		--RECOVERING
		strength = strength + 0.001
		
		--print(force[1].." "..force[2].." "..force[3]);

		messages = {}

		return force, {0,0,0}, strength, state, messages

	end

	stateAction.captainRun = captainRun

	--FIRE TRANSITION	
	if not (stateAction[state])
	then
		print("Captain behaviour: WARNING: unknown state "..state)
		return stateAction.captainHold(agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	else
		return stateAction[state](agentID,position,strength,maxStrength,velocity,state,attributes,inbox,neighbours)
	end
end
