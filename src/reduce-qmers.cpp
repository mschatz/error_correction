#include  <string>
#include  <iostream>
using namespace std;

long long CNT_ALL = 0;
long long CNT_PRINTED = 0;

////////////////////////////////////////////////////////////////////////////////
// reduce-qmers
//
// Accept sorted input from count-qmers and reduce by summing counts with the
// same header (which will always be adjacent).
////////////////////////////////////////////////////////////////////////////////
int  main (int argc, char * argv [])
{
     string mykmer, kmer;
     double mycount, count;
     
     // read initial line
     cin >> mykmer;
     cin >> mycount;

     CNT_ALL++;
     
     while(cin >> kmer) 
     {
       cin >> count;
         
       CNT_ALL++;
        
        // if same as last, increment count
        if(kmer == mykmer) 
        {
            mycount += count;
             
        // else print last, initialize new
        } else {
            cout << mykmer << "\t" << mycount << endl;
            CNT_PRINTED++;

            mykmer = kmer;
            mycount = count;
        }
     }
     
     // print last
     cout << mykmer << "\t" << mycount << endl;
     CNT_PRINTED++;

     cerr << "Printed " << CNT_PRINTED << " of " << CNT_ALL << endl;
     
     return 0;
}
