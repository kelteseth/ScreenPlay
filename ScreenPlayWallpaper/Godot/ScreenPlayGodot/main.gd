extends Node3D

func _ready():
	var path = ""
	var success = ProjectSettings.load_resource_pack (path)
	if success:
		var scene_resource = load("res://wallpaper.tscn")
		if scene_resource:
			var scene_instance = scene_resource.instantiate()
			add_child(scene_instance)
		else:
			print("Failed to load the wallpaper.tscn scene.")
	else:
		print("Failed to load the PCK file.")
	Engine.set_max_fps(24)
