#pragma once

#include <set>

// #include "storage/label/label.hpp"
#include "utils/reference_wrapper.hpp"

class Label;
using label_ref_t = ReferenceWrapper<const Label>;

class LabelCollection
{
public:
    auto begin();
    auto begin() const;
    auto cbegin() const;

    auto end();
    auto end() const;
    auto cend() const;

    bool add(const Label &label);
    bool has(const Label &label) const;
    size_t count() const;
    bool remove(const Label &label);
    void clear();
    const std::set<label_ref_t> &operator()() const;

private:
    std::set<label_ref_t> _labels;
};
