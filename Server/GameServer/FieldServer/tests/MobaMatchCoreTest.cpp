#include "../MobaMatchCore.h"
#include <assert.h>
#include <stdio.h>

int main()
{
    CMobaMatchCore match;
    assert(!match.Start());
    assert(!match.Prepare(1000));
    for (unsigned int i = 1; i <= 5; ++i)
    {
        assert(match.AddPlayer(CMobaMatchCore::BLUE, i));
        assert(match.AddPlayer(CMobaMatchCore::RED, i + 5));
    }
    assert(!match.AddPlayer(CMobaMatchCore::BLUE, 11));
    assert(!match.AddPlayer(CMobaMatchCore::RED, 1));
    assert(match.Prepare(1000));
    assert(match.GetState() == CMobaMatchCore::READY);
    assert(!match.AddPlayer(CMobaMatchCore::BLUE, 11));
    assert(match.Start());
    match.Tick(100);
    assert(match.GetElapsedMs() == 100);
    assert(!match.ApplyBaseDamage(CMobaMatchCore::BLUE, 0));
    assert(match.ApplyBaseDamage(CMobaMatchCore::RED, 999));
    assert(match.GetBaseHp(CMobaMatchCore::RED) == 1);
    assert(match.GetWinner() == CMobaMatchCore::NO_WINNER);
    assert(match.ApplyBaseDamage(CMobaMatchCore::RED, 99));
    assert(match.GetState() == CMobaMatchCore::FINISHED);
    assert(match.GetWinner() == CMobaMatchCore::BLUE_WINS);
    assert(!match.ApplyBaseDamage(CMobaMatchCore::BLUE, 1));
    match.Reset();
    assert(match.GetState() == CMobaMatchCore::WAITING);
    assert(match.GetTeamSize(CMobaMatchCore::BLUE) == 0);
    printf("MobaMatchCore tests passed\n");
    return 0;
}
