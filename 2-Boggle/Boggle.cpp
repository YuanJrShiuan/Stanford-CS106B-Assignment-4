/*
 * File: Boggle.cpp
 * -----------------------
 * Name: [TODO: enter name here]
 * This file is the starter code for the Boggle Game problem
 * from Assignment #4.
 * [TODO: extend the documentation]
 */

#include <cctype>
#include <fstream>
#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
//#include <QMediaPlayer>
#include "console.h"

using namespace std;

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const string STANDARD_CUBES_FIlE = "cubes16.txt";
const string BIG_BOGGLE_CUBES_FILE = "cubes25.txt";

/* Function prototypes */

void welcome();
void giveInstructions();
void playBoggle();
void FillBoardWithWords(Grid<char> & TheBoggleBoard);
void SetupByComputer(Grid<char> & TheBoggleBoard);
void SetupByUser(Grid<char> & TheBoggleBoard);
void CheckTheWordOfMan(const string TheWordOfMan,const Grid<char> & TheBoggleBoard,Lexicon & usedWords,const Lexicon & english);
bool VaildOnTheBoard(const string TheWordOfMan,const Grid<char> & TheBoggleBoard,Grid<bool> &user_flag);
Set<GridLocation> RecVaild(GridLocation tag,GridLocation pre_p,const string &word,const string &str,Set<GridLocation> nei,const Grid<char> & TheBoggleBoard,const Set<GridLocation> &highlightpoints);
Set<GridLocation> Findneighbor(GridLocation p,int row,int col);
void RecOfComputer(Set<string> &computer_word,Lexicon &english,const Grid<char> & TheBoggleBoard,GridLocation tag,GridLocation pre_p,string current,Set<GridLocation> nei);
void TheTurnOfComputer(const Grid<char> & TheBoggleBoard,Lexicon & usedWords,Lexicon &english);
/* Main program */

// You need to design specific data structures and functions
// to complete this assignment.

int main() {

    welcome();
    if (getYesOrNo("Do you need instructions? ")) {
       giveInstructions();
    }

   playBoggle();
   return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
   cout << "Welcome!  You're about to play an intense game ";
   cout << "of mind-numbing Boggle.  The good news is that ";
   cout << "you might improve your vocabulary a bit.  The ";
   cout << "bad news is that you're probably going to lose ";
   cout << "miserably to this little dictionary-toting hunk ";
   cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
   cout << endl;
   cout << "The boggle board is a grid onto which I ";
   cout << "I will randomly distribute cubes. These ";
   cout << "6-sided cubes have letters rather than ";
   cout << "numbers on the faces, creating a grid of ";
   cout << "letters on which you try to form words. ";
   cout << "You go first, entering all the words you can ";
   cout << "find that are formed by tracing adjoining ";
   cout << "letters. Two letters adjoin if they are next ";
   cout << "to each other horizontally, vertically, or ";
   cout << "diagonally. A letter can only be used once ";
   cout << "in each word. Words must be at least four ";
   cout << "letters long and can be counted only once. ";
   cout << "You score points based on word length: a ";
   cout << "4-letter word is worth 1 point, 5-letters ";
   cout << "earn 2 points, and so on. After your puny ";
   cout << "brain is exhausted, I, the supercomputer, ";
   cout << "will find all the remaining words and double ";
   cout << "or triple your paltry score." << endl << endl;
   cout << "Hit return when you're ready...";
   getLine();
}

void playBoggle()
{
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    gw.setLocation(50,50);
    initGBoggle(gw);
    Lexicon english("EnglishWords.txt");

    while (true)
    {
        drawBoard(4, 4);
        initializeScoreBox();
        Grid<char> TheBoggleBoard(4,4);
        Lexicon usedWords={};

        FillBoardWithWords(TheBoggleBoard);

        cout<<"Ok, take all the time you want and find all the words you can!"<<endl;
        cout<<"Signal that you're finished by entering an empty line."<<endl;

        auto TheWordOfMan=toLowerCase(getLine("Enter a word:"));

        while(TheWordOfMan!="")
        {
            CheckTheWordOfMan(TheWordOfMan,TheBoggleBoard,usedWords,english);
            TheWordOfMan=toLowerCase(getLine("Enter a word:"));
        }

        TheTurnOfComputer(TheBoggleBoard,usedWords,english);

        if (!getYesOrNo("Would you like to play again? ")) break;

        recordWordForPlayer("",HUMAN);
        recordWordForPlayer("",COMPUTER);
    }
    exitGraphics();
}

void FillBoardWithWords(Grid<char> & TheBoggleBoard)
{
    cout<<endl;
    cout<<"I'll give you a chance to set up the board to your specification,";
    cout<<"which makes it easier to confirm your boggle program is working."<<endl;

    if(!getYesOrNo("Do you want to force the board configuration?"))
        SetupByComputer(TheBoggleBoard);
    else
        SetupByUser(TheBoggleBoard);

    pause(1000);
}

void SetupByComputer(Grid<char> & TheBoggleBoard)
{
    fstream cubes16("cubes16.txt");
    string line;
    Vector<string> ChooseWord={};

    getline(cubes16,line);
    while(line!="")
    {
        ChooseWord+=line;
        getline(cubes16,line);
    }

    for(int i=0;i<ChooseWord.size();i++)
    {
        auto r=randomInteger(i,ChooseWord.size()-1);
        swap(ChooseWord[i],ChooseWord[r]);
    }


    for(int i=0;i<4;++i)
    {
        for(int j=0;j<4;++j)
        {
            int r=randomInteger(0,5);
            TheBoggleBoard[i][j]=ChooseWord[i*4+j][r];
            labelCube(i,j,TheBoggleBoard[i][j]);
        }
    }
    cubes16.close();
}

void SetupByUser(Grid<char> & TheBoggleBoard)
{
    cout<<"Enter a 16-character string to identify which letters you want on the cubes."<<endl;
    cout<<"The first 4 letters are the cubes on the top row from left to right,"
          "the next 4 letters are the second row, and so on."<<endl;

    string string=toLowerCase(getLine("Enter the string:"));
    while(string.length()!=16)
       string=getLine("String must include 16 characters! Try again:" );

    int r=0;
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
        {
            TheBoggleBoard[i][j]=string[r++];
            labelCube(i,j,TheBoggleBoard[i][j]);
        }
}

void CheckTheWordOfMan(const string TheWordOfMan,const Grid<char> & TheBoggleBoard,
                        Lexicon & usedWords,const Lexicon & english)
{
    if(TheWordOfMan.size()<4)
    {
        cout<<"I'm sorry,but we have our standards."<<endl;
        cout<<"That word doesn't meet the minimum word length 4."<<endl;
        return;
    }

    if(!english.contains(TheWordOfMan))
    {
        cout<<"That's not a word!"<<endl;
        return;
    }

    if(usedWords.contains(TheWordOfMan))
    {
        cout<<"You've already guessed that!"<<endl;
        return;
    }

    Grid<bool> user_flag(TheBoggleBoard.numRows(),TheBoggleBoard.numCols());
    user_flag.fill(false);       //用一个一样大小的Grid在找到的字母处做标记，用来高亮
    if(VaildOnTheBoard(TheWordOfMan,TheBoggleBoard,user_flag))
    {
        usedWords.add(TheWordOfMan);
        for(int i = 0;i<TheBoggleBoard.numRows();i++){
            for(int j = 0;j<TheBoggleBoard.numCols();j++)
                highlightCube(i,j,user_flag[i][j]);
        }
        recordWordForPlayer(TheWordOfMan,HUMAN);
    }
}

bool VaildOnTheBoard(const string TheWordOfMan,const Grid<char> & TheBoggleBoard,Grid<bool> &user_flag)
{
    Set<GridLocation> highlightpoints = {};  //存储正确方格的坐标
    for(int i =0;i<TheBoggleBoard.numRows();i++){
        for(int j =0;j<TheBoggleBoard.numCols();j++){   //对于每一个和字符串首字母相同的格子进行递归
            if(toLowerCase(TheBoggleBoard[i][j]) == TheWordOfMan[0]){
                Set<GridLocation> d = {};
                GridLocation tag(i,j);
                GridLocation pre_p(-1,-1);
                string first;
                first += TheWordOfMan[0];
                highlightpoints += RecVaild(tag,pre_p,TheWordOfMan,first,Findneighbor(tag,TheBoggleBoard.numRows(),TheBoggleBoard.numCols()),TheBoggleBoard,d +tag);
            }
        }
    }

    if(highlightpoints.size()!=0){   //坐标存在则将其标记
        for(GridLocation p:highlightpoints){
            user_flag.set(p,true);
        }
        return true;
    }
    return false;
}

Set<GridLocation> RecVaild(GridLocation tag,GridLocation pre_p,const string &word,const string &str,Set<GridLocation> nei,const Grid<char> & TheBoggleBoard,const Set<GridLocation> &highlightpoints){
    if(str == word){
        auto tmp = highlightpoints;
        tmp += tag;
        return tmp;
    }
    if(str != word.substr(0,str.size())){
        return {};
    }
    Set<GridLocation> final;
    for(GridLocation p:nei){
        if(p != pre_p){
            char m = toLowerCase(TheBoggleBoard.get(p));
            final += RecVaild(p,tag,word,str+m,Findneighbor(p,TheBoggleBoard.numRows(),TheBoggleBoard.numCols()),TheBoggleBoard,highlightpoints+p);
        }
    }
    return final;

}

Set<GridLocation> Findneighbor(GridLocation p,int row,int col){   //找该点的合法邻居
   Set<GridLocation> nei;
   int r = p.row;
   int c = p.col;
   if(r-1>=0) {
       GridLocation f(r-1,c);
       nei.add(f);
   }
   if(r+1<=row-1){
       GridLocation f(r+1,c);
       nei.add(f);
   }
   if(c+1<=col-1){
       GridLocation f(r,c+1);
       nei.add(f);
   }
   if(c-1>=0){
       GridLocation f(r,c-1);
       nei.add(f);
   }
   if(c-1>=0 && r-1>=0){
       GridLocation f(r-1,c-1);
       nei.add(f);
   }
   if(c-1>=0 && r+1<=row-1){
       GridLocation f(r+1,c-1);
       nei.add(f);
   }
   if(c+1<=col-1 && r-1>=0){
       GridLocation f(r-1,c+1);
       nei.add(f);
   }
   if(c+1<=col-1 && r+1<=row-1){
       GridLocation f(r+1,c+1);
       nei.add(f);
   }
   return nei;
}
void TheTurnOfComputer(const Grid<char> & TheBoggleBoard,Lexicon & usedWords,Lexicon &english)
{
    Set<string> computer_word = {};
    for(int i =0;i< TheBoggleBoard.numRows();i++){  //对每一个方格做递归
        for(int j = 0;j< TheBoggleBoard.numCols();j++){
            GridLocation tag(i,j);
            GridLocation pre_p(-1,-1);
            RecOfComputer(computer_word,english,TheBoggleBoard,tag,pre_p,"",Findneighbor(tag,TheBoggleBoard.numRows(),TheBoggleBoard.numCols()));
        }
    }
    for(string p : computer_word){
        if(! usedWords.contains(p) && p.size()>4){
            recordWordForPlayer(p,COMPUTER);
        }
    }
}
void RecOfComputer(Set<string> &computer_word,Lexicon &english,const Grid<char> & TheBoggleBoard,GridLocation tag,GridLocation pre_p,string current,Set<GridLocation> nei){
    for(GridLocation p :nei){
        if(p != pre_p){
            string test = current + toLowerCase(TheBoggleBoard.get(p));
            if(english.contains(test) && (!(computer_word.contains(test)))){
                computer_word.add(test);
            }
            if(english.containsPrefix(test)){
                RecOfComputer(computer_word,english,TheBoggleBoard,p,tag,test,Findneighbor(p,TheBoggleBoard.numRows(),TheBoggleBoard.numCols()));
            }
        }
    }
}






