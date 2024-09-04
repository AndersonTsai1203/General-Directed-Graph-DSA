#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace gdwg {
	template<typename N, typename E>
	class edge {
	 public:
		// Constructors and Destructors
		edge() = default;
		edge(N src, N dst)
		: src_(src)
		, dst_(dst) {}

		virtual ~edge() = default;

		// Pure Virtual Functions
		virtual auto print_edge() const -> std::string = 0;
		virtual auto is_weighted() const noexcept -> bool = 0;
		virtual auto get_weight() const -> std::optional<E> = 0;
		virtual auto get_nodes() const noexcept -> std::pair<N, N> = 0;
		auto operator==(edge const& other) const noexcept -> bool {
			if (this->is_weighted() == other.is_weighted()) {
				return this->get_nodes() == other.get_nodes() and this->get_weight() == other.get_weight();
			}
			return false;
		}

	 protected:
		N src_;
		N dst_;

	 private:
		// You may need to add data members and member functions
		// template<N, E>
		// friend class graph;
	};

	template<typename N, typename E>
	class weighted_edge : public edge<N, E> {
	 public:
		// Constructors and Destructors
		weighted_edge() = default;
		weighted_edge(N src, N dst, E weight)
		: edge<N, E>{src, dst}
		, weight_(weight) {}

		~weighted_edge() = default;

		// Member Functions
		auto print_edge() const -> std::string override {
			std::stringstream ss;
			ss << this->src_ << " -> " << this->dst_ << " | W | " << this->weight_;
			return ss.str();
		}
		auto is_weighted() const noexcept -> bool override {
			return true;
		}
		auto get_weight() const -> std::optional<E> override {
			return this->weight_;
		}
		auto get_nodes() const noexcept -> std::pair<N, N> override {
			return std::pair<N, N>{this->src_, this->dst_};
		}

	 private:
		E weight_;
	};

	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		// Constructors and Destructors
		unweighted_edge() = default;
		unweighted_edge(N src, N dst)
		: edge<N, E>{src, dst} {}
		~unweighted_edge() = default;

		// Member Functions
		auto print_edge() const -> std::string override {
			std::stringstream ss;
			ss << this->src_ << " -> " << this->dst_ << " | U";
			return ss.str();
		}
		auto is_weighted() const noexcept -> bool override {
			return false;
		}
		auto get_weight() const -> std::optional<E> override {
			return std::nullopt;
		}
		auto get_nodes() const noexcept -> std::pair<N, N> override {
			return std::pair<N, N>{this->src_, this->dst_};
		}
	};

	template<typename N, typename E>
	class graph {
	 public:
		class iterator;
		// Constructors and Destructors
		graph() = default;
		~graph() = default;

		// Initializer List Constructor
		graph(std::initializer_list<N> il) {
			for (auto const& node : il) {
				insert_node(node);
			}
		}

		// Range Constructor
		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto it = first; it != last; ++it) {
				insert_node(*it);
			}
		}

		// Move Constructor
		graph(graph&& other) noexcept
		: nodes_(std::move(other.nodes_))
		, adjacency_list_(std::move(other.adjacency_list_)) {
			other.clear();
		}

		// Move Assignment
		auto operator=(graph&& other) noexcept -> graph& {
			if (this != &other) {
				this->nodes_ = std::move(other.nodes_);
				this->adjacency_list_ = std::move(other.adjacency_list_);
				other.clear();
			}
			return *this;
		}

		// Copy Constructor
		graph(graph const& other)
		: nodes_(other.nodes_)
		, adjacency_list_() {
			// iterate through and copy all the edges in the set, we cannot copy unique pointers
			// has to be like a deep copy of edges.
			for (auto const& [src, edges] : other.adjacency_list_) {
				for (auto const& edge : edges) {
					if (edge->is_weighted()) {
						auto new_edge = std::make_unique<weighted_edge<N, E>>(edge->get_nodes().first,
						                                                      edge->get_nodes().second,
						                                                      *edge->get_weight());
						adjacency_list_[src].emplace(std::move(new_edge));
					}
					else {
						auto new_edge =
						    std::make_unique<unweighted_edge<N, E>>(edge->get_nodes().first, edge->get_nodes().second);
						adjacency_list_[src].emplace(std::move(new_edge));
					}
				}
			}
		}

		// Copy Assignment
		auto operator=(graph const& other) -> graph& {
			if (this != &other) {
				*this = graph{other};
			}
			return *this;
		}

		// Modifiers
		auto insert_node(N const& value) -> bool {
			return nodes_.insert(value).second;
		};

		auto insert_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does "
				                         "not exist");
			}

			auto& edges = adjacency_list_[src];
			auto it = std::find_if(edges.begin(), edges.end(), [&dst, &weight](auto const& edge) {
				return edge->get_nodes().second == dst and edge->get_weight() == weight;
			});

			if (it != edges.end()) {
				return false;
			}

			if (weight) {
				auto new_edge = std::make_unique<weighted_edge<N, E>>(src, dst, *weight);
				auto [it, inserted] = adjacency_list_[src].emplace(std::move(new_edge));
				return inserted;
			}
			else {
				auto new_edge = std::make_unique<unweighted_edge<N, E>>(src, dst);
				auto [it, inserted] = adjacency_list_[src].emplace(std::move(new_edge));
				return inserted;
			}
		};

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
			}

			if (is_node(new_data)) {
				return false;
			}

			nodes_.erase(old_data);
			nodes_.insert(new_data);

			auto new_adjacency_list = std::map<N, std::set<std::unique_ptr<edge<N, E>>>>{};
			for (auto& [src, edges] : adjacency_list_) {
				for (auto& e : edges) {
					auto new_src = e->get_nodes().first == old_data ? new_data : e->get_nodes().first;
					auto new_dst = e->get_nodes().second == old_data ? new_data : e->get_nodes().second;
					if (e->is_weighted()) {
						auto new_edge = std::make_unique<weighted_edge<N, E>>(new_src, new_dst, *e->get_weight());
						new_adjacency_list[new_src].insert(std::move(new_edge));
					}
					else {
						auto new_edge = std::make_unique<unweighted_edge<N, E>>(new_src, new_dst);
						new_adjacency_list[new_src].insert(std::move(new_edge));
					}
				}
				if (src == old_data) {
					new_adjacency_list[new_data] = std::move(adjacency_list_[old_data]);
				}
				else {
					new_adjacency_list[src] = std::move(edges);
				}
			}
			adjacency_list_ = std::move(new_adjacency_list);

			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) or !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they "
				                         "don't "
				                         "exist "
				                         "in the graph");
			}

			nodes_.erase(old_data);

			auto new_adjacency_list = std::map<N, std::set<std::unique_ptr<edge<N, E>>>>{};
			for (auto& [src, edges] : adjacency_list_) {
				for (auto& e : edges) {
					auto new_src = e->get_nodes().first == old_data ? new_data : e->get_nodes().first;
					auto new_dst = e->get_nodes().second == old_data ? new_data : e->get_nodes().second;
					if (e->is_weighted()) {
						auto new_edge = std::make_unique<weighted_edge<N, E>>(new_src, new_dst, *e->get_weight());
						new_adjacency_list[new_src].insert(std::move(new_edge));
					}
					else {
						auto new_edge = std::make_unique<unweighted_edge<N, E>>(new_src, new_dst);
						new_adjacency_list[new_src].insert(std::move(new_edge));
					}
				}
				if (src == old_data) {
					new_adjacency_list[new_data] = std::move(adjacency_list_[old_data]);
				}
				else {
					new_adjacency_list[src] = std::move(edges);
				}
			}
			adjacency_list_ = std::move(new_adjacency_list);
		}

		auto erase_node(N const& value) -> bool {
			if (!is_node(value)) {
				return false;
			}

			nodes_.erase(value);

			auto new_adjacency_list = std::map<N, std::set<std::unique_ptr<edge<N, E>>>>{};
			for (auto& [src, edges] : adjacency_list_) {
				if (src == value) {
					continue;
				}

				auto new_edges = std::set<std::unique_ptr<edge<N, E>>>{};
				for (auto& e : edges) {
					if (e->get_nodes().second != value) {
						if (e->is_weighted()) {
							new_edges.insert(std::make_unique<weighted_edge<N, E>>(e->get_nodes().first,
							                                                       e->get_nodes().second,
							                                                       *e->get_weight()));
						}
						else {
							new_edges.insert(
							    std::make_unique<unweighted_edge<N, E>>(e->get_nodes().first, e->get_nodes().second));
						}
					}
				}
				new_adjacency_list[src] = std::move(new_edges);
			}
			adjacency_list_ = std::move(new_adjacency_list);

			return true;
		}

		auto erase_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist "
				                         "in the "
				                         "graph");
			}

			auto new_adjacency_list = std::map<N, std::set<std::unique_ptr<edge<N, E>>>>{};
			auto edge_removed = false;
			for (auto& [current_src, edges] : adjacency_list_) {
				auto new_edges = std::set<std::unique_ptr<edge<N, E>>>{};
				for (auto& e : edges) {
					if (current_src == src and e->get_nodes().second == dst and e->get_weight() == weight) {
						edge_removed = true;
						continue;
					}
					if (e->is_weighted()) {
						new_edges.insert(std::make_unique<weighted_edge<N, E>>(e->get_nodes().first,
						                                                       e->get_nodes().second,
						                                                       *e->get_weight()));
					}
					else {
						new_edges.insert(
						    std::make_unique<unweighted_edge<N, E>>(e->get_nodes().first, e->get_nodes().second));
					}
				}
				if (!new_edges.empty()) {
					new_adjacency_list[current_src] = std::move(new_edges);
				}
			}

			adjacency_list_ = std::move(new_adjacency_list);
			return edge_removed;
		}

		auto erase_edge(iterator i) -> iterator {
			auto [src, dst, weight] = *i;
			erase_edge(src, dst, weight);
			++i;
			if (i == end()) {
				return end();
			}
			return i;
		}

		auto erase_edge(iterator i, iterator s) -> iterator {
			while (i != s) {
				i = erase_edge(i);
			}
			if (i == end()) {
				return end();
			}
			return s;
		}

		auto clear() noexcept -> void {
			nodes_.clear();
			adjacency_list_.clear();
		}

		// Accessors
		[[nodiscard]] auto is_node(N const& value) const -> bool {
			return nodes_.find(value) != nodes_.end();
		}

		[[nodiscard]] auto empty() const -> bool {
			return nodes_.empty();
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist "
				                         "in the "
				                         "graph");
			}
			auto const& edges = adjacency_list_.at(src);
			return std::any_of(edges.begin(), edges.end(), [&dst](auto const& edge) {
				return edge->get_nodes().second == dst;
			});
		}

		[[nodiscard]] auto nodes() const -> std::vector<N> {
			auto result = std::vector<N>{};
			result.reserve(nodes_.size()); // Reserve space to avoid multiple allocations
			for (auto const& node : nodes_) {
				result.push_back(node);
			}
			std::sort(result.begin(), result.end());
			return result;
		}

		[[nodiscard]] auto edges(N const& src, N const& dst) const -> std::vector<std::unique_ptr<edge<N, E>>> {
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the "
				                         "graph");
			}
			auto result = std::vector<std::unique_ptr<edge<N, E>>>{};
			auto unweighted_edges = std::vector<std::unique_ptr<unweighted_edge<N, E>>>{};
			auto weighted_edges = std::vector<std::unique_ptr<weighted_edge<N, E>>>{};

			auto const& edges = adjacency_list_.at(src);
			for (auto& e : edges) {
				if (e->is_weighted()) {
					weighted_edges.push_back(std::make_unique<weighted_edge<N, E>>(e->get_nodes().first,
					                                                               e->get_nodes().second,
					                                                               *e->get_weight()));
				}
				else {
					unweighted_edges.push_back(
					    std::make_unique<unweighted_edge<N, E>>(e->get_nodes().first, e->get_nodes().second));
				}
			}
			if (unweighted_edges.size() != 0) {
				for (auto& e : unweighted_edges) {
					result.push_back(std::move(e));
				}
			}

			std::sort(weighted_edges.begin(), weighted_edges.end(), [](auto const& a, auto const& b) {
				return a->get_weight() < b->get_weight();
			});

			for (auto& e : weighted_edges) {
				result.push_back(std::move(e));
			}
			return result;
		}

		[[nodiscard]] auto find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) const -> iterator {
			auto const& node_it = adjacency_list_.find(src);
			if (node_it == adjacency_list_.end()) {
				return end();
			}
			auto edge_it = std::find_if(node_it->second.begin(), node_it->second.end(), [&dst, &weight](auto const& e) {
				if (weight.has_value()) {
					return e->get_nodes().second == dst and e->get_weight() == weight;
				}
				else {
					return e->get_nodes().second == dst and !e->get_weight().has_value();
				}
			});
			return edge_it != node_it->second.end() ? iterator(node_it, adjacency_list_.end(), edge_it) : end();
		}

		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
				                         "graph");
			}
			auto& edges = adjacency_list_.at(src);
			auto connections = std::set<N>{};
			for (auto& e : edges) {
				connections.insert(e->get_nodes().second);
			}
			return std::vector<N>(connections.begin(), connections.end());
		}

		// Iterator Access
		[[nodiscard]] auto begin() const -> iterator {
			auto node_it = adjacency_list_.begin();
			while (node_it != adjacency_list_.end() and node_it->second.empty()) {
				++node_it;
			}
			if (node_it == adjacency_list_.end()) {
				return end();
			}
			return iterator(node_it, adjacency_list_.end(), node_it->second.begin());
		}

		[[nodiscard]] auto end() const -> iterator {
			auto node_it = adjacency_list_.begin();
			while (node_it != adjacency_list_.end() and node_it->second.empty()) {
				++node_it;
			}
			if (node_it == adjacency_list_.end()) {
				return end();
			}
			return iterator(node_it, adjacency_list_.end(), node_it->second.end());
		}

		// Comparisons
		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			// Check if the sets of nodes are the same
			if (nodes_ != other.nodes_) {
				return false;
			}

			// Check if the adjacency lists have the same structure
			if (adjacency_list_.size() != other.adjacency_list_.size()) {
				return false;
			}

			for (auto const& [src, edges] : adjacency_list_) {
				if (!other.is_node(src)) {
					return false;
				}
				auto const& other_edges = other.adjacency_list_.at(src);
				if (edges.size() != other_edges.size()) {
					return false;
				}

				// Convert the sets of edges to vectors for sorting and comparison
				std::vector<edge<N, E>*> edges_vector;
				for (auto const& edge : edges) {
					edges_vector.push_back(edge.get());
				}

				std::vector<edge<N, E>*> other_edges_vector;
				for (auto const& edge : other_edges) {
					other_edges_vector.push_back(edge.get());
				}

				// Sort the vectors
				auto edge_comparator = [](const edge<N, E>* lhs, const edge<N, E>* rhs) {
					if (lhs->get_nodes().second != rhs->get_nodes().second) {
						return lhs->get_nodes().second < rhs->get_nodes().second;
					}
					if (lhs->is_weighted() and rhs->is_weighted()) {
						return lhs->get_weight() < rhs->get_weight();
					}
					if (!lhs->is_weighted() and !rhs->is_weighted()) {
						return false;
					}
					return lhs->is_weighted() < rhs->is_weighted();
				};
				std::sort(edges_vector.begin(), edges_vector.end(), edge_comparator);
				std::sort(other_edges_vector.begin(), other_edges_vector.end(), edge_comparator);

				// Compare the sorted vectors
				for (size_t i = 0; i < edges_vector.size(); ++i) {
					if (!(edges_vector[i]->get_nodes() == other_edges_vector[i]->get_nodes()
					      and edges_vector[i]->get_weight() == other_edges_vector[i]->get_weight()))
					{
						return false;
					}
				}
			}
			return true;
		}

		// Extractor
		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			if (g.empty()) {
				os << "";
				return os;
			}
			std::vector<std::tuple<N, N, std::optional<E>>> edges;

			for (auto const& [src, edge_set] : g.adjacency_list_) {
				for (auto const& edge : edge_set) {
					edges.emplace_back(src, edge->get_nodes().second, edge->get_weight());
				}
			}

			// Sort the edges by src, dst, and weight
			std::sort(edges.begin(), edges.end(), [](auto const& a, auto const& b) {
				if (std::get<0>(a) != std::get<0>(b)) {
					return std::get<0>(a) < std::get<0>(b);
				}
				if (std::get<1>(a) != std::get<1>(b)) {
					return std::get<1>(a) < std::get<1>(b);
				}
				if (std::get<2>(a).has_value() and std::get<2>(b).has_value()) {
					return std::get<2>(a).value() < std::get<2>(b).value();
				}
				return std::get<2>(a).has_value() ? false : true;
			});

			for (auto const& node : g.nodes_) {
				os << node << " (\n";

				for (auto const& edge : edges) {
					if (std::get<0>(edge) == node) {
						os << "  " << std::get<0>(edge) << " -> " << std::get<1>(edge) << " | ";
						if (std::get<2>(edge).has_value()) {
							os << "W | " << std::get<2>(edge).value() << "\n";
						}
						else {
							os << "U\n";
						}
					}
				}
				os << ")\n";
			}
			return os;
		};

		class iterator {
		 public:
			using value_type = struct {
				N from;
				N to;
				std::optional<E> weight;
			};
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;
			using node_iterator = std::map<N, std::set<std::unique_ptr<edge<N, E>>>>::const_iterator;
			using edge_iterator = std::set<std::unique_ptr<edge<N, E>>>::const_iterator;

			// Constructors and Destructors
			iterator()
			: current_node_it_{}
			, end_node_it_{}
			, edge_it_{} {}
			~iterator() = default;

			explicit iterator(node_iterator current_node_it, node_iterator end_node_it, edge_iterator edge_it)
			: current_node_it_{current_node_it}
			, end_node_it_{end_node_it}
			, edge_it_{edge_it} {
				if (current_node_it_ != end_node_it_) {
					edge_it_ = current_node_it_->second.begin();
				}
			}

			// Iterator Source
			auto operator*() const -> reference {
				return {current_node_it_->first, (*edge_it_)->get_nodes().second, (*edge_it_)->get_weight()};
			}

			// Iterator Traversal (Pre Increment)
			auto operator++() -> iterator& {
				++edge_it_;
				while (current_node_it_ != end_node_it_ and edge_it_ == current_node_it_->second.end()) {
					++current_node_it_;
					if (current_node_it_ != end_node_it_) {
						edge_it_ = current_node_it_->second.begin();
					}
				}
				return *this;
			}

			// Post Increment
			auto operator++(int) -> iterator {
				auto temp = *this;
				++*this;
				return temp;
			}

			// Pre Decrement
			auto operator--() -> iterator& {
				if (edge_it_ == current_node_it_->second.begin()) {
					if (current_node_it_ != end_node_it_) {
						while (current_node_it_ != end_node_it_) {
							--current_node_it_;
							if (!current_node_it_->second.empty()) {
								edge_it_ = current_node_it_->second.end();
								break;
							}
						}
					}
				}
				--edge_it_;
				return *this;
			}

			// Post Decrement
			auto operator--(int) -> iterator {
				auto temp = *this;
				--*this;
				return temp;
			}

			// Iterator Comparisons
			auto operator==(iterator const& other) const noexcept -> bool {
				return current_node_it_ == other.current_node_it_
				       and (current_node_it_ == end_node_it_ or edge_it_ == other.edge_it_);
			}

		 private:
			node_iterator current_node_it_;
			node_iterator end_node_it_;
			edge_iterator edge_it_;
		};

	 private:
		std::set<N> nodes_;
		std::map<N, std::set<std::unique_ptr<edge<N, E>>>> adjacency_list_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
