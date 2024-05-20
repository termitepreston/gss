#include <catch2/catch_test_macros.hpp>

#include <list.hpp>
#include <utility>

TEST_CASE("list<T> should be zero in len when newly constructed.", "[list]") {
    list<int> l{};

    REQUIRE(l.size() == 0);
}

TEST_CASE("test list<T>::empty() simple", "[list]") {
    list<int> l{};

    REQUIRE(l.empty());

    l.push_back(200);
    l.push_back(300);
    l.push_back(400);

    l.pop_back();
    l.pop_back();

    REQUIRE_FALSE(l.empty());

    l.pop_back();
    REQUIRE(l.empty());
}

TEST_CASE("test list<T>::empty() complex", "[list]") {
    list<int> l{};

    l.push_back(200);
    l.push_back(300);

    l.pop_back();
    l.pop_back();

    REQUIRE(l.empty());
}

TEST_CASE("test list<T>::size() non-zero size", "[list]") {
    list<int> l{};
    l.push_back(40);
    l.push_back(90);

    REQUIRE(l.size() == 2);

    l.pop_back();

    REQUIRE(l.size() == 1);
}

TEST_CASE("test list<T>::push_back()", "[list]") {
    list<int> l{};
    l.push_back(40);
    l.push_back(90);

    REQUIRE(l.print() == "40->90");

    auto v = l.back();
    REQUIRE(v == 90);
    l.pop_back();
    REQUIRE(l.print() == "40");
}

TEST_CASE("test list<T>::pop_front()", "[list]") {
    list<int> l{};
    l.push_back(40);
    l.push_back(90);
    l.push_back(200);

    REQUIRE(l.print() == "40->90->200");

    auto v = l.front();
    REQUIRE(v == 40);
    l.pop_front();
    REQUIRE(l.print() == "90->200");

    REQUIRE(l.front() == 90);
    l.pop_front();
    REQUIRE(l.print() == "200");
}

TEST_CASE("test list<T> illegal operation detected", "[list]") {
    list<int> l{};

    REQUIRE_THROWS(l.pop_back());

    REQUIRE_THROWS(l.pop_front());
}

TEST_CASE("test list::iterator", "[list]") {
    list<int> l{};

    l.push_back(90);
    l.push_back(60);
    l.push_back(30);

    std::vector<int> vec{};
    vec.reserve(l.size());

    for (auto elem : l)
        vec.emplace_back(elem);

    REQUIRE(l.back() == vec[2]);
    l.pop_back();
    REQUIRE(l.back() == vec[1]);
    l.pop_back();
    REQUIRE(l.back() == vec[0]);
}

TEST_CASE("test list<T>::erase_all single element", "[list]") {
    list<int> l{};

    l.push_back(100);

    REQUIRE(l.size() == 1);

    l.erase_all();

    REQUIRE(l.empty());
}

TEST_CASE("test list<T>::erase_all()", "[list]") {
    list<int> l{};

    l.push_back(100);
    l.push_back(200);
    l.push_back(300);

    REQUIRE(l.size() == 3);

    l.erase_all();

    REQUIRE(l.empty());
}

TEST_CASE("test list<T> copy constructor.", "[list]") {
    list<int> l{};

    l.push_back(100);
    l.push_back(200);
    l.push_back(300);

    list<int> m{l};

    REQUIRE(l.size() == m.size());

    l.erase_all();

    REQUIRE(l.empty());
}

TEST_CASE("test list<T> move assignment op.", "[list]") {
    list<int> l{};

    l.push_back(100);
    l.push_back(200);
    l.push_back(300);

    list<int> m{};

    m = std::move(l);

    REQUIRE(l.empty());

    REQUIRE(m.size() == 3);
}
