extends CSGBox3D

var rotation_speed = 2.0
var hover_height = 0.5  # Half of the total hover distance
var hover_speed = 1  # Speed of the hover effect
var hover_offset = 0.0  # Offset for the sine function

func _process(delta):
	rotate_y(rotation_speed * delta)

	# Increment the hover offset
	hover_offset += hover_speed * delta

	# Calculate the new Y position based on the sine wave
	var hover_y = hover_height * sin(hover_offset)

	# Update the position with the hover effect
	position.y = hover_y
