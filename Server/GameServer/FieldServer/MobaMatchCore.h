#ifndef ATUM_MOBA_MATCH_CORE_H
#define ATUM_MOBA_MATCH_CORE_H

// Standalone MOBA rules core. No packets, persistence, or arena modifications.
// Designed for the legacy FieldServer C++ toolchain.
class CMobaMatchCore
{
public:
    enum State { WAITING = 0, READY = 1, RUNNING = 2, FINISHED = 3 };
    enum Team { BLUE = 0, RED = 1, TEAM_COUNT = 2 };
    enum Result { NO_WINNER = -1, BLUE_WINS = BLUE, RED_WINS = RED };
    enum { PLAYERS_PER_TEAM = 5, MAX_PLAYERS = 10 };

    CMobaMatchCore() { Reset(); }

    void Reset()
    {
        m_state = WAITING;
        m_winner = NO_WINNER;
        m_elapsedMs = 0;
        m_baseMaxHp = 0;
        for (int t = 0; t < TEAM_COUNT; ++t)
        {
            m_count[t] = 0;
            m_baseHp[t] = 0;
            for (int p = 0; p < PLAYERS_PER_TEAM; ++p)
                m_players[t][p] = 0;
        }
    }

    bool AddPlayer(Team team, unsigned int characterId)
    {
        if (m_state != WAITING || !ValidTeam(team) || characterId == 0 ||
            m_count[team] >= PLAYERS_PER_TEAM || Contains(characterId))
            return false;
        m_players[team][m_count[team]++] = characterId;
        return true;
    }

    bool RemovePlayer(unsigned int characterId)
    {
        if (m_state != WAITING) return false;
        for (int t = 0; t < TEAM_COUNT; ++t)
            for (int i = 0; i < m_count[t]; ++i)
                if (m_players[t][i] == characterId)
                {
                    for (int j = i; j + 1 < m_count[t]; ++j)
                        m_players[t][j] = m_players[t][j + 1];
                    m_players[t][--m_count[t]] = 0;
                    return true;
                }
        return false;
    }

    bool Prepare(unsigned int baseHp)
    {
        if (m_state != WAITING || baseHp == 0 ||
            m_count[BLUE] != PLAYERS_PER_TEAM ||
            m_count[RED] != PLAYERS_PER_TEAM)
            return false;
        m_baseMaxHp = baseHp;
        m_baseHp[BLUE] = baseHp;
        m_baseHp[RED] = baseHp;
        m_state = READY;
        return true;
    }

    bool Start()
    {
        if (m_state != READY) return false;
        m_elapsedMs = 0;
        m_state = RUNNING;
        return true;
    }

    void Tick(unsigned int deltaMs)
    {
        if (m_state != RUNNING) return;
        const unsigned int maxValue = ~0u;
        m_elapsedMs = deltaMs > maxValue - m_elapsedMs
            ? maxValue : m_elapsedMs + deltaMs;
    }

    // Caller must authenticate the attacker/team and validate in-world range,
    // ownership, protection, cooldown, and damage before invoking this method.
    bool ApplyBaseDamage(Team target, unsigned int damage)
    {
        if (m_state != RUNNING || !ValidTeam(target) || damage == 0 ||
            m_baseHp[target] == 0) return false;
        m_baseHp[target] = damage >= m_baseHp[target]
            ? 0 : m_baseHp[target] - damage;
        if (m_baseHp[target] == 0)
        {
            m_winner = target == BLUE ? RED_WINS : BLUE_WINS;
            m_state = FINISHED;
        }
        return true;
    }

    bool Contains(unsigned int characterId) const
    {
        if (characterId == 0) return false;
        for (int t = 0; t < TEAM_COUNT; ++t)
            for (int i = 0; i < m_count[t]; ++i)
                if (m_players[t][i] == characterId) return true;
        return false;
    }

    State GetState() const { return m_state; }
    Result GetWinner() const { return m_winner; }
    unsigned int GetBaseHp(Team team) const
    { return ValidTeam(team) ? m_baseHp[team] : 0; }
    unsigned int GetBaseMaxHp() const { return m_baseMaxHp; }
    int GetTeamSize(Team team) const
    { return ValidTeam(team) ? m_count[team] : 0; }
    unsigned int GetElapsedMs() const { return m_elapsedMs; }

private:
    static bool ValidTeam(Team team)
    { return team == BLUE || team == RED; }

    State m_state;
    Result m_winner;
    unsigned int m_elapsedMs;
    unsigned int m_baseHp[TEAM_COUNT];
    unsigned int m_baseMaxHp;
    unsigned int m_players[TEAM_COUNT][PLAYERS_PER_TEAM];
    int m_count[TEAM_COUNT];
};
#endif
