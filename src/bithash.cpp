#include "bithash.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace::std;

bithash::bithash(int _k)
   :bits( (unsigned long long int)pow(4.0,_k) )
{
  k = _k;
  mask = (unsigned long long)pow(4.0,k) - 1;
}

bithash::~bithash() {
}

////////////////////////////////////////////////////////////
// add
//
// Add a single sequence to the bitmap
////////////////////////////////////////////////////////////
void bithash::add(unsigned long long kmer) {
  bits.set(kmer);
}


////////////////////////////////////////////////////////////
// check
//
// Check for the presence of a sequence in the tree
//
// Can handle N's!  Returns False!
////////////////////////////////////////////////////////////
bool bithash::check(unsigned kmer[]) {
  unsigned long long kmermap = 0;
  for(int i = 0; i < k; i++) {
    if(kmer[i] < 4) {
      kmermap <<= 2;
      kmermap |= kmer[i];
    } else
      return false;
  }
  return bits[kmermap];
}

////////////////////////////////////////////////////////////
// check
//
// Check for the presence of a sequence in the tree.
// Pass the kmer map value back by reference to be re-used
//
// Can't handle N's!
////////////////////////////////////////////////////////////
bool bithash::check(unsigned kmer[], unsigned long long & kmermap) {
  kmermap = 0;
  for(int i = 0; i < k; i++) {
    if(kmer[i] < 4) {
      kmermap <<= 2;
      kmermap |= kmer[i];
    } else {
      cerr << "Non-ACGT given to bithash::check" << endl;
      exit(EXIT_FAILURE);
    }
  }
  return bits[kmermap];
}

////////////////////////////////////////////////////////////
// check
//
// Check for the presence of a sequence in the tree.
////////////////////////////////////////////////////////////
bool bithash::check(unsigned long long kmermap) {
  return bits[kmermap];
}

////////////////////////////////////////////////////////////
// check
//
// Check for the presence of a sequence in the tree.
// Pass the kmer map value back by reference to be re-used
//
// Can't handle N's!
////////////////////////////////////////////////////////////
bool bithash::check(unsigned long long & kmermap, unsigned last, unsigned next) {
  if(next >= 4) {
    cerr << "Non-ACGT given to bithash::check" << endl;
    exit(EXIT_FAILURE);
  } else {
    kmermap <<= 2;
    kmermap &= mask;
    kmermap |= next;
  }
  return bits[kmermap];
}


////////////////////////////////////////////////////////////
// file_load
//
// Make a prefix_tree from kmers in the file given that
// occur >= "boundary" times
////////////////////////////////////////////////////////////
void bithash::meryl_file_load(const char* merf, const double boundary) {
  ifstream mer_in(merf);
  string line;
  double count;
  bool add_kmer = false;

  while(getline(mer_in, line)) {
    if(line[0] == '>') {
      // get count
      count = atof(line.substr(1).c_str());
      //cout << count << endl;
      
      // compare to boundary
      if(count >= boundary) {
	add_kmer = true;
      } else {
	add_kmer = false;
      }

    } else if(add_kmer) {
      // add to tree
      add(binary_kmer(line));

      // add reverse to tree
      add(binary_rckmer(line));
    }
  }
}

////////////////////////////////////////////////////////////
// file_load
//
// Make a prefix_tree from kmers in the file given that
// occur >= "boundary" times
////////////////////////////////////////////////////////////
void bithash::tab_file_load(istream & mer_in, const double boundary, unsigned long long atgc[]) {
  string line;
  double count;

  while(getline(mer_in, line)) {
    if(line[k] != ' ' && line[k] != '\t') {
      cerr << "Kmers are not of expected length " << k << endl;
      exit(EXIT_FAILURE);
    }

    // get count
    count = atof(line.substr(k+1).c_str());
    //cout << count << endl;
      
    // compare to boundary
    if(count >= boundary) {
      // add to tree
      add(binary_kmer(line.substr(0,k)));

      // add reverse to tree
      add(binary_rckmer(line.substr(0,k)));

      // count gc
      if(atgc != NULL) {
	unsigned int at = count_at(line.substr(0,k));
	atgc[0] += at;
	atgc[1] += (k-at);
      }
    }
  }
}

////////////////////////////////////////////////////////////
// file_load
//
// Make a prefix_tree from kmers in the file given that
// occur >= "boundary" times
////////////////////////////////////////////////////////////
void bithash::tab_file_load(istream & mer_in, const vector<double> boundary, unsigned long long atgc[]) {
  string line;
  double count;
  int at;

  while(getline(mer_in, line)) {
    if(line[k] != '\t') {
      cerr << "Kmers are not of expected length " << k << endl;
      exit(EXIT_FAILURE);
    }

    at = count_at(line.substr(0,k));

    // get count
    count = atof(line.substr(k+1).c_str());
    //cout << count << endl;
      
    // compare to boundary
    if(count >= boundary[at]) {
      // add to tree
      add(binary_kmer(line.substr(0,k)));

      // add reverse to tree
      add(binary_rckmer(line.substr(0,k)));

      // count gc
      if(atgc != NULL) {
	unsigned int my_at = count_at(line.substr(0,k));
	atgc[0] += my_at;
	atgc[1] += (k-my_at);
      }
    }
  }
}

////////////////////////////////////////////////////////////
// binary_file_output
//
// Write bithash to file in binary format
////////////////////////////////////////////////////////////
void bithash::binary_file_output(char* outf) {
  unsigned long long mysize = (unsigned long long)bits.size() / 8ULL;
  char* buffer = new char[mysize];
  unsigned int flag = 1;
  for(unsigned long long i = 0; i < mysize; i++) {
    unsigned int temp = 0;
    for(unsigned int j = 0; j < 8; j++) { // read 8 bits from the bitset
      temp <<= 1;
      //unsigned int tmp = i*8 + j;
      //cout << tmp << ",";
      if(bits[i*8 + j])
	temp |= flag;
    }
    buffer[i] = (char)temp;
  }
  ofstream ofs(outf, ios::out | ios::binary);
  ofs.write(buffer, mysize);
  ofs.close();
}

////////////////////////////////////////////////////////////
// binary_file_input
//
// Read bithash from file in binary format
////////////////////////////////////////////////////////////
/*
void bithash::binary_file_input(char* inf) {
  ifstream ifs(inf, ios::binary);

  // get size of file
  ifs.seekg(0,ifstream::end);
  unsigned long long mysize = ifs.tellg();
  ifs.seekg(0);

  // allocate memory for file content
  char* buffer = new char[mysize];

  // read content of ifs
  ifs.read (buffer, mysize);

  // parse bits
  unsigned int flag = 128;
  unsigned int temp;
  for(unsigned long i = 0; i < mysize; i++) {
    temp = (unsigned int)buffer[i];
    for(unsigned int j = 0; j < 8; j++) {
      if((temp & flag) == flag)
	bits.set(i*8 + j);
      temp <<= 1;
    }
  }

  delete[] buffer;
}
*/

////////////////////////////////////////////////////////////
// binary_file_input
//
// Read bithash from file in binary format
////////////////////////////////////////////////////////////
void bithash::binary_file_input(char* inf, unsigned long long atgc[]) {
  unsigned int flag = 128;
  unsigned int temp;

  ifstream ifs(inf, ios::binary);

  // get size of file
  ifs.seekg(0,ifstream::end);
  unsigned long long mysize = ifs.tellg();
  ifs.seekg(0);

  // allocate memory for file content
  unsigned long long buffersize = 134217728;  // i.e. 4^15 / 8, 16 MB
  if(mysize < buffersize)
       buffersize = mysize;
  char* buffer = new char[buffersize];

  for(unsigned long long b = 0; b < mysize/buffersize; b++) {

    cerr << "loading bit batch: " << b << " of " << (mysize/buffersize) << endl;

    // read content of ifs
    ifs.read (buffer, buffersize);

    // parse bits
    for(unsigned long long i = 0; i < buffersize; i++) {
      temp = (unsigned int)buffer[i];
      for(int j = 0; j < 8; j++) {
	if((temp & flag) == flag) {
	  bits.set((buffersize*b + i)*8 + j);
	  
	  // count gc
	  unsigned int at = count_at((buffersize*b + i)*8 + j);
	  atgc[0] += at;
	  atgc[1] += (k-at);
	}
	temp <<= 1;
      }
    }
  }

  delete[] buffer;
}

////////////////////////////////////////////////////////////
// count_at
//
// Count the A's and T's in the sequence given
////////////////////////////////////////////////////////////
int bithash::count_at(string seq) {
  int at = 0;
  for(int i = 0; i < seq.size(); i++)
    if(seq[i] == 'A' || seq[i] == 'T')
      at +=  1;
  return at;
}

int bithash::count_at(unsigned long long seq) {
  int at = 0;
  unsigned long long mask = 3;
  unsigned long long nt;
  for(int i = 0; i < k; i++) {
    nt = seq & mask;
    seq >>= 2;

    if(nt == 0 || nt == 3)
      at++;
  }
  return at;
}

//  Convert string  s  to its binary equivalent in  mer .
unsigned long long  bithash::binary_kmer(const string & s) {
  int  i;
  unsigned long long mer = 0;
  for  (i = 0; i < s.length(); i++) {
    mer <<= 2;
    mer |= binary_nt(s[i]);
  }
  return mer;
}

//  Convert string s to its binary equivalent in mer .
unsigned long long  bithash::binary_rckmer(const string & s) {
  int  i;
  unsigned long long mer = 0;
  for  (i = s.length()-1; i >= 0; i--) {
    mer <<= 2;
    mer |= 3 - binary_nt(s[i]);
  }
  return mer;
}

//  Return the binary equivalent of  ch .
unsigned bithash::binary_nt(char ch) {
  switch  (tolower (ch)) {
  case  'a' : return  0;
  case  'c' : return  1;
  case  'g' : return  2;
  case  't' : return  3;
  }
}


unsigned int bithash::num_kmers() {
  return (unsigned int)bits.count();
}
