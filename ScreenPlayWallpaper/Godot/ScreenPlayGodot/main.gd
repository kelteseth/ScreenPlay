extends Node3D

@onready var screen_play_wallpaper  = $ScreenPlayGodotWallpaper

func _ready():
	# "C:\\Code\\cpp\\ScreenPlay\\ScreenPlay\\Content\\wallpaper_godot_fjord\\fjord.zip"
	var path = "C:\\Code\\cpp\\ScreenPlay\\ScreenPlay\\Content\\wallpaper_godot_fjord\\fjord.zip"
	var success = ProjectSettings.load_resource_pack (path)
	if success:
		var scene_resource = load("res://wallpaper.tscn")
		if scene_resource:
			var scene_instance = scene_resource.instantiate()
			add_child(scene_instance)
			var ok  = screen_play_wallpaper.init(0)
			print("init ", ok)
		else:
			print("Failed to load the wallpaper.tscn scene.")
	else:
		print("Failed to load the PCK file.")
	Engine.set_max_fps(24)
