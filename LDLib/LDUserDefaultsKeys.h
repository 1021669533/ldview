#ifndef __LDLUSERDEFAULTSKEYS_H__
#define __LDLUSERDEFAULTSKEYS_H__

#define PRIMITIVE_SUBSTITUTION_KEY "AllowPrimitiveSubstitution"
#define FSAA_MODE_KEY "Antialias"
#define AUTO_CROP_KEY "AutoCrop"
#define BACKGROUND_COLOR_KEY "BackgroundColor3"
#define BFC_KEY "BFC"
#define BLACK_HIGHLIGHTS_KEY "BlackHighlights"
#define BOTTOM_MARGIN_KEY "BottomMargin"
#define CHECK_PART_TRACKER_KEY "CheckPartTracker"
#define CHECK_PART_UPDATE_WAIT_KEY "CheckPartUpdateWait"
#define CHECK_PART_WAIT_KEY "CheckPartWait"
#define CONDITIONAL_HIGHLIGHTS_KEY "ConditionalHighlights"
#define CURVE_QUALITY_KEY "CurveQuality"
#define CUSTOM_COLORS_KEY "CustomColors"
#define CUTAWAY_ALPHA_KEY "CutawayAlpha"
#define CUTAWAY_MODE_KEY "CutawayMode"
#define CUTAWAY_THICKNESS_KEY "CutawayThickness"
#define DEFAULT_COLOR_KEY "DefaultColor3"
#define DEFAULT_LAT_LONG_KEY "DefaultLatLong"
#define MODEL_CENTER_KEY "ModelCenter"
#define MODEL_SIZE_KEY "ModelSize"
#define DEFAULT_MATRIX_KEY "DefaultMatrix"
#define DEFAULT_ZOOM_KEY "DefaultZoom"							// NO UI
#define EDITOR_KEY "Editor"
#define EDGES_ONLY_KEY "EdgesOnly"
#define EDGE_THICKNESS_KEY "EdgeThickness"
#define EXTRA_SEARCH_DIRS_KEY "ExtraSearchDirs"
#define FOV_KEY "FOV"
#define FULLSCREEN_DEPTH_KEY "FullscreenDepth"
#define FULLSCREEN_HEIGHT_KEY "FullscreenHeight"
#define FULLSCREEN_REFRESH_KEY "FullscreenRefresh"
#define FULLSCREEN_WIDTH_KEY "FullscreenWidth"
#define GREEN_FRONT_FACES_KEY "GreenFrontFaces"
#define HI_RES_PRIMITIVES_KEY "HiResPrimitives"
#define IGNORE_ALL_OGL_EXTENSIONS "IgnoreAllOGLExtensions"
#define IGNORE_MULTI_DRAW_ARRAYS_KEY "IgnoreMultiDrawArrays"
#define IGNORE_MULTISAMPLE_KEY "IgnoreMultisample"
#define IGNORE_MS_FILTER_HINT_KEY "IgnoreMultisampleFilterHint"
#define IGNORE_PBUFFER_KEY "IgnorePBuffer"
#define IGNORE_PIXEL_FORMAT_KEY "IgnorePixelFormat"
#define IGNORE_VAR_KEY "IgnoreVAR"
#define IGNORE_VBO_KEY "IgnoreVBO"
#define INSTALL_PATH_KEY "InstallPath"							// NO UI
#define LAST_OPEN_PATH_KEY "LastOpenPath"
#define LDRAWDIR_KEY "LDrawDir"
#define LEFT_MARGIN_KEY "LeftMargin"
#define LIGHTING_KEY "Lighting"
#define LINE_SMOOTHING_KEY "LineSmoothing"
#define MAX_RECENT_FILES_KEY "MaxRecentFiles"
#define MEMORY_USAGE_KEY "MemoryUsage"
#define ONE_LIGHT_KEY "OneLight"
#define ORIENTATION_KEY "Orientation"
#define PAPER_SIZE_KEY "PaperSize"
#define PERFORM_SMOOTHING_KEY "PerformSmoothing"
#define POLL_KEY "Poll"
#define POLYGON_OFFSET_KEY "PolygonOffset"
#define PRINT_BACKGROUND_KEY "PrintBackground"
#define PROCESS_LDCONFIG_KEY "ProcessLDConfig"
#define PROXY_PORT_KEY "ProxyPort"
#define PROXY_SERVER_KEY "ProxyServer"
#define PROXY_TYPE_KEY "ProxyType"
#define RECENT_FILES_KEY "RecentFiles"
#define RED_BACK_FACES_KEY "RedBackFaces"
#define REMOVE_HIDDEN_LINES_KEY "RemoveHiddenLines"
#define RIGHT_MARGIN_KEY "RightMargin"
#define SAVE_ACTUAL_SIZE_KEY "SaveActualSize"
#define SAVE_ALPHA_KEY "SaveAlpha"
#define SAVE_DIGITS_KEY "SaveDigits"
#define SAVE_HEIGHT_KEY "SaveHeight"
#define SAVE_IMAGE_TYPE_KEY "SaveImageType"
#define SAVE_SERIES_KEY "SaveSeries"
#define SAVE_SNAPSHOT_KEY "SaveSnapshot"						// NO UI
#define SAVE_WIDTH_KEY "SaveWidth"
#define SAVE_ZOOM_TO_FIT_KEY "SaveZoomToFit"
#define SEAMS_KEY "Seams"
#define SEAM_WIDTH_KEY "SeamWidth"
#define SHOW_ALL_TYPE5_KEY "ShowAllType5"
#define SHOW_ERRORS_KEY "ShowErrors"
#define SHOW_FPS_KEY "ShowFPS"
#define SHOW_HIGHLIGHT_LINES_KEY "ShowHighlightLines"
#define SHOW_TYPE5_CONTROL_POINTS_KEY "ShowType5ControlPoints"
#define SHOW_WARNINGS_KEY "ShowWarnings"
#define SORT_KEY "SortTransparent"
#define STATUS_BAR_KEY "StatusBar"
#define STEREO_MODE_KEY "StereoMode"
#define STEREO_SPACING_KEY "StereoSpacing"
#define SUBDUED_LIGHTING_KEY "SubduedLighting"
#define TEXTURE_FILTER_TYPE_KEY "TextureFilterType"
#define TEXTURE_STUDS_KEY "TextureStuds"
#define TOOLBAR_KEY "Toolbar"
#define TOP_MARGIN_KEY "TopMargin"
#define TOPMOST_KEY "Topmost"
#define TRANS_DEFAULT_COLOR_KEY "TransDefaultColor"
#define FLAT_SHADING_KEY "UseFlatShading"
#define QUALITY_LIGHTING_KEY "UseQualityLighting"
#define QUALITY_STUDS_KEY "UseQualityStuds"
#define SPECULAR_KEY "UseSpecular"
#define STIPPLE_KEY "UseStipple"
#define VERIFY_LDRAW_DIR_KEY "VerifyLDrawDir"
#define VIEW_MODE_KEY "ViewMode"
#define VISUAL_STYLE_ENABLED_KEY "VisualStyleEnabled"
#define WINDOW_HEIGHT_KEY "WindowHeight"
#define WINDOW_MAXIMIZED_KEY "WindowMaximized"
#define WINDOW_WIDTH_KEY "WindowWidth"
#define WIREFRAME_KEY "Wireframe"
#define WIREFRAME_FOG_KEY "WireframeFog"
#define WIREFRAME_THICKNESS_KEY "WireframeThickness"
#define ZOOM_MAX_KEY "ZoomMax"									// NO UI

// The following are undocumented, and should stay that way.
#define DEBUG_LEVEL_KEY "DebugLevel"
#define DEFAULT_COLOR_NUMBER_KEY "DefaultColorNumber"
#define HOT_KEYS_KEY "HotKeys"
#define PREFERENCE_SET_KEY "PreferenceSet"
#define PRINT_DPI_KEY "PrintDPI"								// NOT USED
#define USE_PRINTER_DPI_KEY "UsePrinterDPI"						// NOT USED
#define LAST_LIBRARY_UPDATE_KEY "LastLibraryUpdate"

// The following was added to improve load performance.  Since it didn't really
// help much, I decided not to publicize it, so it will probably never be
// documented.
#define SKIP_VALIDATION_KEY "SkipValidation"

// The following are undocumented, but need to be documented.
#define LIGHT_VECTOR_KEY "LightVector"

// The following are undocumented, but perhaps need to be documented.
#define SS_SIZE_KEY "SSSize"
#define SS_SPEED_KEY "SSSpeed"
#define SS_ROTATION_SPEED_KEY "SSRotationSpeed"
#define SS_FILENAME_KEY "SSFilename"
#define SS_DIRECTORY_KEY "SSDirectory"
#define SS_LAST_OPEN_PATH_KEY "SSLastOpenPath"
#define SS_FILE_MODE_KEY "SSFileMode"
#define SS_SLEEP_WORKAROUND_KEY "SSSleepWorkaround"
#define SS_RANDOM_PREF_SET_KEY "SSRandomPrefSet"
#define HFOV_KEY "HFOV"
#define CAMERA_GLOBE_KEY "CameraGlobe"

#endif // __LDLUSERDEFAULTSKEYS_H__
