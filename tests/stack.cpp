#include <stack.hpp>
#include <catch.hpp>

SCENARIO("init", "[init]") {
	stack<int> A;
	REQUIRE(A.count() == 0);
}