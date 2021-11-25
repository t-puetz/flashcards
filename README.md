# flashcards

Super simple flashcards program based on GTK 4 

#### Dependencies
  - GTK 4 (v3.99.1 for now but final v4.0 is scheduled to be out soon!)
  - ffmpeg that provides ffplay (to play the sound of the front side from 
  the sound file specified in the 4th column of your CSV deck file)

Flashcards can be loaded from a CSV file.

#### Features:
* Done
  - Load deck and as many decks after that in the same session
  - Play sound for front side (For vocab training: where the foreign to learn language should be)
  - Frorward, reverse and flip button
* Todo
  - Filter the files the GtkFileChooser shows (GtkFileFilter will not work, I tried many implementations)
  - Make flashcard box (Label+Frame+Fixed) resize with the size of its text

#### Required format of a card deck you want to load:
* It has to be a semicolon seperated CSV
* The header needs to look like this:
  - id;front_content;back_content;front_sound_file_path
* The id needs to be a valid integer
* The other columns need to be valid strings
* You may use spaces and newlines ('\n') in those columns
  - They will be interpreted correctly and help you position the text nicely in the GUI's centered place
    where the card(s) reside (GtkFrame with a GtkLabel as the child. The GtkLabel actually displays the text)

#### Build systems
* GNU make (will add clang support later, for now just replace gcc in Makefile with clang and it will work)
* zig: Zig version: master @ 58ee5f4
       Install newest zig from master (glibc 2.32 support bugfixes are not in official release, yet)
       run: rm -rf zig-cache ; zig build -Drelease-fast


