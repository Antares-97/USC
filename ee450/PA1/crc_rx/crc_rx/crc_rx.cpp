//
//  crc_rx.cpp
//  crc_rx
//
//  Created by LJZ's macbook pro on 9/22/19.
//  Copyright © 2019 LJZ's macbook pro. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
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
    /**
       Decide whether data D should be accepted according to generator G or not.
     */
    freopen("./data2Rx.txt", "r", stdin);
    
    string D;
    string G;
    while(cin>>D>>G)
    {
        long len = D.length();
        
        string S = D.substr(0, G.length());
        string R = binary_divide(S, G);
        for (int i=0; i<len-G.length(); i++)
        {
            R = R.append(D, (G.length()+i), 1);
            R = binary_divide(R, G);          // Computes Remainer of D divided by G, if R=0, then passes
        }

        size_t found = R.find("1");           // R is a sequence, we try to detect whether R is an all-zero sequence or not
        if (found!=std::string::npos)         // Find an "1" at certain position of R, which means the R is not all zeros, failes
            cout<<"Not pass"<<endl;
        else cout<<"pass"<<endl;
    }
    
    return 0;
}

