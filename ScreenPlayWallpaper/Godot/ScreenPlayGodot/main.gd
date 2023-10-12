extends Node3D

@onready var screen_play_wallpaper = $ScreenPlayGodotWallpaper

@onready var alive_timer = $AliveTimer

func ping_alive_screenplay():
	var ping_alive_screenplay = screen_play_wallpaper.ping_alive_screenplay()
	var msg = screen_play_wallpaper.read_from_pipe()
	if not msg.isEmpty():
		print("message", msg)
		if "quit" in msg:
			get_tree().quit()
			return
			
	#print("ping_alive_screenplay: ", ping_alive_screenplay)
	
func _ready():
	alive_timer.timeout.connect(ping_alive_screenplay)
	
	if not screen_play_wallpaper:
		printerr("ERROR INVALID SCREENPLAY OBJECT")
		return
		
	var path
	var args = OS.get_cmdline_args()
	if args.size() > 1:
		if not parse_args():
			get_tree().quit()
			return
		#screen_play_wallpaper.set_projectPath("C:\\Code\\cpp\\ScreenPlay\\ScreenPlay\\Content\\wallpaper_godot_fjord")
		path = screen_play_wallpaper.get_projectPath() + "/project.zip"
	else:
		get_tree().quit()
		return
	print(path)
	if not load_scene(path):
		print("Failed to load the PCK file.")
		get_tree().quit()
		return
	Engine.set_max_fps(24)
	var ok = screen_play_wallpaper.init(screen_play_wallpaper.get_activeScreensList()[0])
	print("init ", ok)
	if not screen_play_wallpaper.get_pipeConnected():
		print("connect to ScreenPlay")
		var ok_connect_to_named_pipe = screen_play_wallpaper.connect_to_named_pipe()
		print("connection: ", ok_connect_to_named_pipe)
	if not screen_play_wallpaper.get_screenPlayConnected():
		print("send_welcome")
		var send_welcome = screen_play_wallpaper.send_welcome()
		print("send_welcome: ", send_welcome)
		if send_welcome:
			alive_timer.start()


func load_scene(path):
	var success = ProjectSettings.load_resource_pack(path)
	if success:
		var scene_resource = load("res://wallpaper.tscn")
		if scene_resource:
			var scene_instance = scene_resource.instantiate()
			if scene_instance:
				add_child(scene_instance)
			else:
				print("Failed to instantiate the wallpaper.tscn scene.")
				return false
		else:
			print("Failed to load the wallpaper.tscn scene.")
			return false
	else:
		print("Failed to load the PCK file.")
		return false
	return true

func parse_args():
	var args = OS.get_cmdline_args()
	print("Parse args:", args)
	var offset = 0
	if args[0] == "res://main.tscn":
		offset = 1

	if args.size() < 7:  # Adjust this number based on the expected number of arguments
		print("Not enough arguments provided!")
		return false

	var activeScreensList = []
	if args[0 + offset].is_valid_int():
		activeScreensList.append(args[0 + offset].to_int())
	else:
		var potentialInts = args[0 + offset].split(",")
		for val in potentialInts:
			if not val.is_valid_int():
				print("Invalid argument: Not an integer:", val)
				return false
			else:
				activeScreensList.append(val.to_int())

	screen_play_wallpaper.set_projectPath(args[1 + offset])
	screen_play_wallpaper.set_appID(args[2 + offset].replace("appID=", ""))
	screen_play_wallpaper.set_volume(float(args[3 + offset]))
	#screen_play_wallpaper.set_fillmode(int(args[4 + offset]))
	var type = args[5]  # This might need further parsing depending on its expected format
	screen_play_wallpaper.set_checkWallpaperVisible(args[6 + offset].to_lower() == "true")
	screen_play_wallpaper.set_activeScreensList(activeScreensList)

	# Print or use the parsed values as needed
	print("Parsing done:", activeScreensList, screen_play_wallpaper.get_projectPath(), screen_play_wallpaper.get_appID(), screen_play_wallpaper.get_volume(), type, screen_play_wallpaper.get_checkWallpaperVisible())
	return true
