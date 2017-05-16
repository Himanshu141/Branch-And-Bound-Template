#include <mpi.h>

#include <cstdio>
#include <queue>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <algorithm>
using namespace std;
int explorationDiameter = 3;
class Solution{
	public:
		virtual void printSolution(int pid) =0;
		virtual int upperBound() =0;
};

template <typename Prob> class Problem{
	public:
		virtual Prob nullSolution() =0;
		virtual Prob worstSolution() =0;
		virtual vector<Prob> expand(Prob s) =0;
};

template <typename Prob, typename Sol> class SkeletonBNB{
	public:
		void solve(Prob p);
};

char buf[3000];

struct compareBound {
    bool operator()(Solution  & s1, Solution  & s2) {
        return s1.upperBound() > s2.upperBound();
    }
};


template <typename Prob, typename Sol>
void SkeletonBNB<Prob,Sol>::solve(Prob p){
	explorationDiameter = p.getExplorationDiameter() ;
	priority_queue<Sol, vector<Sol>, compareBound> q ;
	int processId, count;

	MPI_Init(0,NULL);
	
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	
	MPI_Status stat;
	
	Prob problem;
	
	int topCost;
	if(processId == 0){
		//Master
		Sol start = p.nullSolution(), best = p.worstSolution();
		bool isFree[count+1];
		int countFree;
		for(int i=1;i<count;i++){
			isFree[i] = true; // mark all as idle
		}
		countFree = count - 1;
		//encode initial problem and empty sol into buf
		int position  =0;
		position += sprintf(buf+position,"%s ","PROB ");
		position += p.encodeMessage(buf+position);
		position += start.encodeMessage(buf+position);
		position += sprintf(buf+position, "%d \0",best.cost);
		//send it to idle processor no. 1
		MPI_Send(buf,strlen(buf),MPI_CHAR,1,0,MPI_COMM_WORLD);
		isFree[1] = false;
		countFree--;
		while(countFree < count - 1){
			
			MPI_Recv(buf,3000,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&stat);
			
			char msg[10];
			int position = 0, b;
			sscanf(buf+position,"%s%n",msg,&b);
			position += b;
			if(!strcmp(msg,"GET_SLAVES")){
				int sl_needed;
				int r = stat.MPI_SOURCE;
				sscanf(buf+position,"%d%n",&sl_needed,&b);
				position += b;
				int sl_given = min(sl_needed, countFree);
				position = 0;
				position += sprintf(buf+position,"%d ",sl_given);
				position += sprintf(buf+position,"%d ",best.cost);
				int i = 1;
				while(sl_given){
					if(!isFree[i])
						i++;
					else{
						position += sprintf(buf+position,"%d ",i);
						isFree[i] = false;
						countFree--;
						sl_given--;
					}
				}
				position += sprintf(buf+position,"\0");
				MPI_Send(buf,strlen(buf)+1,MPI_CHAR,r,0,MPI_COMM_WORLD);
			}
			else if(!strcmp(msg,"IDLE")){
				//slave processor has become idle
				countFree++;
				isFree[stat.MPI_SOURCE] = true;
			}
			else if(!strcmp(msg,"DONE")){
				//slave has sent a feasible solution
				Sol cand_sol = Prob::decodeSolMessage(buf,position);
				if( ( p.goal && cand_sol.cost > best.cost) ||
						(!p.goal && cand_sol.cost < best.cost) ){
					best = cand_sol;
					topCost = cand_sol.cost;
				}
			}

		}
		for(int i=1;i<count;i++){
			strcpy(buf,"FINISH \0");
			MPI_Send(buf,strlen(buf)+1,MPI_CHAR,i,0,MPI_COMM_WORLD);
		}
		best.printSolution(processId);
	}
	else{
		//Slave
		while(1){
			MPI_Recv(buf,2000,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&stat);
			int position = 0; //position of pointer in buf
			char msg[10]; // message telling the slave what to do
			int b;
			sscanf(buf+position,"%s%n",msg,&b);
			position += b;
			char req[2000];
			if(!strcmp(msg,"PROB")){
				//slave has been given a partially solved problem to expand
				problem = Prob::decodeProbMessage(buf,position);
				Sol s = Prob::decodeSolMessage(buf,position);
				s.probPtr = &problem;
				q.push(s);
				fflush(stdout);
				sscanf(buf+position,"%d%n",&topCost,&b);
				position += b;
				int expDiameter = explorationDiameter ; // change expDiameter here
				while(!q.empty()){
					//take out one element from queue, expand it
					Sol sol = q.top();
					q.pop();
					if(sol.pos == ((sol.probPtr)->n)){
						fflush(stdout);
						position = 0;
						position += sprintf(buf+position, "DONE ");
						position += sol.encodeMessage(buf+position);
						position += sprintf(buf+position," \0");
                        
						fflush(stdout);
                        
						//tell master that feasible solution is reached
						MPI_Send(buf,strlen(buf)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
						if(expDiameter == 0)break;
						else expDiameter-- ;
						
						continue ;
					} 
					vector<Sol> v = problem.expand(sol);

					for(int i=0;i<v.size();i++)
						q.push(v[i]);
					if(q.empty())
						break;
					//request master for slaves
					sprintf(req,"GET_SLAVES %d ",(int)q.size());
					MPI_Send(req,strlen(req),MPI_CHAR,0,0,MPI_COMM_WORLD);
					//get master's response
					//master sends count_avbl_slaves <space> topCost <space> list of slaves
					req[0] = '\0';
					MPI_Recv(req,200,MPI_CHAR,0,0,MPI_COMM_WORLD,&stat);
					int slaves_avbl, p1 = 0;
					sscanf(req+p1,"%d%n",&slaves_avbl,&b);
					p1 += b;
					sscanf(req+p1,"%d%n",&topCost,&b);
					p1 += b;
					for(int i=0;i<slaves_avbl;i++){
						//give a problem to each slave
						int sl_no;
						sscanf(req+p1,"%d%n",&sl_no,&b);
						p1 += b;
						position = 0;
						position += sprintf(buf+position,"%s ","PROB ");
						position += problem.encodeMessage(buf+position);
						Sol sln = q.top();
						q.pop(); 
						position += sln.encodeMessage(buf+position);
						position += sprintf(buf+position, "%d \0",topCost);
						//send it to idle processor
						MPI_Send(buf,strlen(buf),MPI_CHAR,sl_no,0,MPI_COMM_WORLD);
					}
				}
				//This slave has now become idle (its queue is empty). Master is to be informed about it .
				sprintf(req,"IDLE \0");
				MPI_Send(req,strlen(req)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
			}
			else if(!strcmp(msg,"FINISH")){
				assert(stat.MPI_SOURCE==0); // only master can tell it to finish
				break; //from the while(1) loop
			}
		}
	}
	MPI_Finalize();
}
