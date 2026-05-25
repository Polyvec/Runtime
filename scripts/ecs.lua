local world = ecs.world.new()

local cmd = math.matrix4.identity()
local layout = math.matrix4.identity()
local pipe = math.matrix4.identity()

local position = world:component("position", math.vector3.new(0, 0, 0))
local velocity = world:component("velocity", math.vector3.new(0, 0, 0))
local transform = world:component("transform", math.matrix4.identity())
print(tostring(position))
print(tostring(velocity))
print(tostring(transform))

local car = world:spawn()
local link = world:component("link", car)
print(tostring(car))
print(tostring(link))

local custom = { drag = 0.02, mass = 1.0 }
local dynamics = world:component("dynamics", custom)
print(tostring(dynamics))

local actor = world:spawn()
print(tostring(actor))

world:attach(actor, position, math.vector3.new(0, 10, 5))
world:attach(actor, velocity, math.vector3.new(0, 0, 25))
world:attach(actor, dynamics, { drag = 0.01, mass = 2.5 })
world:attach(actor, link, car)

if world:has(actor, position) then
    local check = world:get(actor, position)
    print(tostring(check))
end

local physics = world:query():with(position):with(velocity):with(dynamics)
local static = world:query():with(position):without(velocity)
local volatile = world:query():any({velocity, dynamics}):without(transform)
local tree = world:query():with(position):with(link)
print(tostring(physics:has(position)))

world:batch(function()
    physics:dispatch(cmd, layout, pipe, {position, velocity, dynamics})

    world:execute(tree, function(count, positions, links)
        print(tostring(count))
        for index = 1, count do
            local target = links:get(index)
            print(tostring(target))
            if target and world:has(target, position) then
                local parent = world:get(target, position)
                positions:set(index, positions:get(index) + parent)
                print(tostring(positions:get(index)))
            end
        end
    end)
end)

if physics:has(position) and not physics:has(transform) then
    print(tostring(true))
end

world:batch(function()
    world:detach(actor, velocity)
    world:kill(actor)
end)