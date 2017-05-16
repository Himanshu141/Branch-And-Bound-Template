#include "Skeleton_MPI.cpp"

class NodeProb;
int explDiameter = 3 ;
class NodeSol: public Solution{
	public:
		int wt,cost;
		NodeProb *probPtr;
		vector<int> taken; //Number of elements taken till this node
		int pos; // number of objects explored till this node
		void printSolution(int pid);
		int upperBound();
		int encodeMessage(char *buf); // MPI message will be passed as char stream
};

class NodeProb: public Problem<NodeSol>{
	public:
		vector<int> costs;
		vector<int> weights;
		int n; // number of elements
		int W; // upper limit on total weight of objects selected
		bool goal ;
		int getExplorationDiameter() ;
		NodeSol nullSolution();
		NodeSol worstSolution();
		vector<NodeSol> expand(NodeSol s);
		int encodeMessage(char *buf);
		static NodeSol decodeSolMessage(char *buf, int &pos);
		static NodeProb decodeProbMessage(char *buf, int &pos);
};




int NodeProb::getExplorationDiameter(){
	return explDiameter ;
}

NodeSol NodeProb::decodeSolMessage(char *buf, int &pos){
	int b, ntaken;
	NodeSol ks;
	sscanf(buf+pos,"%d%d%d%d%n",&(ks.wt),&(ks.cost),&(ks.pos),&ntaken,&b);
	pos += b;
	for(int i=0;i<ntaken;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		ks.taken.push_back(temp);
	}
	return ks;
}

int NodeSol::encodeMessage(char *buf){
	int p = 0;
	p += sprintf(buf+p,"%d %d %d %d ",wt,cost,pos,(int)taken.size());
	for(int i=0;i<taken.size();i++)
		p += sprintf(buf+p, "%d ",taken[i]);
	return p;
}

struct item{
	int wt, cost;
	double ratio;
};

bool comp(item a, item b){
	return a.ratio > b.ratio;
}

int NodeSol::upperBound(){
	int bound = cost;
	vector<item> elements; // elements remaining
	for(int i=pos;i< (probPtr) ->n;i++){
		item it;
		it.wt = (probPtr) -> weights[i];
		it.cost = (probPtr) -> costs[i];
		it.ratio = ( (double) it.cost) / ( (double) it.wt);
		elements.push_back(it);
	}
	int w = wt;
	sort(elements.begin(),elements.end(),comp); // decreasing order of ratio of cost to weight;
	for(int j=0;j<elements.size() && w <= (probPtr)->W;j++){
		//taking items till weight is exceeded
		bound += elements[j].cost;
		w += elements[j].wt;
	}
	return bound;
}



void NodeSol::printSolution(int pid){
	//cout<<"Called by process "<<pid ;
	cout<<"Max Value of "<<cost<<" by taking elements ";
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

int NodeProb::encodeMessage(char* buf){
	int p = 0;
	p += sprintf(buf+p,"%d %d ",n,W);
	for(int i=0;i<n;i++)
		p += sprintf(buf+p,"%d ",weights[i]);
	for(int i=0;i<n;i++)
		p += sprintf(buf+p,"%d ",costs[i]);
	return p;
}

NodeProb NodeProb::decodeProbMessage(char *buf, int &pos){
	int b;
	NodeProb kp;
	//kp = true ;
	sscanf(buf+pos,"%d%d%n",&(kp.n),&(kp.W),&b);
	pos += b;
	for(int i=0;i<kp.n;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		kp.weights.push_back(temp);
	}
	for(int i=0;i<kp.n;i++){
		int temp;
		sscanf(buf+pos,"%d%n",&temp,&b);
		pos += b;
		kp.costs.push_back(temp);
	}
	return kp;
}

vector<NodeSol> NodeProb::expand(NodeSol s){
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
	SkeletonBNB<NodeProb, NodeSol> bnbs;
	NodeProb p;
	p.goal = true ;
	cin>>explDiameter ;
	cin>>p.n;
	cin>>p.W;
	for(int i=0;i<p.n;i++){
		int temp;
		cin>>temp;
		p.weights.push_back(temp);
	}
	for(int i=0;i<p.n;i++){
		int temp;
		cin>>temp;
		p.costs.push_back(temp);
	}
	bnbs.solve(p);
	return 0;
}
