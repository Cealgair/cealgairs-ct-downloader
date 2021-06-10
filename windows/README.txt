CEALGAIR'S CT DOWNLOADER v. beta 2.2-1

 How to use
 ==========

  Setup
  -----
  
  This "setup" part only needs to be done once and for all.
  
  First of all, you'll need to install wit and wszst if you haven't already. You can find them at https://wit.wiimm.de/ and https://szs.wiimm.de/ respectively. Then, you'll need to install bash and wget (which you probably already have). If you want to use the icons program, you'll need imagemagick and its optional dependencies to convert svg files. Finally, you'll need to download and extract the latest version of LE-CODE (which can be found at https://download.wiimm.de/lecode/distribution-support/lecode-bin/) and WBZ Converter (which can be found at http://wiki.tockdom.com/wiki/WBZ_Converter). WBZ converter also needs to be set up, as explained in its readme.
  
  When everything is installed, you'll need an image of the Mario Kart Wii Disk. Once you have it, you'll need to extract all the files into a directory, which we'll call for example "original". This is done with wit, you can learn how at http://wiki.tockdom.com/wiki/Wiimms_ISO_Tools.
  
  At this point, you'll need to copy "original" into another directory, which we'll call for example "new". Then, you'll need to rename all of the szs files in "new/files/Race/Course", as explained in the introduction of http://wiki.tockdom.com/wiki/LE-CODE/Distribution_Tutorial. Then, follow the instructions given in the "Modified files" section of the same page.
  
  Now you'll need to edit three of the files provided with this toolser. "path-to-disc-root.txt" should contain the path to the directory we've called "new", ending with a /. Similarly, "path-to-wbz-converter.txt" should contain the path to the directory of WBZ Converter (the one that contains "convert-WBZ-to-SZS.sh" among other stuff). Also, "path-to-lecode-build.txt" should contain the path to the file "lecode-*.bin", where "*" can be "JAP", "KOR", "PAL" or "USA" depending on the version of the disc you have.
  
  Finally, you'll probably need to give yourself the permission to execute the various programs. To do so, run the following command:
  
  chmod u+x autoremove bmg builder ct-downloader icons patch bin/generate-icons
  
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
  
  This program allows you to make "CTFILE.txt", the former being an essential file to make the whole thing work and the other being a precursor to the file containing all of the track names.
  
  Usage of "patch"
  ----------------
  
  This program patches LE-CODE in order to make it work with your selection of tracks. If you want to change the settings that are set by this program, you can patch LE-CODE manually as explained at http://wiki.tockdom.com/wiki/LE-CODE/Distribution_Tutorial, or change its source code and re-compile it.
  
  Usage of "icons"
  ----------------
  
  This Program creates and copies your cup icons directly into your MKW Game
  
  Usage of "bmg"
  --------------
  
  This program patches the file containing all of the track names, according to what it finds in "tracks.bmg.txt". You might want to fix the names of the Nintendo tracks and those of the Wiimm cup before running this program.
  
  Build the ISO
  -------------
  
  When everything is done, you can create the iso for your distribution from the directory we called "new" using wit (again, check http://wiki.tockdom.com/wiki/Wiimms_ISO_Tools for how to do so).
  
  Usage of "autoremove"
  ------------------
  
  This program allows you to automatically remove all of the files that ct-downloader created. I implemented this because hundreds of unused szs files can take up valuable space and are very tedious to manually remove.
  
 Source code
 ===========
 
 I have included the source code with this toolset. You can do whatever you want with it, but please contact me first. I know I stated in the readme of the previous version that I didn't plan to update this toolset, but this is no longer the case.
 
 Keep in mind that you cannot download more than 20 tracks in a single hour from ct.wiimm.de due to the anti-bot measures that are in place, so that's why I implemented the "brain" function in "ct-downloader". Also, you can technically reduce the three seconds of sleep, but you might want to talk with Wiimm beforehand.
 
 I tried to be sensible while writing the code, and I even put some comments, but if you need anything to understand it better feel free to contact us via the same ways listed below for reporting bugs.
 
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
 
 First of all, check if you have the latest version. This includes versions made by other people that are modification of this toolset. If this is the latest version, then check if it has already been listed at http://wiki.tockdom.com/wiki/Cealgair%27s_CT_Downloader. If it hasn't, keep reading this section.
 
 The preferred way of reporting stuff is to report it on the talk/discussion page of this toolset at http://wiki.tockdom.com/wiki/Cealgair%27s_CT_Downloader. If you don't have an account on the Wiiki and you don't want to make one, you can contact Cealgair (cealgair#9784) or Fynn93 (Fynn#2867) on discord .
 
 Usage of Batch files
 ====================

 === patch.bat ===
 This Batchfile will patch your LECODE Binarys
 
 == How to use it ==
 Open Commandprompt and run: patch.bat "<lecode definiton file here>" "<lpar here>"
 

 === icons.bat ===
 This Batchfile will create the ct_icons.tpl file for you
 
 == How to use it ==
 Open Commandprompt and run: icons.bat "<png file of your cup icons here>"
 
 
 === bmg.bat ===
 This Batchfile patches Common.bmg
 
 == How to use it ==
 Open Commandprompt and run: bmg.bat "<name of definiton file here>"
 
 
 === builder.bat ===
 This Batchfile will create the definition file for you
 
 == How to use it ==
 Open Commandprompt and run: builder.bat "<name of definition file here>"