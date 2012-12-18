
#define TIB_SZ 8192

#define STACK_SIZE (sizeof(Cell) * 100)
#define NULL_DOES ((APF) 0)

#define TRUE   ((Cell)-1)
#define FALSE  ((Cell)0)

#define tib      ((char*)user_area[3])
#define to_in    (user_area[4])
#define debug    (user_area[5])

/* Use 2constant for 64 bit numbers */
typedef long Cell;
typedef unsigned long uCell;
typedef long long DoubleCell;
typedef unsigned long long uDoubleCell;
typedef void (*PFV)();

/**
 * The codefield is a Pointer to a Function returning a Void (PFV).
 * ACF: Address of the Code Field
 * APF: Address of the Parameter Field
 *      The parameter field contains an ACF
 * ALF: Address of the Link Field
 *      The link field contains an ACF
 * ANF: Address of the Name Field
 */
typedef PFV *codefield;
typedef codefield *ACF;
typedef ACF       *APF;
typedef ACF       *ALF;
typedef char      *ANF;

Cell *Here();

void push( Cell );
Cell pop();

APF Body( char * );

void Primitive( char *, PFV );
void Variable( char * );
void nVariable( char *, int );
Cell *AddressOf( char * );
void To( Cell, char * );

void Colon( char * );
void Constant( char *, Cell );
void End();
void ColdChain( char * );

void Begin();
void While();
void Repeat();
void Do();
void Loop();
void If();
void Then();
void Literal();
void StringLiteral();
void Abort();

void c();

void Dprintf( char * );

/* Globals */

extern Cell tos, *sp, *rp;
extern APF ip;
