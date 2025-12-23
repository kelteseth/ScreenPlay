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
	screen_play_wallpaper.exit()
	get_tree().quit()

# Checks for messages from the main ScreenPlay instance 
# for example for propery changes or commands like quit
func check_messages():
	var msg = screen_play_wallpaper.read_from_pipe()
	if msg.is_empty():
		return
	
	# Echo back the received message to Qt for debugging
	var echo_msg = "ECHO_FROM_GODOT:" + msg
	screen_play_wallpaper.writeToPipe(echo_msg)
	print("Received and echoed message: ", msg)
	
	var json_parser = JSON.new()
	var status = json_parser.parse(msg)
	if status == OK:
		var data = json_parser.get_data()
		print("New message: ", data)
		if typeof(data) == TYPE_DICTIONARY:
			# Handle quit command
			if data.has("command") and data["command"] == "quit":
				return terminate()
			
			# Handle property changes
			if data.has("godotFps"):
				var fps_map = {
					"Fps1": "1",
					"Fps6": "6",
					"Fps12": "12",
					"Fps24": "24",
					"Fps30": "30",
					"Fps60": "60",
					"Fps120": "120",
					"Fps144": "144",
					"Unlimited": "0",
					"Vsync": "vsync"
				}
				var fps_value = str(data["godotFps"])
				if fps_map.has(fps_value):
					apply_fps(fps_map[fps_value])
				else:
					print("Unknown godotFps value: ", fps_value)
			
			if data.has("godot3DScale"):
				var scale_value = float(data["godot3DScale"])
				apply_3d_scale(scale_value)
			
			if data.has("godot3DScaleMode"):
				var mode_map = {
					"Bilinear": "0",
					"FSR1_0": "1",
					"FSR2_2": "2"
				}
				var mode_value = str(data["godot3DScaleMode"])
				if mode_map.has(mode_value):
					apply_3d_scale_mode(mode_map[mode_value])
				else:
					print("Unknown godot3DScaleMode value: ", mode_value)
			
func apply_fps(fps_value: String):
	if fps_value == "vsync":
		DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_ENABLED)
		print("VSync enabled")
	elif fps_value == "0":
		Engine.set_max_fps(0)  # Unlimited FPS
		print("FPS set to unlimited")
	elif fps_value.is_valid_int():
		var fps_int = fps_value.to_int()
		Engine.set_max_fps(fps_int)
		print("FPS set to: ", fps_int)
	else:
		print("Invalid fps value, using default 60")
		Engine.set_max_fps(60)

func apply_3d_scale(scale_value: float):
	if scale_value >= 0.25 and scale_value <= 2.0:
		get_viewport().scaling_3d_scale = scale_value
		print("3D render scale set to: ", scale_value)
	else:
		print("Invalid 3d-scale value (must be 0.25-2.0), ignoring")

func apply_3d_scale_mode(mode_value: String):
	if mode_value == "0":
		get_viewport().scaling_3d_mode = Viewport.SCALING_3D_MODE_BILINEAR
		print("3D scaling mode set to: Bilinear")
	elif mode_value == "1":
		get_viewport().scaling_3d_mode = Viewport.SCALING_3D_MODE_FSR
		print("3D scaling mode set to: FSR 1.0")
	elif mode_value == "2":
		get_viewport().scaling_3d_mode = Viewport.SCALING_3D_MODE_FSR2
		print("3D scaling mode set to: FSR 2.2")
	else:
		print("Invalid 3d-scale-mode value, ignoring")

func _on_scene_value_received(key: String, value: String):
	match key:
		"godotFps":
			# Convert enum value to fps string
			var fps_map = {
				"Fps1": "1",
				"Fps6": "6",
				"Fps12": "12",
				"Fps24": "24",
				"Fps30": "30",
				"Fps60": "60",
				"Fps120": "120",
				"Fps144": "144",
				"Unlimited": "0",
				"Vsync": "vsync"
			}
			if fps_map.has(value):
				apply_fps(fps_map[value])
			else:
				print("Unknown godotFps value: ", value)
		
		"godot3DScale":
			var scale_float = value.to_float()
			apply_3d_scale(scale_float)
		
		"godot3DScaleMode":
			# Convert enum value to mode string
			var mode_map = {
				"Bilinear": "0",
				"FSR1_0": "1",
				"FSR2_2": "2"
			}
			if mode_map.has(value):
				apply_3d_scale_mode(mode_map[value])
			else:
				print("Unknown godot3DScaleMode value: ", value)
		
		_:
			print("Unhandled property change: ", key, " = ", value)
	
func _ready():
	ping_alive_timer.wait_time = 0.5
	ping_alive_timer.timeout.connect(ping_alive)
	
	check_messages_timer.wait_time = 0.5
	check_messages_timer.timeout.connect(check_messages)
	
	screen_play_wallpaper.scene_value_received.connect(_on_scene_value_received)
	
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
	if args.size() == 2:
		args = ["--projectpath", 
				#"C:/Code/cpp/ScreenPlay/ScreenPlay/Content/wallpaper_godot_fjord", 
				"C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/2025_11_10_085058",
				"--appID", "qmz9lq4wglox5DdYaXumVgRSDeZYAUjC", 
				"--screens", "{0}", 
				"--volume", "1", 
				"--check", "0",
				"--projectPackageFile","project-v1.zip",
				"--fps", "60"
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

	# Handle optional fps argument
	if arg_dict.has("fps"):
		apply_fps(arg_dict["fps"])
	else:
		# Default to 60 FPS if no fps argument provided
		Engine.set_max_fps(60)
		print("No fps argument provided, using default 60 FPS")

	# Handle optional 3D scale argument
	if arg_dict.has("3d-scale"):
		var scale_value = arg_dict["3d-scale"].to_float()
		apply_3d_scale(scale_value)
	else:
		# Default to 1.0 if no 3d-scale argument provided
		get_viewport().scaling_3d_scale = 1.0
		print("No 3d-scale argument provided, using default 1.0")

	# Handle optional 3D scale mode argument
	if arg_dict.has("3d-scale-mode"):
		apply_3d_scale_mode(arg_dict["3d-scale-mode"])
	else:
		# Default to Bilinear if no 3d-scale-mode argument provided
		get_viewport().scaling_3d_mode = Viewport.SCALING_3D_MODE_BILINEAR
		print("No 3d-scale-mode argument provided, using default Bilinear")

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
