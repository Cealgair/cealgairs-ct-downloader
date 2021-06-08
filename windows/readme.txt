CEALGAIR'S CT DOWNLOADER v. beta 2.3-win
Copyright (C) 2020-2021 Cealgair, Fynn93

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 How to use
 ==========

  Setup
  -----
  
  This "setup" part only needs to be done once and for all.
  
  First of all, you'll need to install wit and wszst if you haven't already. You can find them at https://wit.wiimm.de/ and https://szs.wiimm.de/ respectively. Then, you'll need to install bash and wget (which you probably already have). If you want to use the icons program, you'll need imagemagick and its optional dependencies to convert svg files. Finally, you'll need to download and extract the latest version of LE-CODE (which can be found at https://download.wiimm.de/lecode/distribution-support/lecode-bin/) and WBZ Converter (which can be found at http://wiki.tockdom.com/wiki/WBZ_Converter). WBZ converter also needs to be set up, as explained in its readme.
  
  When everything is installed, you'll need an image of the Mario Kart Wii Disk. Once you have it, you'll need to extract all the files into a directory, which we'll call for example "original". This is done with wit, you can learn how at http://wiki.tockdom.com/wiki/Wiimms_ISO_Tools.
  
  At this point, you'll need to copy "original" into another directory, which we'll call for example "new". Then, you'll need to rename all of the szs files in "new/files/Race/Course", as explained in the introduction of http://wiki.tockdom.com/wiki/LE-CODE/Distribution_Tutorial. Then, follow the instructions given in the "Modified files" section of the same page.
  
  Now you'll need to edit three of the files provided with this toolser. "path-to-disc-root.txt" should contain the path to the directory we've called "new", ending with a /. Similarly, "path-to-wbz-converter.txt" should contain the path to the directory of WBZ Converter (the one that contains "convert-WBZ-to-SZS.sh" among other stuff). Also, "path-to-lecode-build.txt" should contain the path to the file "lecode-*.bin", where "*" can be "JAP", "KOR", "PAL" or "USA" depending on the version of the disc you have.
  
  Then, you'll probably need to give yourself the permission to execute the various programs. To do so, run the following command:
  
  chmod u+x autoremove bmg builder ct-downloader icons patch bin/generate-icons bin/icons-setup
  
  Finally, you will also need to run bin/icons-setup once.
  
  If you want to give the very same permissions to everyone that uses your computer, just replace "u+x" with "a+x".
  
  Usage of "ct-downloader"
  ------------------------
  
  This program allows you to make a list of Mario Kart Wii custom tracks and to download them all from ct.wiimm.de without having to tediously do that by hand.
  
  This is not intended as a tool to get information about the tracks. To do that, visit wiki.tockdom.com and ct.wiimm.de.
  
  Keep in mind that, in order to not overload Wiimm's servers, this program is fairly slow and cannot download more than 20 wbz files in a single hour.
  
  To use this program, you first have to edit the file called input.txt and then run ct-downloader. Each line of that file, with some exceptions addressed below, will be a search query that the program will do on ct.wiimm.de, so you can also use keywords (visit ct.wiimm.de for further information on them). The exceptions are empty lines (of course), comments (which will be ignored), and lines that begin with an asterisk (*), that are explained below.
  
  Lines that begin with a hash sign (#) are treated as comments.
  
  A line consisting of only two hash signs is treated as the start of a comment, that ends at a line consisting of only three hash signs or at the end of the file.
  
  The lines cannot be too long (199 characters max, and even then they might cause issues because the percent sign counts as three characters). This includes comments. I kinda want to fix this but I don't know how.
  
  Keep in mind that this program discards all of the results that are not perfect matches and all of those that are not downloadable. Also, it only searches for tracks, so do not use it for custom arenas or distributions. Also, it only keeps the first 50 results, so don't search for vague stuff like "circuit".
  
  If for a certain query the program finds many good results and is not sure which one to choose, it will ask you to make a decision. Those decisions are then saved in the directory "old-decisions". If you want the program to forget a decision you have permanently confirmed, delete the right file in that directory.
  
  The line "*force decision" (without quotation marks) forces the program to ask you to make the decision for the following query even if there is only one "select" result, unless you've already permanently confirmed your decision.
  
  The line "*music LC" tells the program that the tracks found on the next query are to be put on the music slot of Luigi Circuit, overwriting the information from ct.wiimm.de. Instead of "LC", you can put anything that works as described at https://szs.wiimm.de/info/ct+le-code.html#slot.
  
  The line "*menuname whatever" tells the program that the tracks found on the next query should have "whatever" as their name on the menu.
  
  The line "*extra whatever" tells the program that the tracks found on the next query should have "whatever" as their extra information, which if not empty is usually put in brackets at the end of the menuname.
  
  All other lines beginning with an asterisk are currently discarded.
  
  After this program has ran, you will find the track files in the folders "szs-files" and "wbz-files". You can safely delete all files in the folder "wbz-files", or even the folder itself. This program won't lose time downloading files that you already have even if they are only in the "szs-files" folder.
  
  This program will also make the file "filename-list.txt", which is later used by "builder", and the file "log.txt", which lists all of the queries that produced no results, all of the newly downloaded tracks, all of the tracks that were already downloaded and all of the files in the "szs-files" and "info-files" folders that are unused.
  
  Tip for your sanity: mantain the lines of "input.txt" in a somewhat alphabetical order, cause it might get large.
  
  Known bug: when user is asked to input something, if they input a long string weird stuff happens. Nothing that completely breaks anything, and the string needs to be large, but still.
  
  Known bug: the program behaves weirdly when reading the last line of "input.txt", so keep it empty. Maybe even keep the final two lines empty.
  
  Known bug: If ct.wiimm.de is not reachable, the program acts as if the search produced no results.
  
  Usage of "builder"
  ------------------
  
  This program allows you to make "CTFILE.txt" and "tracks.bmg.txt", the former being an essential file to make the whole thing work and the other being a precursor to the file containing all of the track names. It also makes "CTFILE.INTRO.txt" and "intro.bmg.txt", which are needed to make the messages that appear during the intro cameras of the tracks.
  
  "builder" tries to make all of the track names distinct for the menu by adding the author or the version or some extra information if the tracks have the same name. Once every track name is unique, they are sorted in alphabetical order.
  
  If you just ran "ct-downloader" and you already got all the CTs that you want, you can just run this program. Otherwise, keep reading this section.
  
  This program reads "filename-list.txt", which is created by "ct-downloader", and "other-filename-list.txt". If there are some tracks that you want to include in your distribution but are not on ct.wiimm.de (for example because they are texture hacks, or not officially released), you should put them in szs format into the folder calles "other-szs-files" and put all of their filenames into "other-filename-list.txt", without the ".szs" extensions and separated by line feeds.
  
  You'll also want to create an info file for each of those tracks in the directory "other-info-files". Check the ones in "info-files" after you run "ct-downloader" as an example.
  
  After all of that, you can run this program and get the "CTFILE.txt" and the "tracks.bmg.txt", with the similar files for the intro cameras.
  
  If there's a track that you don't want in the distribution anymore, you just need to delete his filename from "other-filename-list.txt".
  
  Known bug: I haven't figured out how to put the names of the Wiimm cup in "tracks.bmg.txt", so you'll have to do that manually.
  
  Known bug: I haven't figured out how to set the names of the Nintendo tracks properly in "tracks.bmg.txt" and "intro.bmg.txt", so you'll have to do that manually.
  
  Usage of "patch"
  ----------------
  
  This program patches LE-CODE in order to make it work with your selection of tracks. If you want to change the settings that are set by this program, you can patch LE-CODE manually as explained at http://wiki.tockdom.com/wiki/LE-CODE/Distribution_Tutorial, or change its source code and re-compile it.
  
  Usage of "icons"
  ----------------
  
  This program creates the menu icons for your distribution. It keeps the original icons for the Nintendo cups, puts a die as the icon for the Wiimm cup and puts the first three letters of the first track for every other cup. If you want it to behave differently, you can change its source code or make the icons manually.
  
  Usage of "bmg"
  --------------
  
  This program patches the file containing all of the track names, according to what it finds in "tracks.bmg.txt" and  "intro.bmg.txt". You might want to fix the names of the Nintendo tracks and those of the Wiimm cup before running this program.
  
  Build the iso
  -------------
  
  When everything is done, you can create the iso for your distribution from the directory we called "new" using wit (again, check http://wiki.tockdom.com/wiki/Wiimms_ISO_Tools for how to do so).
  
  Usage of "autoremove"
  ------------------
  
  This program allows you to automatically remove all of the files in the "szs-files" and "info-files" folders that are listed as unused in "log.txt". Before running this program, make sure to remove any lines in "log.txt" which list files that are currently unused but that you want to keep. I implemented this because hundreds of unused szs files can take up valuable space and are very tedious to manually remove.
  
  Known bug: the program behaves weirdly when reading the last line of "log.txt", so keep it empty. Maybe even keep the final two lines empty. Note that, when "ct-downloader" automatically creates "log.txt", it already leaves a few empty lines at the end, so you don't really have to worry about it.
 
 Source code
 ===========
 
 I have included the source code with this toolset. You can do whatever you want with it, but please contact me first. I know I stated in the readme of the previous version that I didn't plan to update this toolset, but this is no longer the case.
 
 Keep in mind that you cannot download more than 20 tracks in a single hour from ct.wiimm.de due to the anti-bot measures that are in place, so that's why I implemented the "brain" function in "ct-downloader". Also, you can technically reduce the three seconds of sleep, but you might want to talk with Wiimm beforehand. Also, if you make a Windows port or something, ask Wiimm if those three seconds of sleep are still ok or if they need to be increased, because I don't want to get in trouble with him.
 
 I tried to be sensible while writing the code, and I even put some comments, but if you need anything to understand it better feel free to contact me via the same ways listed below for reporting bugs.
 
 Some cool things you could implement if you want to modify this toolset include:
 * implement a way to support groups of random tracks
 * fix the known bugs (both the ones mentioned in this file and the ones listed at http://wiki.tockdom.com/wiki/Cealgair%27s_CT_Downloader)
 * improve "ct-downloader" by not making it always ask ct.wiimm.de if we already have the track downloaded
 * improve the way that override files are handled by "ct-downloader"
 * implement a smarter way to handle _d files
 * improve "builder" by adding an option to sort by name without prefixes
 * improve "builder" and "bmg" to support all of the fancy stuff like colored prefixes
 
 Report bugs, etc.
 =================
 
 First of all, check if you have the latest version. This includes versions made by other people that are modification of this toolset. If this is the latest version, then check if the bug is has already been listed at http://wiki.tockdom.com/wiki/Cealgair%27s_CT_Downloader. If it hasn't, keep reading this section.
 
 The preferred way of reporting stuff is to report bugs on the talk/discussion page of this toolset at http://wiki.tockdom.com/wiki/Cealgair%27s_CT_Downloader. If you don't have an account on the Wiiki and you don't want to make one, you can contact me on discord (cealgair#9784), but keep in mind that I don't check it very often.
 
 
