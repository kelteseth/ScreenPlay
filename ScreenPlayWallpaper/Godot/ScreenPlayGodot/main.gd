extends Node3D

@onready var screen_play_wallpaper: ScreenPlayGodotWallpaper = $ScreenPlayGodotWallpaper
@onready var ping_alive_timer: Timer = $PingAliveTimer
@onready var check_messages_timer: Timer = $CheckMessagesTimer
var send_welcome: bool = false

# Pings main ScreenPlay application that
# this wallpaper is still active
func ping_alive():
	var success = screen_play_wallpaper.send_ping()
	if not success:
		terminate()

func terminate():
	var exit = screen_play_wallpaper.exit()
	print("exit ", exit)
	get_tree().quit()
	
# Checks for messages from the main ScreenPlay instance 
# for example for propery changes or commands like quit
func check_messages():
	var msg = screen_play_wallpaper.read_from_pipe()
	if not msg.is_empty():
		if "quit" in msg:
			return terminate()
			

func _ready():
	ping_alive_timer.wait_time = 0.5
	ping_alive_timer.timeout.connect(ping_alive)
	
	check_messages_timer.wait_time = 0.5
	check_messages_timer.timeout.connect(check_messages)
	
	if not screen_play_wallpaper:
		printerr("ERROR INVALID SCREENPLAY OBJECT")
		return
		
	var path
	if not parse_args():
		get_tree().quit()
		return

	path = screen_play_wallpaper.get_projectPath() + "/" + screen_play_wallpaper.get_projectPackageFile()

	if not load_scene(path):
		print("Failed to load the PCK file.")
		# No not call terminate here because we did not
		# yet setup via screenplay_manager.init()
		get_tree().quit()
		return
	Engine.set_max_fps(144)
	
	var ok = screen_play_wallpaper.init(screen_play_wallpaper.get_activeScreensList()[0])
	if not ok:
		printerr("Unable to setup screen")
		get_tree().quit()
		return
	if not screen_play_wallpaper.get_pipeConnected():
		var _ok_connect_to_named_pipe = screen_play_wallpaper.connect_to_named_pipe()
		
func _process(_delta):
	
	if not send_welcome:
		if not screen_play_wallpaper.get_screenPlayConnected():
			send_welcome = screen_play_wallpaper.send_welcome()
			if send_welcome:
				check_messages_timer.start()
				ping_alive_timer.start()


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
	
	# Check if only the default argument is provided
	if args.size() == 1:
		args = ["--projectpath", 
				#"C:/Code/cpp/ScreenPlay/ScreenPlay/Content/wallpaper_godot_fjord", 
				"C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/2023_12_08_205323",
				"--appID", "qmz9lq4wglox5DdYaXumVgRSDeZYAUjC", 
				"--screens", "{0}", 
				"--volume", "1", 
				"--check", "0",
				"--projectPackageFile","project-v1.zip"
				]
				
	 # Remove the first argument if it's the main.tscn file
	if args.size() > 0 and args[0] == "res://main.tscn":
		var new_args = []
		for i in range(1, args.size()):
			new_args.append(args[i])
		args = new_args
	
	# Create a dictionary to hold the key-value pairs
	var arg_dict = {}
	var i = 0
	while i < args.size():
		var key = args[i]
		if key.begins_with("--"):
			key = key.substr(2)  # Remove '--' prefix
			i += 1
			if i < args.size():
				arg_dict[key] = args[i]
		i += 1

	# Check for all required arguments
	var required_args = ["screens", "projectpath", "appID", "volume", "check", "projectPackageFile"]
	for req_arg in required_args:
		if not arg_dict.has(req_arg):
			print("Missing argument:", req_arg)
			return false

	# Parse the 'screens' argument
	var activeScreensList = []
	var screens_str = arg_dict["screens"]
	if screens_str.begins_with("{") and screens_str.ends_with("}"):
		screens_str = screens_str.substr(1, screens_str.length() - 2)  # Remove '{' and '}'
		var screens = screens_str.split(",")
		for screen in screens:
			if screen.is_valid_int():
				activeScreensList.append(screen.to_int())
			else:
				print("Invalid screens argument:", screen)
				return false
	else:
		print("Invalid format for screens argument")
		return false

	# Assign the values to the respective properties
	screen_play_wallpaper.set_projectPath(arg_dict["projectpath"])
	screen_play_wallpaper.set_appID(arg_dict["appID"])
	screen_play_wallpaper.set_volume(float(arg_dict["volume"]))
	screen_play_wallpaper.set_projectPackageFile(arg_dict["projectPackageFile"])
	screen_play_wallpaper.set_checkWallpaperVisible(arg_dict["check"] == "1")
	screen_play_wallpaper.set_activeScreensList(activeScreensList)

	# Print or use the parsed values as needed
	print("Parsing done: ", activeScreensList, 
		  " ", screen_play_wallpaper.get_projectPath(), 
		  " ", screen_play_wallpaper.get_appID(),
		  " ", screen_play_wallpaper.get_volume(),
		  " ", screen_play_wallpaper.get_projectPackageFile(),
		  " ", screen_play_wallpaper.get_checkWallpaperVisible())

	return true
