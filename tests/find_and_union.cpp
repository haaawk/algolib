#define BOOST_TEST_MODULE find_and_union tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <find_and_union.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <ostream>
#include <vector>

namespace data = boost::unit_test::data;

template <typename Func>
void for_each_element(int size, Func&& fn) {
    for (int element = 0; element < size; ++element) {
        fn(element);
    }
}

BOOST_DATA_TEST_CASE( find_and_union_creation_with_nonpositive_sizes_throws,
                      data::xrange<int>(-100, 1),
                      size) {
    BOOST_REQUIRE_THROW(find_and_union{size}, std::invalid_argument);
}

BOOST_DATA_TEST_CASE( find_and_union_creation_with_positive_sizes,
                      data::xrange<int>(1, 101),
                      size) {
    find_and_union tested{size};
    BOOST_TEST( tested.count() == size );
    for_each_element(size, [&] (int element) {
        BOOST_TEST( tested.find(element) == element );
        BOOST_TEST( tested.size(element) == 1 );
    });
}

BOOST_DATA_TEST_CASE( find_and_union_find_corner_cases,
                      data::xrange<int>(1, 101) * data::xrange(1, 3),
                      size,
                      error) {
    find_and_union tested{size};
    BOOST_REQUIRE_THROW( tested.find(-error), std::out_of_range );
    BOOST_REQUIRE_THROW( tested.find(size + error - 1), std::out_of_range );
}

BOOST_DATA_TEST_CASE( find_and_union_size_corner_cases,
                      data::xrange<int>(1, 101) * data::xrange(1, 3),
                      size,
                      error) {
    find_and_union tested{size};
    BOOST_REQUIRE_THROW( tested.size(-error), std::out_of_range );
    BOOST_REQUIRE_THROW( tested.size(size + error - 1), std::out_of_range );
}

BOOST_DATA_TEST_CASE( find_and_union_merge_two_sets,
                      data::xrange<int>(1, 101) * data::xrange(1, 101),
                      size1,
                      size2) {
    int total = size1 + size2;
    find_and_union tested{total};
    for (int i = 1; i < size1; ++i) {
        BOOST_TEST( tested.merge(0, i) == (i + 1) );
    }
    for (int i = 1; i < size2; ++i) {
        BOOST_TEST( tested.merge(total - 1, total - i - 1) == (i + 1) );
    }
    for (int i = 0; i < size1; ++i) {
        BOOST_TEST( tested.size(i) == size1 );
        BOOST_TEST( tested.find(i) == tested.find(0) );
    }
    for (int i = size1; i < total; ++i) {
        BOOST_TEST( tested.size(i) == size2 );
        BOOST_TEST( tested.find(i) == tested.find(total - 1) );
    }
    BOOST_TEST( tested.merge(0, total - 1) == total );
    for_each_element(total, [&] (int element) {
        BOOST_TEST( tested.size(element) == total );
        BOOST_TEST( tested.find(element) == tested.find(0) );
    });
}

BOOST_DATA_TEST_CASE( find_and_union_merge_same_has_no_effect,
                      data::xrange<int>(1, 101),
                      size) {
    find_and_union tested{size};
    for_each_element(size, [&] (int element) {
        BOOST_TEST( tested.merge(element, element) == 1 );
        BOOST_TEST( tested.count() == size );
    });
}

BOOST_DATA_TEST_CASE( find_and_union_merge_with_too_low_throws,
                      data::xrange<int>(1, 101) * data::xrange(1, 3),
                      size,
                      error) {
    find_and_union tested{size};
    for_each_element(size, [&] (int element) {
        BOOST_REQUIRE_THROW( tested.merge(element, -error), std::out_of_range );
        BOOST_REQUIRE_THROW( tested.merge(-error, element), std::out_of_range );
    });
}

BOOST_DATA_TEST_CASE( find_and_union_merge_with_too_high_throws,
                      data::xrange<int>(1, 101) * data::xrange(1, 3),
                      size,
                      error) {
    find_and_union tested{size};
    for_each_element(size, [&] (int element) {
        BOOST_REQUIRE_THROW( tested.merge(element, size + error - 1), std::out_of_range );
        BOOST_REQUIRE_THROW( tested.merge(size + error - 1, element), std::out_of_range );
    });
}

BOOST_AUTO_TEST_CASE( find_and_union_merge_two_sets_works ) {
    find_and_union tested{2};
    BOOST_TEST( tested.count() == 2 );
    BOOST_TEST( tested.merge(0, 1) == 2 );
    BOOST_TEST( tested.count() == 1 );
    BOOST_TEST( tested.find(0) == tested.find(1) );
    BOOST_TEST( tested.size(0) == 2 );
    BOOST_TEST( tested.size(1) == 2 );
}

BOOST_AUTO_TEST_CASE( find_and_union_merge_same_set_does_nothing ) {
    find_and_union tested{3};
    BOOST_TEST( tested.merge(0, 1) == 2 );
    BOOST_TEST( tested.size(0) == 2 );
    BOOST_TEST( tested.size(1) == 2 );
    BOOST_TEST( tested.size(2) == 1 );
    BOOST_TEST( tested.merge(0, 2) == 3 );
    BOOST_TEST( tested.size(0) == 3 );
    BOOST_TEST( tested.size(1) == 3 );
    BOOST_TEST( tested.size(2) == 3 );
    BOOST_TEST( tested.find(1) == tested.find(2) );
    BOOST_TEST( tested.count() == 1 );
    BOOST_TEST( tested.merge(1, 2) == 3 );
    BOOST_TEST( tested.find(1) == tested.find(2) );
    BOOST_TEST( tested.count() == 1 );
}
BOOST_DATA_TEST_CASE( find_and_union_randomized,
                      data::xrange<int>(2, 1001),
                      size) {
    find_and_union tested{size};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, size - 1);
    while (tested.count() > 1) {
        int a = dis(gen);
        int b = dis(gen);
        int pa = tested.find(a);
        BOOST_TEST( tested.find(pa) == pa );
        int pb = tested.find(b);
        BOOST_TEST( tested.find(pb) == pb );
        int sizea = tested.size(a);
        BOOST_TEST( tested.size(pa) == sizea );
        int sizeb = tested.size(b);
        BOOST_TEST( tested.size(pb) == sizeb );
        int count = tested.count();
        if (pa == pb) {
            BOOST_TEST( sizea == sizeb );
            BOOST_TEST( tested.merge(a, b) == sizea );
            BOOST_TEST( tested.count() == count );
            BOOST_TEST( tested.size(a) == sizea );
            BOOST_TEST( tested.size(b) == sizeb );
            BOOST_TEST( tested.find(a) == pa );
            BOOST_TEST( tested.find(a) == pb );
            BOOST_TEST( tested.size(pa) == sizea );
            BOOST_TEST( tested.find(pa) == pa );
        } else {
            BOOST_TEST( tested.merge(a, b) == sizea + sizeb );
            BOOST_TEST( tested.count() == count - 1 );
            BOOST_TEST( tested.size(a) == sizea + sizeb );
            BOOST_TEST( tested.size(b) == sizea + sizeb );
            BOOST_TEST( tested.find(a) == tested.find(b) );
            int p = tested.find(a);
            BOOST_TEST( tested.size(p) == sizea + sizeb );
            BOOST_TEST( tested.find(p) == p );
        }
    }
}
