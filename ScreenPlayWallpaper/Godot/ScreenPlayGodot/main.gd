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
	var path 
	var args = OS.get_cmdline_args()
	if( args.size() > 1):
		parse_args()
	
	path = "C:\\Code\\cpp\\ScreenPlay\\ScreenPlay\\Content\\wallpaper_godot_fjord\\fjord.zip"
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

	if args.size() < 7:  # Adjust this number based on the expected number of arguments
		print("Not enough arguments provided!")
		return

	# Parse the arguments based on their expected positions
	activeScreensList = args[0].split(",")
	for idx in range(activeScreensList.size()):
		activeScreensList[idx] = int(activeScreensList[idx])

	projectPath = args[1]
	appID = args[2]
	volume = float(args[3])
	fillmode = int(args[4])
	# Assuming 'type' is not a global variable since it's not listed above
	var type = args[5]  # This might need further parsing depending on its expected format
	checkWallpaperVisible = args[6].to_lower() == "true"

	# Print or use the parsed values as needed
	print(activeScreensList, projectPath, appID, volume, fillmode, type, checkWallpaperVisible)
