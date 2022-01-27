# Minesweeper
This is a simple minesweeper game created with Qt that I challenged myself to make the majority of in under 12 hours.

![](/Pictures/mainPicture.png)


## Playing the Game
  - ### Rules
    - Like minesweeper, there is a grid of squares and you are trying to uncover every square that isn't a bomb.
    - The number on each square when revealed will tell you how many bombs are around it.
    - Right clicking squares will flag them as bombs, placing an "X" on them and decreasing the bomb counter. It looks like this:

      ![](/Pictures/bombFlagExample.png)
      
    - If a square has all possible bombs around it flagged, you can middle click to reveal every additional square around it (and possibly more). Here's an example:
     
      Before middle click (on the 3):
      
      ![](/Pictures/middleClickBegin.png)  
      
      After middle click:
      
      ![](/Pictures/middleClickEnd.png)
      
  - ### Losing
    - If you click on a bomb, or middle click and reveal a bomb, you will lose, as shown here:
  
      ![](/Pictures/losePicture.png)
      
- ### Winning
  - Once you have clicked every square that is not a bomb, you will win! An example is shown for you here:

    ![](/Pictures/winPicture.png)
    
- ### Resetting the game
  - The handy button at the top of the program will reset the game for you at any time, if you feel that you just want to start over, or you won and want to play again.


## Building the Project on Windows
To build the application yourself (for developers), follow these steps.

1. Download and install [Qt Open Source (6.2.1)](https://www.qt.io/download-open-source).
  1. If you already have a Qt installation go to its directory and run `MaintenanceTool`.
  2. Create an account if you do not have one or log in.
  3. Click through the Wizard selecting *Add or remove components*.
  4. Find *Qt 6.2.1* in the list, expand it, and select the following items:
    - MSVC 2019 64-bit
    - Sources *(optional, for debugging)*
    - Qt Debug Information Files *(optional, for debugging)*
2. Download and install [Visual Studio 2019: Community 2019](https://visualstudio.microsoft.com/vs/).
  1. In the **Workloads** tab, select *Desktop development with C++*.
  2. In the **Individual components** tab, select *Windows 10 SDK*.
  3. Once installed, open `Minesweeper.sln` then go to *Extensions* -> *Manage extensions*, find and download **Qt Visual Studio Tools**.
  4. Go to *Extensions* -> *Qt VS Tools* -> *Qt Versions* and add your Qt version to the list (as `6.2.1_msvc2019_64`).
  5. Build the project.

