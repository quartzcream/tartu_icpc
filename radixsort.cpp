#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
//!escape \section{Radixsort 50M 64 bit integers as single array in 1 sec}
//!begin_codebook
//!start
typedef unsigned char uchar;

template<typename T>
void msd_radixsort(T *start, T *sec_start, int arr_size, int d=sizeof(T)-1){
  const int msd_radix_lim = 100;
  const T mask = 255;
	int bucket_sizes[256]{};
	for(T *it = start; it!=start+arr_size; ++it){
		++bucket_sizes[((*it)>>(d*8))&mask];
		//++bucket_sizes[*((uchar*)it+d)];
	}
	T *locs_mem[257];
	locs_mem[0] = sec_start;
  T **locs = locs_mem+1;
	locs[0] = sec_start;
	for(int j=0; j<255; ++j){
		locs[j+1] = locs[j]+bucket_sizes[j];
	}
	for(T *it = start; it!=start+arr_size; ++it){
    uchar bucket_id = ((*it)>>(d*8))&mask;
    *(locs[bucket_id]++) = *it;
	}
  locs = locs_mem;
	if(d){
    T *locs_old[256];
    locs_old[0] = start;
    for(int j=0; j<255; ++j){
      locs_old[j+1] = locs_old[j]+bucket_sizes[j];
    }
		for(int j=0; j<256; ++j){
			if(locs[j+1]-locs[j] < msd_radix_lim){
				std::sort(locs[j], locs[j+1]);
        if(d & 1){
          copy(locs[j], locs[j+1], locs_old[j]);
        }
			}
			else{
				msd_radixsort(locs[j], locs_old[j], bucket_sizes[j], d-1);
			}
		}
	}
}
//!finish

const int nmax = 5e7;

ll arr[nmax], tmp[nmax];

int main(){
  for(int i=0; i<nmax; ++i)
    arr[i] = ((ll)rand()<<32)|rand();
  msd_radixsort(arr, tmp, nmax);
  assert(is_sorted(arr, arr+nmax));
}
//!end_codebook
