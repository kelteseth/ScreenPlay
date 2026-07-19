extends Node3D

@onready var screen_play_wallpaper: ScreenPlayGodotWallpaper = $ScreenPlayGodotWallpaper
@onready var ping_alive_timer: Timer = $PingAliveTimer
@onready var check_messages_timer: Timer = $CheckMessagesTimer
@onready var file_watch_timer: Timer = $FileWatchTimer

var send_welcome: bool = false
var last_pck_modified_time: int = 0
var loaded_scene_instance: Node = null
var cmdline_args: PackedStringArray = []

# Guards against overlapping reloads (e.g. the file watcher firing while a
# timeline-switch reload is still in flight).
var is_reloading: bool = false

# Crossfade overlay: a snapshot of the last rendered frame is shown on top and
# faded out to reveal the freshly loaded scene, mirroring the video crossfade.
const TRANSITION_DURATION: float = 0.4
var transition_layer: CanvasLayer = null
var transition_rect: TextureRect = null

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
	# read_from_pipe() returns one complete message per call (the C++ side
	# reassembles partial/coalesced pipe reads) - drain them all this tick.
	# NOTE: never write raw text back into the pipe here: unframed bytes
	# corrupt the shared frame stream on the ScreenPlay side.
	while true:
		var msg = screen_play_wallpaper.read_from_pipe()
		if msg.is_empty():
			return
		_handle_message(msg)

# Maps the enum key names sent by the main app (e.g. "Fps60", "Bilinear") to
# the numeric strings the native wrapper stores.
const FPS_MAP := {
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

const SCALE_MODE_MAP := {
	"Bilinear": "0",
	"FSR1_0": "1",
	"FSR2_2": "2"
}

func _handle_message(msg: String):
	var json_parser = JSON.new()
	var status = json_parser.parse(msg)
	if status != OK:
		return

	var data = json_parser.get_data()
	print("New message: ", data)
	if typeof(data) != TYPE_DICTIONARY:
		return

	# Commands are dispatched explicitly and never fall through to the
	# generic property handlers below.
	if data.has("command"):
		match str(data["command"]):
			"quit":
				terminate()
			"replace":
				_handle_replace(data)
			_:
				print("Unknown command: ", data["command"])
		return

	# Live single-property changes arrive without a command field.
	_apply_properties(data)

## Applies any Godot settings present in a message (fps, 3D scale, scale mode).
## Shared by the live-property path and the replace command.
func _apply_properties(data: Dictionary) -> void:
	if data.has("godotFps"):
		_set_fps_from_enum(str(data["godotFps"]))

	if data.has("godot3DScale"):
		screen_play_wallpaper.set_scale3d(float(data["godot3DScale"]))
		apply_3d_scale(screen_play_wallpaper.get_scale3d())

	if data.has("godot3DScaleMode"):
		_set_scale_mode_from_enum(str(data["godot3DScaleMode"]))

## Handles a full wallpaper swap on a Godot->Godot timeline switch. Mirrors the
## Qt SDK's "replace" command: update the content path, apply the settings that
## ride along in the message, and reload the scene if the content changed.
func _handle_replace(data: Dictionary) -> void:
	var content_changed := false

	if data.has("absolutePath"):
		var new_path := str(data["absolutePath"])
		if not new_path.is_empty() and new_path != screen_play_wallpaper.get_projectPath():
			screen_play_wallpaper.set_projectPath(new_path)
			content_changed = true

	if data.has("projectPackageFile"):
		var new_package := str(data["projectPackageFile"])
		if not new_package.is_empty() and new_package != screen_play_wallpaper.get_projectPackageFile():
			screen_play_wallpaper.set_projectPackageFile(new_package)
			content_changed = true

	# Apply settings first so reload_wallpaper() re-applies them to the new scene.
	_apply_properties(data)

	if content_changed:
		# Sync the watcher up-front so it doesn't race this reload and fire a
		# redundant one when it notices the (expected) path change.
		last_pck_modified_time = FileAccess.get_modified_time(screen_play_wallpaper.get_fullPckPath())
		reload_wallpaper()

func _set_fps_from_enum(enum_key: String) -> void:
	if FPS_MAP.has(enum_key):
		screen_play_wallpaper.set_fps(FPS_MAP[enum_key])
		apply_fps(screen_play_wallpaper.get_fps())
	else:
		print("Unknown godotFps value: ", enum_key)

func _set_scale_mode_from_enum(enum_key: String) -> void:
	if SCALE_MODE_MAP.has(enum_key):
		screen_play_wallpaper.set_scale3dMode(SCALE_MODE_MAP[enum_key])
		apply_3d_scale_mode(screen_play_wallpaper.get_scale3dMode())
	else:
		print("Unknown godot3DScaleMode value: ", enum_key)

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

## Checks if the PCK file has been modified since last load
func check_file_changes():
	var pck_path = screen_play_wallpaper.get_fullPckPath()
	if pck_path.is_empty():
		return
	
	var current_modified_time = FileAccess.get_modified_time(pck_path)
	if current_modified_time == 0:
		return  # File doesn't exist or error
	
	if last_pck_modified_time > 0 and current_modified_time != last_pck_modified_time:
		print("PCK file changed, reloading wallpaper...")
		reload_wallpaper()
	
	last_pck_modified_time = current_modified_time

## Unloads the current scene and reloads the PCK, crossfading from the previous
## frame to the newly loaded scene.
func reload_wallpaper():
	if is_reloading:
		return
	is_reloading = true

	var pck_path = screen_play_wallpaper.get_fullPckPath()
	print("Reloading wallpaper from: ", pck_path)

	# Snapshot the current frame so it can be faded out over the new scene.
	_capture_transition_overlay()

	# Unload current scene
	unload_scene()

	# Small delay to ensure resources are freed
	await get_tree().process_frame
	await get_tree().process_frame

	# Load new scene
	if not load_scene(pck_path):
		print("Failed to reload wallpaper scene")
		_fade_out_transition()
		# Keep the watcher in sync so it does not immediately retry the reload.
		last_pck_modified_time = FileAccess.get_modified_time(pck_path)
		is_reloading = false
		return

	# Reapply current settings
	apply_all_settings()

	# Let the new scene render a couple of frames before revealing it, so the
	# crossfade lands on a real frame instead of a black one.
	await get_tree().process_frame
	await get_tree().process_frame
	_fade_out_transition()

	# Keep the watcher's timestamp in sync with the file we just loaded so it
	# does not detect a "change" and reload a second time.
	last_pck_modified_time = FileAccess.get_modified_time(pck_path)
	print("Wallpaper reloaded successfully")
	is_reloading = false

## Captures the current viewport frame into a full-screen overlay on top of the
## scene, ready to be faded out by _fade_out_transition().
func _capture_transition_overlay() -> void:
	var viewport_texture := get_viewport().get_texture()
	if viewport_texture == null:
		return
	var img := viewport_texture.get_image()
	if img == null or img.is_empty():
		return
	var tex := ImageTexture.create_from_image(img)

	if transition_layer == null:
		transition_layer = CanvasLayer.new()
		transition_layer.layer = 128  # Render above the wallpaper scene.
		add_child(transition_layer)
		transition_rect = TextureRect.new()
		transition_rect.set_anchors_preset(Control.PRESET_FULL_RECT)
		transition_rect.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
		transition_rect.stretch_mode = TextureRect.STRETCH_SCALE
		transition_layer.add_child(transition_rect)

	transition_rect.texture = tex
	transition_rect.modulate = Color(1, 1, 1, 1)
	transition_rect.visible = true

## Fades the captured overlay out, revealing the new scene underneath.
func _fade_out_transition() -> void:
	if transition_rect == null or not transition_rect.visible:
		return
	var tween := create_tween()
	tween.tween_property(transition_rect, "modulate:a", 0.0, TRANSITION_DURATION)
	await tween.finished
	transition_rect.visible = false

## Unloads the currently loaded wallpaper scene
func unload_scene():
	if loaded_scene_instance != null and is_instance_valid(loaded_scene_instance):
		print("Unloading current scene...")
		loaded_scene_instance.queue_free()
		loaded_scene_instance = null
		# Force garbage collection of resources
		await get_tree().process_frame

## Applies all current settings (used after reload)
func apply_all_settings():
	apply_fps(screen_play_wallpaper.get_fps())
	apply_3d_scale(screen_play_wallpaper.get_scale3d())
	apply_3d_scale_mode(screen_play_wallpaper.get_scale3dMode())

## Applies initial settings from parsed arguments (call after init)
func apply_initial_settings(arg_dict: Dictionary):
	# Handle optional fps argument
	if arg_dict.has("fps"):
		screen_play_wallpaper.set_fps(arg_dict["fps"])
		apply_fps(screen_play_wallpaper.get_fps())
	else:
		screen_play_wallpaper.set_fps("60")
		Engine.set_max_fps(60)
		print("No fps argument provided, using default 60 FPS")

	# Handle optional 3D scale argument
	if arg_dict.has("3d-scale"):
		screen_play_wallpaper.set_scale3d(arg_dict["3d-scale"].to_float())
		apply_3d_scale(screen_play_wallpaper.get_scale3d())
	else:
		screen_play_wallpaper.set_scale3d(1.0)
		get_viewport().scaling_3d_scale = 1.0
		print("No 3d-scale argument provided, using default 1.0")

	# Handle optional 3D scale mode argument
	if arg_dict.has("3d-scale-mode"):
		screen_play_wallpaper.set_scale3dMode(arg_dict["3d-scale-mode"])
		apply_3d_scale_mode(screen_play_wallpaper.get_scale3dMode())
	else:
		screen_play_wallpaper.set_scale3dMode("0")
		get_viewport().scaling_3d_mode = Viewport.SCALING_3D_MODE_BILINEAR
		print("No 3d-scale-mode argument provided, using default Bilinear")

func _ready():
	ping_alive_timer.wait_time = 0.5
	ping_alive_timer.timeout.connect(ping_alive)
	
	check_messages_timer.wait_time = 0.5
	check_messages_timer.timeout.connect(check_messages)
	
	file_watch_timer.wait_time = 1.0  # Check every second
	file_watch_timer.timeout.connect(check_file_changes)

	if not screen_play_wallpaper:
		printerr("ERROR INVALID SCREENPLAY OBJECT")
		return

	var arg_dict = parse_args()
	if arg_dict.is_empty():
		get_tree().quit()
		return

	var path = screen_play_wallpaper.get_fullPckPath()
	
	# Store initial modification time
	last_pck_modified_time = FileAccess.get_modified_time(path)

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
	
	# Apply settings after init() is called
	apply_initial_settings(arg_dict)
	
	if not screen_play_wallpaper.get_pipeConnected():
		var _ok_connect_to_named_pipe = screen_play_wallpaper.connect_to_named_pipe()
	
	# Start file watching
	file_watch_timer.start()
		
func _process(_delta):
	if not send_welcome:
		if not screen_play_wallpaper.get_screenPlayConnected():
			send_welcome = screen_play_wallpaper.send_welcome()
			if send_welcome:
				check_messages_timer.start()
				ping_alive_timer.start()


func load_scene(path: String) -> bool:
	var success = ProjectSettings.load_resource_pack(path)
	if success:
		var scene_resource = load("res://wallpaper.tscn")
		if scene_resource:
			var scene_instance = scene_resource.instantiate()
			if scene_instance:
				add_child(scene_instance)
				loaded_scene_instance = scene_instance
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

func parse_args() -> Dictionary:
	cmdline_args = OS.get_cmdline_args()
	print("Parse args:", cmdline_args)
	
	# Override with test args if only the default argument is provided
	if cmdline_args.size() == 2:
		cmdline_args = PackedStringArray([
			"--projectpath", 
			"C:/Program Files (x86)/Steam/steamapps/workshop/content/672870/2025_11_10_085058",
			"--appID", "qmz9lq4wglox5DdYaXumVgRSDeZYAUjC", 
			"--screens", "{0}", 
			"--volume", "1", 
			"--check", "0",
			"--projectPackageFile", "project-v1.zip",
			"--fps", "60",
			"--3d-scale", "1.0",
			"--3d-scale-mode", "0"
		])
	
	# Convert to regular array for processing
	var args: Array = Array(cmdline_args)
				
	# Remove the first argument if it's the main.tscn file
	if args.size() > 0 and args[0] == "res://main.tscn":
		args = args.slice(1)
	
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

	print("Parsed arg_dict: ", arg_dict)

	# Check for all required arguments
	var required_args = ["screens", "projectpath", "appID", "volume", "check", "projectPackageFile"]
	for req_arg in required_args:
		if not arg_dict.has(req_arg):
			print("Missing argument:", req_arg)
			return {}

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
				return {}
	else:
		print("Invalid format for screens argument")
		return {}

	# Store all settings directly in screen_play_wallpaper
	screen_play_wallpaper.set_projectPath(arg_dict["projectpath"])
	screen_play_wallpaper.set_appID(arg_dict["appID"])
	screen_play_wallpaper.set_volume(float(arg_dict["volume"]))
	screen_play_wallpaper.set_projectPackageFile(arg_dict["projectPackageFile"])
	screen_play_wallpaper.set_checkWallpaperVisible(arg_dict["check"] == "1")
	screen_play_wallpaper.set_activeScreensList(activeScreensList)

	# Print or use the parsed values as needed
	print("Parsing done: ", screen_play_wallpaper.get_activeScreensList(), 
		  " ", screen_play_wallpaper.get_projectPath(), 
		  " ", screen_play_wallpaper.get_appID(),
		  " ", screen_play_wallpaper.get_volume(),
		  " ", screen_play_wallpaper.get_projectPackageFile(),
		  " ", screen_play_wallpaper.get_checkWallpaperVisible())

	return arg_dict
