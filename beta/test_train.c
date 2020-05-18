/*  author: samtenka
 *  change: 2020-05-17
 *  create: 2020-05-17
 *  descrp: 
 *  to use: 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "colors.h"
#include "concept_table.h"
#include "enumerator.h"
#include "interpreter.h"
#include "lambda.h"
#include "toy_data.h"

#include "../verbose.h"
#include "../containers/fixpoint.h" 
#include "../containers/mapping.h" 
#include "../containers/count_heap.h" 

const float exp_thouth = 1.001000500;
const float exp_hundth = 1.010050167;
const float exp_tenth  = 1.105170918;
const float exp_one    = 2.718281828;

float plog(float p)
{
    if ( p == 0.0 ) { return -9999.0; }
    float bulk = 0.0;
    while ( p < 1.0 ) { p *= exp_one   ; bulk -= 1.   ; }
    while ( 1.0 < p ) { p /= exp_tenth ; bulk +=  .1  ; }
    while ( p < 1.0 ) { p *= exp_hundth; bulk -=  .01 ; } 
    while ( 1.0 < p ) { p /= exp_thouth; bulk +=  .001; }

    float d = 1.0-p; 
    float remainder = d*(1/1.0 + d*(1/2.0 + d/3.0));
    return bulk - remainder; 
}

typedef struct Primitive Primitive;
struct Primitive {
    char name[16];
    int nb_args;
    EType type;
    float weight;
    bool is_const;
    bool needs_nonconst;
    bool commutes;
    bool needs_unequal;
    bool absorbs_self;
};

const float ABST_PROB = 0.0;

#define NB_PRIMITIVES 23//24
Primitive my_prims[] = {                        /*const  nnonc  comm   uneq   absbs*/
    {"here"    , 0, tCEL               ,      64 , false, 1    , 0    , 0    , 0    }, 
    {"offset"  , 2, tCEL_CEL_DIR       ,   16    , 1    , 1    , 0    , 0    , true }, 
                    
    {"east"    , 0, tDIR               ,      64 , 1    , 1    , 0    , 0    , 0    },
    {"north"   , 0, tDIR               ,      64 , 1    , 1    , 0    , 0    , 0    },
    {"south"   , 0, tDIR               ,      64 , 1    , 1    , 0    , 0    , 0    },
    {"west"    , 0, tDIR               ,      64 , 1    , 1    , 0    , 0    , 0    },
    {"plus"    , 2, tDIR_DIR_DIR       ,      64 , 1    , false, true , 0    , 0    },
    {"diff"    , 2, tDIR_CEL_CEL       , 4       , 1    , 1    , 0    , true , 0    },
    {"negate"  , 1, tDIR_DIR           , 4       , 1    , 1    , 0    , 0    , 0/**/},
                    
    //{"query"   , 0, tHUE               ,      64 , false, 1    , 0    , 0    , 0    },
    {"black"   , 0, tHUE               ,      64 , 1    , 1    , 0    , 0    , 0    },
    {"view"    , 1, tHUE_CEL           ,      64 , 1    , 1    , 0    , 0    , 0    }, 
    {"cobalt"  , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"crimson" , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"forest"  , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"gold"    , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"lead"    , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"magenta" , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"salmon"  , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"sky"     , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"tan"     , 0, tHUE               ,   16    , 1    , 1    , 0    , 0    , 0    },
    {"abyss"   , 0, tHUE               , 4       , 1    , 1    , 0    , 0    , 0    },
                    
    {"has_hue" , 2, tTWOCEL_HUE        ,      64 , 1    , 1    , 0    , 0    , 0    }, 
    {"sees"    , 3, tTWOCEL_DIR_TWOCEL , 4       , 1    , 1    , 0    , 0    , 0    }, 
};
float eval_scores[NB_TYPES];
float leaf_scores[NB_PRIMITIVES];
EType leaf_types[NB_PRIMITIVES];

char leaf_names[NB_PRIMITIVES][16];
int  nb_args[NB_PRIMITIVES];
bool is_const[NB_PRIMITIVES];
bool needs_nonconst[NB_PRIMITIVES];
bool commutes[NB_PRIMITIVES];
bool needs_unequal[NB_PRIMITIVES];
bool absorbs_self[NB_PRIMITIVES];

void initialize_primitive_scores()
{
    float weight_to[NB_TYPES];

    fprintf(stderr, "hi!\n");
    for ( int t = 0; t != NB_TYPES; ++t ) { weight_to[t] = 0.0; }

    fprintf(stderr, "hi!\n");
    for ( int l = 0; l != NB_PRIMITIVES; ++l ) {
        Primitive p = my_prims[l];
        EType t = p.type; 
        while ( t != -1 ) { 
            weight_to[t] += p.weight;
            t = out_type[t];
        }; 
    }

    fprintf(stderr, "hi!\n");
    for ( int l = 0; l != NB_PRIMITIVES; ++l ) {
        Primitive* p = &(my_prims[l]);
        EType t = p->type;
        leaf_types[l] = t;
        leaf_scores[l] = plog( p->weight / weight_to[t] );

        strcpy(leaf_names[l], p->name); 
        nb_args[l] = p->nb_args;
        is_const[l] = p->is_const;
        needs_nonconst[l] = p->needs_nonconst;
        commutes[l] = p->commutes;
        needs_unequal[l] = p->needs_unequal;
        absorbs_self[l] = p->absorbs_self;
    }

    fprintf(stderr, "hi!\n");
    for ( int t = 0; t != NB_TYPES; ++t) {
        eval_scores[t] = plog(
            /*  ( ( t == TTWOCEL ) ? (1-ABST_PROB) : 1.0 ) *   */
            ( is_func[t] ? ( weight_to[t] / weight_to[out_type[t]]) : 0.0 )
        );
    }
}

void handle_interrupt(int sig)
{
    free_lamb_expr_pool();
    lava(); printf("cleaned lambda pool!\n"); defc();
    exit(0);
}

void init_grammar(Grammar* G)
{
    *G = (Grammar){
        .nb_leaves      = NB_PRIMITIVES   ,
        .nb_args        = nb_args         ,
        .leaf_scores    = leaf_scores     ,
        .leaf_types     = leaf_types      ,
        .eval_score     = eval_scores     ,
        .is_const       = is_const        ,
        .needs_nonconst = needs_nonconst  ,
        .commutes       = commutes        ,
        .needs_unequal  = needs_unequal   ,
        .absorbs_self   = absorbs_self    ,
        .abst_score     = plog(ABST_PROB) ,
    };
}

void init_lamb_list(Grammar const* G, LambList* ll, float nats)
{
    clock_t start, end;
    float diff;

    start = clock();
    {
        *ll = enumerate(G, -nats, tTWO); 
    }
    end = clock();
    diff = ((float)(end-start))/CLOCKS_PER_SEC;

    lime(); printf("enumerated "); defc();
    lava(); printf("%d ", ll->len); defc();
    printf("programs in ");
    lava(); printf("%.3f ", 1000*diff); defc();
    printf("ms, or ");
    lava(); printf("%.3f ", 1000*diff/(ll->len)); defc();
    printf("ms per program \n");
}

void init_valuations(CTable* ct, ValGrid const* input, LambList* ll)
{
    clock_t start, end;
    float diff;

    start = clock();
    {
        init_table(ct, CARGO_VALUED);
        for ( int pi = 0; pi != ll->len; ++pi ) {
            ValGrid const* v = evaluate(input, ll->arr[pi].e, ct, nb_args);
        }
    }
    end = clock();
    diff = ((float)(end-start))/CLOCKS_PER_SEC;

    lime(); printf("evaluated "); defc();
    lava(); printf("%d ", ll->len); defc();
    printf("programs in ");
    lava(); printf("%.3f ", 1000*diff); defc();
    printf("ms, or ");
    lava(); printf("%.3f ", 1000*diff/(ll->len)); defc();
    printf("ms per program \n");
}


void main()
{
    initialize_primitive_scores();

    lava(); printf("hello!\n"); defc();

    init_lamb_expr_pool();
    signal(SIGINT, handle_interrupt);
    {
        Grammar G;              init_grammar(&G);
        LambList ll;            init_lamb_list(&G, &ll, 17.0);
        ValGrid const* input;   input = a_pairs[0].x;
        CTable ct;              init_table(&ct, CARGO_VALUED);

        init_valuations(&ct, input, &ll);

        Tasks tasks;            init_tasks(&tasks, 1);
        Task t = {make_charss(5), make_charss(5), 100};
        push_tasks(&tasks, t);
        for ( int si = 0; si != 10; ++si ) {
            char label = /* output @ sample*/ si%2;
            charss* points = label ? &(tasks.data[0].pospoints) :
                                     &(tasks.data[0].negpoints)  ; 
            push_charss(points, make_chars(100));
            for ( int di = 0; di != 100; ++ di ) {
                push_chars(&points->data[points->len-1], /* dim @ sample*/ si & (1<<di)); 
            }
        }

        TaskView tv;            rand_taskview(&tv, &(tasks.data[0]));

        Trees trees;            init_trees(&trees, 1);
        DecTree dt;
        init_tree(&dt);
        train_tree(&tv, &dt);
        print_tree(&dt);
        push_trees(&trees, dt);
        free_trees(&trees);
        

        wipe_table(&ct);
        free(ll.arr);
    }
    handle_interrupt(0);
}



