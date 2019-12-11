//
//  crc_tx.cpp
//  EE450_PA1
//
//  Created by LJZ's macbook pro on 9/19/19.
//  Copyright © 2019 LJZ's macbook pro. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
using namespace std;


/**
    Given two strings D (as dividend), and G (as divisor)
    Compute a single step of division, returns the remainer R.
 */
string binary_divide(string D, string G)
{
    if(D[0]=='0')
    {
        string R(D.begin()+1, D.end());
        return R;
    }
    else
    {
        string R = "";
        for (int i=1; i<G.length(); i++)
        {
            if (D[i]==G[i]) R.append("0");
            else R.append("1");
        }
        return R;
    }
}


int main()
{
    freopen("./data2Tx.txt", "r", stdin);                         // read data from files.

    string D;
    string G;
    while(cin>>D>>G)                                              // D stores Data, G stores Generator sequence
    {
        long len = D.length();
        for (int i=0; i<G.length()-1; i++)  D.append("0");        // To compute CRC, first compact Data with zeros.
        
        string S = D.substr(0, G.length());
        string R = binary_divide(S, G);
        for (int i=0; i<len-1; i++)
        {
            R = R.append(D, (G.length()+i), 1);
            R = binary_divide(R, G);                             // Compute the remainer of D divided by G, stores in string R
        }
        
        D.replace(len, R.length(), R);                           // append the remainer R at the end of D.
        cout<<"codeword:\n"<<D<<endl;
        cout<<"crc:\n"<<R<<endl;
    }

    return 0;
}

