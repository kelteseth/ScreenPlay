extends Path3D

# Reference to the PathFollow node
@onready var path_follow = $PathFollow3D
@onready var path_follow = 

# Speed of the movement along the path
var speed = 0.8


func _process(delta):
	# Update the offset to move the object along the path
	path_follow.progress += speed * delta
	
	# Loop back to the start if we've reached the end of the path
	if path_follow.progress >= 18.81:
		path_follow.progress = 0.0
