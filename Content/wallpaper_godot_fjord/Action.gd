extends Node

# Reference to the PathFollow node
@onready var path_follow = $Path3D/PathFollow3D
@onready var label = $Label3D 
var debug_path
var screenplaywallpaper 
var appID = ""
# Speed of the movement along the path
var speed = 0.8


func _process(delta):
	# Update the offset to move the object along the path
	path_follow.progress += speed * delta

	# Loop back to the start if we've reached the end of the path
	if path_follow.progress >= 18.81:
		path_follow.progress = 0.0
		
	screenplaywallpaper = get_node("/root/Wallpaper/ScreenPlayGodotWallpaper")
	if screenplaywallpaper:
		appID = screenplaywallpaper.get_appID()
	else:
		appID = "404"
	label.text = "Godot Wallpaper\n" + Time.get_time_string_from_system() + "\nAppID: " + appID
