#include <fit/match.hpp>
#include <fit/static.hpp>
#include <fit/lambda.hpp>
#include "test.hpp"

#include <memory>

namespace test1
{
    struct int_class
    {
        int operator()(int) const
        {
            return 1;
        }
    };

    struct foo
    {};

    struct foo_class
    {
        foo operator()(foo) const
        {
            return foo();
        }
    };

    static constexpr fit::static_<fit::match_adaptor<int_class, foo_class> > fun = {};

    static_assert(std::is_same<int, decltype(fun(1))>::value, "Failed match");
    static_assert(std::is_same<foo, decltype(fun(foo()))>::value, "Failed match");
}

struct int_class
{
    constexpr int operator()(int) const
    {
        return 1;
    }
};

struct foo
{};

struct foo_class
{
    constexpr int operator()(foo) const
    {
        return 2;
    }
};

static constexpr fit::match_adaptor<int_class, foo_class> fun = {};

FIT_TEST_CASE()
{
    
    FIT_TEST_CHECK(fun(1) == 1);
    FIT_TEST_CHECK(fun(foo()) == 2);

    FIT_STATIC_TEST_CHECK(fun(1) == 1);
    FIT_STATIC_TEST_CHECK(fun(foo()) == 2);
};

FIT_TEST_CASE()
{
    
    FIT_TEST_CHECK(fit::reveal(fun)(1) == 1);
    FIT_TEST_CHECK(fit::reveal(fun)(foo()) == 2);

    FIT_STATIC_TEST_CHECK(fit::reveal(fun)(1) == 1);
    FIT_STATIC_TEST_CHECK(fit::reveal(fun)(foo()) == 2);
};
#if FIT_HAS_STATIC_LAMBDA
FIT_TEST_CASE()
{
    
    constexpr auto lam = fit::match(
        FIT_STATIC_LAMBDA(int) { return 1; },
        FIT_STATIC_LAMBDA(foo) { return 2; }
    );
    
    FIT_TEST_CHECK(lam(1) == 1);
    FIT_TEST_CHECK(lam(foo()) == 2);
};
#endif

FIT_TEST_CASE()
{
    int i = 0;
    auto lam = fit::match(
        [&](int) { return i+1; },
        [&](foo) { return i+2; }
    );
// Disable this check on msvc, since lambdas might be default constructible
#ifndef _MSC_VER
    STATIC_ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(decltype(lam));
#endif
    FIT_TEST_CHECK(lam(1) == 1);
    FIT_TEST_CHECK(lam(foo()) == 2);
};


FIT_TEST_CASE()
{
    struct not_default_constructible
    {
        int i;
        not_default_constructible(int x) : i(x)
        {}
    };
    STATIC_ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(not_default_constructible);
    not_default_constructible ndc = not_default_constructible(0);
    auto lam = fit::match(
        [&](int) { return ndc.i+1; },
        [&](foo) { return ndc.i+2; }
    );
// Disable this check on msvc, since lambdas might be default constructible
#ifndef _MSC_VER
    STATIC_ASSERT_NOT_DEFAULT_CONSTRUCTIBLE(decltype(lam));
#endif
    
    FIT_TEST_CHECK(lam(1) == 1);
    FIT_TEST_CHECK(lam(foo()) == 2);
};


struct int_move_class
{
    std::unique_ptr<int> i;
    int_move_class() : i(new int(1))
    {}
    int operator()(int) const
    {
        return *i;
    }
};

struct foo_move_class
{
    std::unique_ptr<int> i;
    foo_move_class() : i(new int(2))
    {}
    int operator()(foo) const
    {
        return *i;
    }
};

FIT_TEST_CASE()
{
    auto fun_move = fit::match(int_move_class(), foo_move_class());
    FIT_TEST_CHECK(fun_move(1) == 1);
    FIT_TEST_CHECK(fun_move(foo()) == 2);

};


