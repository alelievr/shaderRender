
local prog = gol.get_prog()

if prog then
	gol.use_prog( prog )
	local positions = {
		{-1.0, 1.0, 0.0}, -- top-left
		{-1.0, -1.0, 0.0}, -- bot-left
		{1.0, 1.0, 0.0}, -- top-right
		{1.0, -1.0, 0.0} -- bot-right
	}
	local texCoords = {
		{0.0, 0.0},
		{1.0, 0.0},
		{0.0, 1.0},
		{1.0, 1.0}
	}
	local indexes = {0, 1, 2, 1, 3, 2}
	local elem = gol.create_element(indexes, positions, texCoords)
	gol.delete_element(elem)
end

