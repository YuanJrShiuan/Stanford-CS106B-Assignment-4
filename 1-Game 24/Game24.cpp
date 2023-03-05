/*
 * File: Game 24.cpp
 * ----------------------------
 * Name: 樊望轩
 * This file is the starter code for the Game 24 problem
 * from Assignment #4.
 * [TODO: extend the documentation]
 */

#include <string>
#include "console.h"
#include "error.h"
#include "simpio.h"
#include "tokenscanner.h"
#include "vector.h"
#include "set.h"
#include "strlib.h"
#include "card.h"
#include "display.h"
using namespace std;

/* Constants */
const int BIGNUM = 24;
struct Label{
    string name;
    Set<string> addition;
    Set<string> mult;
    string aderse;
};
Set<string> countTwentyFours(Vector<Card> & cards);
Set<string> ReccountTwentyFours(Vector<Card> & cards,Vector<Label> &list);
bool no_add_dup(const Card p1,const Card p2,Vector<Label> &list);
void append_add(const Card p1,const Card p2,Vector<Label> &list);
bool no_mult_dup(const Card p1,const Card p2,Vector<Label> &list);
void append_mult(const Card p1,const Card p2,Vector<Label> &list);
void append_adverse(const Card p1,const Card p2,Vector<Label> &list,Card p3);
int main(){
    initGraphics("GAME 24");
    while (true){
        //string handCardsString = "5C 10S 4H 9C";
        string handCardsString = getLine("Please input a hand of Cards:");
        if (handCardsString == "") break;
        TokenScanner scanner(handCardsString);
        scanner.ignoreWhitespace();
        Vector<Card> cards;
        bool isLegalCard = true;
        while (scanner.hasMoreTokens()) {
            string tokenString = scanner.nextToken();
            Card handCard = Card(tokenString);
            if(!handCard.fail())
               cards.add(handCard);
            else
               isLegalCard = false;
        }
        int cardsNum = cards.size();
        if (isLegalCard && cardsNum > 1 && cardsNum < 7){
            Set<string> solutionsStrings = countTwentyFours(cards);
            displayHandCards(handCardsString, cardsNum);
            displaySolutions(solutionsStrings, cardsNum);
        }
        else
            cout << "Please give at least 2 cards, up to 6 cards!" << endl << endl;
    }
    cout<<endl<<"Game Over and the windows will be closed in a few seconds..."<<endl;
    pause(3000);
    closeGraphics();
    return 0;
}
Set<string> countTwentyFours(Vector<Card> & cards){ //嵌套递归函数
    Vector<Label> list;  //表示数组，用来减少重复
    return ReccountTwentyFours(cards,list);
}
Set<string> ReccountTwentyFours(Vector<Card> & cards,Vector<Label> &list){
    //   TODO: modify the code and implement the recursive strategy.
    Set<string> solutionsStrings = {};
    if(cards[0].getRank() == 24 && cards.size() == 1){  //满足条件退出,base case
        solutionsStrings += cards[0].toExpressionString();
        return solutionsStrings;
    }
    if(cards[0].getRank() != 24 && cards.size() <= 1){
        return solutionsStrings;
    }

    for(int i = 0;i<cards.size();i++){
        for(int j =0;j<cards.size();j++){
            if(i != j){
                Vector<Card> card_copy = cards;
                Vector<Card> results;
                if(i<j){
                    card_copy.remove(i);
                    card_copy.remove(j-1);
                }
                else{
                    card_copy.remove(j);
                    card_copy.remove(i-1);
                }
                if(no_add_dup(cards[i],cards[j],list)){                   
                    results += (cards[i] + cards[j]);
                    append_add(cards[i],cards[j],list);
                    Vector<Card> card_copy1 = card_copy;
                    card_copy1.add(results[results.size()-1]);
                    solutionsStrings += ReccountTwentyFours(card_copy1,list);
                }  //加法处理

                results += (cards[i] - cards[j]);
                append_adverse(cards[i],cards[j],list,(cards[i] - cards[j]));
                Vector<Card> card_copy2 = card_copy;
                card_copy2.add(results[results.size()-1]);
                solutionsStrings += ReccountTwentyFours(card_copy2,list);
                //减法处理
                if(no_mult_dup(cards[i],cards[j],list)){            
                    results += (cards[i] * cards[j]);
                    append_mult(cards[i],cards[j],list);
                    Vector<Card> card_copy3 = card_copy;
                    card_copy3.add(results[results.size()-1]);
                    solutionsStrings += ReccountTwentyFours(card_copy3,list);
                } //乘法处理

                results += (cards[i] / cards[j]);
                Vector<Card> card_copy4 = card_copy;
                card_copy4.add(results[results.size()-1]);
                solutionsStrings += ReccountTwentyFours(card_copy4,list);
                //除法处理
            }
        }
    }
    return solutionsStrings;
}
bool no_add_dup(const Card p1,const Card p2,Vector<Label> &list){  //判断是否两个数已经相加
    if(list.size() == 0) return true;
    for(int i = 0;i<list.size();i++){
        if(list[i].name == p1.toExpressionString()){
            if(list[i].addition.contains(p2.toExpressionString())) return false;
        }
    }
    return true;
}
void append_add(const Card p1,const Card p2,Vector<Label> &list){  //在标识中添加对应加数，用来判断
    bool flag1 = false;
    bool flag2 = false;
    if(list.size()!=0){
    for(int i = 0;i<list.size();i++){
        if(list[i].name == p1.toExpressionString()){
            list[i].addition.add(p2.toExpressionString());
            flag1 = true;
        }
        if(list[i].name == p2.toExpressionString()){
            list[i].addition.add(p1.toExpressionString());
            flag2 = true;
        }
    }
    }
    if(flag1 == false){

        Label a = {
            p1.toExpressionString(),{p2.toExpressionString()},{},""
        };
        list.add(a);
    }
    if(flag2 == false){
        Label b = {
            p2.toExpressionString(),{p1.toExpressionString()},{},""
        };
        list.add(b);
    }
}
bool no_mult_dup(const Card p1,const Card p2,Vector<Label> &list){  //判断是否两个数已经相乘或他们的相反数是否相乘
    if(list.size() == 0) return true;
    for(int i = 0;i<list.size();i++){
        if(list[i].name == p1.toExpressionString()){
            if(list[i].mult.contains(p2.toExpressionString())) return false;
        }
    }
    string adv2;
    string adv1;
    if(p1.getRank()*p2.getRank() == -p1.getRank()* -p2.getRank()){
        for(Label c:list){
            if(c.name == p2.toExpressionString()){
                adv1 = c.aderse;
            }
        }
        for(Label c:list){
            if(c.name == p1.toExpressionString()){
                adv2 = c.aderse;
            }
        }
        for(Label c:list){
            if(c.name == adv2 && c.mult.contains(adv1))
                return false;
        }
    }
    return true;
}
void append_mult(const Card p1,const Card p2,Vector<Label> &list){ //在标识中添加对应乘数，用来判断
    bool flag1 = false;
    bool flag2 = false;
    if(list.size()!=0){
    for(int i = 0;i<list.size();i++){
        if(list[i].name == p1.toExpressionString()){
            list[i].mult.add(p2.toExpressionString());
            flag1 =true;
        }
        if(list[i].name == p2.toExpressionString()){
            list[i].mult.add(p1.toExpressionString());
            flag2 = true;
        }
       }
    }
    if(flag1 == false){
        Label a = {
            p1.toExpressionString(),{},{p2.toExpressionString()},""
        };
        list.add(a);
    }
    if(flag2 == false){
        Label b = {
            p2.toExpressionString(),{},{p1.toExpressionString()},""
        };
        list.add(b);
    }
}
void append_adverse(const Card p1,const Card p2,Vector<Label> &list,const Card p3){  //标记相反数
    string pname = p3.toExpressionString();
    Card p4 = (p2 - p1);
    for(Label c :list){
        if(c.name == pname){
            c.aderse = p4.toExpressionString();
            break;
        }
    }
    Label m ={
        pname,{},{},p4.toExpressionString()
    };
    list.add(m);
}



