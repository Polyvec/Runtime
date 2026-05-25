local velocity = math.vector2.new(10.5, 20.5)
local acceleration = math.vector2.new(2.0, 4.0)
local force = velocity + acceleration
local impulse = force * 3.0
local momentum = impulse / 2.0
print(tostring(momentum))

local position = math.vector3.new(5.0, 10.0, 15.0)
local destination = math.vector3.new(1.0, 2.0, 3.0)
local displacement = position - destination
local direction = displacement:normalized()
local combined = position * 2.0
local separated = combined / 4.0
print(tostring(direction))
print(tostring(separated))

local coordinate = math.vector4.new(1.0, 2.0, 3.0, 4.0)
local weight = math.vector4.new(2.0, 2.0, 2.0, 2.0)
local projection = coordinate + weight
local scaling = projection * 1.5
local fraction = scaling * (1.0 / 3.0)
print(tostring(fraction))

local rotation = math.quaternion.new(0.0, 0.0, 0.0, 1.0)
local orientation = math.quaternion.new(0.7071, 0.0, 0.0, 0.7071)
local spin = rotation * orientation
local inverted = spin:inverse()
print(tostring(inverted))

local tensor = math.matrix3.identity()
local bounds = math.vector2.new(2.0, 2.0)
local factor = math.matrix3.factor(bounds)
local linear = tensor * factor
print(tostring(linear))

local perspective = math.matrix4.perspective(60.0, 1.5, 0.1, 100.0)
local up = math.vector3.new(0.0, 1.0, 0.0)
local view = math.matrix4.look(position, destination, up)
local camera = perspective * view
print(tostring(camera))

local size = math.vector3.new(1.0, 1.0, 1.0)
local transform = math.transform.new(position, rotation, size)
local forward = transform:forward()
print(tostring(forward))