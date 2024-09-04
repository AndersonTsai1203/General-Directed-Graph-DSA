#include "gdwg_graph.h"

#include <catch2/catch.hpp>

TEST_CASE("basic test") {
	auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(g.is_node(n));
}

TEST_CASE("Graph Constructors - Default") {
	// checking member variables are empty
	auto g = gdwg::graph<int, int>{};
	auto nodes = g.nodes();
	CHECK(nodes == std::vector<int>{});
}

TEST_CASE("Graph Constructors - empty graph") {
	auto g = gdwg::graph<int, int>{};
	CHECK(g.empty());
}

TEST_CASE("Graph Constructors - nodes exist") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(g.is_node(3));
}

TEST_CASE("Graph Constructors - nodes not exist") {
	auto g = gdwg::graph<int, int>{4, 5, 6};
	CHECK(g.is_node(4));
	CHECK(g.is_node(5));
	CHECK(g.is_node(6));
	REQUIRE_FALSE(g.is_node(1));
	REQUIRE_FALSE(g.is_node(2));
	REQUIRE_FALSE(g.is_node(3));
}

TEST_CASE("Graph Constructors - copy and move") {
	auto g1 = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g1.insert_edge(1, 2, 10));

	// Copy constructor
	auto g2 = g1;
	CHECK(g2.is_node(1));
	CHECK(g2.is_node(2));
	CHECK(g2.is_node(3));
	CHECK(g2.is_connected(1, 2));

	// Move constructor
	auto g3 = std::move(g1);
	CHECK(g3.is_node(1));
	CHECK(g3.is_node(2));
	CHECK(g3.is_node(3));
	CHECK(g3.is_connected(1, 2));
	CHECK(g1.empty());
}

TEST_CASE("Weighted Edge - Constructor") {}

TEST_CASE("Weighted Edge - Print Edge") {
	auto e = gdwg::weighted_edge<int, int>(1, 2, 10);
	CHECK(e.print_edge() == "1 -> 2 | W | 10");
}

TEST_CASE("Weighted Edge - Is Weighted") {
	auto e = gdwg::weighted_edge<int, int>(1, 2, 10);
	CHECK(e.is_weighted() == true);
}

TEST_CASE("Weighted Edge - Get Weight") {
	auto e = gdwg::weighted_edge<int, int>(1, 2, 10);
	CHECK(e.get_weight().has_value() == true);
	CHECK(e.get_weight().value() == 10);
}

TEST_CASE("Weighted Edge - Get Nodes") {
	auto e = gdwg::weighted_edge<int, int>(1, 2, 10);
	auto nodes = e.get_nodes();
	CHECK(nodes.first == 1);
	CHECK(nodes.second == 2);
}

TEST_CASE("Weighted Edge - Equality Operator") {
	auto e1 = gdwg::weighted_edge<int, int>(1, 2, 10);
	auto e2 = gdwg::weighted_edge<int, int>(1, 2, 10);
	auto e3 = gdwg::weighted_edge<int, int>(1, 3, 10);
	auto e4 = gdwg::weighted_edge<int, int>(1, 2, 20);
	CHECK(e1 == e2);
	REQUIRE_FALSE(e1 == e3);
	REQUIRE_FALSE(e1 == e4);
}

TEST_CASE("Unweighted Edge - Constructor") {}

TEST_CASE("Unweighted Edge - Print Edge") {
	auto e = gdwg::unweighted_edge<int, int>(1, 2);
	CHECK(e.print_edge() == "1 -> 2 | U");
}

TEST_CASE("Unweighted Edge - Is Weighted") {
	auto e = gdwg::unweighted_edge<int, int>(1, 2);
	CHECK(e.is_weighted() == false);
}

TEST_CASE("Unweighted Edge - Get Weight") {
	auto e = gdwg::unweighted_edge<int, int>(1, 2);
	CHECK(e.get_weight().has_value() == false);
}

TEST_CASE("Unweighted Edge - Get Nodes") {
	auto e = gdwg::unweighted_edge<int, int>(1, 2);
	auto nodes = e.get_nodes();
	CHECK(nodes.first == 1);
	CHECK(nodes.second == 2);
}

TEST_CASE("Unweighted Edge - Equality Operator") {
	auto e1 = gdwg::unweighted_edge<int, int>(1, 2);
	auto e2 = gdwg::unweighted_edge<int, int>(1, 2);
	auto e3 = gdwg::unweighted_edge<int, int>(1, 3);
	CHECK(e1 == e2);
	REQUIRE_FALSE(e1 == e3);
}

TEST_CASE("Insert Node and Weighted Edge") {
	auto g = gdwg::graph<int, int>{};
	CHECK(g.empty());
	CHECK(g.insert_node(1));
	CHECK(g.insert_node(2));
	CHECK(g.insert_node(3));
	REQUIRE_FALSE(g.insert_node(1)); // node cannot be duplicated

	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	CHECK(g.insert_edge(3, 3, 30));
	REQUIRE_FALSE(g.insert_edge(1, 2, 10));
}

TEST_CASE("Insert Weighted Edge - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	REQUIRE_THROWS_WITH(g.insert_edge(1, 4, 30),
	                    "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does not exist");
	REQUIRE_THROWS_WITH(g.insert_edge(5, 2, 40),
	                    "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does not exist");
}

TEST_CASE("Insert Node and Unweighted Edge") {
	auto g = gdwg::graph<int, int>{};
	CHECK(g.empty());
	CHECK(g.insert_node(1));
	CHECK(g.insert_node(2));
	CHECK(g.insert_node(3));
	REQUIRE_FALSE(g.insert_node(1)); // node cannot be duplicated

	CHECK(g.insert_edge(1, 2));
	CHECK(g.insert_edge(1, 3));
	REQUIRE_FALSE(g.insert_edge(1, 2));
}

TEST_CASE("Insert Unweighted Edge - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.insert_edge(1, 3));
	REQUIRE_THROWS_WITH(g.insert_edge(1, 4),
	                    "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does not exist");
	REQUIRE_THROWS_WITH(g.insert_edge(5, 2),
	                    "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does not exist");
}

TEST_CASE("Replace Node") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));

	CHECK(g.replace_node(1, 4));
	REQUIRE_FALSE(g.is_node(1));
	CHECK(g.is_node(4));
	CHECK(g.is_connected(4, 2));
	CHECK(g.is_connected(4, 3));
}

TEST_CASE("Replace Node - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	REQUIRE_THROWS_WITH(g.replace_node(5, 6), "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
}

TEST_CASE("Merge and Replace Node") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 5};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));

	g.merge_replace_node(1, 5);
	REQUIRE_FALSE(g.is_node(1));
	CHECK(g.is_node(5));
	CHECK(g.is_connected(5, 2));
	CHECK(g.is_connected(5, 3));
}

TEST_CASE("Merge and Replace Node - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 5};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	REQUIRE_THROWS_WITH(g.merge_replace_node(4, 5),
	                    "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they don't exist in "
	                    "the graph");
	REQUIRE_THROWS_WITH(g.merge_replace_node(3, 6),
	                    "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they don't exist in "
	                    "the graph");
}

TEST_CASE("Erase Node") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.erase_node(3));
	REQUIRE_FALSE(g.is_node(3));
}

TEST_CASE("Erase Weighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	CHECK(g.erase_edge(1, 2, 10));
	REQUIRE_FALSE(g.is_connected(1, 2));
}

TEST_CASE("Erase Weighted Edge - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	REQUIRE_THROWS_WITH(g.erase_edge(1, 4, 10),
	                    "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the graph");
	REQUIRE_THROWS_WITH(g.erase_edge(5, 2, 40),
	                    "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the graph");
}

TEST_CASE("Erase Unweighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.insert_edge(1, 3));

	REQUIRE(g.erase_edge(1, 2));
	REQUIRE_FALSE(g.is_connected(1, 2));
}

TEST_CASE("Erase Unweighted Edge - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.insert_edge(1, 3));
	REQUIRE_THROWS_WITH(g.erase_edge(1, 4),
	                    "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the graph");
	REQUIRE_THROWS_WITH(g.erase_edge(5, 2),
	                    "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist in the graph");
}

TEST_CASE("Erase Edge - Pointed to by Iterator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));

	auto it1 = g.find(1, 2, 10);
	CHECK((*it1).from == 1);
	CHECK((*it1).to == 2);
	CHECK((*it1).weight == 10);

	g.erase_edge(it1);
	REQUIRE_FALSE(g.is_connected(1, 2));
}

TEST_CASE("Erase Edge - Between Iterators") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	CHECK(g.insert_edge(1, 4, 30));
	CHECK(g.insert_edge(2, 3, 40));
	CHECK(g.insert_edge(3, 4, 50));

	auto it1 = g.find(1, 2, 10);
	auto it2 = g.find(1, 4, 30);

	g.erase_edge(it1, it2);

	REQUIRE_FALSE(g.is_connected(1, 2));
	REQUIRE_FALSE(g.is_connected(1, 3));
	CHECK(g.is_connected(1, 4));
	CHECK(g.is_connected(2, 3));
	CHECK(g.is_connected(3, 4));
}

TEST_CASE("Clear All Nodes with Edges from Graph") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.clear();
	CHECK(g.empty());
}

TEST_CASE("Clear All Nodes without Edges from Graph") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	g.clear();
	CHECK(g.empty());
}

TEST_CASE("Accessor - Is Node") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(g.is_node(3));
	REQUIRE_FALSE(g.is_node(4));
}

TEST_CASE("Accessor - Empty") {
	auto g = gdwg::graph<int, int>{};
	CHECK(g.empty());
	CHECK(g.insert_node(1));
	CHECK(g.insert_node(2));
	REQUIRE_FALSE(g.empty());
}

TEST_CASE("Accessor - Is Connected - Weighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.is_connected(1, 2));
}

TEST_CASE("Accessor - Is Connected - Unweighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.is_connected(1, 2));
}

TEST_CASE("Accessor - Is Connected - Weighted Edge - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.is_connected(1, 2));
	REQUIRE_THROWS_WITH(g.is_connected(1, 4),
	                    "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
	REQUIRE_THROWS_WITH(g.is_connected(5, 2),
	                    "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
}

TEST_CASE("Accessor - Is Connected - Unweighted Edge - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.is_connected(1, 2));
	REQUIRE_THROWS_WITH(g.is_connected(1, 4),
	                    "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
	REQUIRE_THROWS_WITH(g.is_connected(5, 2),
	                    "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist in the graph");
}

TEST_CASE("Accessor - Nodes") {
	auto g = gdwg::graph<int, int>{3, 2, 4, 1};
	auto nodes = g.nodes();
	CHECK(nodes == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("Accessor - Edges - Weighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 2, 20));

	auto edges = g.edges(1, 2);
	CHECK(edges.size() == 2);
	CHECK(edges[0]->get_nodes() == std::make_pair(1, 2));
	CHECK(edges[0]->get_weight() == 10);
	CHECK(edges[1]->get_nodes() == std::make_pair(1, 2));
	CHECK(edges[1]->get_weight() == 20);
}

TEST_CASE("Accessor - Edges - Unweighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2));

	auto edges = g.edges(1, 2);
	CHECK(edges.size() == 1);
	CHECK(edges[0]->get_nodes() == std::make_pair(1, 2));
}

TEST_CASE("Accessor - Edges - Mix Edge Type") {
	auto g = gdwg::graph<int, int>{1, 2};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 2, 20));

	auto edges = g.edges(1, 2);
	CHECK(edges.size() == 3);
	CHECK(edges[0]->get_nodes() == std::make_pair(1, 2));
	CHECK(edges[1]->get_nodes() == std::make_pair(1, 2));
	CHECK(edges[1]->get_weight() == 10);
	CHECK(edges[2]->get_nodes() == std::make_pair(1, 2));
	CHECK(edges[2]->get_weight() == 20);
}

TEST_CASE("Accessor - Edges - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2};
	REQUIRE_THROWS_WITH(g.edges(1, 4),
	                    "Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the graph");
	REQUIRE_THROWS_WITH(g.edges(5, 2),
	                    "Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the graph");
}

TEST_CASE("Accessor - Find - Weighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));

	auto iter1 = g.find(1, 2, 10);
	auto [src, dst, weight] = *iter1;
	CHECK(src == 1);
	CHECK(dst == 2);
	CHECK(weight == 10);

	auto iter2 = g.find(2, 3, 30);
	CHECK(iter2 == g.end());
}

TEST_CASE("Accessor - Find - Unweighted Edge") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2);

	auto iter1 = g.find(1, 2);
	auto [src, dst, weight] = *iter1;
	CHECK(src == 1);
	CHECK(dst == 2);
	CHECK(weight == std::nullopt);

	auto iter2 = g.find(2, 3);
	CHECK(iter2 == g.end());
}

TEST_CASE("Accessor - Connections") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2));
	CHECK(g.insert_edge(1, 3));
	CHECK(g.insert_edge(2, 3));

	auto connections_1 = g.connections(1);
	CHECK(connections_1.size() == 2);
	CHECK(connections_1 == std::vector<int>{2, 3});

	auto connections_2 = g.connections(2);
	CHECK(connections_2.size() == 1);
	CHECK(connections_2 == std::vector<int>{3});
}

TEST_CASE("Accessor - Connections - Throw Error") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	REQUIRE_THROWS_WITH(g.connections(4), "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the graph");
}

TEST_CASE("Iterator Access - Begin") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	auto it = g.begin();
	auto [src, dst, weight] = *it;
	CHECK(src == 1);
	CHECK(dst == 2);
	CHECK(weight == 10);
}

TEST_CASE("Iterator Access - End") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	auto it = g.end();
	*--it;
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 10);
}

TEST_CASE("Graphs Comparison - Equality Operator") {
	auto g1 = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g1.insert_edge(1, 2, 10));
	CHECK(g1.insert_edge(1, 3, 20));
	auto g2 = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g2.insert_edge(1, 2, 10));
	CHECK(g2.insert_edge(1, 3, 20));

	CHECK(g1 == g2);

	CHECK(g2.erase_edge(1, 2, 10));
	REQUIRE_FALSE(g1 == g2);

	auto g3 = gdwg::graph<int, int>{1, 2, 5, 6};
	REQUIRE_FALSE(g2 == g3);
}

TEST_CASE("Graph Extractor - Output Operator - Empty") {
	auto g = gdwg::graph<int, int>{};
	std::ostringstream os;
	os << g;
	auto const expected_os = std::string_view(R"()");
	CHECK(os.str() == expected_os);
}

TEST_CASE("Graph Extractor - Output Operator - Normal") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	CHECK(g.insert_edge(4, 3));

	std::ostringstream os;
	os << g;
	auto const expected_os = std::string_view(R"(1 (
  1 -> 2 | W | 10
  1 -> 3 | W | 20
)
2 (
)
3 (
)
4 (
  4 -> 3 | U
)
)");
	CHECK(os.str() == expected_os);
}

TEST_CASE("Graph Extractor - Output Operator - Complex") {
	auto g = gdwg::graph<int, int>{};
	CHECK(g.insert_node(4));
	CHECK(g.insert_node(2));
	CHECK(g.insert_node(3));
	CHECK(g.insert_node(1));
	CHECK(g.insert_node(6));
	CHECK(g.insert_node(5));
	CHECK(g.insert_node(64));
	CHECK(g.insert_edge(4, 1, -4));
	CHECK(g.insert_edge(3, 2, 2));
	CHECK(g.insert_edge(2, 4));
	CHECK(g.insert_edge(2, 4, 2));
	CHECK(g.insert_edge(2, 1, 1));
	CHECK(g.insert_edge(4, 1));
	CHECK(g.insert_edge(6, 2, 5));
	CHECK(g.insert_edge(6, 3, 10));
	CHECK(g.insert_edge(1, 5, -1));
	CHECK(g.insert_edge(3, 6, -8));
	CHECK(g.insert_edge(4, 5, 3));
	CHECK(g.insert_edge(5, 2));

	std::ostringstream os;
	os << g;
	auto const expected_os = std::string_view(R"(1 (
  1 -> 5 | W | -1
)
2 (
  2 -> 1 | W | 1
  2 -> 4 | U
  2 -> 4 | W | 2
)
3 (
  3 -> 2 | W | 2
  3 -> 6 | W | -8
)
4 (
  4 -> 1 | U
  4 -> 1 | W | -4
  4 -> 5 | W | 3
)
5 (
  5 -> 2 | U
)
6 (
  6 -> 2 | W | 5
  6 -> 3 | W | 10
)
64 (
)
)");
	CHECK(os.str() == expected_os);
}

TEST_CASE("Iterator - Default Constructor") {}

TEST_CASE("Iterator - Explicit Constructor") {}

TEST_CASE("Iterator - Reference Operator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	g.insert_edge(1, 3);
	auto it = g.begin();
	auto val1 = *it;
	auto val2 = *++it;
	CHECK(val1.from == 1);
	CHECK(val1.to == 2);
	CHECK(val1.weight == 10);
	CHECK(val2.from == 1);
	CHECK(val2.to == 3);
	CHECK(val2.weight == std::nullopt);
}

TEST_CASE("Iterator - Pre-Increment Operator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	g.insert_edge(1, 3, 20);
	auto it = g.begin();
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 10);
	auto [src, dst, weight] = *++it;
	CHECK(src == 1);
	CHECK(dst == 3);
	CHECK(weight == 20);
}

TEST_CASE("Iterator - Post-Increment Operator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	g.insert_edge(1, 3, 20);
	auto it = g.begin();
	auto [src, dst, weight] = *it++;
	CHECK(src == 1);
	CHECK(dst == 2);
	CHECK(weight == 10);
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == 20);
}

TEST_CASE("Iterator - Pre-Decrement Operator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	g.insert_edge(1, 3, 20);
	auto it = g.end();
	*--it;
	CHECK((*it).from == 1);
	CHECK((*it).to == 3);
	CHECK((*it).weight == 20);
}

TEST_CASE("Iterator - Post-Decrement Operator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 10);
	g.insert_edge(1, 3, 20);
	auto it = g.end();
	*it--;
	auto [src, dst, weight] = *it--;
	CHECK(src == 1);
	CHECK(dst == 3);
	CHECK(weight == 20);
	CHECK((*it).from == 1);
	CHECK((*it).to == 2);
	CHECK((*it).weight == 10);
}

TEST_CASE("Iterator - Equality Operator") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 10));
	CHECK(g.insert_edge(1, 3, 20));
	auto it1 = g.begin();
	auto it2 = g.begin();
	CHECK(it1 == it2);
	++it1;
	REQUIRE_FALSE(it1 == it2);
}
