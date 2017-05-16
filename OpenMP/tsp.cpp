#include "OpenMP_Skeleton.cpp"

int numThreads = 4 ; 

class NodeProb;

class NodeSol: public Solution{
	public:
		int cost;
		vector<int> taken; // list of vertices traversed
		int pos; // length of the current path
		NodeProb *probPtr ;
		void printSolution();
		int upperBound();
};

class NodeProb: public Problem<NodeSol>{
	public:
    	vector<vector<int> > graph;
		int n; // number of vertices in the graph
    		int minEdge;
		bool goal ;
		int getThreads() ;
		NodeSol nullSolution();
		NodeSol worstSolution();
		vector<NodeSol> expandNode(NodeSol s);
		
};






int NodeSol::upperBound(){
  // In this case upper bound is lower bound
	int left = ((probPtr)->n) - taken.size();
  return cost+(left*((probPtr)->minEdge));
}

int NodeProb::getThreads(){
	return numThreads ;
}


void NodeSol::printSolution(){
  cout<<"Num of Threads:- "<<numThreads<<endl ;
  cout<<"Minimum cost of "<<cost<<" achieved for the path with vertices ";
	for(int i=0;i<taken.size();i++)
		cout<<taken[i]<<", ";
	cout<<endl;
}



NodeSol NodeProb::nullSolution(){
	NodeSol s1;
  // include the starting vertex 
	s1.pos = 0;
	s1.probPtr = this;
	s1.cost = 0;
	return s1;
}

NodeSol NodeProb::worstSolution(){
	NodeSol s1;
	s1.probPtr = this;
	s1.cost = 100000000;
  	s1.pos = 0;
	return s1;
}

vector<NodeSol> NodeProb::expandNode(NodeSol s){
  bool flag[(s.probPtr)->n];
  vector<NodeSol> ret;
  memset(flag,false,sizeof(flag));
  for(int i=0;i<s.taken.size();i++)
  {   
    flag[s.taken[i]] = true;
  }
  if(s.taken.size()==0)
  {
     for(int i=0;i<(s.probPtr)->n;i++)
     {
       NodeSol first = s;
       first.pos = 1;
       first.taken.push_back(i);
       ret.push_back(first);
     }
     return ret;
  }
  for(int i=0;i<(s.probPtr)->n;i++)
  {
    if(!flag[i])
    {
	    NodeSol s1 = s;
      s1.pos++;
      s1.taken.push_back(i);
      int prev;
      prev = s1.taken[s1.taken.size()-2];
      s1.cost += graph[prev][i];
      ret.push_back(s1);
    }
      
  }
  return ret;
}

int main(){
    
	NodeProb problem;
	Skeleton<NodeProb, NodeSol> skeletonBNB;
	cin>>numThreads ;
	cin>>problem.n;
  	problem.minEdge = 9894350;
  	problem.goal = false ; // Minimization
	for(int i=0;i<problem.n;i++){
        	vector<int> temp;
        	for(int j=0;j<problem.n;j++)
        	{
            		int val;
            		cin>>val;
            		problem.minEdge = min(problem.minEdge,val);
            		temp.push_back(val);
        	}
        	problem.graph.push_back(temp);
        }

	skeletonBNB.solve(problem);
	return 0;
}
