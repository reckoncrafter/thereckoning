#include "map.h"

Map GenerateMap(){
    // World
    static Map aa, ab, ac, ba, bb, bc, ca, cb, cc;

    // Linking
    aa.Link(nullptr, &ba, nullptr, &ab);
    ab.Link(nullptr, &bb, &aa, &ac);
    ac.Link(nullptr, &bc, &ab, nullptr);

    ba.Link(&aa, &ca, nullptr, &bb);
    bb.Link(&ab, &cb, &ba, &bc);
    bc.Link(&ac, &cc, &bb, nullptr);

    ca.Link(&ba, nullptr, nullptr, &cb);
    cb.Link(&bb, nullptr, &ca, &cc);
    cc.Link(&bc, nullptr, &cb, nullptr);
}
/*
    A        B        C
A  [aa] <-> [ab] <-> [ac]
    %        %        %
B  [ba] <-> [bb] <-> [bc]
    %        %        %
C  [ca] <-> [cb] <-> [cc]
  
*/