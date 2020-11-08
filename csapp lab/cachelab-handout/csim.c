#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <getopt.h>

int b=0,s=0,B=0,S=0;
int E=0, verbosity=0;
long long int lru_counter=1;
char* trace_file=NULL;
int hit_count=0,miss_count=0,eviction_count=0;

typedef long long unsigned mem_addr_t;
struct cache_line_t{
    int valid;
    mem_addr_t tag;
    unsigned int lru;
};
typedef struct cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;
cache_t cache;
mem_addr_t set_index_mask=0;

void printUsage(char** argv){
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n",*argv);
    puts("Options:");
    puts("  -h         Print this help message.");
    puts("  -v         Optional verbose flag.");
    puts("  -s <num>   Number of set index bits.");
    puts("  -E <num>   Number of lines per set.");
    puts("  -b <num>   Number of block offset bits.");
    puts("  -t <file>  Trace file.");
    puts("\nExamples:");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", *argv);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", *argv);
    exit(0);
}

void initCache()
{
    cache=(cache_t)malloc(sizeof(cache_set_t)*S);
    for(int i=0;i<S;i++){
        cache[i]=(cache_set_t)malloc(sizeof(struct cache_line_t)*E);
        for(int j=0;j<E;j++){
            cache[i][j].valid=0;
            cache[i][j].tag=0;
            cache[i][j].lru=0;
        }
    }
    set_index_mask=pow(2,s)-1;
}

void accessData(mem_addr_t addr)
{
    int i;
    int eviction_line=0;
    unsigned int eviction_lru=-1;
    mem_addr_t tag=addr>>(s+b);
    cache_set_t cache_set=cache[(addr>>b)&set_index_mask];
    for(i=0; ; i++)
    {
        if(i>=E)
        {
            ++miss_count;
            if(verbosity) printf("miss ");
            for(int ia=0;ia<E;++ia)
            {
                if(cache_set[ia].lru<eviction_lru)
                {
                    eviction_line=ia;
                    eviction_lru=cache_set[ia].lru;
                }
            }
            if(cache_set[eviction_line].valid)
            {
                ++eviction_count;
                if(verbosity) printf("eviction ");
            }
            cache_set[eviction_line].valid=1;
            cache_set[eviction_line].tag=tag;
            cache_set[eviction_line].lru=++lru_counter;
            return ;
        }
        if(cache_set[i].tag==tag && cache_set[i].valid) break;
    }
    ++hit_count;
    if(verbosity) printf("hit ");
    cache_set[i].lru=lru_counter++;
}

void replayTrace(char* trace_file){

    char buf[1000];
    FILE* trace_fp=fopen(trace_file,"r");
    if(!trace_fp){
        int* err_line=__errno_location();
        char* err_str=strerror(*err_line);
        fprintf(stderr,"%s: %s\n",trace_file,err_str);
        exit(1);
    }
    while(fgets(buf,1000,trace_fp))
    {
        unsigned int len=0;
        mem_addr_t addr=0;
        if(buf[1]=='S'||buf[1]=='L'||buf[1]=='M')
        {
            sscanf(&buf[3],"%llx,%u",&addr,&len);
            if(verbosity) printf("%c %llx,%u ",buf[1],addr,len);
            accessData(addr);
            if(buf[1]=='M') accessData(addr);
            if(verbosity) putchar('\n');
        }
    }
    fclose(trace_fp);
}

void freeCache(){
    for(int i=0;i<S;i++){
        free(cache[i]);
    }
    free(cache);
}

int main(int argc, char** argv)
{
    char c;
    while((c = getopt(argc, argv, "s:E:b:t:vh")) != -1)
    {
        switch(c){
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'h':
                printUsage(argv);
                return 0;
            case 's':
                s=atoi(optarg);
                break;
            case 't':
                trace_file=optarg;
                break;
            case 'v':
                verbosity=1;
                break;
            default:
                printUsage(argv);
                return 0;
        }
    }
    if(!s||!E||!b||!trace_file)
    {
        printf("%s: Missing required command line argument\n",*argv);
        printUsage(argv);
    }
    S=pow(2,s);
    B=pow(2,b);
    initCache();
    replayTrace(trace_file);
    freeCache();
    printSummary(hit_count,miss_count,eviction_count);
    //printSummary(0,0,0);
    return 0;
}