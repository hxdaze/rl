//
// Copyright (c) 2009, Markus Rickert
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#ifndef RL_PLAN_PRM_H
#define RL_PLAN_PRM_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/pending/disjoint_sets.hpp>

#include "Metric.h"
#include "NearestNeighbors.h"
#include "Planner.h"
#include "VectorPtr.h"

namespace rl
{
	namespace plan
	{
		class Model;
		class Sampler;
		class Verifier;
		
		/**
		 * Probabilistic Roadmaps.
		 *
		 * Lydia Kavraki and Jean-Claude Latombe. Randomized preprocessing of
		 * configuration space for path planning: Articulated robots. In Proceedings
		 * of the IEEE/RSJ/GI International Conference on Intelligent Robots and
		 * Systems, pages 1764-1771, Munich, Germany, September 1994.
		 *
		 * http://dx.doi.org/10.1109/IROS.1994.407619
		 *
		 * Lydia E. Kavraki, Petr &Scaron;vestka, Jean-Claude Latombe, and Mark H.
		 * Overmars. Probabilistic roadmaps for path planning in high-dimensional
		 * configuration spaces. IEEE Transactions on Robotics and Automation,
		 * 12(4):566-580, August 1996.
		 *
		 * http://dx.doi.org/10.1109/70.508439
		 */
		class RL_PLAN_EXPORT Prm : public Planner
		{
		public:
			enum class Search
			{
				astar,
				dijkstra
			};
			
			Prm();
			
			virtual ~Prm();
			
			virtual void construct(const ::std::size_t& steps);
			
			::std::size_t getMaxDegree() const;
			
			::std::size_t getMaxNeighbors() const;
			
			::rl::math::Real getMaxRadius() const;
			
			virtual ::std::string getName() const;
			
			NearestNeighbors* getNearestNeighbors() const;
			
			::std::size_t getNumEdges() const;
			
			::std::size_t getNumVertices() const;
			
			VectorList getPath();
			
			Sampler* getSampler() const;
			
			Search getSearch() const;
			
			Verifier* getVerifier() const;
			
			void reset();
			
			void setMaxDegree(const ::std::size_t& degree);
			
			void setMaxNeighbors(const ::std::size_t& k);
			
			void setMaxRadius(const ::rl::math::Real& radius);
			
			void setNearestNeighbors(NearestNeighbors* nearestNeighbors);
			
			void setSampler(Sampler* sampler);
			
			void setSearch(const Search& search);
			
			void setVerifier(Verifier* verifier);
			
			bool solve();
			
			bool astar;
			
			/** Maximum degree per vertex. */
			::std::size_t degree;
			
			/** Maximum number of tested neighbors. */
			::std::size_t k;
			
			/** Maximum radius for connecting neighbors. */
			::rl::math::Real radius;
			
			Sampler* sampler;
			
			Verifier* verifier;
			
		protected:
			struct EdgeBundle
			{
				::rl::math::Real weight;
			};
			
			struct GraphBundle;
			
			typedef ::boost::adjacency_list_traits<
				::boost::listS,
				::boost::listS,
				::boost::undirectedS,
				::boost::listS
			>::vertex_descriptor Vertex;
			
			struct VertexBundle
			{
				::boost::default_color_type color;
				
				::rl::math::Real cost;
				
				::rl::math::Real distance;
				
				::std::size_t index;
				
				Vertex parent;
				
				Vertex predecessor;
				
				VectorPtr q;
				
				::std::size_t rank;
			};
			
			typedef ::boost::adjacency_list<
				::boost::listS,
				::boost::listS,
				::boost::undirectedS,
				VertexBundle,
				EdgeBundle,
				GraphBundle
			> Graph;
			
			struct GraphBundle
			{
				NearestNeighbors* nn;
			};
			
			class AStarHeuristic : public ::boost::astar_heuristic<Graph, ::rl::math::Real>
			{
			public:
				AStarHeuristic(const Model* model, const Graph& graph, const Vertex& goal);
				
				::rl::math::Real operator()(Vertex u);
				
			protected:
				
			private:
				const Vertex& goal;
				
				const Graph& graph;
				
				const Model* model;
			};
			
			typedef ::boost::graph_traits<Graph>::edge_descriptor Edge;
			
			typedef ::boost::graph_traits<Graph>::edge_iterator EdgeIterator;
			
			typedef ::std::pair<EdgeIterator, EdgeIterator> EdgeIteratorPair;
			
			typedef NearestNeighbors::Neighbor Neighbor;
			
			typedef ::boost::graph_traits<Graph>::vertex_iterator VertexIterator;
			
			typedef ::std::pair<VertexIterator, VertexIterator> VertexIteratorPair;
			
			typedef ::boost::property_map<Graph, Vertex VertexBundle::*>::type VertexParentMap;
			
			typedef ::boost::property_map<Graph, ::std::size_t VertexBundle::*>::type VertexRankMap;
			
			Edge addEdge(const Vertex& u, const Vertex& v, const ::rl::math::Real& weight);
			
			Vertex addVertex(const VectorPtr& q);
			
			void insert(const Vertex& vertex);
			
			Vertex begin;
			
			::boost::disjoint_sets<VertexRankMap, VertexParentMap> ds;
			
			Vertex end;
			
			Graph graph;
			
		private:
			
		};
	}
}

#endif // RL_PLAN_PRM_H
