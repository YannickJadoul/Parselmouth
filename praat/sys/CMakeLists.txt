target_sources(praat PRIVATE
	Thing.cpp Data.cpp Simple.cpp Collection.cpp Strings.cpp
	Graphics.cpp Graphics_linesAndAreas.cpp Graphics_text.cpp Graphics_colour.cpp
	Graphics_image.cpp Graphics_record.cpp
	Graphics_utils.cpp Graphics_grey.cpp Graphics_altitude.cpp
	GraphicsPostscript.cpp Graphics_surface.cpp
	ManPage.cpp ManPages.cpp Script.cpp machine.cpp
	GraphicsScreen.cpp Printer.cpp
	Preferences.cpp site.cpp
	Picture.cpp Ui.cpp UiFile.cpp UiPause.cpp Editor.cpp DataEditor.cpp HyperPage.cpp Manual.cpp TextEditor.cpp
	praat.cpp praat_actions.cpp praat_menuCommands.cpp praat_picture.cpp sendpraat.c sendsocket.c
	praat_script.cpp praat_statistics.cpp praat_logo.cpp praat_library.cpp
	praat_objectMenus.cpp InfoEditor.cpp ScriptEditor.cpp ButtonEditor.cpp Interpreter.cpp Formula.cpp
	StringsEditor.cpp DemoEditor.cpp
	motifEmulator.cpp GuiText.cpp GuiWindow.cpp Gui.cpp GuiObject.cpp GuiDrawingArea.cpp
	GuiMenu.cpp GuiMenuItem.cpp GuiButton.cpp GuiLabel.cpp GuiCheckButton.cpp GuiRadioButton.cpp
	GuiDialog.cpp GuiList.cpp GuiFileSelect.cpp GuiScale.cpp GuiScrollBar.cpp GuiScrolledWindow.cpp
	GuiControl.cpp GuiForm.cpp GuiOptionMenu.cpp GuiProgressBar.cpp GuiShell.cpp GuiThing.cpp Gui_messages.cpp
)
