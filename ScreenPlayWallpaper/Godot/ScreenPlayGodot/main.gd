extends Node3D

@onready var screen_play_wallpaper  = $ScreenPlayGodotWallpaper

var activeScreensList = []
var projectPath = ""
var appID = ""
var volume = 0.0
var fillmode = 0
var checkWallpaperVisible = false

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

func parse_args():
	var args = OS.get_cmdline_args()
	var i = 0	
	while i < args.size():
		match args[i]:
			"--ActiveScreensList":
				i += 1
				activeScreensList = args[i].split(",")
				for idx in range(activeScreensList.size()):
					activeScreensList[idx] = int(activeScreensList[idx])
			"--ProjectPath":
				i += 1
				projectPath = args[i]
			"--appID":
				i += 1
				appID = args[i]
			"--volume":
				i += 1
				volume = float(args[i])
			"--fillmode":
				i += 1
				fillmode = int(args[i])
			"--CheckWallpaperVisible":
				i += 1
				checkWallpaperVisible = args[i].to_lower() == "true"
		i += 1
