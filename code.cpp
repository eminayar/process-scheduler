#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <climits>
#include <algorithm>

using namespace std;

template<class Container>
void mySplit( const string &str, Container& cont){
    istringstream iss(str);
    copy(istream_iterator<string>(iss), istream_iterator<string>(),back_inserter(cont));
}

struct Process{
    string name;
    int priority,arrival_time,line_number,id;
    Process( string _name="", int _priority=INT_MAX,
            int _arrival_time= INT_MIN , int _id=INT_MAX){
        this->id=_id;
        this->name=_name;
        this->priority=_priority;
        this->arrival_time=_arrival_time;
        this->line_number=1;
    }
    friend bool operator < ( const Process &p1, const Process &p2 ){
        if( p1.arrival_time != p2.arrival_time )
            return p1.arrival_time < p2.arrival_time;
        return p1.priority < p2.priority;
    }
};

vector <ifstream> readers;
vector <Process> process,ready_queue;
vector <int> exit_time;
vector <int> instruction_time;
long event_timer=0;
int ready_queue_head=0,process_head=1;

bool compare( const Process &p1 , const Process &p2 ){
    if( p1.priority == p2.priority )
        return p1.arrival_time < p2.arrival_time;
    return p1.priority < p2.priority;
}

void ready_queue_insert( const Process &P ){
    ready_queue.push_back( P );
    for( int it=ready_queue.size()-1 ; it != ready_queue_head ; it-- ){
        if( compare( ready_queue[it] , ready_queue[it-1] ) ){
            swap( ready_queue[it] , ready_queue[it-1] );
        }
        else break;
    }
}

void print_queue(){
    printf("%d:HEAD-",event_timer);
    if( ready_queue_head == ready_queue.size() ){
        printf("-TAIL\n");
        return;
    }
    for( int i=ready_queue_head ; i!= ready_queue.size() ; i++ ){
        printf("%s[%d]-",ready_queue[i].name.c_str(),ready_queue[i].line_number);
    }
    printf("TAIL\n");
}

bool comp( const Process& p1, const Process& p2 ){
    return p1.id < p2.id;
}

int main(){
    
    ifstream definition("definition.txt");
    
    freopen("output.txt","w",stdout);
    
    process.push_back(Process());
    string line;
    while( getline(definition,line) ){
        vector <string> info;
        mySplit( line,info );
        readers.push_back(ifstream(info[2]+".txt"));
        exit_time.push_back(0);
        instruction_time.push_back(0);
        process.push_back( Process( info[0], stoi(info[1]),
                                   stoi(info[3]), readers.size()-1 ));
        
    }
    
    sort( process.begin() , process.end() );
    
    bool finished_process=true;
    while( ready_queue_head != (int)ready_queue.size() ||
          process_head != (int)process.size() ){
        bool should_i_print = false;
        while( process_head != process.size() &&
              process[process_head].arrival_time <= event_timer ){
            ready_queue_insert( process[process_head] );
            should_i_print=true;
            process_head++;
        }
        if( finished_process || should_i_print )
            print_queue();
        finished_process=false;
        if( ready_queue_head != ready_queue.size() ){
            Process &current=ready_queue[ready_queue_head];
            getline(readers[current.id],line);
            vector <string> info;
            mySplit( line, info);
            if( info[0] == "exit" ){
                finished_process=true;
                exit_time[current.id]=event_timer+stoi(info[1]);
                ready_queue_head++;
            }
            current.line_number++;
            event_timer+=stoi(info[1]);
            instruction_time[current.id]+=stoi(info[1]);
        }else{
            event_timer=process[process_head].arrival_time;
        }
    }
    print_queue();
    puts("");
    
    sort( process.begin() , process.end() , comp );
    
    for( int i=0 ; i<(int)exit_time.size() ; i++){
        printf("Turnaround time for %s = %d ms\n",
               process[i].name.c_str(),exit_time[i]-process[i].arrival_time);
        printf("Waiting time for %s = %d\n",
               process[i].name.c_str(),exit_time[i]-process[i].arrival_time-instruction_time[i]);
    }
    
    return 0;
}
