extends CSGBox3D

var rotation_speed = 3.0

func _process(delta): 
	rotate_y(rotation_speed * delta)
