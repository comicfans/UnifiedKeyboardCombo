/*
 * =====================================================================================
 *
 *       Filename:  ToKeyMap.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014/12/14 12:30:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

struct Entry{
    int index;  
    string name;

};

int main(int argc,char ** argv){


    string name ;
    int hexValue;

    string results[256];
    fill_n (results,256,"invalid");

    while(cin>>name>>hex>>hexValue){
        results[hexValue]=name;
    }

    for(string str:results){
        cout<<"_T(\""<<str<<"\"),\n";
    }

}

