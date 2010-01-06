#ifndef READ_H
#define READ_H

//#include "prefix_tree.h"
#include "bithash.h"
#include <string>
#include <vector>
#include <fstream>

using namespace::std;

////////////////////////////////////////////////////////////
// correction
//
// Simple structure for corrections to reads
////////////////////////////////////////////////////////////
class correction {
public:
  correction(int i, int t) {
    index = i;
    to = t;
  };
  // default copy constructor should suffice

  int index;
  //int from;
  int to;
};

////////////////////////////////////////////////////////////
// corrected_read
//
// Simple structure for corrected reads
////////////////////////////////////////////////////////////
class corrected_read {
public:
 corrected_read(vector<correction*> & c, vector<int> & u, float l, int re, bool ch)
    :untrusted(u) {
    likelihood = l;
    region_edits = re;
    checked = ch;
    for(int i = 0; i < c.size(); i++)
      corrections.push_back(new correction(*c[i]));
  };
 corrected_read(vector<int> & u, float l, int re, bool ch)
    :untrusted(u) {
    likelihood = l;
    region_edits = re;
    checked = ch;
  };
  ~corrected_read() {
    while(corrections.size() > 0) {
      delete corrections.back();
      corrections.pop_back();
    }
  }
  // default destructor should call the correction vector
  // destructor which should call the correction destructor

  vector<correction*> corrections; 
  vector<int> untrusted; // inaccurate until pop'd off queue and processed
  float likelihood;
  int region_edits;
  bool checked;
};

////////////////////////////////////////////////////////////
// Read
////////////////////////////////////////////////////////////
class Read {
 public:
  Read(const string & h, const unsigned int* s, const string & q, vector<int> & u, const int read_length);
  ~Read();
  //bool correct(prefix_tree* trusted, ofstream & out);
  bool correct(bithash* trusted, ofstream & out);
  vector<int> error_region();
  //bool check_trust(corrected_read *cr, prefix_tree *trusted);
  bool check_trust(corrected_read *cr, bithash *trusted);

  string header;
  int read_length;
  unsigned int* seq;
  float* prob;
  vector<int> untrusted;
  corrected_read *trusted_read;

 private:
  bool untrusted_intersect(vector<int> & region);
  void untrusted_union(vector<int> & region);
  void quality_quicksort(vector<int> & indexes, int left, int right);
  string print_seq();
  string print_corrected(corrected_read* cr);

  float likelihood;
  const static float trust_spread_t = .1;
  const static float correct_min_t = .00001;
};

#endif
