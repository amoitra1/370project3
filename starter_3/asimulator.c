/*
 * EECS 370, University of Michigan, Fall 2023
 * Project 3: LC-2K Pipeline Simulator
 * Instructions are found in the project spec: https://eecs370.github.io/project_3_spec/
 * Make sure NOT to modify printState or any of the associated functions
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Machine Definitions
#define NUMMEMORY 65536 // maximum number of data words in memory
#define NUMREGS 8 // number of machine registers

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 // will not implemented for Project 3
#define HALT 6
#define NOOP 7

const char* opcode_to_str_map[] = {
    "add",
    "nor",
    "lw",
    "sw",
    "beq",
    "jalr",
    "halt",
    "noop"
};

#define NOOPINSTR (NOOP << 22)

typedef struct IFIDStruct {
    int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
    int instr;
	int pcPlus1;
	int valA;
	int valB;
	int offset;
} IDEXType;

typedef struct EXMEMStruct {
    int instr;
	int branchTarget;
    int eq;
	int aluResult;
	int valB;
} EXMEMType;

typedef struct MEMWBStruct {
    int instr;
	int writeData;
} MEMWBType;

typedef struct WBENDStruct {
    int instr;
	int writeData;
} WBENDType;

typedef struct stateStruct {
    unsigned int numMemory;
    unsigned int cycles; // number of cycles run so far
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
} stateType;

static inline int opcode(int instruction) {
    return instruction>>22;
}

static inline int field0(int instruction) {
    return (instruction>>19) & 0x7;
}

static inline int field1(int instruction) {
    return (instruction>>16) & 0x7;
}

static inline int field2(int instruction) {
    return instruction & 0xFFFF;
}

// convert a 16-bit number into a 32-bit Linux integer
static inline int convertNum(int num) {
    return num - ( (num & (1<<15)) ? 1<<16 : 0 );
}

void printState(stateType*);
void printInstruction(int);
void readMachineCode(stateType*, char*);


int main(int argc, char *argv[]) {

    /* Declare state and newState.
       Note these have static lifetime so that instrMem and
       dataMem are not allocated on the stack. */

    static stateType state, newState;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    readMachineCode(&state, argv[1]);

    /* ------------ Initialize State ------------ */
    // At the start of the program, initialize the pc and all registers to zero. Initialize the instruction field in all pipeline registers to the noop instruction (0x1c00000).
    state.pc = 0;
    state.cycles = 0;
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }
    state.IFID.instr = NOOPINSTR;
    state.IDEX.instr = NOOPINSTR;
    state.EXMEM.instr = NOOPINSTR;
    state.MEMWB.instr = NOOPINSTR;
    state.WBEND.instr = NOOPINSTR;
    /* ------------------- END ------------------ */

    newState = state;

    while (opcode(state.MEMWB.instr) != HALT) {

        printState(&state);
        newState.cycles += 1;

        /* ---------------------- IF stage --------------------- */
        int stall = 0;

        // load-use stall check (unchanged)
        int idnstr = state.IFID.instr;
        int exnstr = state.IDEX.instr;
        if (opcode(exnstr) == LW) {
            int lwDest = field1(exnstr);
            if (lwDest == field0(idnstr) || lwDest == field1(idnstr)) {
                stall = 1;
            }
        }

        if (!stall) {
            // always fetch from instrMem; do NOT substitute NOOP after HALT
            int fetchedInstr = state.instrMem[state.pc]; // past-end words are 0 -> add 0 0 0
            newState.IFID.instr = fetchedInstr;
            newState.IFID.pcPlus1 = state.pc + 1;
            newState.pc = state.pc + 1;
        } else {
            newState.IFID = state.IFID;
            newState.pc = state.pc;
            newState.IDEX.instr = NOOPINSTR;
        }

        /* ---------------------- ID stage --------------------- */
        int instruction = state.IFID.instr;
        int opcod = opcode(instruction);

        if (!stall) {
            newState.IDEX.instr = instruction;
            newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
            newState.IDEX.valA = state.reg[field0(instruction)];
            newState.IDEX.valB = state.reg[field1(instruction)];
            newState.IDEX.offset = convertNum(field2(instruction));
        }

        /* ---------------------- EX stage --------------------- */
        instruction = state.IDEX.instr;
        opcod = opcode(instruction);

        newState.EXMEM.instr = instruction;

        int valA = state.IDEX.valA;
        int valB = state.IDEX.valB;

        // forwarding from EX/MEM (unchanged)
        int prevop = opcode(state.EXMEM.instr);
        int exmemDest;
        if (prevop == LW) {
            exmemDest = field1(state.EXMEM.instr);
        } else {
            exmemDest = field2(state.EXMEM.instr);
        }
        if (prevop == ADD || prevop == NOR) {
            if (field0(instruction) == exmemDest) {
                valA = state.EXMEM.aluResult;
            }
            if (field1(instruction) == exmemDest) {
                valB = state.EXMEM.aluResult;
            }
        }

        // forwarding from MEM/WB (unchanged)
        prevop = opcode(state.MEMWB.instr);
        int memwbdest;
        if (prevop == LW) {
            memwbdest = field1(state.MEMWB.instr);
        } else {
            memwbdest = field2(state.MEMWB.instr);
        }
        if (prevop == ADD || prevop == NOR) {
            if (field0(instruction) == memwbdest) {
                valA = state.MEMWB.writeData;
            }
            if (field1(instruction) == memwbdest) {
                valB = state.MEMWB.writeData;
            }
        } else if (prevop == LW) {
            if (field0(instruction) == memwbdest) {
                valA = state.MEMWB.writeData;
            }
            if (field1(instruction) == memwbdest) {
                valB = state.MEMWB.writeData;
            }
        }

        int resultalu = 0;
        if (opcod == ADD) {
            resultalu = valA + valB;
        } else if (opcod == NOR) {
            resultalu = ~(valA | valB);
        } else if (opcod == LW || opcod == SW) {
            resultalu = valA + state.IDEX.offset;
        } else if (opcod == BEQ) {
            resultalu = valA - valB;
        }

        newState.EXMEM.aluResult = resultalu;
        newState.EXMEM.valB = valB;
        newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
        newState.EXMEM.eq = (valA == valB); // in 1 lune

        /* --------------------- MEM stage --------------------- */
        instruction = state.EXMEM.instr;
        opcod = opcode(instruction);

        newState.MEMWB.instr = instruction;
        int datawrite = 0;
        if (opcod == LW) {
            datawrite = state.dataMem[state.EXMEM.aluResult];
        } else if (opcod == SW) {
            newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.valB;
        } else if (opcod == ADD || opcod == NOR) {
            datawrite = state.EXMEM.aluResult;
        }
        newState.MEMWB.writeData = datawrite;

        if (opcod == BEQ && state.EXMEM.eq) {
            newState.pc = state.EXMEM.branchTarget;
            newState.IFID.instr = NOOPINSTR;
            newState.IDEX.instr = NOOPINSTR;
        }

        /* ---------------------- WB stage --------------------- */
        instruction = state.MEMWB.instr;
        opcod = opcode(instruction);

        newState.WBEND.instr = instruction;
        newState.WBEND.writeData = state.MEMWB.writeData;

        if (opcod == ADD || opcod == NOR) {
            int dest = field2(instruction);
            newState.reg[dest] = state.MEMWB.writeData;
        } else if (opcod == LW) {
            int dest = field1(instruction);
            newState.reg[dest] = state.MEMWB.writeData;
        }

        /* ------------------------ END ------------------------ */
        state = newState;
    }


    printf("Machine halted\n");
    printf("Total of %d cycles executed\n", state.cycles);
    printf("Final state of machine:\n");
    printState(&state);
}

/*
* DO NOT MODIFY ANY OF THE CODE BELOW.
*/

void printInstruction(int instr) {
    const char* instr_opcode_str;
    int instr_opcode = opcode(instr);
    if(ADD <= instr_opcode && instr_opcode <= NOOP) {
        instr_opcode_str = opcode_to_str_map[instr_opcode];
    }

    switch (instr_opcode) {
        case ADD:
        case NOR:
        case LW:
        case SW:
        case BEQ:
            printf("%s %d %d %d", instr_opcode_str, field0(instr), field1(instr), convertNum(field2(instr)));
            break;
        case JALR:
            printf("%s %d %d", instr_opcode_str, field0(instr), field1(instr));
            break;
        case HALT:
        case NOOP:
            printf("%s", instr_opcode_str);
            break;
        default:
            printf(".fill %d", instr);
            return;
    }
}

void printState(stateType *statePtr) {
    printf("\n@@@\n");
    printf("state before cycle %d starts:\n", statePtr->cycles);
    printf("\tpc = %d\n", statePtr->pc);

    printf("\tdata memory:\n");
    for (int i=0; i<statePtr->numMemory; ++i) {
        printf("\t\tdataMem[ %d ] = 0x%08X\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (int i=0; i<NUMREGS; ++i) {
        printf("\t\treg[ %d ] = %d\n", i, statePtr->reg[i]);
    }

    // IF/ID
    printf("\tIF/ID pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->IFID.instr);
    printInstruction(statePtr->IFID.instr);
    printf(" )\n");
    printf("\t\tpcPlus1 = %d", statePtr->IFID.pcPlus1);
    if(opcode(statePtr->IFID.instr) == NOOP){
        printf(" (Don't Care)");
    }
    printf("\n");
    
    // ID/EX
    int idexOp = opcode(statePtr->IDEX.instr);
    printf("\tID/EX pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->IDEX.instr);
    printInstruction(statePtr->IDEX.instr);
    printf(" )\n");
    printf("\t\tpcPlus1 = %d", statePtr->IDEX.pcPlus1);
    if(idexOp == NOOP){
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\tvalA = %d", statePtr->IDEX.valA);
    if (idexOp >= HALT || idexOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\tvalB = %d", statePtr->IDEX.valB);
    if(idexOp == LW || idexOp > BEQ || idexOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\toffset = %d", statePtr->IDEX.offset);
    if (idexOp != LW && idexOp != SW && idexOp != BEQ) {
        printf(" (Don't Care)");
    }
    printf("\n");

    // EX/MEM
    int exmemOp = opcode(statePtr->EXMEM.instr);
    printf("\tEX/MEM pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->EXMEM.instr);
    printInstruction(statePtr->EXMEM.instr);
    printf(" )\n");
    printf("\t\tbranchTarget %d", statePtr->EXMEM.branchTarget);
    if (exmemOp != BEQ) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\teq ? %s", (statePtr->EXMEM.eq ? "True" : "False"));
    if (exmemOp != BEQ) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\taluResult = %d", statePtr->EXMEM.aluResult);
    if (exmemOp > SW || exmemOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");
    printf("\t\tvalB = %d", statePtr->EXMEM.valB);
    if (exmemOp != SW) {
        printf(" (Don't Care)");
    }
    printf("\n");

    // MEM/WB
	int memwbOp = opcode(statePtr->MEMWB.instr);
    printf("\tMEM/WB pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->MEMWB.instr);
    printInstruction(statePtr->MEMWB.instr);
    printf(" )\n");
    printf("\t\twriteData = %d", statePtr->MEMWB.writeData);
    if (memwbOp >= SW || memwbOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");     

    // WB/END
	int wbendOp = opcode(statePtr->WBEND.instr);
    printf("\tWB/END pipeline register:\n");
    printf("\t\tinstruction = 0x%08X ( ", statePtr->WBEND.instr);
    printInstruction(statePtr->WBEND.instr);
    printf(" )\n");
    printf("\t\twriteData = %d", statePtr->WBEND.writeData);
    if (wbendOp >= SW || wbendOp < 0) {
        printf(" (Don't Care)");
    }
    printf("\n");

    printf("end state\n");
    fflush(stdout);
}

// File
#define MAXLINELENGTH 1000 // MAXLINELENGTH is the max number of characters we read

void readMachineCode(stateType *state, char* filename) {
    char line[MAXLINELENGTH];
    FILE *filePtr = fopen(filename, "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", filename);
        exit(1);
    }

    printf("instruction memory:\n");
    for (state->numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; ++state->numMemory) {
        if (sscanf(line, "%x", state->instrMem+state->numMemory) != 1) {
            printf("error in reading address %d\n", state->numMemory);
            exit(1);
        }
        printf("\tinstrMem[ %d ] = 0x%08X ( ", state->numMemory, 
            state->instrMem[state->numMemory]);
        printInstruction(state->dataMem[state->numMemory] = state->instrMem[state->numMemory]);
        printf(" )\n");
    }
}
