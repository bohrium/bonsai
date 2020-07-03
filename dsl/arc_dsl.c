/*  author: samtenka
 *  change: 2020-07-03
 *  create: 2020-06-30
 *  descrp: 
 *  to use: 
 */

#include <stdbool.h>
#include "../dsl/grammar.h"
#include "../lambda/type.h"

#define NB_PRIMITIVES 26
Primitive my_prims[] = {
    /* name   arity   type          weight  const  nnonc  comm   uneq   absbs*/
    {"here"   ,0,tCEL              ,    16, false, 1    , 0    , 0    , 0    }, 
    {"offset" ,2,tCEL_CEL_DIR      ,  4   , 1    , 1    , 0    , 0    , true }, 
                 
    {"east"   ,0,tDIR              ,    16, 1    , 1    , 0    , 0    , 0    },
    {"north"  ,0,tDIR              ,    16, 1    , 1    , 0    , 0    , 0    },
    {"south"  ,0,tDIR              ,    16, 1    , 1    , 0    , 0    , 0    },
    {"west"   ,0,tDIR              ,    16, 1    , 1    , 0    , 0    , 0    },
    {"plus"   ,2,tDIR_DIR_DIR      ,    16, 1    , false, true , 0    , 0    },
    {"diff"   ,2,tDIR_CEL_CEL      ,1     , 1    , 1    , 0    , true , 0    },
    {"negate" ,1,tDIR_DIR          ,1     , 1    , 1    , 0    , 0    , 0/**/},
                 
    {"view"   ,1,tHUE_CEL          ,    16, 1    , 1    , 0    , 0    , 0    }, 
    {"abyss"  ,0,tHUE              ,    16, 1    , 1    , 0    , 0    , 0    },
    {"black"  ,0,tHUE              ,    16, 1    , 1    , 0    , 0    , 0    },
    {"cobalt" ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"crimson",0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"forest" ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"gold"   ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"lead"   ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"magenta",0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"salmon" ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"sky"    ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
    {"tan"    ,0,tHUE              ,1     , 1    , 1    , 0    , 0    , 0    },
                 
    {"has_hue",2,tTWOCEL_HUE       ,    16, 1    , 1    , 0    , 0    , 0    }, 
    {"sees"   ,3,tTWOCEL_DIR_TWOCEL,1     , 1    , 1    , 0    , 0    , 0    }, 

    {"not"    ,1,tTWOTWO           ,1     , 1    , 1    , 0    , 0    , 0    }, 
    {"or"     ,2,tTWOTWO_TWO       ,1     , 1    , 1    , 1    , 1    , 0    }, 
    {"and"    ,2,tTWOTWO_TWO       ,1     , 1    , 1    , 1    , 1    , 0    }, 
};

char my_leaf_names[][16] = {
"here"          , 
"offset"        , 
                  
"east"          , 
"north"         , 
"south"         , 
"west"          , 
"plus"          , 
"diff"          ,
"negate"        , 
                  
"view"          , 
"abyss"         , 
"black"         , 
"cobalt"        , 
"crimson"       , 
"forest"        , 
"gold"          , 
"lead"          , 
"magenta"       , 
"salmon"        , 
"sky"           , 
"tan"           , 
                  
"has_hue"       , 
"sees"          , 
                  
"not"           , 
"or"            , 
"and"           , 
};