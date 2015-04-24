#ifndef NEIGHBOR_H
#define NEIGHBOR_H

template<int Connectivity>
class TNeighbor
{
private:
    int Offset[Connectivity];
    int Index;
    
public:
    TNeighbor(const int xOffset, const int yOffset) {
        switch(Connectivity) {
        case 4:
            Offset[0] = xOffset;
            Offset[1] = yOffset;
            Offset[2] = - xOffset;
            Offset[3] = - yOffset;
            break;
        case 8:
            Offset[0] = xOffset;
            Offset[1] = xOffset + yOffset;
            Offset[2] = yOffset;
            Offset[3] = - xOffset + yOffset;
            Offset[4] = - xOffset;
            Offset[5] = - xOffset - yOffset;
            Offset[6] = - yOffset;
            Offset[7] = xOffset - yOffset;
            break;
        default:
            throw "Neighbor::Neighbor => invalid value of the connectivity";
        }

    }
    void Begin() {
        Index = 0;
    }
    bool End() {
        return Index >= Connectivity;
    }
    void Next() {
        Index++;
    }
    int Where(const int Center) {
        return Center + Offset[Index];
    }
    int Where(const int Center, const int JumpIndex) {
        return Center + Offset[JumpIndex];
    }
    int Backtrack(const int Where) {
        return Where - Offset[Index];
    }
    int Backtrack(const int Where, const int JumpIndex) {
        return  Where - Offset[JumpIndex];
    }
    int Direction() {
        return Index;
    }
};

#endif
