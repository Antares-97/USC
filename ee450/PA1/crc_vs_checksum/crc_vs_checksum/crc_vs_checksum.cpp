//
//  crc_vs_checksum.cpp
//  crc_vs_checksum
//
//  Created by LJZ's macbook pro on 9/22/19.
//  Copyright © 2019 LJZ's macbook pro. All rights reserved.
//

#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include <sstream>
using namespace std;

/**
    Transfer other type of variants into String
 */
template <typename T>
std::string ToString(T val)
{
    std::ostringstream stream;
    stream << val;
    return stream.str();
}


/**
    Introduce random error on data sequence D, according to the error bits sequence T.
    D and T has the same length
 */
string random_error(string D, string T)
{
    string _D(D.begin(), D.end());
    for (int i=0; i<T.length(); i++)
    {
        if(T[i]=='1')                   // If T[i] is 1, then flip D's bit at the corresponding position.
        {
            switch (_D[i])
            {
                case '1':
                    _D[i]='0';
                    break;
                    
                case '0':
                    _D[i]='1';
                    break;
            }
        }
    }
    return _D;
}


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


/**
    Compute the Remainer of Data D divided by the generator G
    Returns the string D appended with R at its end.
 */
string CRC(string D, string G)
{
    string _D(D.begin(), D.end());
    long len = _D.length();
    for (int i=0; i<G.length()-1; i++)  _D.append("0");       // Compact Data with zeros
    string R = binary_divide(_D.substr(0, G.length()), G);
    for (int i=0; i<len-1; i++)
    {
        R = R.append(_D, (G.length()+i), 1);
        R = binary_divide(R, G);
    }
    _D.replace(len, R.length(), R);
    return _D;
}


/**
    Given a 1/0 string, compute the decimal number.
 */
int Bin2Dec(string str)
{
    long long len = str.length();
    int num = 0;
    for (int i=0; i<len; i++)
    {
        if(str[i]=='0') continue;
        num += pow(2, len-1-i);
    }
    return num;
}


/**
    Given a integer smaller than 255 (inclusive),
    returns a string indicates the binary version of this number.
    The string always has 8 bits. (255 == 11111111 (binary))
 */
string Dec2Bin(int num)
{
    string str = "";
    while(num > 1)
    {
        str.append(ToString(num % 2));
        num = num / 2;
    }
    str.append(ToString(num));
    num = int(8 - str.length());
    while(num--) str.append("0");               // compact the answer string with zeros according to its length
    std::reverse(str.begin(), str.end());       // The right answer is the reversed string.
    return str;
}


/**
    Compute the one-byte checksum of a string D
    In this algorithm, we suppose that the length of D is the multiple of 8
    Therefore, we compute the number of every 8 bits, and sum them up
 */
int checksum(string D)
{
    int checksum = 0;
    for (int i=0; i<D.length(); )
    {
        checksum += Bin2Dec(D.substr(i, 8));             // length of the substring is always 8
        i += 8;
    }
    return checksum;
}


/**
    Produce the cheksum of string D according to 1-byte checksum algorithm
    Returns string D appended with checksum string at its end.
 */
string one_byte_checksum(string D)                       // Compute the 8-bit checksum of D
{
    string _D(D.begin(), D.end());
    int cks = checksum(_D);
    int Q = cks / 256;
    int R = cks % 256;
    while( (Q+R) > 255 )
    {
        int S = Q + R;
        Q = S / 256;
        R = S % 256;
    }
    cks = 255 - (Q+R);
    _D.append(Dec2Bin(cks));
    return _D;
}


/**
    This part computes CRC and 1-byte checksum of a data string D separately
    And then flip the answer string according to the random error string T
    Computes the F-1 score respectively
 */
void crc_VS_cks()
{
    string D;
    string G;
    string T;
    int TP_crc = 0, TP_cks = 0;
    int FP_crc = 0, FP_cks = 0;
    int FN_crc = 0, FN_cks = 0;
    
    while(cin>>D>>G>>T)                     // Stores data D, generator G, and random error sequence T
    {
        string crc = CRC(D, G);             // CRC string = D + R
        string cks = one_byte_checksum(D);  // checksum string = D + CheckSum

        size_t found = T.find("1");         // See if random error sequence T is an all-zero sequence
        if (found==std::string::npos)       // all zeros, no error introduced, compute TP and FN
        {
            string check_crc = CRC(crc, G).substr(crc.length(), G.length()-1);      //check CRC
            found = check_crc.find("1");
            if (found==std::string::npos)   // R is all zeros, passes
                TP_crc++;
            else FN_crc++;
                
            if( (checksum(cks.substr(0, cks.length()-8)) +
                    checksum(cks.substr(cks.length()-8, 8))) % 255 == 0)     // checksum equal to zero, pass
                TP_cks++;
            else FN_cks++;
            continue;
        }
        //  Else, flip the bits according to the random error sequence T
        string error_crc = random_error(crc, T);    // check error CRC sequence
        string check_crc = CRC(error_crc, G).substr(error_crc.length(), G.length()-1);
        found = check_crc.find("1");
        if (found==std::string::npos)               // R is all zeros, passes
            FP_crc++;
            
        string error_cks = random_error(cks, T);    // check error Checksum sequence
        if( (checksum(error_cks.substr(0, error_cks.length()-8)) +
             checksum(error_cks.substr(error_cks.length()-8, 8))) % 255 == 0)     // sum equal to zero, pass
            FP_cks++;
    }
    cout<< TP_crc << " " << FP_crc << " " << FN_crc << " " << TP_cks << " " << FP_cks << " " << FN_cks << endl;
        
    double precision_crc = double(TP_crc) / (TP_crc + FP_crc);
    double recall_crc = double(TP_crc) / (TP_crc + FN_crc);
    double precision_cks = double(TP_cks) / (TP_cks + FP_cks);
    double recall_cks = double(TP_cks) / (TP_cks + FN_cks);
    cout<< precision_crc << " " << recall_crc << " " << precision_cks << " " << recall_cks<<endl;
    
    cout<<"F1 score of 1-byte checksum: " << (2.0 * precision_cks * recall_cks) /
                                             (precision_cks + recall_cks) << endl;
    cout<<"F1 score of CRC: " << (2.0 * precision_crc * recall_crc) /
                                 (precision_crc + recall_crc) << endl;
}


int main()
{
    freopen("./data2Vs.txt", "r", stdin);
    crc_VS_cks();
    return 0;
}

