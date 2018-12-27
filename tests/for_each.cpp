#define BOOST_TEST_MODULE for_each tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <for_each.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <ostream>
#include <vector>
#include <set>
#include <list>
#include <forward_list>
#include <utility>

namespace data = boost::unit_test::data;

template <typename Container>
void test_for_each_pair_less_than_two_elements(Container& c) {
    for_each_pair(c, [&] (auto& a, auto& b) {
        BOOST_FAIL( "No pair should have been found." );
    });

    for_each_pair(c, [&] (int ia, int ib, auto& a, auto& b) {
        BOOST_FAIL( "No pair should have been found." );
    });
}

template <typename Container>
void test_for_each_pair(int size, Container& c) {
    {
        using value_type = typename Container::value_type;
        std::set<std::pair<value_type, value_type>> seen;
        for_each_pair(c, [&] (auto& a, auto& b) {
            BOOST_TEST( a != b );
            BOOST_TEST( seen.insert({a, b}).second );
            BOOST_TEST( seen.insert({b, a}).second );
        });
        BOOST_TEST ( seen.size() == size * (size - 1) );
    }

    {
        std::set<std::pair<int, int>> seen;
        for_each_pair(c, [&] (int ia, int ib, auto& a, auto& b) {
            BOOST_TEST( ia != ib );
            BOOST_TEST( seen.insert({ia, ib}).second );
            BOOST_TEST( seen.insert({ib, ia}).second );
            BOOST_TEST( *std::next(std::begin(c), ia) == a );
            BOOST_TEST( *std::next(std::begin(c), ib) == b );
        });
        BOOST_TEST ( seen.size() == size * (size - 1) );
    }

    {
        int count = 0;
        for_each_pair(c, [&] (auto& a, auto& b) {
            ++count;
            return true;
        });
        BOOST_TEST( count == 1 );
    }

    {
        int count = 0;
        for_each_pair(c, [&] (int ia, int ib, auto& a, auto& b) {
            ++count;
            return true;
        });
        BOOST_TEST( count == 1 );
    }
}

std::vector<int> create_vector(int size) {
    std::vector<int> container(size);
    for (int i = 0; i < size; ++i) {
        container[i] = i;
    }
    return container;
}

BOOST_DATA_TEST_CASE( for_each_pair_vector_less_than_two_elements,
                      data::xrange<int>(0, 2),
                      size) {
    auto container = create_vector(size);
    test_for_each_pair_less_than_two_elements(container);
}

BOOST_DATA_TEST_CASE( for_each_pair_vector,
                      data::xrange<int>(2, 101),
                      size) {
    auto container = create_vector(size);
    test_for_each_pair(size, container);
}

std::set<int> create_set(int size) {
    std::set<int> container;
    for (int i = 0; i < size; ++i) {
        container.insert(i);
    }
    return container;
}

BOOST_DATA_TEST_CASE( for_each_pair_set_less_than_two_elements,
                      data::xrange<int>(0, 2),
                      size) {
    auto container = create_set(size);
    test_for_each_pair_less_than_two_elements(container);
}

BOOST_DATA_TEST_CASE( for_each_pair_set,
                      data::xrange<int>(2, 101),
                      size) {
    auto container = create_set(size);
    test_for_each_pair(size, container);
}

std::list<int> create_list(int size) {
    std::list<int> container;
    for (int i = 0; i < size; ++i) {
        container.push_back(i);
    }
    return container;
}

BOOST_DATA_TEST_CASE( for_each_pair_list_less_than_two_elements,
                      data::xrange<int>(0, 2),
                      size) {
    auto container = create_list(size);
    test_for_each_pair_less_than_two_elements(container);
}

BOOST_DATA_TEST_CASE( for_each_pair_list,
                      data::xrange<int>(2, 101),
                      size) {
    auto container = create_list(size);
    test_for_each_pair(size, container);
}

std::forward_list<int> create_forward_list(int size) {
    std::forward_list<int> container;
    for (int i = size - 1; i > -1; --i) {
        container.push_front(i);
    }
    return container;
}

BOOST_DATA_TEST_CASE( for_each_pair_forward_list_less_than_two_elements,
                      data::xrange<int>(0, 2),
                      size) {
    auto container = create_forward_list(size);
    test_for_each_pair_less_than_two_elements(container);
}

BOOST_DATA_TEST_CASE( for_each_pair_forward_list,
                      data::xrange<int>(2, 101),
                      size) {
    auto container = create_forward_list(size);
    test_for_each_pair(size, container);
}
