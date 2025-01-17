// Programer: Robert Christensen
// email: robertc@cs.utah.edu
// header file for the jefast index
#pragma once

// NOTE: THE INITIAL IMPLEMENTATION IS VERY RIGID, ONLY TO VERIFY THE IDEA
//    IS VALID.  THE INTERFACE WILL LIKELY CHANGE VERY SOON
#include <map>
#include <memory>
#include <tuple>
#include <sstream>
#include <random>

#include "Table.h"
#include "DatabaseSharedTypes.h"
#include "jefastLevel.h"

static constexpr const jfkey_t virtual_key = 0;

class jefastIndexBase {
public:
    virtual ~jefastIndexBase()
    {};

    // get the total number of join possibilities.
    virtual weight_t GetTotal() = 0;
    virtual uint64_t GetTransformedTotal() = 0;

    virtual void GetJoinNumber(weight_t joinNumber, std::vector<int64_t> &out)= 0;
    virtual std::vector<weight_t> GetJoinNumberWithWeights(weight_t joinNumber, std::vector<int64_t> &out)= 0;

    virtual void GetRandomJoin(std::vector<int64_t> &out) = 0;
    virtual std::vector<weight_t> GetRandomJoinWithWeights(std::vector<int64_t> &out) = 0;

    virtual std::pair<std::vector<std::vector<int64_t>>, std::vector<std::vector<uint64_t>>> GenerateData(size_t count) = 0;
    virtual std::pair<std::vector<int64_t>, std::vector<uint64_t>> GenerateSampleData() = 0;
    virtual std::pair<int64_t, uint64_t> GenerateFirstEntry(uint64_t tupleIndex) = 0;

    // return the number of levels in this jefastIndex
    // (how large a vector will be if a join value is reported)
    virtual int GetNumberOfLevels() = 0;
};

class jefastIndexLinear : public jefastIndexBase {
public:
    virtual ~jefastIndexLinear()
    {};

    // get the total number of join possibilities.
    weight_t GetTotal();
    uint64_t GetTransformedTotal() {
        // TODO
        return 0;
    }

    void GetJoinNumber(weight_t joinNumber, std::vector<int64_t> &out);
    std::vector<weight_t> GetJoinNumberWithWeights(weight_t joinNumber, std::vector<int64_t> &out);
    
    void GetRandomJoin(std::vector<int64_t> &out);
    std::vector<weight_t> GetRandomJoinWithWeights(std::vector<int64_t> &out);

    std::pair<std::vector<std::vector<int64_t>>, std::vector<std::vector<uint64_t>>> GenerateData(size_t count);
    std::pair<std::vector<int64_t>, std::vector<uint64_t>> GenerateSampleData();
    std::pair<int64_t, uint64_t> GenerateFirstEntry(uint64_t tupleIndex);

    // return the number of levels in this jefastIndex
    // (how large a vector will be if a join value is reported)
    virtual int GetNumberOfLevels();

    // insert a new item into the index
    void Insert(int table_id, jefastKey_t record_id);

    void Delete(int table_id, jefastKey_t record_id);

    std::vector<weight_t> MaxOutdegree();

    int64_t MaxIndegree();

    void print_search_weights()
    {
        m_levels[0]->dump_weights();
    }

    void rebuild_initial();
    void set_postponeRebuild(bool value = true)
    {
        postpone_rebuild = value;
    }
private:
    jefastIndexLinear()
        : postpone_rebuild{ false }
    {};

    std::vector<std::shared_ptr<JefastLevel<jfkey_t> > > m_levels;
    weight_t start_weight;

    // random number stuff for reporting random results of the join
    std::default_random_engine m_generator;
    std::uniform_int_distribution<weight_t> m_distribution;

    bool postpone_rebuild;

    friend class JefastBuilder;
    friend class jefastBuilderWJoinAttribSelection;
    friend class jefastBuilderWNonJoinAttribSelection;
};

class jefastIndexFork: public jefastIndexBase {
public:
    ~jefastIndexFork() {}

    weight_t GetTotal() {
        return m_start_weight;
    }

    uint64_t GetTransformedTotal() {
        std::ostringstream stream; stream << m_start_weight;
        return static_cast<uint64_t>(std::stoull(stream.str()));
    }

    void GetJoinNumber(weight_t joinNumber, std::vector<int64_t> &out);
    std::vector<weight_t> GetJoinNumberWithWeights(weight_t joinNumber, std::vector<int64_t> &out);

    void GetRandomJoin(std::vector<int64_t> &out);
    std::vector<weight_t> GetRandomJoinWithWeights(std::vector<int64_t> &out);

    std::pair<std::vector<std::vector<int64_t>>, std::vector<std::vector<uint64_t>>> GenerateData(size_t count);
    std::pair<std::vector<int64_t>, std::vector<uint64_t>> GenerateSampleData();
    std::pair<int64_t, uint64_t> GenerateFirstEntry(uint64_t tupleIndex);

    int GetNumberOfLevels() {
        return (int) m_levels.size() + 1;
    }

private:
    std::vector<std::shared_ptr<JefastLevel<jfkey_t> > > m_levels;
    std::vector<int> m_parent_tables;
    std::vector<bool> m_is_last_child;
    weight_t m_start_weight;

    std::default_random_engine m_generator;
    std::uniform_int_distribution<weight_t> m_distribution;

    friend class JefastBuilder;
};

