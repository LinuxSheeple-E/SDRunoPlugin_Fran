SDRuno Frequency Annotate Plugin (Fran)  Version 0.0.3    August 1, 2020

This plugin is still a sample or "proof of concept" plugin for SDRPlay SDRuno Version 1.4. SDRuno 1.4 is currently in a rc (release candidate) state. This version will not work in 1.4 RC3 or earlier. This plugin might again break in a future 1.4 release. It is amazing it even works!

The plugin uses files with either the swskeds csv format or SDRuno bank format. The swskeds file combines several sources and has numerous duplicate entries. The source information in the loaded swskeds file is added to the selection list in the Sources combobox. Although multiple swskeds files can be loaded, I would not recommend it. The bank file is a SDRuno memory bank file. Mutiple Bank files can be loaded.

I have included a recent swskeds file. You can also get a zip file containing the swskeds csv file in the files section of the swskeds group on groups.io.

Changes since Version 0.0.2
* Updated to work with SDRuno 1.4 RC4. This version will not work with RC3 or earlier ("Boldly going forward, 'cause we can't find reverse").

* Change Annotator Style to Marker. This also required some adjustment to the item spacing. A lot of stuff is still hardwired.

* I was not too familiar with Microsoft's use of Interface Classes in C++. The code is refactored to move the IUnoAnnotator interface to the main plugin class instead of deriving a separate class.

* Minor code improvements.

Changes since Version 0.0.1
* Added support for limiting the station display to one source in the swskeds file. This is interactive and can reduce the number of duplicate stations displayed.

* Improved parsing of the days field in the swskeds file. The formats that represent a day of a month (Jun 21th) or a day of the week (1st Fr) are now supported. This should reduce the display of stations not really broadcasting. 

* Minor fixes and stability improvements.

Changes since Version 0.0.0
* Fixed stability issues when tuning to certain frequencies. SDRuno would crash from exceptions being thown due to CSV file format problems. Also annotate text has a 63 character limit.

* Added support for the SDRuno Bank CSV format.

* Added support for loading multiple annotation files.

* Added text color by language (swskeds file only). There is no way to currently change them or turn them off.
	Arabic  Green
	English Purple
	French  Cyan
	Japanese Lt Red
	Mandarin/Chinese Orange
	Russian Drk Red
	Spanish Yellow

* Minor fixes like lessening the crowding of entries on the screen and displaying the annotation at the lowest and highest frequency

Installation
Unzip the files to a directory. Copy the SDRunoPlugin_Fran.dll file to the SDRuno Plugins directory (C:\Program Files (x86)\SDRplay\SDRuno\Plugins).  Say yes to copy the file as an administrator. Run SDRuno and press the PLUGINS button. Fran should be on the list. Highlight the Fran line and press Load Plugins. Press the File button on the plugin window and select the .CSV file to load.  The area at the bottom of the plugin window will either show the number of records loaded or where an error occured in the csv file.  Close the plugin window to exit.

Operation
Best results occur when using a zoomed spectrum as the swskeds file has numerous stations closely spaced in most bands. The text for the station will appear to the immediate right of the frequency since the styles have not been implemented. Text will appear and disappear as stations are scheduled to go on and off the air. I hope to implement a fade in and fade out function to indicate stations about to go on-air and about to go off  air. The sorting and the positioning of the text on screen is very rudimentary and more "jumpy" to my liking.  There is a limit of 64 text entries so they are centered around the vfo area. 

The user can select the source in the swsked file to display using the Sources combobox. The default ALL entry enables all sources. Any active station without a source specified is displayed regardless of the Sources selection.

Mutiple files can be loaded. Although multiple swskeds files can be loaded, I would not recommend it. One swskeds and one or more Bank files can be loaded. Exit the plugin to clear the annotations and start over.

I have included the sources in the sources.zip file.  Plugin was compiled using Microsoft Visual Studio 2017.

Portions are Copyright (C) 2020 Eric A. Cottrell
Contact: eric.c.boston@gmail.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CSV reading library:
Copyright (c) 2015, ben-strasser
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of fast-cpp-csv-parser nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

