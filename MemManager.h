#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define CCHAROFFSET 65
#define PTBR_TO_INDEX (PTBR/virPage)
#define PTBR_TO_PROCESS ((PTBR_TO_INDEX) + 65)
typedef enum
{
    LRU,
    RANDOM
} TLBPolicy;

typedef enum
{
    FIFO,
    CLOCK
} PagePolicy;

typedef enum
{
    LOCAL,
    GLOBAL
} AllocPolicy;

typedef struct
{
    bool valid;
    uint16_t vpn;
    uint16_t pfn;
} TLBEntry;
TLBEntry tlbEntry[32];

typedef struct
{
    union
    {
        uint8_t byte;
        struct
        {
            uint8_t reference : 1;
            uint8_t present : 1;
        } bits;
    } bitField;
    int16_t pfn_dbi;
} PageTableEntry;

typedef struct ReplaceListType
{
    uint8_t proc;
    uint16_t vpn;
    struct ReplaceListType *prev;
    struct ReplaceListType *next;
} ReplaceListType;
ReplaceListType *curReplaceNode;

typedef struct
{
    struct ReplaceListType *head;
} ReplaceListHeadType;
ReplaceListHeadType *curLocalReplaceNode;

typedef struct
{
    bool frameValid;
} PhyMem;
PhyMem *phyMem;
PhyMem *swapSpace;

typedef struct
{
    float tlbHitCnt;
    float refTlbCnt;
    float pageFaultCnt;
    float refPageCnt;
} StatsType;
StatsType *stats;
PageTableEntry *pageTable;
AllocPolicy allocPolicy;
TLBPolicy tlbPolicy;
PagePolicy pagePolicy;
int phyPage, virPage, numProc;
uint32_t tlbCounter=0;
uint32_t tlbLRU[32];
FILE *ftraceOutput;
void flushTLB();
int16_t TLBLookup(uint16_t vpn);
int kickTLBEntry();
void fillTLB(uint16_t vpn, int pfn);
bool getSysInfo(TLBPolicy *tlbPolicy, PagePolicy *pagePolicy, AllocPolicy *allocPolicy,
                int *numProc, int *virPage, int *phyPage);
void switchProcess(int *PTBR, int *perProcPTBR, char process);
int16_t pageTableLookup(int PTBR, uint16_t vpn);
void fillPTE(int PTBR, uint16_t vpn, int pfn);
int kickPage(int PTBR, uint16_t refPage);
int freeFrameManager();
int16_t pageFaultHandler(int PTBR, uint16_t refPage);
int16_t addrTrans(int PTBR, uint16_t refPage);
