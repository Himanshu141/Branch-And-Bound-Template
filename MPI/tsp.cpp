#include "Skeleton_MPI.cpp"
class NodeProb;
int explDiameter = 3 ;
class NodeSol: public Solution{
	public:
		int cost;
		vector<int> taken; // list of vertices traversed
		int pos; // length of the current path
		NodeProb *probPtr;
		void printSolution(int pid);
		int upperBound(); //here upper bound means lower bound as this is a minimization problem
		int encodeMessage(char *buf);
};

class NodeProb: public Problem<NodeSol>{
	public:
    	vector<vector<int> > graph;
		int n; // num of vertices in the graph
    		int minEdge;
		bool goal ;
		NodeSol nullSolution();
		NodeSol worstSolution();
		int getExplorationDiameter() ;
		vector<NodeSol> expand(NodeSol s);
		int encodeMessage(char *buf);
		static NodeProb decodeProbMessage(char *buf, int &pos);
		static NodeSol decodeSolMessage(char *buf, int &pos);
};


int NodeProb::getExplorationDiameter(){
	return explDiameter ;
}

int NodeSol::encodeMessage(char *buf){
	int p = 0;
	p += sprintf(buf+p," %d %d ",pos,cost);
	for(int i=0;i<pos;i++)
		p += sprintf(buf+p," %d ",taken[i]);
	return p;
}

NodeSol NodeProb::decodeSolMessage(char *buf, int &pos){
	NodeSol s;
	int b;
	sscanf(buf+pos,"%d%d%n",&(s.pos),&(s.cost),&b);
	pos += b;
	for(int i=0;i<s.pos;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		s.taken.push_back(temp);
	}
	return s;
}

int NodeProb::encodeMessage(char *buf){
	int p = 0;
	p += sprintf(buf+p," %d %d ",n,minEdge);
	for(int i=0;i<n;i++)
		for(int j=0;j<n;j++)
			p += sprintf(buf+p," %d ",graph[i][j]);
	return p;
}

NodeProb NodeProb::decodeProbMessage(char *buf, int &pos){
	NodeProb prob;
	int b;
	prob.goal = false ;
	sscanf(buf+pos,"%d%d%n",&(prob.n),&(prob.minEdge),&b);
	pos += b;
	for(int i=0;i<prob.n;i++){
		vector<int> v;
		for(int j=0;j<prob.n;j++){
			int temp;
			sscanf(buf+pos,"%d%n",&temp,&b);
			pos += b;
			v.push_back(temp);
		}
		prob.graph.push_back(v);
	}
	return prob;
}



int NodeSol::upperBound(){
  int left = ((probPtr)->n) - taken.size();
  return cost+(left*(((NodeProb *)probPtr)->minEdge));
}




void NodeSol::printSolution(int pid){
  //cout<<"Called by process "<<pid ;
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

vector<NodeSol> NodeProb::expand(NodeSol s){
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
      //cout<<"Position of the new q element"<<s1.pos<<endl;
      s1.cost += graph[prev][i];
      ret.push_back(s1);
      //return;
    }
      
  }
  return ret;
}

int main(){
	NodeProb p;
	SkeletonBNB<NodeProb, NodeSol> bnbs;
	cin>>explDiameter ;
	cin>>p.n;
	p.goal = false ;
  	p.minEdge = 1000000;
	for(int i=0;i<p.n;i++){
    vector<int> temp;
    for(int j=0;j<p.n;j++)
    {
      int val;  
      cin>>val;
      p.minEdge = min(p.minEdge,val);
      temp.push_back(val);
    }
    p.graph.push_back(temp);
	}

	bnbs.solve(p);
	return 0;
}
