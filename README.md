<p align="center">
    <img src="icon/katoob-large.png">
</p>

# Katoob

Katoob is a lightweight, multilingual and bidirectional-aware unicode text editor written
in C++ using [Gtkmm](https://gtkmm.org).

## Copyright & License

* Katoob, Copyright © 2008-2018 Frederic-Gerald Morcos.
* Katoob, Copyright © 2002-2007 Mohammed Sameer.

This program is free software; you can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

See the file `LICENSE` for details.

## Authors

* Author and maintainer (2008-2021): Fred Morcos <fm+Katoob@fredmorcos.com>
* Original author (2002-2007): Mohammed Sameer <msameer@foolab.org>

## About

Katoob is a lightweight multilingual BiDi aware text editor. It supports opening and
saving files in multiple encodings.  The main support is some for Arabic language specific
features.

Katoob features a traditional, easily discoverable user interface with menus and toolbars,
à la Gtk+ and Gnome 2. Even though there are plans to port Katoob to Gtk 3 and eventually
Gtk 4, the user interface will keep the advantageous properties of traditional menus,
toolbars and status bars.

It should run on the BSDs, Linuxes, POSIX-compatible operating systems and
Windows. However, Windows support is currently untested. For the time being, its best bet
to run on Windows would be using the [Windows Subsystem for
Linux](https://docs.microsoft.com/en-us/windows/wsl/about).

## Features

* Multiple codepage support, not just UTF-8.
* The ability to Open/Save files in multiple encodings.
* Search/Replace/Replace all.
* Configurable Undo/Redo.
* Follows the XDG specification for cache and config files.
* Read from stdin via a pipe, e.g. `cat file | katoob -`.
* Open multiple files from the command line.
* An MDI Interface.
* Line wrapping.
* Left and right line numbers.
* The ability to select the toolbar style.
* Recent files menu.
* An encodings menu to change the encoding on the fly.
* The statusbar has an indicator weather the file is modified.
* Read-only files can't be modified.
* Display the used encoding in the status-bar.
* Gnome 2 human interface guidelines compliant interface.
* Desktop menu entry.
* Syntax highlighting via GtkSourceView.
* On-the-fly and on-demand spell-checking.
* Per document settings on the fly.
* A configurable extended toolbar offering some common functions.
* A keyboard emulator.
* Configurable multipress for different languages.
* Auto-save.
* Support recovery of files in case of a crash.

## Screenshots

<p align="center">
    <img src="screenshots/Screenshot_2021-11-07_20-42-37.png">
</p>

## Requirements

Katoob is being developed and modernized on [Archlinux](https://archlinux.org/). It
originally was being developed on [Debian GNU/Linux](https://www.debian.org/) but it
should work on any Linux distro, the BSDs and perhaps even MacOS and Windows.

### Dependencies

* gtkmm-2.4
* glibmm-2.4
* giomm-2.4
* gtksourceview-2.0
* enchant-2
* dbus-1
* dbus-glib-1
* expat
* libcurl
* zlib
* shared-mime-info
* isocodes

### Building and Installation

After downloading Katoob or cloning this repository:

```sh
cd katoob-VERSION
meson setup build --prefix /installation/prefix
cd build
ninja
```

and to install:

```sh
ninja install
```

To change the build type, change the `meson setup build` line to the following:

```sh
meson setup build --prefix /installation/prefix --buildtype debug|debugoptimized|release|minsize
```

To see the different build options, use `meson configure`.

## News and changelogs

* 2021-11-07 Fred Morcos <fm+Katoob@fredmorcos.com>
  - Version 0.5.9.2
  - Branched from 0.5.9 (skipped the broken 0.5.9.1 and 0.6.0-alpha)
  - Enchant-2
  - Much improved meson build
  - Kept Windows support (untested for the time being)
  - Removed Maemo support (long live)
  - Fixed build issues due to bitrot over the past ~13 years
  - Replace custom icon PNGs with Gtk built-ins
  - Use GIO instead of xdgmime for guessing file mimetypes
  - Fixed issue with how the spell-checking popup was being displayed
  - Katoob now respects and uses `XDG_CONFIG` and `CACHE` directories
  - Consistent formatting of files
  - Updated man-page
  - Fixed an issue with encoding conversions
  - Fixed an issue when defining words using the DICT protocol
  - Improved dialogs for problems related to the emulator and multipress
  - There are still bugs

* 2018-11-02 Fred Morcos <fm+Katoob@fredmorcos.com>
  - Version 0.6.0-alpha
  - Move from enchant-1 to enchant-2
  - Move from autotools to meson
  - Remove native Windows support (WSL can be used instead)
  - Remove Maemo support
  - Fix many build issues due to bitrot over the past 10 years
  - Use more Gtk stock icons instead of custom PNGs
  - Merge patches from Debian
  - Use GIO instead of xdgmime

## TODO

- [ ] Look into moving to `enchant++` and `gtksourceviewmm`.

- [X] Use xdg-config-dir and xdg-cache-dir for config and cache/recovery files.

- [ ] Remove all #defines.

- [ ] Get rid of `get_data_path()` and `get_data_dir()`. Use `config.h` defines instead.

- [ ] Replace `macros.h` with glib defined gettext helpers.

- [ ] Add support for users editing the extended toolbar and available multipress and
      emulator configurations.

- [X] Support build-time user-defined directories for emulator and multipress configs.

- [ ] Use gresources for application icons and emulator and multipress config files.

- [ ] Get translations working again.

- [X] Consistent code formatting with clang-format.

- [X] Use modern config and cache directories (e.g. `~/.config/katoob` and
      `~/.cache/katoob`).

- [ ] Replace hardcoded paper sizes in print dialog with something from Gtk.

- [ ] Replace usages of `Gtk::manage(new ...)` with `Gtk::make_managed<W>(...)`.

- [X] Replace the modified icons (`red.png` and `green.png`) with a Gtk widget.

- [ ] Add CI/CD pipelines for Ubuntu, Windows and MacOS.

- [ ] Port printing to be handled by `Gtk` and perhaps `cairo`.

- [ ] Fix compilation warnings.

- [ ] Fix runtime warnings.

- [ ] Grep for `TODO`s and `FIXME`s.

- [ ] Port to `Gtk3` and `gtksourceview3-mm`.

- [ ] Port to `Gtk4` and `gtksourceview4`.

- [ ] Add Emacs bindings.

- [ ] Separate and improve the usability of the search and spell checking dialogs.

- [ ] Re-design the preferences dialog.

- [ ] Improve Undo/Redo state.
  - [ ] Recognize when the document reaches back to a saved state.

- [ ] Display document information in the status bar.

- [ ] Auto-save.

- [ ] Multiple clipboards.

- [ ] Auto-wrapping.

- [ ] Insert common text like date, time, licenses, username and other options.

- [ ] Ability to change the editor's theme.

- [ ] Ability to change the color of the spell-checker's underline.

- [ ] A basic or advanced HTML Editor.

- [ ] Export to HTML.

- [ ] Plugins interface.

- [ ] Customizable toolbar.

- [ ] Git support.

- [ ] Support UNIX, Windows and MacOS line-endings.

- [ ] Load & save settings support for PrintDialog.

- [ ] Improve the encodings menu.

- [ ] Execute command on highlighted text.

- [ ] Export HTML numeric character references again.

- [ ] HTML numeric character references cover all non-latin range.

- [ ] BiDi control characters are visible.

- [ ] Shortcut for RTL emulator.

- [ ] No way to force a document to RTL or LTR.

- [ ] Do we need the Xkb switcher back?

- [ ] Complete the languages names in the encodings drop-down menu.

- [ ] Some BiDi related problems.

- [ ] Show Hindi/Arabic digits based on context.

- [ ] Possibility of using a different font per-script.

- [ ] The shortcuts don't work anymore when using the emulator.
  - E.g. typing Ctrl-Q there inserts an Arabic qaf.

## Thanks (Old)

* Khaled Hosny <KhaledHosny@eglug.org>
  - Reviewing and correcting the Arabic translation for 0.5.9
  - various small i18n and l10n tips

* Igor Bogomazov
  - He reported and helped debugging the crash under the utf8 locale

* Jens Seidel <jensseidel@users.sf.net>
  - German PO file corrections (Debian bug #314022)

* Islam Motab Amer know as phaeron <greatrambo@yahoo.com>
  - Helped debugging the lam-alef handling code
  - Helped discover the multiple lines paste bug
  - Helped discover the replace all endless loop bug

* Mostafa Hussein <mostafa@linux-egypt.org>
  - New Icons for katoob
  - Switching of the line numbers to the right side into the RTL mode idea

* Thanks Leonardo Boshell <leonardop@gentoo.org>
  - Patches to fix compilation errors when disabling the spell checker
    and when freetype andfreetype2 are present

* Pav Lucistnik <pav@oook.cz> & Samy Al Bahra <samy@kerneled.com>
  - Fixes segmentation fault on start-up with several systems

* Samy Al Bahra <samy@kerneled.com>
  - Several Code cleanups and optimisations, Fix many potential buffer
    overflows. Probably he deserves more than a thanks ;-)

* Mohammed Elzubeir < elzubeir@arabeyes.org>
  - Hosting, Supporting the project

* Hicham AMAOUI <amaoui@altern.org>
  - Redhat 9.0 segfault patch

* Youcef Rabah Rahal <yrrahal42@hotmail.com> & Isam Bayazidi <bayazidi@arabeyes.org>
  - Helping me with the Arabic translation

* Munzir Taha Obeid <munzirtaha@myrealbox.com>
  - Helped me crushing bugs, iproving the UI, fixing many issues
    through his many suggestions and bug reports. He deserve more than
    a THANK YOU, but this is what i can offer :-(

* Alaa Abd-El Fatah <alaa@annosoor.org>
  - Help in debugging katoob 0.2
  - Multiple encodings support idea
  - Helping in fixing the segmentation fault while saving files with non valid
    ISO characters "Actually he was the one who discovered it ;)"
  - The complete Arabic emulator file
  - The idea of HTML numerical reference saving
  - His numerous ideas for improving the code, and efficiency
  - Modifying the emulator code to get rid of the switch statement

* Arabeyes Developer mailing list <developer@arabeyes.org>
  - Our main development mailing list, ideas and support

* Anatoly Asviyan <arsen@alice.nc.huji.ac.il> <aanatoly@linuxmail.org>
  - For writing "gkrellxkb", I borrowed the main idea and some code from it ;)

* Jens Askengren <jensus@linux.nu>
  - GNOME human interface idea
  - A patch that changes the text direction buttons to use radio buttons

* Krzysztof Krzyzaniak <eloy@transilvania.eu.org>
  - The Polish translation for version 0.3.0

* Mohammed Yousif <mhdyousif@gmx.net>
  - Slack package for 0.3 "slack 8.1"

* Roozbeh Pournader <roozbeh@sharif.edu>
  - Providing the IRAN SYSTEM table

* Arash Zeini <a.zeini@farsikde.org>
  - Pointing me to "Shabredo" where i borrowed some code ;)

* Han Boetes <han@linux-mandrake.com>
  - Helping me with the Mandrake spec file, And maintaining Mandrake package

* UTUMI Hirosi <utuhiro78@yahoo.co.jp>
  - Japanese translation and japanese encodings code, And a screenshot :-)

* Diego Iastrubni <elcuco@kdemail.net>
  - Several patches that fixed some segfaults
  - The hebrew emulator file
  - Hebrew to Logical Hebrew patch

* Michelle Konzack <linux4michelle@freenet.de>
  - The 2nd bug hunting feast, Many thanks. ;)
  - German manpage

* Fribidi and Gtkspell authors.

## Translations

* Arabic
  - Mohammed Sameer <msameer@foolab.org> (All versions up to 0.5.8)
  - Khaled Hosny <KhaledHosny@eglug.org> (0.5.8+)

* Polish
  - Krzysztof Krzyzaniak <eloy@transilvania.eu.org> (0.2.1)

* Swedish
  - Jens Askengren <jensus@linux.nu> (0.3.1)

* Hebrew
  - GNU/Linux Kinneret project (http://www.linux-kinneret.org/).

* Japanese
  - UTUMI Hirosi <utuhiro78@yahoo.co.jp>

* German
  - Michelle Konzack <linux4michelle@freenet.de>

* French
  - Emmanuel Beffara <manu@beffara.org> (0.3.8)

* Czech
  - Jan Grmela <mail@grmela.com>

## Old News and Changelogs

* XXXX-XX-XX Mohammed Sameer <msameer@foolab.org>
  - Version 0.6.0-alpha
  - We now use GtkUIManager for the UI.
  - Use GKeyFile for storage.
  - Fixed a crash with enchant.
  - Documents are now managed by DocFactory, not be MDI anymore.

* 2008-01-26 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.9.1
  - Fixed a problem that caused katoob to ignore keypresses

* 2008-01-11 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.9
  - This version includes a lot of fixes and changes. Enchant is used
    as the spelling backend.  iso-codes is used to provide "pretty
    names" for the spell checker dictionaries. Enhancements to the
    recent menu and to the extended toolbar. Gtk is now used to
    print. Gtksourceview2 is used for the syntax
    highlighting. Autosave and crash recovery have been
    implemented. More GNOME HIG compliance. libcurl is used for all of
    the network-related operations. Maemo support. A few bug fixes and
    better Arabic translation.
  - Changelog:
	* Enchant is now used for spell checking instead of aspell
	* The iso-codes package can be used to display friendly names for
      the spell checker dictionaries.
	* The recent menu is displays the name of the file not the full
      path.
	* Opening a non-existing recent file triggers an error.
	* Better quality Arabic translation (Thanks Khaled Hosny).
	* Maemo support.
	* libcurl is now used for the network.
	* Cairo is not used anymore to draw the extended toolbar extra
      icons.
	* Make the buttons in the dialogs HIG compliant
	* Pressing Escape will close the spell checking dialog
	* We now use the gtk printing capabilities for printing.
	* Syntax highlighting now depends on gtksourceview2 and it's been
      rewritten.
	* We now save the documents every minute and recover any files
      when we start.
	* We also autosave if we crash.

* 2007-04-28 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.8
  - Implemented a multipress input method. Reorganized the input
    menu. A couple of bug fixes. Added a window to show the multipress
    and emulator mappings.
  - Changelog:
	* Implemented Multipress. Phone style typing.
	* When we create a file, make sure we have read & write
      permissions (By passing `S_IRWXU` to `open()`)
	* Added a window to show the emulator and multipress mappings.
	* Line numbers aren't now all bold. Only the active line.

* 2007-03-31 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.7
  - This is mainly a bug fix release. Fixed a bug that can crash the
    application when OK or Apply is clicked in the preferences dialog
    under utf8 locales.  A lot of small features and enhancements as
    well as some code cleanups.  We also now use gtksourceview not
    gtksourceviewmm.
  - Changelog:
	* Code cleanups.
	* Depend on gtksourceview instead of gtksourceviewmm
	* The current line is drawn in bold when we show the line numbers.
	* In the document right click menu, show the dictionary menu item
      only if we have a word.
	* Add more drag actions so we can accept data from konqueror.
	* DND should work fine with konqueror now.
	* Dropping a file on a document will open it.
	* Don't use the glib file saving functions as it'll fail if we
      don't have write access to the target directory even if the
      target file is writable.
	* Some problems related to the encoding conversion.
	* Started to comment the code via the doxygen style.
	* The tab width is now configurable.
	* Fixed a crash under utf8 locales when clicking "Apply" or "OK"
      in the preferences dialog.
	* CXXFLAGS now should get CFLAGS prepended. Now --enable-release
      is really effective!

* 2006-11-05 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.6
  - Shouldn't fail when utf8 is selected from the encodings
    menu. Reset the GUI when we close a document from the document
    label close button.
  - Changelog:
	* Don't try to convert the encoding if the requested one is
      utf8. return false if the text is not valid utf8 after we
      convert it.
	* Reset the gui when we close the document from the document label
      close button.

* 2006-11-02 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.5
  - Opening large files shouldn't be slow anymore. Dropped the "find
    and replace" button from the replace dialog.
  - Syntax highlighting support using libgtksourceviewmm.
  - Changelog:
	* Replace dialog: Removed "Find And Replace".
	* Severe optimization for the lam-alef detection and manipulation
      code.
	* Syntax highlighting support (Via libgtksourceviewmm)

* 2006-10-23 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.4
  - Importing or exporting text with bidi and shaping applied won't
    reverse it.
  - Updated Japanese translation. The preferences dialog sections are
    now translated.
  - Changelog:
	* When we import or export bidi and shaped text. We process the
      text line by line because fribidi is reversing the whole text
      and ignoring line separators.
	* Thanks UTUMI Hirosi <utuhiro78@yahoo.co.jp>
	  - Updated Japanese translation.
	  - The preferences dialog sections are now translated.
	  - Various small updates for the desktop.in file.

* 2006-10-11 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.3
  - Can now import/export text that has bidi and shaping applied to.
  - The plain text exporting now depends on fribidi.
  - Changelog:
	* Can export text after applying bidi and shaping.
	* Exporting plain text depends on fribidi now.
	* Can now import text that has been shaped and had bidi applied to
      previously.

* 2006-09-20 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.2
  - Can now open remote files using an external command. Added an
    option to insert files.
  - Some small code improvements and cleanups.
  - Changelog:
	* Added Insert file.
	* Some small code cleanups.
	* Implemented open location to open remote locations.
	* Check for the return value of the executed subprocess
	* Implemented the Thread class. Migrated our dict lookup code to
      it.
	* Can now open remote locations.
	* Depend on gtkmm >= 2.6.0 instead of 2.6.5

* 2006-09-5 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.1
  - Fix compilation error when dbus is disabled.

* 2006-09-4 Mohammed Sameer <msameer@foolab.org>
  - Release 0.5.0
  - Rewritten in C++ and gtkmm

* 2006-05-15 Mohammed Sameer <msameer@foolab.org>
  - Release 0.3.9
  - New French and Czech translations.
  - Correction to the German translation, a german manpage was added.
  - Updated Hebrew translation.
  - A French to Arabic transliteration.
  - A few bugs have been fixed as well as some features enhancements.
  - Changelog:
	* German manpage (Michelle Konzack)
	* Updated Hebrew translation.
	* New French translation.
	* French to Arabic transliteration emulator layout by Emmanuel
      Beffara
	* When we are saving, If the user wrote a filename in the text
      entry, we should use it instead.
	* New Czech translation (Thanks Jan Grmela)
	* Fixed a weired segfault during startup (Thanks oren maurer for
      reporting)
	* German PO file corrections (Debian bug #314022)
	* Extended toolbar searching is now case insensitive.
	* An arrow should appear with icons from the toolbar if the
      display width is not enough.
	* Should really save the aspell lists.
	* Arabic text should be handled by aspell as usual.
	* Added diacritics buttons to the extended toolbar.
	* Regenerated the icons from the SVG.
	* Updated COPYING file, The FSF address changed.

* 2004-08-30 Mohammed Sameer <msameer@foolab.org>
  - Released 0.3.8
  - Long time no releases, But I kept my word, The release happened
    this month ;-)
  - What to expect ? A long list of bug fixes and improvements,
  - The last release was really buggy as hell. I hope this one'll be
    fine.
  - Hey, We have a new icon now!
  - A crash? Should be able to debug itself, and you won't loose your
    files.
  - Hebrew and Japanese translations.
  - Small UI tweaks here and there.
  - A lot more.
  - Changelog:
	* The ability to open multiple files from the open dialog (Bug
      #54).
	* Remembering the last place from where files were opened/saved
      (Bug #55).
	* Implemented own command line arguments parser.
	* katoobdocument.c: Fixed compilation errors when the spell
      checker is disabled (Thanks Leonardo Boshell
      <leonardop@gentoo.org> for the patch)
	* src/Makefile.am: Fixed compilation errors when freetype and
      freetype 2 are there! (Thanks Leonardo Boshell
      <leonardop@gentoo.org> for the patch)
	* Dict protocol support.
	* New icons for katoob (Thanks Mostafa Hussein
      <mostafa@linux-egypt.org>).
	* Fixed the bug that caused all the window parts to be shown even
      if they are disabled in the preferences dialog.
	* Some speed optimisation.
	* Katoob won't complain about backup problems when saving for the
      1st time.
	* A statusbar indicator for the insert button.
	* The crash dialog should be fine now.
	* Katoob try to save the open files if it crash.
	* A new configuration file parser.
	* Don't modify the stored directory path if the user canceled the
      open dialog.
	* Cleaned the config saving and freeing routines.
	* Some fixes for the Makefiles.
	* print dialog, properly destroy the dialog so it can be opened
      again!.
	* Replaced `katoob_debug(__FUNCTION__)`; with a macro:
      `KATOOB_DEBUG__FUNCTION`
	* Code cleanup: katoob compiles under g++ 2.95 & 3.3.1.
	* Don't connect to the dict server except when the user clicks on
      the "Dictionary" item.
	* UTF-8 is the default encoding for saving (#66)
	* Memory leak in the case insensitive search code.
	* Added a hebrew translation.
	* Converted pl.po to UTF-8. (Bug #50)
	* katoob.po: removed, ar.po: corrected an error. (bug #141)
	* A Japanese translation.
	* src/katoob.h: Katoob now compiles under gtk+ 2.4
	* Added --enable-new-widgets to enable using the new gtk 2.4
      widgets instead of the old ones
	* Added --enable-release to ommit the additional debugging symbols
	* save.c: saving with the same filename but different encoding
      should update the document encoding now.
	* katoobdocument.c: Fixed bug #150.
	* search.c: fixed a segfault when doing "replace all" 2 times.
	* conf.c: return when we can't open the print conf. file "Samy Al
      Bahra".
	* file selector: Don't remember the path accross sessions.  And
      always start from the c\urrent dir, and fail to the home dir.
	* Try to recover text till the illigal byte when the "illigal byte
      sequence" error appears.
	* A hebrew file for the emulator "Diego Iastrubni <elcuco@kdemail.net>"
	* conf.c: fixed a segfault.
	* Hebrew is now Logical Hebrew (Thanks cuco)
	* German translation (Thanks Michelle Konzack)
	* some fixes for the print dialog.
	* BUG: `config_struct_foreach()` free `config->backup_dir` instead
      of `config->backup_ext`
	* Display an error dialog when no spellchecking dictionaries are
      available, Don't pass a NULL PspellManager variable to
      `pspell_manager_check()` as it'll segfault.
	* Disable the RTL/LTR text alignment under gtk 2.4 :-(
	* A new about box per Michelle Konzack's request.
	* Fixed an infinite loop when u replace all and the from string is
      a substring of the to string.
	* Handle the LAM-ALEF presentation forms, Replace them by lam+alef
      "incomplete".
	* Convert hindi digits to arabic when we save in cp1256
      "disabled".

* 2003-08-26 Mohammed Sameer <msameer@foolab.org>
  - Released 0.3.5
  - What can I say? A long time since the latest stable release.
  - Changes since the last development release:
    * DnD Support
    * More UI enhancements.
    * Backup before saving.
    * Rewritten BiDi line direction detection algorithm.
    * Native printing support, No more txtbdf2ps stuff ;-)
    * IDL files are detected and highlighted.
    * Numerous code cleanups, bug and mem leaks fixes, potential
      segfaults were fixed and stability increased.
  - Changes since the last stable release:
    * Rewrote many parts of the code.
    * A new emulator infrastructure.
    * BiDi support.
    * All ISO and Windows encodings are supported now.
    * Many UI enhancements.
    * Spell checking is Ok now.
    * Source highlighting.
    * More and more than I can remember.
  - Changelog:
	* Dropping a file on katoob window result in opening it.
	* Segfault when freeing xkb groups on redhat 9.0.
	* Three dots after all menu entries that lead to dialog boxes.
	* Preferences dialog categories are now translated.
	* Backup before saving implemented.
	* Fixed some memory leaks and code cleanups.
	* Calculate the cursor position correctly when tabs are inserted
	* Added "..." to all menu entries popping a question dialog.
	* The document created on startup grab the keyboard input.
	* recheck the document when changing the dictionary.
	* spell check button to the extended toolbar.
	* fixed close all not canceling when hitting cancel.
	* Added the emulator entries to a submenu.
	* unset readonly when saving a readonly document.
	* fixed a bug that caused the selected text to be unselected when
      right clicking.
	* Renamed Toolbar to Main toolbar, and added the extended toolbar
      option to the toolbars submenu.
	* A small fix for the man page.
	* misc.c: segfault "katoob - - - - - - - - -"
	* No beep when no documents found, and when no undo or redo items.
	* Signal handler to try and save the open files.
	* Removed the preferences shortcut.
	* Makefile.am: A bug caused linking to the system fribidi if
      present.
	* mdi.c: memory leak.
	* Increased katoob window icon size.
	* encodings.c: some tweaking.
	* prefs-dlg.c: The text direction isn't selected correctly.
	* Rewrote the BiDi line direction detection algorithm to fix the
      high CPU and memory usage.
	* conf.c: disable_xft is FALSE by default.
	* main.c: Disabled the signal handler
	* moved xkb_grp and xkbEventType from the config struct to the
      katoob struct.
	* Added --enable experimental and marked all the crash handler
      stuff as experimental. #ifdef EXPERIMENTAL ... #endif
	* New fribidi based on version 0.10.4 + fribidi_log2vis for the
      printing.
	* The import and export menu items are not check items.
	* The document encoding is set to UTF-8 if the opened file is
      valid UTF-8
	* Save as with a different encoding sets the document encoding to
      it.
	* recent.c, conf.c: fclose the config files after loading and/or
      saving.
	* prefs-dlg: Fixed the backup directory selector title.
	* Native printing support.
	* Exporting doesn't unset the modified state for the document.
	* Fixed compiling errors when the spell checker and printing
      support are disabled.
	* replaced getenv() with `g_get_home_dir()`

* 2003-06-25 Mohammed Sameer <msameer@foolab.org>
  - Released 0.3.4.91
  - This version should fix all the problems related to detecting
    aspell/pspell via the configure script, Added java, TeX, Ada to
    the automatically highlighted languages.
  - Changelog:
	* configure.in: Fixed all pspell/aspell detection problems
	* html.c, misc.c: #include <stdlib.h> for atoi()
	* mdi.c: removed extern UI *katoob from
      `katoob_create_doc_from_file()`
	* print.c: #include <stdlib.h> for system()
	* highlight.c: `katoob_get_highlight_type()` Added java, tex and
      ada
	* mdi.c: fixed compile error when enabling highlight.

* 2003-06-24 Mohammed Sameer <msameer@foolab.org>
  - Released 0.3.4.90
  - This is a 0.3.5 prerelease, Offering many features and
    enhancements, It's mainly a rewrite, Spell checker support, Syntax
    highlighting, Many encodings are now supported, And More!
  - Changelog
	* Rewrite using GtkObject model.
	* encodingslist.h: Added some more encoding names, fixed some
      others.
	* bidi.*: New files, All the BIDI code is there.
	* conf.c, katoob.h: Added text_dir & bidi options.
	* main.c: New "text direction" menu.
	* Almost complete BIDI support.
	* Added text direction to the preferences dialog.
	* mdi.c: the close button is generated but hidden if
      config->showclose = FALSE
	* Changing the "show close button option" Applies to the opened
      tabs.
	* Added ISO-8859-16 "Romanian" To the list of supported encodings.
	* More Language names in the encodings menu.
	* A new implementation for the encoding menu & encoding list
      generation, Making it more easy to add more encodings.
	* The ability to display line numbers
	* The configuration struct is freed before quit
	* Fixed choosing not to save on exit doesn't save for that time
      only!
	* saveas.c: removed, all functions are in save.c, renamed
      katoob_saveas() to katoob_save_as()
	* autogen.sh: Check for libtool & intltool.
	* The text buffer is scrolled to the highlighted item.
	* The ability to disable antialiasing.
	* encodings.c: Changing the encoding of an unassosiated buffer,
      treat it as if it was encoded in the default encoding.
	* The ability to change the editor fonts.
	* Integrated the spell checker into KatoobDocument.
	* Katoob skip Arabic words spelling.
	* The recent menu should be really fixed ;-)
	* save as add the file to the recent menu.
	* Removed the encoding option from the command line options.
	* man page cleaning.
	* Unsupported command line options are ignored instead of
      displaying the help and exiting with return code 1.
	* Debugging output is printed to stdout instead of stderr.

* 2003-01-19 Mohammed Sameer <msameer@foolab.org>
  - Release 20030119
  - Full BiDi support is now implemented.
  - The configure script problems is now fixed, Fixed "fribidi.h" not
    found compilation problem.
  - Update gtkspell to 2.0.4 "can now make use of either aspell or
    pspell", and some optimization for the spell checker.
  - The configure script links against libstdc++ which pspell or
    aspell is linked against.
  - The spell checker is disabled by default, The bidi support is
    enabled by default.
  - Changelog:
	* configure.in: fixed "./configure: bidi: command not found" when
      disabling bidi
	* Fixed the complain about missing fribidi.h when disabling BIDI
      (Thanks Alaa & Uri Elias for reporting)
	* gtkspell.c: replace all gtk_text_view_get_buffer() by
      katoob->active_buffer, Removed the destroy callback
	* mdi.c: NULL katoob->active_buffer when no more open documents
	* A man page is now included.
	* Update gtkspell to 2.0.4 "can now make use of either aspell or
      pspell" & updates for the configure.in script
	* mdi.c mdi.h katoob.h gtkspell.c gtkspell.h: Changed
      `HAVE_PSPELL` to `HAVE_SPELL`
	* BiDi algorithm works when opening files
	* configure.in: gtkspell is disabled by default, bidi is enabled
      by default.

* 2003-01-18 Mohammed Sameer <msameer@foolab.org>
  - Released 0.3.1
  - This is mainly a bugfix release.
  - Fixed a bug that caused the mainwindow to be destroyed without
    katoob quitting, A bug that fixes the compilation problems against
    glibc 2.3.1 and bug that caused the about box to be opened more
    than once!
  - Added an Arabic translation for the GPL, And a Swedish translation
  - A man page is now included.
  - Changelog:
	* src/file.c: Applied patch from bug #12
	* src/main.c: Applied patch from bug #11
	* Added po/pl.po
	* Added po/sv.po
	* src/misc.c: Fixed warnings when displaying the emulator file not
      found error Fixed a bug that enables the user to open more than
      1 about dialog
	* Added GPL.ar, An Arabic copy of the GPL
	* Makefile.am: Added `GPL.ar` to `EXTRA_DIST`, cleaned duplicate
      THANKS entry in the `EXTRA_DIST`
	* katoob.spec.in: Modified the description.
	* Added a man page.

* 2003-01-02 Mohammed Sameer <msameer@foolab.org>
  - Released 20030102
  - This is a development release.
  - All compile time problems should be fixed in this version but
    maybe some pspell related problems are still around ;)
  - Fixed gcc 2.9.5 compilation failure, RedHat problems with the
    autogen.sh script.
  - Properly using intltool to translate GNOME desktop entry!
  - Fixed a segfault when auto selecting an encoding in the save
    dialog "The displayed name is different than the actual encoding
    name!"
  - Changelog:
	* acconfig.h: deprecated & removed
	* configure.in: all symbols are defind there with description
	* better I18N using intltool "translation for katoob.desktop
      file".
	* Removed the bundled fribidi option, Always link against the
      bundled one
	* configure.in: changed --disable-gtkspell to --enable-gtk-spell
	* Fixed compile time disabling of the spell checker
	* src/iransystem.c: Fixed gcc 2.9.5 compile errors
	* src/file.c: Fixed a segfault when auto selecting an encoding in
      the save dialog "The displayed name is different than the actual
      encoding name!" (Thanks Uri Elias
      <elias@techunix.technion.ac.il> for reporting)

* 2002-12-08 Mohammed Sameer <msameer@foolab.org>
  - Released 20021208
  - This is a development release.
  - This version fixes the broken encodings menu, the encoding display
    in the statusbar & fixes several memory leaks. Fribidi is now used
    to handle the BIDI algorithm.
  - Please have a look at the TODO files for the current limitations.
  - Changelog:
	* Fixed the encoding display problems in the statusbar
	* Spell checking using gtkspell
	* Fixed the encodings menu
	* Several memory leaks
	* Fribidi is used to handle the BIDI algorithm

* 2002-11-02 Mohammed Sameer <msameer@foolab.org>
  - Released 01112002
  - This is a development release.
  - Limitations:
    * The encoding display in the statusbar doesn't work correctly
    * The encoding menu is broken
    * The BIDI support is incomplete
    * The RTL/LTR doesn't work when switching pages
    * The HTML reference doesn't cover all characters
    * No way to choose the default encoding
    * Several memory leaks
    * Remove or keep the xkb switcher??
    * Recent menu doesn't save in the correct order
    * FIX `REAL_OPEN`
  - Changelog:
	* Rewrote all the language routines to facilitate the addition of
      other encodings.
	* RTL, LTR are now radio buttons "patch by Jens Askengren".
	* Fixed a bug that caused the main window to be destroyed when
      choosing cancel or saving fails. when prompted to save the files
      upon exit.
	* Updated Polish translation
	* BIDI algorithm "EXPERIMENTAL"
	* Fixed a bug with the about box that allowed opening 2 dialogs.
	* Fixed a bug that prevents the display of the close button when
      choosing "No" upon trying to open a non existing file from the
      recent menu.
	* Re organised the commandline files opening routines. "reading
      from stdin works if '-' is anywhere"
	* The statusbar now displayes the encoding
	* Fixed a bug that caused the configuration to be saved even if
      disabled from the preferences dialog.

* 2002-09-12 Mohammed Sameer <msameer@foolab.org>
  - Released 0.3.0
  - This version comes after a very long time and big efforts spent in
    the development. This version offers the new MDI interface,
    Follows the GNOME human interface guidelines, Fixes many bugs and
    segfaults "Most of them happens when the user does an unexpected
    behaviour as opening binary files as HTML style". A new
    preferences dialog. More options are now customizable, Saving in
    plain UTF-8, ISO-8859-6 & cp1256 works well. The ability to import
    and export HTML numerical characters references. The new Arabic
    emulator.  You really got to try this version. ;)
  - Changelog:
	* The new MDI interface
	* A Polish translation "Krzysztof Krzyzaniak
      <eloy@transilvania.eu.org>"
	* The statusbar update followes the cursor movement.
	* Line & Column count begins from 1 not 0.
	* Removed all images from the executable file, They are now loaded
      at runtime
	* The textbuffer is now always in focus
	* Enter in the "search for" or "replace with" activates the "find"
      button
	* GNOME interface guidelines compliant menubar "suggested by Jens
      Askengren"
	* Goto line in the menubar
	* All dialogs are now transient on the main window
	* New icons for katoob!
	* An arabic keyboard emulator to enablr the user to type Arabic
      without configuring an Arabic keyboard
	* Added "Text beside icons" To the toolbar style
	* A popup menu to the toolbar to select the style
	* A buffer status indicator in the label
	* Save a copy has been implemented
	* Implemented revert
	* Rewrote the recent routines to avoid possible segfaults
	* Many code cleanups, Numerous bug fixes
	* Fixed a bug when trying to save text with BIDI tags in windows
      or iso encoding
	* Enabling/Disabling, Limiting the size of the Undo history
	* Limiting the size of the Recent documents menu
	* More improvements to the user interface
	* Opening multiple files from the command line
	* The ability to import, export files in the "HTML Numerical
      characters reference"
	* Katoob doesn't display an error when the configuration file is
      not found
	* Fixed the error when saving empty files
	* Enhancements for the encoding menu
	* --enable-debug defines `g_malloc()` as `malloc()`, `g_free()` as
      `free()` and includes stdlib.h
	* Katoob doesn't save when the disk is full

* 2002-08-14 Mohammed Sameer <msameer@foolab.org>
  - Released 0.2.1
  - This version is mainly a bug fix, fixed many crashes, The most
    important is crashing when saving a buffer in the ISO encoding
    whice contains non ISO valid characters. And more error checking
    to prevent possible ones. The interface changes are kept to the
    minimum in this version. And fixed a bug that caused a file opened
    via command line to be aded more than once to the recent menu.
  - Changelog:
    * Fixed the search dialog where some buttons didn't display the
      whole label
	* Added a GNOME menu entry, with a modified smaller logo, Adapted
      the .spec.in
	* The statusbar now displayes the cursor position, The state of
      the file
	* More error checking in `realopen()`, `real_save()` and
      `real_saveas()`
	* Fixed a segfault when changing the encoding of a non valid
      encoded file
	* Fixed a bug in the recent menu routines caused a file to be
      added more than once when opened via command line arguments
	* Fixed the "replaceall" Warning by gcc
	* Fixed a segfault when trying to save a file in ISO with non ISO
      valid characters in the UTF buffer

* 2002-08-10 Mohammed Sameer <msameer@foolab.org>
  - Released 0.2
  - This version is A major interface improvement. With the recent
    menu, preferences dialog, toolbar & statusbar customization
    options. With some more features discover it yourself ;)
  - Changelog:
  	* Recent documents menu
	* Preferences dialog
	* katoob can now read from the standard input "cat foo | katoob -"
	* A menu option to save window attributes when exit
	* A menu item to show/hide the toolbar
	* The toolbar can now be text only, icons only, or both
	* A menu item to show/hide the statusbar
	* In addition to the new configuration file layout, code
      optimization and of course the Arabic translation
	* No need to set the Codepage in the environmental variables to
      display the Arabic interface
	* The main window now has an icon "similar to that in the about
      box"

* 2002-07-27 Mohammed Sameer <msameer@foolab.org>
  - First public release 0.1
  - The first public release, after months of work. What more to say
    ;)
