#include "OpenMP_Skeleton.cpp"

int numThreads = 4 ; 

class NodeProb;

class NodeSol: public Solution{
	public:
		int wt,cost;
		vector<int> taken; // object nos. chosen
		NodeProb *probPtr ;
		int pos; // number of objects "considered"
		void printSolution();
		int upperBound();
};

class NodeProb: public Problem<NodeSol>{
	public:
		vector<int> costs;
		vector<int> weights;
		int n; // num of objects
		int W; // upper limit on total weight of objects selected
		bool goal  ;
		int getThreads() ;
		NodeSol nullSolution();
		NodeSol worstSolution();
		vector<NodeSol> expandNode(NodeSol s);
		
};



int NodeProb::getThreads(){
	return numThreads ;
}

struct element{
	int wt, cost;
	double ratio;
};

bool comp(element a, element b){
	return a.ratio > b.ratio;
}

int NodeSol::upperBound(){
	int bound = cost;
	vector<element> remElements; // remaining elements
	for(int i=pos;i< (probPtr) ->n;i++){
		element it;
		it.wt = (probPtr) -> weights[i];
		it.cost = (probPtr) -> costs[i];
		it.ratio = ( (double) it.cost) / ( (double) it.wt);
		remElements.push_back(it);
	}
	int w = wt;
	sort(remElements.begin(),remElements.end(),comp); // decreasing order of ratio of cost to weight;
	for(int j=0;j<remElements.size() && w <= (probPtr)->W;j++){
		//taking elements till weight is exceeded
		bound += remElements[j].cost;
		w += remElements[j].wt;
	}
	return bound;
}


void NodeSol::printSolution(){
	cout<<"Num of Threads:- "<<numThreads<<endl ;
	cout<<"Maximum val of "<<cost<<" achieved by taking objects ";
	for(int i=0;i<taken.size();i++)
		cout<<taken[i]<<", ";
	cout<<endl;
}

NodeSol NodeProb::nullSolution(){
	NodeSol s1;
	s1.pos = 0;
	s1.probPtr = this;
	s1.wt = s1.cost = 0;
	return s1;
}

NodeSol NodeProb::worstSolution(){
	NodeSol s1;
	s1.probPtr = this;
	s1.cost = -1000000;
	s1.wt = 0;
	return s1;
}



vector<NodeSol> NodeProb::expandNode(NodeSol s){
	//either take object at pos, or don't
	NodeSol s1 = s;
	s1.pos++;
	vector<NodeSol> ret;
	ret.push_back(s1); // without taking object at index pos
	if(weights[s1.pos-1] + s.wt <=  W){
		s1.taken.push_back(s1.pos-1);
		s1.wt +=  weights[s1.pos-1];
		s1.cost += costs[s1.pos-1];
		ret.push_back(s1); // taking object at index pos
	}
	return ret;
}

int main(){
	Skeleton<NodeProb, NodeSol> skeletonBNB;
	NodeProb problem;
	problem.goal = true ;//Maximization
	cin>>numThreads;
	cin>>problem.n;
	cin>>problem.W;
	for(int i=0;i<problem.n;i++){
		int temp;
		cin>>temp;
		problem.weights.push_back(temp);
	}
	for(int i=0;i<problem.n;i++){
		int temp;
		cin>>temp;
		problem.costs.push_back(temp);
	}
	skeletonBNB.solve(problem);
	return 0;
}
