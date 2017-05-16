#include <cstdio>
#include <queue>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <omp.h>


//Some Implementation Details
//Their are three two subclasses problem and solution. 
//Branch and Bound is implemented using Depth First Search.


using namespace std;

class Solution{
	public:
		virtual void printSolution() =0;
		virtual int upperBound() =0;
};

template <typename Prob> class Problem{
	public:
		virtual Prob nullSolution() =0;
		virtual Prob worstSolution() =0;
		virtual vector<Prob> expandNode(Prob s) =0;
};

template <typename Prob, typename Sol> class Skeleton{
	public:
		void solve(Prob p);
};

struct compareBound {
    bool operator()(Solution  & s1, Solution  & s2) {
        return s1.upperBound() > s2.upperBound();
    }
};


template<typename Prob,typename Sol>
void Skeleton<Prob,Sol>::solve(Prob p){
   int numThreads =  p.getThreads() ;
   cout<<"thread is : "<<numThreads<<endl;
   omp_set_num_threads(numThreads);
   //priority_queue<Sol, vector<Sol>, compareBound> q ;
   queue<Sol> q;
   q.push(p.nullSolution());
   Sol best = p.worstSolution();
  while(true)
  {
    int nt;
    if(q.size()>=numThreads) nt = numThreads;
    else nt = q.size();
    Sol data[nt];
    for(int i=0;i<nt;i++)
    {
       data[i] = q.front();
       //data[i] = q.top();
       q.pop();
    }
    if(nt==0) break;
 
    for(int i=0;i<nt;i++)
    {
      	Sol s;
      	s = data[i];
      	
      	if(s.pos == (s.probPtr)->n){
  		if(p.goal){ 
  			if(s.cost > best.cost){ 
               			best = s;
  			}
  		}else{ 
          		if(s.cost < best.cost){
              			best = s;
  			}
  		}
  	}else{
		if(p.goal){ 
  			if(s.upperBound() > best.cost ) { 
              			vector<Sol> ret = p.expandNode(s);
              			
              			{
                		for(int l=0;l<ret.size();l++)
                  			q.push(ret[l]);
              			}
  			}
        	}else{
            		if(s.upperBound() < best.cost) {
            			vector<Sol> ret = p.expandNode(s);
            			
            			{
                		for(int l=0;l<ret.size();l++)
                			q.push(ret[l]);  
                		}
  			}
  		}
      }
  }
  }
	best.printSolution();
}


