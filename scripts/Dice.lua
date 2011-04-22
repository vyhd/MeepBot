MeepBot.Help["roll"] = "rolls one or more dice with the given number of sides, e.g. '2d8' rolls two 8-sided dice. (Ops)"
MeepBot.Help["rollx"] = "rolls one die with up to 4294967295 sides, if you really need that sort of thing. (Ops)"

local DEFAULT_NUM_SIDES = 6
local MAX_ROLLS = 20
local MAX_SIDES = 1000
local MAX_ROLLX_SIDES = math.pow(2,32) - 1

local function DoRoll( sides )
	return MeepBot.Utils.Rand( sides )
end

local function RollsToStr( rolls )
	local ret = tostring( rolls[1] )
	local total = rolls[1]

	for i=2,#rolls do
		total = total + rolls[i]
		ret = ret .. ", " .. tostring(rolls[i])
	end

	return ret .. " = " .. tostring(total)
end

MeepBot.Commands["roll"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_OP) then return end

	-- throwaway, number of dice, number of sides per die
	local _, num, sides = nil, 1, DEFAULT_NUM_SIDES

	if params then
		_, _, num, sides = params:find( "^(%d+)d?(%d*)" )
		num = tonumber(num)
		sides = tonumber(sides)
	end

	if not num or num == 0 then
		MeepBot.SayOrPM( type, caller, "I need at least one die.")
		return
	end

	if not sides or sides == 0 then
		MeepBot.SayOrPM( type, caller, "I need at least one side!" )
		return
	end

	if num < 0 or num > MAX_ROLLS then
		local str = ("Must be between 1 and %d rolls."):format(MAX_ROLLS)
		MeepBot.SayOrPM( type, caller, str )
		return
	end

	if sides < 0 or sides > MAX_SIDES then
		local str = ("Must be between 1 and %d sides."):format(MAX_SIDES)
		MeepBot.SayOrPM( type, caller, str )
		return
	end

	local rolls = { }
	for i=1,num do rolls[i] = DoRoll( sides ) end

	-- change the text slightly for PMs
	local response = (type == TYPE_PM and "You" or caller) .. " rolled"

	if num == 1 then
		response = response .. (" a %d."):format( rolls[1] )
	else
		response = response .. (": %s"):format( RollsToStr(rolls) )
	end

	MeepBot.SayOrPM( type, caller, response )

end

MeepBot.Commands["rollx"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_OP) then return end

	print( ("rollx %s (caller: %s, type: %d)"):format(params,caller,type) )

	if not params then
		MeepBot.SayOrPM( type, caller, "Usage: !rollx [number]." )
		return
	end

	-- if someone supplies e.g. 'd20', read it as '20'
	if params:sub(1,1) == "d" then params = params:sub(2) end

	local sides = tonumber(params)

	if not sides or sides == 0 then
		MeepBot.SayOrPM( type, caller, "I need a non-zero integer. 8)" )
		return
	elseif sides > MAX_ROLLX_SIDES then
		MeepBot.SayOrPM( type, caller, ("Sides is limited to %d."):format(MAX_ROLLX_SIDES) )
		return
	end

	local result = DoRoll( sides )

	-- change the text slightly for PMs
	local salutation = type == TYPE_PM and "You" or caller

	local response = ("%s rolled a %d."):format(salutation, result)
	MeepBot.SayOrPM( type, caller, response )
end

