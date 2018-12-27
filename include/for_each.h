#pragma once

#include <iterator>
#include <type_traits>

template <typename Container, typename Func>
void for_each_pair(Container& c, Func&& fn) {
    using element_reference = typename Container::reference;
    static_assert(std::is_invocable_v<Func, int, int, element_reference, element_reference>
                  || std::is_invocable_v<Func, element_reference, element_reference>,
                  "Wrong parameter types in function passed as argument");
    int i = 0;
    for(auto it1 = std::begin(c); it1 != std::end(c); ++i, ++it1) {
        int j = i + 1;
        for (auto it2 = std::next(it1); it2 != std::end(c); ++j, ++it2) {
            if constexpr (std::is_invocable_v<Func, int, int, element_reference, element_reference>) {
                if constexpr (std::is_same_v<bool, std::invoke_result_t<Func, int, int, element_reference, element_reference>>) {
                    if (fn(i, j, *it1, *it2)) return;
                } else {
                    fn(i, j, *it1, *it2);
                }
            } else if constexpr (std::is_invocable_v<Func, element_reference, element_reference>) {
                if constexpr (std::is_same_v<bool, std::invoke_result_t<Func, element_reference, element_reference>>) {
                    if (fn(*it1, *it2)) return;
                } else {
                    fn(*it1, *it2);
                }
            }
        }
    }
}
