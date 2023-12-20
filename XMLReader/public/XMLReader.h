#pragma once

#include "symbol.h"
#include "codeSource.h"

#include <map>
#include <list>
#include <string>
#include <functional>

namespace xml_reader
{
	struct Node
	{
		scripting::ISymbol* m_rootSymbol;

		std::string m_tagName;
		std::map<std::string, std::string> m_tagProps;

		std::list<scripting::ISymbol*> m_data;

		Node* m_parent = nullptr;
		std::list<Node*> m_children;
	};


	class XMLTree
	{
		friend void ReadXML(const std::string& src, XMLTree& xmlTree);

	private:
		scripting::CodeSource m_codeSource;
		std::list<Node*> m_allNodes;
		std::list<Node*> m_rootNodes;

	public:
		~XMLTree();

		void FindNodes(const std::function<bool(const Node*)>& predicate, std::list<const Node*>& outNodes) const;
		const Node* FindNode(const std::function<bool(const Node*)>& predicate) const;
		
		void FindChildNodes(
			const Node* rootNode,
			const std::function<bool(const Node*)>& predicate,
			bool directChild, std::list<const Node*>& outNodes) const;

		const Node* FindChildNode(
			const Node* rootNode,
			const std::function<bool(const Node*)>& predicate,
			bool directChild) const;
	};

	void ReadXML(const std::string& src, XMLTree& xmlTree);
	void Boot();
}